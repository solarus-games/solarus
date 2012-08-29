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
#include <lua.hpp>
#include "lua/LuaContext.h"
#include "lowlevel/Color.h"
#include "lowlevel/Surface.h"
#include "movements/Movement.h"
#include "Sprite.h"
#include "TransitionFade.h"

const std::string LuaContext::surface_module_name = "sol.surface";

/**
 * @brief Initializes the surface features provided to Lua.
 */
void LuaContext::register_surface_module() {

  static const luaL_Reg methods[] = {
      { "create", surface_api_create },
      { "get_size", surface_api_get_size },
      { "fill_color", surface_api_fill_color },
      { "set_transparency_color", surface_api_set_transparency_color },
      { "set_opacity", surface_api_set_opacity },
      { "draw", drawable_api_draw },
      { "fade_in", drawable_api_fade_in },
      { "fade_out", drawable_api_fade_out },
      { "start_movement", drawable_api_start_movement },
      { "stop_movement", drawable_api_stop_movement },
      { NULL, NULL }
  };
  static const luaL_Reg metamethods[] = {
      { "__eq", userdata_meta_eq },
      { "__gc", drawable_meta_gc },
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
Surface& LuaContext::check_surface(lua_State* l, int index) {
  return static_cast<Surface&>(check_userdata(l, index, surface_module_name));
}

/**
 * @brief Pushes a surface userdata onto the stack.
 * @param l a Lua context
 * @param surface a surface
 */
void LuaContext::push_surface(lua_State* l, Surface& surface) {
  push_userdata(l, surface);
}

/**
 * @brief Implementation of \ref lua_api_surface_create_empty,
 * \ref lua_api_surface_create_from_file and
 * \ref lua_api_surface_create_from_surface.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::surface_api_create(lua_State* l) {

  Surface* surface;
  if (lua_gettop(l) == 0) {
    // create an empty surface with the screen size
    surface = new Surface();
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
    other_surface.draw_region(Rectangle(x, y, width, height), *surface);
  }

  get_lua_context(l).add_drawable(surface);
  push_surface(l, *surface);

  return 1;
}

/**
 * @brief Implementation of \ref lua_api_surface_get_size.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::surface_api_get_size(lua_State* l) {

  Surface& surface = check_surface(l, 1);

  lua_pushinteger(l, surface.get_width());
  lua_pushinteger(l, surface.get_height());

  return 2;
}

/**
 * @brief Implementation of \ref lua_api_surface_fill_color.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::surface_api_fill_color(lua_State* l) {

  Surface& surface = check_surface(l, 1);
  Color color = check_color(l, 2);

  if (lua_gettop(l) >= 3) {
    int x = luaL_checkinteger(l, 3);
    int y = luaL_checkinteger(l, 4);
    int width = luaL_checkinteger(l, 5);
    int height = luaL_checkinteger(l, 6);
    Rectangle where(x, y, width, height);
    surface.fill_with_color(color, where);
  }
  else {
    surface.fill_with_color(color);
  }

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_surface_set_transparency_color.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::surface_api_set_transparency_color(lua_State* l) {

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
int LuaContext::surface_api_set_opacity(lua_State* l) {

  Surface& surface = check_surface(l, 1);
  int opacity = luaL_checkinteger(l, 2);

  surface.set_opacity(opacity);

  return 0;
}

