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

  static const luaL_Reg methods[] = {
      { "stop", timer_api_stop },
      { NULL, NULL }
  };
  static const luaL_Reg metamethods[] = {
      { "__eq", userdata_meta_eq },
      { "__gc", userdata_meta_gc },
      { NULL, NULL }
  };
  register_type(timer_module_name, methods, metamethods);

  // Make sol.main able to store timers to allow global timers.
  lua_getglobal(l, "sol");
  lua_getfield(l, -1, "main");
  enable_timers(-1);
}

/**
 * @brief Returns whether a timer just created should be initially suspended.
 * @return true to initially suspend a new timer
 */
bool Script::is_new_timer_suspended(void) {

  if (apis_enabled & GAME_API) {
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
 * @brief Makes a Lua object capable of running timers.
 *
 * The object will then have a start_timer() method and a stop_timers()
 * methods.
 *
 * @param table_index Index of a Lua table.
 */
void Script::enable_timers(int table_index) {

  if (table_index < 0) {
    // ensure a positive index
    table_index = lua_gettop(l) + table_index + 1;
  }

  luaL_checktype(l, table_index, LUA_TTABLE);
                                  // ... object ...
  lua_pushcfunction(l, timer_api_start_timer);
                                  // ... object ... start_timer
  lua_setfield(l, table_index, "start_timer");
                                  // ... object ...
  lua_pushcfunction(l, timer_api_stop_timers);
                                  // ... object ... stop_timers
  lua_setfield(l, -2, "stop_timers");
                                  // ... object ...
}

/**
 * @brief Stops all timers currently running for a Lua object.
 * @param table_index Index of a Lua table.
 */
void Script::disable_timers(int table_index) {

  remove_timers(table_index);
}

/**
 * @brief Registers a timer into a table.
 * @param timer A timer.
 * @param table_index Index of a table in the stack.
 * @param callback_index Index of the function to call when the timer finishes.
 */
void Script::add_timer(Timer* timer, int table_index, int callback_index) {

  const void* table_pointer = lua_topointer(l, table_index);
  lua_pushvalue(l, callback_index);
  int callback_ref = create_ref();
  lua_pop(l, 1);

  set_created(timer);
  table_timers[table_pointer][timer] = callback_ref;

  if (get_script(l).is_new_timer_suspended()) {
    timer->set_suspended(true);
  }
  increment_refcount(timer);
}

/**
 * @brief Unregisters a timer associated to a table.
 * @param timer A timer.
 */
void Script::remove_timer(Timer* timer) {

  bool found = false;
  std::map<const void*, std::map<Timer*, int> >::iterator it;
  for (it = table_timers.begin(); it != table_timers.end() && !found; ++it) {

    std::map<Timer*, int> timers = it->second;
    if (timers.count(timer) > 0) {

      if (!timer->is_finished()) {
        destroy_ref(timers[timer]);
      }
      timers.erase(timer);
      decrement_refcount(timer); // will be deleted if Lua has already collected it
    }
  }

  Debug::check_assertion(found, "Failed to remove this timer: timer not found");
}

/**
 * @brief Unregisters a timer associated to a table.
 * @param timer A timer.
 * @param table_index Index of the table containing this timer.
 */
void Script::remove_timer(Timer* timer, int table_index) {

  const void* table_pointer = lua_topointer(l, table_index);
  if (!timer->is_finished()) {
    destroy_ref(table_timers[table_pointer][timer]);
  }
  table_timers[table_pointer].erase(timer);
  decrement_refcount(timer); // will be deleted if Lua has already collected it
}

/**
 * @brief Unregisters all timers associated to a table.
 * @param table_index Index of a table.
 */
void Script::remove_timers(int table_index) {

  const void* table_pointer = lua_topointer(l, table_index);
  std::map<Timer*, int>::iterator it;
  for (it = table_timers[table_pointer].begin(); it != table_timers[table_pointer].end(); it++) {
    Timer* timer = it->first;
    if (!timer->is_finished()) {
      destroy_ref(it->second);
    }
    decrement_refcount(timer);
  }
  table_timers[table_pointer].clear();
}

/**
 * @brief Unregisters all existing timers.
 */
void Script::remove_timers() {

  std::map<const void*, std::map<Timer*, int> >::iterator it;
  for (it = table_timers.begin(); it != table_timers.end(); ++it) {

    std::map<Timer*, int> timers = it->second;

    std::map<Timer*, int>::iterator it2;
    for (it2 = timers.begin(); it2 != timers.end(); ++it2) {
      Timer* timer = it2->first;
      if (!timer->is_finished()) {
        destroy_ref(it2->second);
      }
      decrement_refcount(timer);
    }
    timers.clear();
  }
  table_timers.clear();
}

/**
 * @brief Updates all timers currently running for this script.
 */
void Script::update_table_timers() {

  std::map<const void*, std::map<Timer*, int> >::iterator it;
  for (it = table_timers.begin(); it != table_timers.end(); ++it) {

    const void* table_pointer = it->first;
    std::map<Timer*, int> timers = it->second;

    std::map<Timer*, int>::iterator it2;
    for (it2 = timers.begin(); it2 != timers.end(); ++it2) {
      Timer* timer = it2->first;
      int callback_ref = it2->second;

      timer->update();
      if (timer->is_finished()) {
        do_callback(callback_ref);
        lua_pushlightuserdata(l, (void*) table_pointer);
        remove_timer(timer, -1);
        lua_pop(l, 1);
        break;
      }
    }
  }
}

/**
 * @brief Implementation of \ref lua_api_timer_start_timer.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int Script::timer_api_start_timer(lua_State *l) {

  // parameters: delay [with_sound] callback
  LuaContext& lua_context = (LuaContext&) get_script(l);

  luaL_checktype(l, 1, LUA_TTABLE);
  uint32_t delay = luaL_checkinteger(l, 2);
  bool with_sound = false;
  int index = 3;
  if (lua_isboolean(l, index)) {
    with_sound = lua_toboolean(l, index);
    index++;
  }
  luaL_checktype(l, index, LUA_TFUNCTION);
  lua_settop(l, index); // make sure the function is on top of the stack

  if (delay > 0) {
    // create the timer
    Timer* timer = new Timer(delay, with_sound);
    lua_context.add_timer(timer, 1, index);
    push_timer(l, *timer);
  }
  else {
    // The delay is zero: call the function right now.
    lua_context.call_function(0, 0, "timer callback");
  }

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_timer_stop.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int Script::timer_api_stop(lua_State* l) {

  Script& script = get_script(l);

  if (lua_type(l, 1) == LUA_TTABLE) {
    // Stop a list of timers.
    // TODO: deprecate this, it's useless since timers are associated to a table.
    lua_pushnil(l); // first key
    while (lua_next(l, 1) != 0) {

      Timer& timer = check_timer(l, -1);
      script.remove_timer(&timer);
      lua_pop(l, 1); // pop the value, let the key for the iteration
    }
  }
  else {
    // Stop an individual timer.
    Timer& timer = check_timer(l, 1);
    script.remove_timer(&timer);
  }

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_timer_stop_timers.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int Script::timer_api_stop_timers(lua_State *l) {

  LuaContext& lua_context = (LuaContext&) get_script(l);

  luaL_checktype(l, 1, LUA_TTABLE);

  lua_context.remove_timers(1);

  return 0;
}

