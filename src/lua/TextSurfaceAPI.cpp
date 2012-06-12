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
#include "lua/Script.h"
#include "lowlevel/TextSurface.h"
#include "lowlevel/StringConcat.h"
#include "StringResource.h"

const std::string Script::text_surface_module_name = "sol.text_surface";

static const char* rendering_mode_names[] = {
    "solid", "shaded", "blended", NULL
};

static const char* horizontal_alignment_names[] = {
    "left", "center", "right", NULL
};

static const char* vertical_alignment_names[] = {
    "top", "middle", "bottom", NULL
};

/**
 * @brief Initializes the text surface features provided to Lua.
 */
void Script::register_text_surface_module() {

  static const luaL_Reg methods[] = {
      { "create", text_surface_api_create },
      { "get_text", text_surface_api_get_text },
      { "set_text", text_surface_api_set_text },
      { "set_text_key", text_surface_api_set_text_key },
      { "start_movement", displayable_api_start_movement },
      { "stop_movement", displayable_api_stop_movement },
      { NULL, NULL }
  };
  static const luaL_Reg metamethods[] = {
      { "__eq", userdata_meta_eq },
      { "__gc", displayable_meta_gc },
      { NULL, NULL }
  };
  register_type(text_surface_module_name, methods, metamethods);
}

/**
 * @brief Checks that the userdata at the specified index of the stack is a
 * text surface and returns it.
 * @param l a Lua context
 * @param index an index in the stack
 * @return the text surface
 */
TextSurface& Script::check_text_surface(lua_State* l, int index) {
  return static_cast<TextSurface&>(
      check_userdata(l, index, text_surface_module_name));
}

/**
 * @brief Pushes a text surface userdata onto the stack.
 * @param l a Lua context
 * @param text_surface a text surface
 */
void Script::push_text_surface(lua_State* l, TextSurface& text_surface) {
  push_userdata(l, text_surface);
}

/**
 * @brief Implementation of \ref lua_api_text_surface_create.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int Script::text_surface_api_create(lua_State* l) {

  TextSurface* text_surface = new TextSurface(0, 0);

  if (lua_gettop(l) > 0) {
    luaL_checktype(l, 1, LUA_TTABLE);

    // Traverse the table, looking for properties.
    lua_pushnil(l); // First key.
    while (lua_next(l, 1) != 0) {

      const std::string& key = luaL_checkstring(l, 2);
      if (key == "font") {
        const std::string& font_id = luaL_checkstring(l, 3);
        text_surface->set_font(font_id);
      }
      else if (key == "rendering_mode") {
        int mode = luaL_checkoption(l, 3, NULL, rendering_mode_names);
        text_surface->set_rendering_mode(TextSurface::RenderingMode(mode));
      }
      else if (key == "horizontal_alignment") {
        int alignment = luaL_checkoption(l, 3, NULL, horizontal_alignment_names);
        text_surface->set_horizontal_alignment(
            TextSurface::HorizontalAlignment(alignment));
      }
      else if (key == "vertical_alignment") {
        int alignment = luaL_checkoption(l, 3, NULL, vertical_alignment_names);
        text_surface->set_vertical_alignment(
            TextSurface::VerticalAlignment(alignment));
      }
      else if (key == "background_color") {
        Color color = check_color(l, 3);
        text_surface->set_background_color(color);
      }
      else if (key == "text_color") {
        Color color = check_color(l, 3);
        text_surface->set_text_color(color);
      }
      else if (key == "text") {
        const std::string& text = luaL_checkstring(l, 3);
        text_surface->set_text(text);
      }
      else if (key == "text_key") {
        const std::string& text_key = luaL_checkstring(l, 3);
        text_surface->set_text(StringResource::get_string(text_key));
      }
      else {
        luaL_error(l, (StringConcat() << "Invalid key '" << key
            << "' for text surface properties").c_str());
      }
      lua_pop(l, 1); // Pop the value, let the key for the iteration.
    }
  }
  get_script(l).add_displayable(text_surface);

  push_text_surface(l, *text_surface);
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_text_surface_get_horizontal_alignment.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int Script::text_surface_api_get_horizontal_alignment(lua_State* l) {

  TextSurface& text_surface = check_text_surface(l, 1);

  TextSurface::HorizontalAlignment align = text_surface.get_horizontal_alignment();

  lua_pushstring(l, horizontal_alignment_names[align]);
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_text_surface_set_horizontal_alignment.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int Script::text_surface_api_set_horizontal_alignment(lua_State* l) {

  TextSurface& text_surface = check_text_surface(l, 1);
  int align = luaL_checkoption(l, 1, NULL, horizontal_alignment_names);

  text_surface.set_horizontal_alignment(TextSurface::HorizontalAlignment(align));

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_text_surface_get_vertical_alignment.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int Script::text_surface_api_get_vertical_alignment(lua_State* l) {

  TextSurface& text_surface = check_text_surface(l, 1);

  TextSurface::VerticalAlignment align = text_surface.get_vertical_alignment();

  lua_pushstring(l, vertical_alignment_names[align]);
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_text_surface_set_vertical_alignment.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int Script::text_surface_api_set_vertical_alignment(lua_State* l) {

  TextSurface& text_surface = check_text_surface(l, 1);
  int align = luaL_checkoption(l, 1, NULL, vertical_alignment_names);

  text_surface.set_vertical_alignment(TextSurface::VerticalAlignment(align));

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_text_surface_get_font.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int Script::text_surface_api_get_font(lua_State* l) {

  TextSurface& text_surface = check_text_surface(l, 1);

  const std::string& font_id = text_surface.get_font();
  lua_pushstring(l, font_id.c_str());

  return 1;
}

/**
 * @brief Implementation of \ref lua_api_text_surface_set_text.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int Script::text_surface_api_set_font(lua_State* l) {

  TextSurface& text_surface = check_text_surface(l, 1);
  const std::string& font_id= luaL_checkstring(l, 2);

  text_surface.set_font(font_id);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_text_surface_get_rendering_mode.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int Script::text_surface_api_get_rendering_mode(lua_State* l) {

  TextSurface& text_surface = check_text_surface(l, 1);

  TextSurface::RenderingMode mode = text_surface.get_rendering_mode();

  lua_pushstring(l, rendering_mode_names[mode]);
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_text_surface_set_rendering_mode.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int Script::text_surface_api_set_rendering_mode(lua_State* l) {

  TextSurface& text_surface = check_text_surface(l, 1);
  int mode = luaL_checkoption(l, 1, NULL, rendering_mode_names);

  text_surface.set_rendering_mode(TextSurface::RenderingMode(mode));

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_text_surface_get_background_color.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int Script::text_surface_api_get_background_color(lua_State* l) {

  TextSurface& text_surface = check_text_surface(l, 1);

  const Color& background_color = text_surface.get_background_color();

  push_color(l, background_color);
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_text_surface_set_background_color.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int Script::text_surface_api_set_background_color(lua_State* l) {

  TextSurface& text_surface = check_text_surface(l, 1);
  const Color& background_color = check_color(l, 2);

  text_surface.set_background_color(background_color);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_text_surface_get_text_color.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int Script::text_surface_api_get_text_color(lua_State* l) {

  TextSurface& text_surface = check_text_surface(l, 1);

  const Color& text_color = text_surface.get_text_color();

  push_color(l, text_color);
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_text_surface_set_text_color.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int Script::text_surface_api_set_text_color(lua_State* l) {

  TextSurface& text_surface = check_text_surface(l, 1);
  const Color& text_color = check_color(l, 2);

  text_surface.set_text_color(text_color);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_text_surface_get_text.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int Script::text_surface_api_get_text(lua_State* l) {

  TextSurface& text_surface = check_text_surface(l, 1);

  const std::string& text = text_surface.get_text();

  lua_pushstring(l, text.c_str());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_text_surface_set_text.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int Script::text_surface_api_set_text(lua_State* l) {

  TextSurface& text_surface = check_text_surface(l, 1);
  const std::string& text = luaL_checkstring(l, 2);

  text_surface.set_text(text);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_text_surface_set_text_key.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int Script::text_surface_api_set_text_key(lua_State* l) {

  TextSurface& text_surface = check_text_surface(l, 1);
  const std::string& key = luaL_checkstring(l, 2);

  text_surface.set_text(StringResource::get_string(key));

  return 0;
}

