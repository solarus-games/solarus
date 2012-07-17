/*
 * Copyright (C) 2006-2012 Christopho, Solarus - http://www.solarus-games.org
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
#include "lua/LuaContext.h"
#include "lowlevel/Debug.h"
#include "Timer.h"
#include "Game.h"
#include <lua.hpp>

const std::string Script::timer_module_name = "sol.timer";

/**
 * @brief Initializes the timer features provided to Lua.
 */
void Script::register_timer_module() {

  // Functions of sol.timer.
  static const luaL_Reg functions[] = {
      { "start", timer_api_start },
      { NULL, NULL }
  };
  register_functions(timer_module_name, functions);

  // Methods of the timer type.
  static const luaL_Reg methods[] = {
      { "stop", timer_api_stop },
      { "is_with_sound", timer_api_is_with_sound },
      { "set_with_sound", timer_api_set_with_sound },
      { NULL, NULL }
  };
  static const luaL_Reg metamethods[] = {
      { "__eq", userdata_meta_eq },
      { "__gc", userdata_meta_gc },
      { NULL, NULL }
  };
  register_type(timer_module_name, methods, metamethods);
}

/**
 * @brief Returns whether a timer just created should be initially suspended.
 * @return true to initially suspend a new timer
 */
bool Script::is_new_timer_suspended(void) {

  if (apis_enabled & MAP_API) {
    // start the timer even if the game is suspended (e.g. a timer started during a camera movement)
    // except when it is suspended because of a dialog box
    return get_game().is_showing_dialog();
  }

  return false;
}

/**
 * @brief Checks that the userdata at the specified index of the stack is a
 * timer and returns it.
 * @param l a Lua context
 * @param index an index in the stack
 * @return the timer
 */
Timer& Script::check_timer(lua_State* l, int index) {
  return static_cast<Timer&>(check_userdata(l, index, timer_module_name));
}

/**
 * @brief Pushes a timer userdata onto the stack.
 * @param l a Lua context
 * @param timer a timer
 */
void Script::push_timer(lua_State* l, Timer& timer) {
  push_userdata(l, timer);
}

/**
 * @brief Registers a timer into a context (table or a userdata).
 * @param timer A timer.
 * @param context_index Index of the table or userdata in the stack.
 * @param callback_index Index of the function to call when the timer finishes.
 */
void Script::add_timer(Timer* timer, int context_index, int callback_index) {

  const void* context = lua_topointer(l, context_index);
  lua_pushvalue(l, callback_index);
  int callback_ref = create_ref();
  lua_pop(l, 1);

  set_created(timer);
  timers[timer].callback_ref = callback_ref;
  timers[timer].context = context;

  if (get_script(l).is_new_timer_suspended()) {
    timer->set_suspended(true);
  }
  increment_refcount(timer);
}

/**
 * @brief Unregisters a timer associated to a context.
 * @param timer A timer.
 */
void Script::remove_timer(Timer* timer) {

  Debug::check_assertion(timers.count(timer) > 0,
      "Cannot remove this timer: timer not found");

  if (!timer->is_finished()) {
    destroy_ref(timers[timer].callback_ref);
  }
  timers.erase(timer);
  decrement_refcount(timer);  // Deleted if Lua has already collected it.
}

/**
 * @brief Unregisters all timers associated to a context.
 * @param context_index Index of a table or userdata containing timers.
 */
void Script::remove_timers(int context_index) {

  std::list<Timer*> timers_to_remove;

  const void* context = lua_topointer(l, context_index);
  std::map<Timer*, LuaTimerData>::iterator it;
  for (it = timers.begin(); it != timers.end(); ++it) {
    Timer* timer = it->first;
    if (it->second.context == context) {
      if (!timer->is_finished()) {
        destroy_ref(it->second.callback_ref);
      }
      decrement_refcount(timer);
      timers_to_remove.push_back(timer);
    }
  }

  std::list<Timer*>::iterator it2;
  for (it2 = timers_to_remove.begin(); it2 != timers_to_remove.end(); ++it2) {
    timers.erase(*it2);
  }
}

/**
 * @brief Unregisters all existing timers.
 */
void Script::remove_timers() {

  std::map<Timer*, LuaTimerData>::iterator it;
  for (it = timers.begin(); it != timers.end(); ++it) {

    Timer* timer = it->first;
    if (!timer->is_finished()) {
      destroy_ref(it->second.callback_ref);
    }
    decrement_refcount(timer);
  }
  timers.clear();
}

/**
 * @brief Updates all timers currently running for this script.
 */
void Script::update_timers() {

  std::list<Timer*> timers_to_remove;

  std::map<Timer*, LuaTimerData>::iterator it;
  for (it = timers.begin(); it != timers.end(); ++it) {

    Timer* timer = it->first;
    timer->update();
    if (timer->is_finished()) {
      do_callback(it->second.callback_ref);
      lua_pushlightuserdata(l, (void*) it->second.context);
      timers_to_remove.push_back(timer);
      lua_pop(l, 1);
      break;
    }
  }

  std::list<Timer*>::iterator it2;
  for (it2 = timers_to_remove.begin(); it2 != timers_to_remove.end(); ++it2) {
    remove_timer(*it2);
  }
}

/**
 * @brief This function is called when the game (if any) is being suspended
 * or resumed.
 * @param suspended true if the game is suspended, false if it is resumed.
 */
void Script::set_suspended_timers(bool suspended) {

  std::map<Timer*, LuaTimerData>::iterator it;
  for (it = timers.begin(); it != timers.end(); ++it) {
    it->first->set_suspended(suspended);
  }
}

/**
 * @brief Implementation of \ref lua_api_timer_start.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int Script::timer_api_start(lua_State *l) {

  // Parameters: [context] delay callback.
  Script& script = get_script(l);

  if (lua_type(l, 1) != LUA_TNUMBER) {
    // The first parameter is the context.
    if (lua_type(l, 1) != LUA_TTABLE
        && lua_type(l, 1) != LUA_TUSERDATA) {
      luaL_typerror(l, 1, "table or userdata");
    }
  }
  else {
    // No context specified: set a default context.
    LuaContext::push_main(l);
    // TODO: instead of sol.main: during a game, use the current map,
    // and outside a game, use the current menu.
    lua_insert(l, 1);
  }
  // Now the first parameter is the context.

  uint32_t delay = luaL_checkinteger(l, 2);
  luaL_checktype(l, 3, LUA_TFUNCTION);

  if (delay == 0) {
    // The delay is zero: call the function right now.
    lua_settop(l, 3);
    script.call_function(0, 0, "callback");
    lua_pushnil(l);
  }
  else {
    // Create the timer.
    Timer* timer = new Timer(delay);
    script.add_timer(timer, 3, 2);
    push_timer(l, *timer);
  }

  return 1;
}

/**
 * @brief Implementation of \ref lua_api_timer_stop.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int Script::timer_api_stop(lua_State* l) {

  Script& script = get_script(l);
  Timer& timer = check_timer(l, 1);
  script.remove_timer(&timer);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_timer_is_with_sound.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int Script::timer_api_is_with_sound(lua_State* l) {

  Timer& timer = check_timer(l, 1);

  lua_pushboolean(l, timer.is_with_sound());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_timer_set_with_sound.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int Script::timer_api_set_with_sound(lua_State* l) {

  Timer& timer = check_timer(l, 1);
  bool with_sound = true;
  if (lua_gettop(l) > 1) {
    with_sound = lua_toboolean(l, 2);
  }

  timer.set_with_sound(with_sound);

  return 0;
}

