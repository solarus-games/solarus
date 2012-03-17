/*
 * Copyright (C) 2006-2012 Christopho, Solarus - http://www.solarus-engine.org
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
#include "lua/Script.h"
#include "Timer.h"
#include "Game.h"
#include <lua.hpp>

const char* Script::timer_module_name = "sol.timer";

/**
 * @brief Initializes the timer features provided to Lua.
 */
void Script::initialize_timer_module() {

  static const luaL_Reg methods[] = {
      { "start", timer_api_start },
      { "stop", timer_api_stop },
      { "stop_all", timer_api_stop_all},
      { NULL, NULL }
  };

  static const luaL_Reg metamethods[] = {
      { "__gc", timer_meta_gc },
      { NULL, NULL }
  };

  // create a table and fill it with the methods
  luaL_register(l, timer_module_name, methods);
                                  // sol.timer

  // create the metatable for the type, add it to the Lua registry
  luaL_newmetatable(l, timer_module_name);
                                  // sol.timer mt
  // fill the metatable
  luaL_register(l, NULL, metamethods);
                                  // sol.timer mt
  lua_pushvalue(l, -2);
                                  // sol.timer mt sol.timer
  // metatable.__index = sol.timer
  lua_setfield(l, -2, "__index");
                                  // sol.timer mt
  lua_pop(l, 2);
                                  // --
}

/**
 * @brief Registers a timer created by this script.
 * @param timer a timer
 * @param callback_ref registry ref of a function to call when the timer
 * finishes
 */
void Script::add_timer(Timer* timer, int callback_ref) {

  set_created(timer);
  timers[timer] = callback_ref;

  if (is_new_timer_suspended()) {
    timer->set_suspended(true);
  }
  increment_refcount(timer);
}

/**
 * @brief Unregisters a timer created by this script.
 * @param timer a timer
 */
void Script::remove_timer(Timer* timer) {

  if (!timer->is_finished()) {
    luaL_unref(l, LUA_REGISTRYINDEX, timers[timer]);
  }
  timers.erase(timer);
  decrement_refcount(timer); // will be deleted if Lua has already collected it
}

/**
 * @brief Updates all timers currently running for this script.
 */
void Script::update_timers() {

  std::map<Timer*, int>::iterator it;
  for (it = timers.begin(); it != timers.end(); ++it) {

    Timer* timer = it->first;
    int callback_ref = it->second;

    timer->update();
    if (timer->is_finished()) {
      do_callback(callback_ref);
      remove_timer(timer);
      break;
    }
  }
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

  if (index < 0) {
    // ensure a positive index
    index = lua_gettop(l) + index + 1;
  }

  Timer** timer =
    (Timer**) luaL_checkudata(l, index, timer_module_name);
  return **timer;
}

/**
 * @brief Pushes a timer userdata onto the stack.
 * @param l a Lua context
 * @param timer a timer
 */
void Script::push_timer(lua_State* l, Timer& timer) {

  Script& script = get_script(l);

  script.increment_refcount(&timer);
                                  // ...
  Timer** block_adress =
    (Timer**) lua_newuserdata(l, sizeof(Timer*));
  *block_adress = &timer;
                                  // ... timer
  luaL_getmetatable(l, timer_module_name);
                                  // ... timer mt
  lua_setmetatable(l, -2);
                                  // ... timer
}

/**
 * @brief Finalizes a timer.
 *
 * - Argument 1 (timer): a timer
 *
 * @param l a Lua state
 * @return number of values to return to Lua
 */
int Script::timer_meta_gc(lua_State* l) {

  Script& script = get_script(l);
  Timer& timer = check_timer(l, 1);

  script.decrement_refcount(&timer);

  return 0;
}

/**
 * @brief Creates a timer.
 *
 * - Argument 1 (number): the timer duration in milliseconds
 * - Optional argument 2 (boolean): plays a clock sound until the timer
 * finishes (default is false)
 * - Argument 3 (function): a Lua function to call (without arguments)
 * when the timer finishes
 * - Return value (timer): the timer created
 *
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int Script::timer_api_start(lua_State *l) {

  // get the parameters
  Script& script = get_script(l);
  uint32_t delay = luaL_checkinteger(l, 1);
  bool with_sound = false;
  int index = 2;
  if (lua_isboolean(l, index)) {
    with_sound = lua_toboolean(l, 2);
    index++;
  }
  luaL_checktype(l, index, LUA_TFUNCTION);
  lua_settop(l, index); // make sure the function is on top of the stack

  // create the timer
  int callback_ref = luaL_ref(l, LUA_REGISTRYINDEX);
  Timer* timer = new Timer(delay, with_sound);
  script.add_timer(timer, callback_ref);
  push_timer(l, *timer);

  return 0;
}

/**
 * @brief Cancels a timer.
 *
 * Argument 1 (timer): the timer to cancel
 *
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
 * @brief Stops a list of timers.
 *
 * Argument 1 (table): a table whose values are timers
 *
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int Script::timer_api_stop_all(lua_State *l) {

  Script& script = get_script(l);
  luaL_checktype(l, 1, LUA_TTABLE);

  // traverse the table
  lua_pushnil(l); // first key
  while (lua_next(l, 1) != 0) {

    Timer& timer = check_timer(l, -1);
    script.remove_timer(&timer);
    lua_pop(l, 1); // pop the value, let the key for the iteration
  }

  return 0;
}

