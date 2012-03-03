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
#include "DynamicDisplayable.h"
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
DynamicDisplayable& Script::check_displayable(lua_State* l, int index) {

  DynamicDisplayable** displayable = NULL;

  if (is_userdata(l, index, surface_module_name)
      || is_userdata(l, index, text_surface_module_name)
      || is_userdata(l, index, sprite_module_name)) {
    displayable = (DynamicDisplayable**) lua_touserdata(l, index);
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
void Script::add_displayable(DynamicDisplayable* displayable) {

  set_created(displayable);
  displayable->set_script(this);
  displayables.insert(displayable);
}

/**
 * @brief Unregisters a displayable object created by this script.
 * @param displayable a displayable object
 */
void Script::remove_displayable(DynamicDisplayable* displayable) {

  displayables.erase(displayable);
}

/**
 * @brief Updates all displayable objects created by this script.
 */
void Script::update_displayables() {

  std::set<DynamicDisplayable*>::iterator it;
  for (it = displayables.begin(); it != displayables.end(); it++) {
    (*it)->update();
  }
}

/**
 * @brief Starts a movement on a displayable object.
 *
 * - Argument 1 (surface, text surface or sprite): a displayable object
 * - Argument 2 (movement): the movement to apply
 * - Optional argument 3 (function): a Lua function to be called when the
 * movement finishes
 *
 * @param l the Lua context that is calling this function
 * @return the number of values to return to Lua
 */
int Script::displayable_api_start_movement(lua_State* l) {

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
 * @brief Stops the movement (if any) of a displayable object.
 *
 * - Argument 1 (surface, text surface or sprite): a displayable object
 *
 * @param l the Lua context that is calling this function
 * @return the number of values to return to Lua
 */
int Script::displayable_api_stop_movement(lua_State* l) {

  DynamicDisplayable& displayable = check_displayable(l, 1);

  displayable.stop_movement();

  return 0;
}

/**
 * @brief Finalizes a displayable object.
 *
 * - Argument 1 (surface, text surface or sprite): a displayable object
 *
 * @param l a Lua state
 * @return number of values to return to Lua
 */
int Script::displayable_meta_gc(lua_State* l) {

  Script& script = get_script(l);

  DynamicDisplayable& displayable = check_displayable(l, 1);

  if (script.has_created(&displayable)) {
    // the object was created by the script
    script.remove_displayable(&displayable);
    script.decrement_refcount(&displayable);
  }

  return 0;
}

