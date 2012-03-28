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
#include <lua.hpp>
#include "lua/Script.h"
#include "lowlevel/Color.h"
#include "lowlevel/Surface.h"
#include "movements/Movement.h"
#include "Sprite.h"
#include "TransitionFade.h"
#include <iostream>

const std::string Script::surface_module_name = "sol.surface";

/**
 * @brief Initializes the surface features provided to Lua.
 */
void Script::register_surface_module() {

  static const luaL_Reg methods[] = {
      { "create", surface_api_create },
      { "get_size", surface_api_get_size },
      { "fill_color", surface_api_fill_color },
      { "draw", surface_api_draw },
      { "set_transparency_color", surface_api_set_transparency_color },
      { "set_opacity", surface_api_set_opacity },
      { "fade_in", surface_api_fade_in },
      { "fade_out", surface_api_fade_out },
      { "start_movement", displayable_api_start_movement },
      { "stop_movement", displayable_api_stop_movement },
      { NULL, NULL }
  };
  static const luaL_Reg metamethods[] = {
      { "__gc", displayable_meta_gc },
      { NULL, NULL }
  };
  register_type(surface_module_name, methods, metamethods);
}

/**
 * @brief Checks that the userdata at the specified index of the stack is a
 * surface and returns it.
 * @param l a Lua context
 * @param index an index in the stack
 * @return the surface
 */
Surface& Script::check_surface(lua_State* l, int index) {
  return static_cast<Surface&>(check_userdata(l, index, surface_module_name));
}

/**
 * @brief Pushes a surface userdata onto the stack.
 * @param l a Lua context
 * @param surface a surface
 */
void Script::push_surface(lua_State* l, Surface& surface) {
  push_userdata(l, surface);
}

/**
 * @brief Creates and returns a surface.
 *
 * To create an empty surface:
 * - Optional argument 1 (integer): width in pixels
 * - Optional argument 2 (integer): height in pixels
 * The default size is the size of the screen.
 *
 * To create a surface from an image file:
 * - Argument 1 (string): name of the image file to load
 * - Optional argument 2 (boolean, default false): true to load a
 * language-specific image
 *
 * To create a surface from a region of an existing surface:
 * - Argument 1 (surface): an existing surface
 * - Argument 2 (integer): x coordinate of the rectangle
 * - Argument 3 (integer): y coordinate of the rectangle
 * - Argument 4 (integer): width of the rectangle
 * - Argument 5 (integer): height of the rectangle
 *
 * @param l the Lua context that is calling this function
 * @return the number of values to return to Lua
 */
int Script::surface_api_create(lua_State* l) {

  Surface* surface;
  if (lua_gettop(l) == 0) {
    // no arguments: create an empty surface with the screen size
    surface = new Surface(320, 240);
  }
  else if (lua_type(l, 1) == LUA_TNUMBER) {
    int width = luaL_checkinteger(l, 1);
    int height = luaL_checkinteger(l, 2);
    surface = new Surface(width, height);
  }
  else if (lua_type(l, 1) == LUA_TSTRING) {
    const std::string& file_name = lua_tostring(l, 1);
    bool language_specific = false;
    if (lua_gettop(l) >= 2) {
      language_specific = lua_toboolean(l, 2);
    }
    surface = new Surface(file_name, language_specific ?
        Surface::DIR_LANGUAGE : Surface::DIR_SPRITES);
  }
  else {
    Surface& other_surface = check_surface(l, 1);
    int x = 0;
    int y = 0;
    int width = other_surface.get_width();
    int height = other_surface.get_height();
    if (lua_gettop(l) >= 2) {
      x = luaL_checkinteger(l, 2);
      y = luaL_checkinteger(l, 3);
      width = luaL_checkinteger(l, 4);
      height = luaL_checkinteger(l, 5);
    }
    surface = new Surface(width, height);
    other_surface.display_region(Rectangle(x, y, width, height), *surface);
  }

  get_script(l).add_displayable(surface);
  push_surface(l, *surface);

  return 1;
}

/**
 * @brief Returns the size of a surface.
 *
 * - Argument 1 (surface): a surface
 * - Return value 1 (integer): width in pixels
 * - Return value 2 (integer): height in pixels
 *
 * @param l the Lua context that is calling this function
 * @return the number of values to return to Lua
 */
int Script::surface_api_get_size(lua_State* l) {

  Surface& surface = check_surface(l, 1);

  lua_pushnumber(l, surface.get_width());
  lua_pushnumber(l, surface.get_height());

  return 2;
}

/**
 * @brief Fills a surface with a color.
 *
 * - Argument 1 (surface): the surface to fill
 * - Argument 2 (table): color (array of RGB values from 0 to 255)
 */
int Script::surface_api_fill_color(lua_State* l) {

  Surface& surface = check_surface(l, 1);
  Color color = check_color(l, 2);
  surface.fill_with_color(color);

  return 0;
}

/**
 * @brief Draws something on a surface.
 *
 * - Argument 1 (surface): the destination surface to modify
 * - Argument 2 (surface, text surface or sprite): the object to draw on this surface
 * - Optional argument 3 (integer): x coordinate of where to draw the object
 * - Optional argument 4 (integer): y coordinate of where to draw the object
 *
 * @param l the Lua context that is calling this function
 * @return the number of values to return to Lua
 */
int Script::surface_api_draw(lua_State* l) {

  Surface& dst_surface = check_surface(l, 1);
  DynamicDisplayable& displayable = check_displayable(l, 2);

  int x = 0;
  int y = 0;
  if (lua_gettop(l) >= 3) {
    x = luaL_checkinteger(l, 3);
    y = luaL_checkinteger(l, 4);
    displayable.display(dst_surface, x, y);
  }
  else {
    displayable.display(dst_surface);
  }

  return 0;
}

/**
 * @brief Sets the transparency color of a surface.
 *
 * - Argument 1 (surface): a surface
 * - Argument 2 (table): color (array of RGB values from 0 to 255)
 *
 * @param l the Lua context that is calling this function
 * @return the number of values to return to Lua
 */
int Script::surface_api_set_transparency_color(lua_State* l) {

  Surface& surface = check_surface(l, 1);
  Color color = check_color(l, 2);
  surface.set_transparency_color(color);

  return 0;
}

/**
 * @brief Sets the opacity of a surface.
 *
 * - Argument 1 (surface): the surface to set
 * - Argument 2 (integer): the opacity (0 to 255)
 *
 * @param l the Lua context that is calling this function
 * @return the number of values to return to Lua
 */
int Script::surface_api_set_opacity(lua_State* l) {

  Surface& surface = check_surface(l, 1);
  int opacity = luaL_checkinteger(l, 2);

  surface.set_opacity(opacity);

  return 0;
}

/**
 * @brief Starts playing a fade-in effect on a surface.
 *
 * - Argument 1 (surface): a surface
 * - Optional argument 2 (number): delay in milliseconds between two frames
 * in the fade-in animation (default 20)
 * - Optional argument 3 (function): a Lua function to be called when the
 * fade-in effect finishes (can be the second argument if you don't set a delay)
 *
 * @param l the Lua context that is calling this function
 * @return the number of values to return to Lua
 */
int Script::surface_api_fade_in(lua_State* l) {

  uint32_t delay = 20;
  int callback_ref = LUA_REFNIL;

  Surface& surface = check_surface(l, 1);

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
  surface.start_transition(*transition, callback_ref);

  return 0;
}

/**
 * @brief Starts playing a fade-out effect on a surface.
 *
 * - Argument 1 (surface): a surface
 * - Optional argument 2 (number): delay in milliseconds between two frames
 * in the fade-out animation (default 20)
 * - Optional argument 3 (function): a Lua function to be called when the
 * fade-out effect finishes (can be the second argument if you don't set a delay)
 *
 * @param l the Lua context that is calling this function
 * @return the number of values to return to Lua
 */
int Script::surface_api_fade_out(lua_State* l) {

  uint32_t delay = 20;
  int callback_ref = LUA_REFNIL;

  Surface& surface = check_surface(l, 1);

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
  surface.start_transition(*transition, callback_ref);

  return 0;
}

