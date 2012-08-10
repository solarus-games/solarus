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
#include "DynamicDisplayable.h"
#include "TransitionFade.h"
#include <lua.hpp>

/* This file contains common code for all displayable types known by Lua,
 * i.e. surfaces, text surfaces and sprites.
 */

/**
 * @brief Check that the userdata at the specified index is a displayable
 * object (surface, text surface of sprite) and returns it.
 * @param l a Lua context
 * @param index an index in the stack
 * @return the surface
 */
DynamicDisplayable& LuaContext::check_displayable(lua_State* l, int index) {

  DynamicDisplayable** displayable = NULL;

  if (is_userdata(l, index, surface_module_name)
      || is_userdata(l, index, text_surface_module_name)
      || is_userdata(l, index, sprite_module_name)) {
    displayable = static_cast<DynamicDisplayable**>(lua_touserdata(l, index));
  }
  else {
    luaL_typerror(l, index, "displayable");
  }

  return **displayable;
}

/**
 * @brief Registers a displayable object created by this script.
 * @param displayable a displayable object
 */
void LuaContext::add_displayable(DynamicDisplayable* displayable) {

  displayables.insert(displayable);
}

/**
 * @brief Unregisters a displayable object created by this script.
 * @param displayable a displayable object
 */
void LuaContext::remove_displayable(DynamicDisplayable* displayable) {

  displayables.erase(displayable);
}

/**
 * @brief Updates all displayable objects created by this script.
 */
void LuaContext::update_displayables() {

  std::set<DynamicDisplayable*>::iterator it;
  for (it = displayables.begin(); it != displayables.end(); it++) {
    (*it)->update();
  }
}

/**
 * @brief Implementation of \ref lua_api_displayable_draw.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::displayable_api_draw(lua_State* l) {

  DynamicDisplayable& displayable = check_displayable(l, 1);
  Surface& dst_surface = check_surface(l, 2);
  int x = luaL_optinteger(l, 3, 0);
  int y = luaL_optinteger(l, 4, 0);
  displayable.display(dst_surface, x, y);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_displayable_fade_in.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::displayable_api_fade_in(lua_State* l) {

  uint32_t delay = 20;
  int callback_ref = LUA_REFNIL;

  DynamicDisplayable& displayable = check_displayable(l, 1);

  if (lua_gettop(l) >= 2) {
    // the second argument can be the delay or the callback
    int index = 2;
    if (lua_isnumber(l, index)) {
      delay = lua_tonumber(l, index);
      index++;
    }
    // the next argument (if any) is the callback
    if (lua_gettop(l) >= index) {
      luaL_checktype(l, index, LUA_TFUNCTION);
      lua_settop(l, index);
      callback_ref = luaL_ref(l, LUA_REGISTRYINDEX);
    }
  }

  TransitionFade* transition = new TransitionFade(Transition::IN);
  transition->set_delay(delay);
  displayable.start_transition(*transition, callback_ref);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_displayable_fade_out.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::displayable_api_fade_out(lua_State* l) {

  uint32_t delay = 20;
  int callback_ref = LUA_REFNIL;

  DynamicDisplayable& displayable = check_displayable(l, 1);

  if (lua_gettop(l) >= 2) {
    // the second argument can be the delay or the callback
    int index = 2;
    if (lua_isnumber(l, index)) {
      delay = lua_tonumber(l, index);
      index++;
    }
    // the next argument (if any) is the callback
    if (lua_gettop(l) >= index) {
      luaL_checktype(l, index, LUA_TFUNCTION);
      lua_settop(l, index);
      callback_ref = luaL_ref(l, LUA_REGISTRYINDEX);
    }
  }

  TransitionFade* transition = new TransitionFade(Transition::OUT);
  transition->set_delay(delay);
  displayable.start_transition(*transition, callback_ref);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_displayable_start_movement.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::displayable_api_start_movement(lua_State* l) {

  DynamicDisplayable& displayable = check_displayable(l, 1);
  Movement& movement = check_movement(l, 2);

  // the next argument (if any) is the callback
  bool callback = lua_gettop(l) >= 3;

  int callback_ref = LUA_REFNIL;
  if (callback) {
    luaL_checktype(l, 3, LUA_TFUNCTION);
    lua_settop(l, 3);
    callback_ref = luaL_ref(l, LUA_REGISTRYINDEX);
  }

  displayable.start_movement(movement, callback_ref);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_displayable_stop_movement.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::displayable_api_stop_movement(lua_State* l) {

  DynamicDisplayable& displayable = check_displayable(l, 1);

  displayable.stop_movement();

  return 0;
}

/**
 * @brief Finalizer of types sprite, surface and text surface.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::displayable_meta_gc(lua_State* l) {

  LuaContext& lua_context = get_lua_context(l);
  DynamicDisplayable& displayable = check_displayable(l, 1);

  lua_context.remove_displayable(&displayable);
  userdata_meta_gc(l);

  return 0;
}

