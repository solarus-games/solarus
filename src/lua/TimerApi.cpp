/*
 * Copyright (C) 2006-2016 Christopho, Solarus - http://www.solarus-games.org
 *
 * Solarus is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Solarus is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "solarus/entities/Entity.h"
#include "solarus/lowlevel/Debug.h"
#include "solarus/lowlevel/System.h"
#include "solarus/lua/ExportableToLuaPtr.h"
#include "solarus/lua/LuaContext.h"
#include "solarus/lua/LuaTools.h"
#include "solarus/Game.h"
#include "solarus/MainLoop.h"
#include "solarus/Map.h"
#include "solarus/Timer.h"
#include <list>
#include <sstream>

namespace Solarus {

/**
 * Name of the Lua table representing the timer module.
 */
const std::string LuaContext::timer_module_name = "sol.timer";

/**
 * \brief Initializes the timer features provided to Lua.
 */
void LuaContext::register_timer_module() {

  // Functions of sol.timer.
  const std::vector<luaL_Reg> functions = {
      { "start", timer_api_start },
      { "stop_all", timer_api_stop_all }
  };

  // Methods of the timer type.
  const std::vector<luaL_Reg> methods = {
      { "stop", timer_api_stop },
      { "is_with_sound", timer_api_is_with_sound },
      { "set_with_sound", timer_api_set_with_sound },
      { "is_suspended", timer_api_is_suspended },
      { "set_suspended", timer_api_set_suspended },
      { "is_suspended_with_map", timer_api_is_suspended_with_map },
      { "set_suspended_with_map", timer_api_set_suspended_with_map },
      { "get_remaining_time", timer_api_get_remaining_time },
      { "set_remaining_time", timer_api_set_remaining_time }
  };

  const std::vector<luaL_Reg> metamethods = {
      { "__gc", userdata_meta_gc }
  };

  register_type(timer_module_name, functions, methods, metamethods);
}

/**
 * \brief Returns whether a value is a userdata of type timer.
 * \param l A Lua context.
 * \param index An index in the stack.
 * \return true if the value at this index is a timer.
 */
bool LuaContext::is_timer(lua_State* l, int index) {
  return is_userdata(l, index, timer_module_name);
}

/**
 * \brief Checks that the userdata at the specified index of the stack is a
 * timer and returns it.
 * \param l a Lua context
 * \param index an index in the stack
 * \return the timer
 */
TimerPtr LuaContext::check_timer(lua_State* l, int index) {
  return std::static_pointer_cast<Timer>(
      check_userdata(l, index, timer_module_name)
  );
}

/**
 * \brief Pushes a timer userdata onto the stack.
 * \param l A Lua context.
 * \param timer A timer.
 */
void LuaContext::push_timer(lua_State* l, const TimerPtr& timer) {
  push_userdata(l, *timer);
}

/**
 * \brief Registers a timer into a context (table or a userdata).
 * \param timer A timer.
 * \param context_index Index of the table or userdata in the stack.
 * \param callback_ref Lua ref to the function to call when the timer finishes.
 */
void LuaContext::add_timer(
    const TimerPtr& timer,
    int context_index,
    const ScopedLuaRef& callback_ref
) {
  const void* context;
  if (lua_type(l, context_index) == LUA_TUSERDATA) {
    ExportableToLuaPtr* userdata = static_cast<ExportableToLuaPtr*>(
        lua_touserdata(l, context_index)
    );
    context = userdata->get();
  }
  else {
    context = lua_topointer(l, context_index);
  }

  Debug::execute_if_debug([&] {
    // Sanity check: check the uniqueness of the ref.
    for (const auto& kvp: timers) {
      if (kvp.second.callback_ref.get() == callback_ref.get()) {
        std::ostringstream oss;
        oss << "Callback ref " << callback_ref.get()
            << " is already used by a timer (duplicate luaL_unref?)";
        Debug::die(oss.str());
      }
    }
  });

  Debug::check_assertion(timers.find(timer) == timers.end(),
      "Duplicate timer in the system");

  timers[timer].callback_ref = callback_ref;
  timers[timer].context = context;

  Game* game = main_loop.get_game();
  if (game != nullptr) {
    // We are during a game: depending on the timer's context,
    // suspend the timer or not.
    if (is_map(l, context_index)
        || is_entity(l, context_index)
        || is_item(l, context_index)) {

      bool initially_suspended = false;

      // By default, we want the timer to be automatically suspended when a
      // camera movement, a dialog or the pause menu starts.
      if (!is_entity(l, context_index)) {
        // The timer normally gets suspended/resumed with the map.
        timer->set_suspended_with_map(true);

        // But in the initial state, we override that rule.
        // We initially suspend the timer only during a dialog.
        // In particular, we don't want to suspend timers created during a
        // camera movement.
        // This would be very painful for users.
        initially_suspended = game->is_dialog_enabled();
      }
      else {
        // Entities are more complex: they also get suspended when disabled.
        // Therefore, they don't simply follow
        // the map suspended state.
        EntityPtr entity = check_entity(l, context_index);
        initially_suspended = entity->is_suspended() || !entity->is_enabled();
      }

      timer->set_suspended(initially_suspended);
    }
  }
}

/**
 * \brief Unregisters a timer associated to a context.
 *
 * This function can be called safely even while iterating on the timer list.
 *
 * \param timer A timer.
 */
void LuaContext::remove_timer(const TimerPtr& timer) {
  if (timers.find(timer) != timers.end()) {
    timers[timer].callback_ref.clear();
    timers_to_remove.push_back(timer);
  }
}

/**
 * \brief Unregisters all timers associated to a context.
 *
 * This function can be called safely even while iterating on the timer list.
 *
 * \param context_index Index of a table or userdata containing timers.
 */
void LuaContext::remove_timers(int context_index) {

  const void* context;
  if (lua_type(l, context_index) == LUA_TUSERDATA) {
    ExportableToLuaPtr* userdata = static_cast<ExportableToLuaPtr*>(
        lua_touserdata(l, context_index));
    context = userdata->get();
  }
  else {
    context = lua_topointer(l, context_index);
  }

  for (auto& kvp: timers) {
    const TimerPtr& timer = kvp.first;
    if (kvp.second.context == context) {
      kvp.second.callback_ref.clear();
      timers_to_remove.push_back(timer);
    }
  }
}

/**
 * \brief Destroys immediately all existing timers.
 */
void LuaContext::destroy_timers() {
  timers.clear();
}

/**
 * \brief Updates all timers currently running for this script.
 */
void LuaContext::update_timers() {

  // Update all timers.
  for (const auto& kvp: timers) {

    const TimerPtr& timer = kvp.first;
    const ScopedLuaRef& callback_ref = kvp.second.callback_ref;
    if (!callback_ref.is_empty()) {
      // The timer is not being removed: update it.
      timer->update();
      if (timer->is_finished()) {
        do_timer_callback(timer);
      }
    }
  }

  // Destroy the ones that should be removed.
  for (const TimerPtr& timer: timers_to_remove) {

    const auto& it = timers.find(timer);
    if (it != timers.end()) {
      timers.erase(it);

      Debug::check_assertion(timers.find(timer) == timers.end(),
          "Failed to remove timer");
    }
  }
  timers_to_remove.clear();
}

/**
 * \brief This function is called when the game (if any) is being suspended
 * or resumed.
 * \param suspended true if the game is suspended, false if it is resumed.
 */
void LuaContext::notify_timers_map_suspended(bool suspended) {

  for (const auto& kvp: timers) {
    const TimerPtr& timer = kvp.first;
    if (timer->is_suspended_with_map()) {
      timer->notify_map_suspended(suspended);
    }
  }
}

/**
 * \brief Suspends or resumes the timers attached to a map entity.
 *
 * This is independent from the Timer::is_suspended_with_map() property.
 *
 * \param entity A map entity.
 * \param suspended \c true to suspend its timers, \c false to resume them.
 */
void LuaContext::set_entity_timers_suspended(
    Entity& entity, bool suspended
) {

  for (const auto& kvp: timers) {
    const TimerPtr& timer = kvp.first;
    if (kvp.second.context == &entity) {
      timer->set_suspended(suspended);
    }
  }
}

/**
 * \brief Executes the callback of a timer.
 *
 * Then, if the callback returns \c true, the timer is rescheduled,
 * otherwise it is discarded.
 *
 * Does nothing if the timer is already finished.
 *
 * \param timer The timer to execute.
 */
void LuaContext::do_timer_callback(const TimerPtr& timer) {

  Debug::check_assertion(timer->is_finished(), "This timer is still running");

  auto it = timers.find(timer);
  if (it != timers.end() &&
      !it->second.callback_ref.is_empty()) {
    ScopedLuaRef& callback_ref = it->second.callback_ref;
    push_ref(l, callback_ref);
    const bool success = call_function(0, 1, "timer callback");

    bool repeat = false;
    if (success) {
      repeat = lua_isboolean(l, -1) && lua_toboolean(l, -1);
      lua_pop(l, 1);
    }

    if (repeat) {
      // The callback returned true: reschedule the timer.
      timer->set_expiration_date(timer->get_expiration_date() + timer->get_initial_duration());
      if (timer->is_finished()) {
        // Already finished: this is possible if the duration is smaller than
        // the main loop stepsize.
        do_timer_callback(timer);
      }
    }
    else {
      callback_ref.clear();
      timers_to_remove.push_back(timer);
    }
  }
}

/**
 * \brief Implementation of sol.timer.start().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::timer_api_start(lua_State *l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    // Parameters: [context] delay callback.
    LuaContext& lua_context = get_lua_context(l);

    if (lua_type(l, 1) != LUA_TNUMBER) {
      // The first parameter is the context.
      if (lua_type(l, 1) != LUA_TTABLE
          && lua_type(l, 1) != LUA_TUSERDATA) {
        LuaTools::type_error(l, 1, "table or userdata");
      }
    }
    else {
      // No context specified: set a default context:
      // - during a game: the current map,
      // - outside a game: sol.main.

      Game* game = lua_context.get_main_loop().get_game();
      if (game != nullptr && game->has_current_map()) {
        push_map(l, game->get_current_map());
      }
      else {
        LuaContext::push_main(l);
      }

      lua_insert(l, 1);
    }
    // Now the first parameter is the context.

    uint32_t delay = uint32_t(LuaTools::check_int(l, 2));
    const ScopedLuaRef& callback_ref = LuaTools::check_function(l, 3);

    // Create the timer.
    TimerPtr timer = std::make_shared<Timer>(delay);
    lua_context.add_timer(timer, 1, callback_ref);

    if (delay == 0) {
      // The delay is zero: call the function right now.
      lua_context.do_timer_callback(timer);
    }

    push_timer(l, timer);

    return 1;
  });
}

/**
 * \brief Implementation of timer:stop().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::timer_api_stop(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    LuaContext& lua_context = get_lua_context(l);
    const TimerPtr& timer = check_timer(l, 1);
    lua_context.remove_timer(timer);

    return 0;
  });
}

/**
 * \brief Implementation of sol.timer.stop_all().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::timer_api_stop_all(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    if (lua_type(l, 1) != LUA_TTABLE
        && lua_type(l, 1) != LUA_TUSERDATA) {
      LuaTools::type_error(l, 1, "table or userdata");
    }

    get_lua_context(l).remove_timers(1);

    return 0;
  });
}

/**
 * \brief Implementation of timer:is_with_sound().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::timer_api_is_with_sound(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const TimerPtr& timer = check_timer(l, 1);

    lua_pushboolean(l, timer->is_with_sound());
    return 1;
  });
}

/**
 * \brief Implementation of timer:set_with_sound().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::timer_api_set_with_sound(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const TimerPtr& timer = check_timer(l, 1);
    bool with_sound = LuaTools::opt_boolean(l, 2, true);

    timer->set_with_sound(with_sound);

    return 0;
  });
}

/**
 * \brief Implementation of timer:is_suspended().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::timer_api_is_suspended(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const TimerPtr& timer = check_timer(l, 1);

    lua_pushboolean(l, timer->is_suspended());
    return 1;
  });
}

/**
 * \brief Implementation of timer:set_suspended().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::timer_api_set_suspended(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const TimerPtr& timer = check_timer(l, 1);
    bool suspended = LuaTools::opt_boolean(l, 2, true);

    timer->set_suspended(suspended);

    return 0;
  });
}

/**
 * \brief Implementation of timer:is_suspended_with_map().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::timer_api_is_suspended_with_map(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const TimerPtr& timer = check_timer(l, 1);

    lua_pushboolean(l, timer->is_suspended_with_map());
    return 1;
  });
}

/**
 * \brief Implementation of timer:set_suspended_with_map().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::timer_api_set_suspended_with_map(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    LuaContext& lua_context = get_lua_context(l);

    const TimerPtr& timer = check_timer(l, 1);
    bool suspended_with_map = LuaTools::opt_boolean(l, 2, true);

    timer->set_suspended_with_map(suspended_with_map);

    Game* game = lua_context.get_main_loop().get_game();
    if (game != nullptr && game->has_current_map()) {
      // If the game is running, suspend/resume the timer like the map.
      timer->notify_map_suspended(game->get_current_map().is_suspended());
    }

    return 0;
  });
}

/**
 * \brief Implementation of timer:get_remaining_time().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::timer_api_get_remaining_time(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const TimerPtr& timer = check_timer(l, 1);

    LuaContext& lua_context = get_lua_context(l);
    const auto it = lua_context.timers.find(timer);
    if (it == lua_context.timers.end() ||
        it->second.callback_ref.is_empty()) {
      // This timer is already finished or was canceled.
      lua_pushinteger(l, 0);
    }
    else {
      int remaining_time = (int) timer->get_expiration_date() - (int) System::now();
      if (remaining_time < 0) {
        remaining_time = 0;
      }
      lua_pushinteger(l, remaining_time);
    }
    return 1;
  });
}

/**
 * \brief Implementation of timer:set_remaining_time().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::timer_api_set_remaining_time(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const TimerPtr& timer = check_timer(l, 1);
    uint32_t remaining_time = LuaTools::check_int(l, 2);

    LuaContext& lua_context = get_lua_context(l);
    const auto it = lua_context.timers.find(timer);
    if (it != lua_context.timers.end() &&
        !it->second.callback_ref.is_empty()) {
      // The timer is still active.
      const uint32_t now = System::now();
      const uint32_t expiration_date = now + remaining_time;
      timer->set_expiration_date(expiration_date);
      if (now >= expiration_date) {
        // Execute the callback now.
        lua_context.do_timer_callback(timer);
      }
    }

    return 0;
  });
}

}

