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
 * @brief Implementation of \ref lua_api_surface_create.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int Script::surface_api_create(lua_State* l) {

  Surface* surface;
  if (lua_gettop(l) == 0) {
    // create an empty surface with the screen size
    surface = new Surface(320, 240);
  }
  else if (lua_type(l, 1) == LUA_TNUMBER) {
    // create an empty surface with the specified size
    int width = luaL_checkinteger(l, 1);
    int height = luaL_checkinteger(l, 2);
    surface = new Surface(width, height);
  }
  else if (lua_type(l, 1) == LUA_TSTRING) {
    // load from a file
    const std::string& file_name = lua_tostring(l, 1);
    bool language_specific = lua_toboolean(l, 2); // default is false
    surface = new Surface(file_name, language_specific ?
        Surface::DIR_LANGUAGE : Surface::DIR_SPRITES);
  }
  else {
    // create from an existing surface
    Surface& other_surface = check_surface(l, 1);
    int x = luaL_optinteger(l, 2, 0);
    int y = luaL_optinteger(l, 3, 0);
    int width = luaL_optinteger(l, 4, other_surface.get_width());
    int height = luaL_optinteger(l, 5, other_surface.get_height());
    surface = new Surface(width, height);
    other_surface.display_region(Rectangle(x, y, width, height), *surface);
  }

  get_script(l).add_displayable(surface);
  push_surface(l, *surface);

  return 1;
}

/**
 * @brief Implementation of \ref lua_api_surface_get_size.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int Script::surface_api_get_size(lua_State* l) {

  Surface& surface = check_surface(l, 1);

  lua_pushnumber(l, surface.get_width());
  lua_pushnumber(l, surface.get_height());

  return 2;
}

/**
 * @brief Implementation of \ref lua_api_surface_fill_color.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int Script::surface_api_fill_color(lua_State* l) {

  Surface& surface = check_surface(l, 1);
  Color color = check_color(l, 2);
  surface.fill_with_color(color);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_surface_draw.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int Script::surface_api_draw(lua_State* l) {

  Surface& dst_surface = check_surface(l, 1);
  DynamicDisplayable& displayable = check_displayable(l, 2);
  int x = luaL_optinteger(l, 3, 0);
  int y = luaL_optinteger(l, 4, 0);
  displayable.display(dst_surface, x, y);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_surface_set_transparency_color.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int Script::surface_api_set_transparency_color(lua_State* l) {

  Surface& surface = check_surface(l, 1);
  Color color = check_color(l, 2);
  surface.set_transparency_color(color);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_surface_set_opacity.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int Script::surface_api_set_opacity(lua_State* l) {

  Surface& surface = check_surface(l, 1);
  int opacity = luaL_checkinteger(l, 2);

  surface.set_opacity(opacity);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_surface_fade_in.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
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
 * @brief Implementation of \ref lua_api_surface_fade_out.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
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

