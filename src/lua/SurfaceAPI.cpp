/*
 * Copyright (C) 2006-2013 Christopho, Solarus - http://www.solarus-games.org
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
#include "lowlevel/VideoManager.h"
#include "movements/Movement.h"
#include "Sprite.h"
#include "TransitionFade.h"

const std::string LuaContext::surface_module_name = "sol.surface";

/**
 * \brief Initializes the surface features provided to Lua.
 */
void LuaContext::register_surface_module() {

  static const luaL_Reg methods[] = {
      { "create", surface_api_create },
      { "get_size", surface_api_get_size },
      { "fill_color", surface_api_fill_color },
      { "get_transparency_color", surface_api_get_transparency_color },
      { "set_transparency_color", surface_api_set_transparency_color },
      { "set_opacity", surface_api_set_opacity },
      { "draw", drawable_api_draw },
      { "draw_region", drawable_api_draw_region },
      { "fade_in", drawable_api_fade_in },
      { "fade_out", drawable_api_fade_out },
      { "get_xy", drawable_api_get_xy },
      { "set_xy", drawable_api_set_xy },
      { "get_movement", drawable_api_get_movement },
      { "stop_movement", drawable_api_stop_movement },
      { NULL, NULL }
  };
  static const luaL_Reg metamethods[] = {
      { "__gc", drawable_meta_gc },
      { NULL, NULL }
  };
  register_type(surface_module_name, methods, metamethods);
}

/**
 * \brief Returns whether a value is a userdata of type surface.
 * \param l A Lua context.
 * \param index An index in the stack.
 * \return true if the value at this index is a surface.
 */
bool LuaContext::is_surface(lua_State* l, int index) {
  return is_userdata(l, index, surface_module_name);
}

/**
 * \brief Checks that the userdata at the specified index of the stack is a
 * surface and returns it.
 * \param l a Lua context
 * \param index an index in the stack
 * \return the surface
 */
Surface& LuaContext::check_surface(lua_State* l, int index) {
  return static_cast<Surface&>(check_userdata(l, index, surface_module_name));
}

/**
 * \brief Pushes a surface userdata onto the stack.
 * \param l a Lua context
 * \param surface a surface
 */
void LuaContext::push_surface(lua_State* l, Surface& surface) {
  push_userdata(l, surface);
}

/**
 * \brief Implementation of sol.surface.create().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::surface_api_create(lua_State* l) {

  Surface* surface = NULL;
  if (lua_gettop(l) == 0) {
    // create an empty surface with the screen size
    surface = new Surface(VideoManager::get_instance()->get_quest_size());
  }
  else if (lua_type(l, 1) == LUA_TNUMBER) {
    // create an empty surface with the specified size
    int width = luaL_checkint(l, 1);
    int height = luaL_checkint(l, 2);
    surface = new Surface(width, height);
  }
  else if (lua_type(l, 1) == LUA_TSTRING) {
    // load from a file
    const std::string& file_name = lua_tostring(l, 1);
    bool language_specific = lua_toboolean(l, 2); // default is false
    surface = Surface::create_from_file(file_name, language_specific ?
        Surface::DIR_LANGUAGE : Surface::DIR_SPRITES);
  }
  else {
    luaL_typerror(l, 1, "number, string or no value");
  }

  if (surface == NULL) {
    // Image file not found or not valid.
    lua_pushnil(l);
  }
  else {
    get_lua_context(l).add_drawable(surface);
    push_surface(l, *surface);
  }
  return 1;
}

/**
 * \brief Implementation of surface:get_size().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::surface_api_get_size(lua_State* l) {

  Surface& surface = check_surface(l, 1);

  lua_pushinteger(l, surface.get_width());
  lua_pushinteger(l, surface.get_height());

  return 2;
}

/**
 * \brief Implementation of surface:fill_color().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::surface_api_fill_color(lua_State* l) {

  Surface& surface = check_surface(l, 1);
  Color color = check_color(l, 2);

  if (lua_gettop(l) >= 3) {
    int x = luaL_checkint(l, 3);
    int y = luaL_checkint(l, 4);
    int width = luaL_checkint(l, 5);
    int height = luaL_checkint(l, 6);
    Rectangle where(x, y, width, height);
    surface.fill_with_color(color, where);
  }
  else {
    surface.fill_with_color(color);
  }

  return 0;
}

/**
 * \brief Implementation of surface:get_transparency_color().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::surface_api_get_transparency_color(lua_State* l) {

  Surface& surface = check_surface(l, 1);

  const Color& color = surface.get_transparency_color();

  push_color(l, color);
  return 1;
}

/**
 * \brief Implementation of surface:set_transparency_color().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::surface_api_set_transparency_color(lua_State* l) {

  Surface& surface = check_surface(l, 1);
  Color color = check_color(l, 2);
  surface.set_transparency_color(color);

  return 0;
}

/**
 * \brief Implementation of surface:set_opacity().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::surface_api_set_opacity(lua_State* l) {

  Surface& surface = check_surface(l, 1);
  int opacity = luaL_checkint(l, 2);

  surface.set_opacity(opacity);

  return 0;
}

