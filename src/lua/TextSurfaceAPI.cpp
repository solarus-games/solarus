/*
 * Copyright (C) 2009-2011 Christopho, Solarus - http://www.solarus-engine.org
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

const char* Script::text_surface_module_name = "sol.text_surface";

/**
 * @brief Initializes the text surface features provided to Lua.
 */
void Script::initialize_text_surface_module() {

  static const luaL_Reg methods[] = {
      { "create", text_surface_api_create },
      { "get_text", text_surface_api_get_text },
      { "set_text", text_surface_api_set_text },
      { NULL, NULL }
  };

  static const luaL_Reg meta_methods[] = {
      { "__gc", text_surface_meta_gc },
      { NULL, NULL }
  };

  // create a table and fill it with the methods
  luaL_openlib(l, text_surface_module_name, methods, 0);
                                  // sol.text_surface module

  // create the metatable for the type, add it to the Lua registry
  luaL_newmetatable(l, text_surface_module_name);
                                  // sol.text_surface mt
  // fill the metatable
  luaL_openlib(l, NULL, meta_methods, 0);
                                  // sol.text_surface mt
  lua_pushvalue(l, -2);
                                  // sol.sol.text_surface mt sol.sol.text_surface
  // metatable.__index = sol.sol.text_surface
  lua_setfield(l, -2, "__index");
                                  // sol.sol.text_surface mt
  lua_pop(l, 2);
                                  // --
}

/**
 * @brief Checks that the userdata at the specified index of the stack is a
 * text surface and returns it.
 * @param l a Lua context
 * @param index an index in the stack
 * @return the text surface
 */
TextSurface& Script::check_text_surface(lua_State* l, int index) {

  if (index < 0) {
    // ensure a positive index
    index = lua_gettop(l) + index + 1;
  }

  TextSurface** surface = (TextSurface**)
      luaL_checkudata(l, index, text_surface_module_name);
  return **surface;
}

/**
 * @brief Pushes a text surface userdata onto the stack.
 * @param text_surface a text surface
 */
void Script::push_text_surface(lua_State* l, TextSurface& text_surface) {

                                  // ...
  TextSurface** block_adress = (TextSurface**)
      lua_newuserdata(l, sizeof(TextSurface*));
  *block_adress = &text_surface;
                                  // ... text_surface
  luaL_getmetatable(l, text_surface_module_name);
                                  // ... text_surface mt
  lua_setmetatable(l, -2);
                                  // ... text_surface
}

/**
 * @brief Creates and returns a text surface.
 *
 * To create an empty surface:
 * - Argument 1 (table): properties, possible keys are
 * x, y, horizontal_alignment, vertical_alignment, font, rendering_mode
 * background_color, text_color, text, text_key
 *
 * @param l the Lua context that is calling this function
 * @return the number of values to return to Lua
 */
int Script::text_surface_api_create(lua_State* l) {

  Script& script = get_script(l);

  luaL_checktype(l, 1, LUA_TTABLE);

  TextSurface* text_surface = new TextSurface(0, 0);
  int x = 0;
  int y = 0;

  // traverse the table, looking for properties
  lua_pushnil(l); // first key
  while (lua_next(l, 1) != 0) {

    const std::string& key = luaL_checkstring(l, 2);
    if (key == "x") {
      x = luaL_checkinteger(l, -1);
    }
    else if (key == "y") {
      y = luaL_checkinteger(l, -1);
    }
    else if (key == "font") {
      const std::string& font_id = luaL_checkstring(l, -1);
      text_surface->set_font(font_id);
    }
    else if (key == "rendering_mode") {
      const std::string& rendering_mode_name = luaL_checkstring(l, -1);
      TextSurface::RenderingMode rendering_mode = TextSurface::TEXT_SOLID;
      if (rendering_mode_name == "solid") {
        rendering_mode = TextSurface::TEXT_SOLID;
      }
      else if (rendering_mode_name == "shaded") {
        rendering_mode = TextSurface::TEXT_SHADED;
      }
      else if (rendering_mode_name == "blended") {
        rendering_mode = TextSurface::TEXT_BLENDED;
      }
      else {
        luaL_error(l, "rendering_mode should be one of \"solid\", \"shaded\" or \"blended\"");
      }
      text_surface->set_rendering_mode(rendering_mode);
    }
    else if (key == "horizontal_alignment") {
      const std::string& alignment_name = luaL_checkstring(l, -1);
      TextSurface::HorizontalAlignment alignment = TextSurface::ALIGN_LEFT;
      if (alignment_name == "left") {
        alignment = TextSurface::ALIGN_LEFT;
      }
      else if (alignment_name == "center") {
        alignment = TextSurface::ALIGN_CENTER;
      }
      else if (alignment_name == "right") {
        alignment = TextSurface::ALIGN_RIGHT;
      }
      else {
        luaL_error(l, "horizontal_alignment should be one of \"left\", \"center\" or \"right\"");
      }
      text_surface->set_horizontal_alignment(alignment);
    }
    else if (key == "vertical_alignment") {
      const std::string& alignment_name = luaL_checkstring(l, -1);
      TextSurface::VerticalAlignment alignment = TextSurface::ALIGN_MIDDLE;
      if (alignment_name == "top") {
        alignment = TextSurface::ALIGN_TOP;
      }
      else if (alignment_name == "middle") {
        alignment = TextSurface::ALIGN_MIDDLE;
      }
      else if (alignment_name == "bottom") {
        alignment = TextSurface::ALIGN_BOTTOM;
      }
      else {
        luaL_error(l, "vertical_alignment should be one of \"top\", \"middle\" or \"bottom\"");
      }
      text_surface->set_vertical_alignment(alignment);
    }
    else if (key == "background_color") {
      Color color = check_color(l, -1);
      text_surface->set_background_color(color);
    }
    else if (key == "text_color") {
      Color color = check_color(l, -1);
      text_surface->set_text_color(color);
    }
    else if (key == "text") {
      const std::string& text = luaL_checkstring(l, -1);
      text_surface->set_text(text);
    }
    else if (key == "text_key") {
      const std::string& text_key = luaL_checkstring(l, -1);
      text_surface->set_text(StringResource::get_string(text_key));
    }
    else {
      luaL_error(l, (StringConcat() << "Invalid key '" << key
          << "' for text surface properties").c_str());
    }
    lua_pop(l, 1); // pop the value, let the key for the iteration
  }

  text_surface->set_position(x, y);
  script.text_surfaces_created.insert(text_surface);
  push_text_surface(l, *text_surface);

  return 1;
}

/**
 * @brief Finalizes a text surface.
 *
 * - Argument 1: a text surface
 *
 * @param l a Lua state
 * @return number of values to return to Lua
 */
int Script::text_surface_meta_gc(lua_State* l) {

  Script& script = get_script(l);

  TextSurface* text_surface = *((TextSurface**)
      luaL_checkudata(l, 1, text_surface_module_name));
  if (script.text_surfaces_created.count(text_surface) > 0) {
    delete text_surface;
    script.text_surfaces_created.erase(text_surface);
  }
  // otherwise don't touch this text surface, it belongs to C

  return 0;
}

/**
 * @brief Returns the string of a text surface.
 *
 * - Argument 1: a text surface
 * - Return value: the text
 *
 * @param l a Lua state
 * @return number of values to return to Lua
 */
int Script::text_surface_api_get_text(lua_State* l) {

  TextSurface& text_surface = check_text_surface(l, 1);
  const std::string text = text_surface.get_text();
  lua_pushstring(l, text.c_str());

  return 1;
}

/**
 * @brief Sets the string of a text surface.
 *
 * - Argument 1: a text surface
 * - Argument 2: the text
 *
 * @param l a Lua state
 * @return number of values to return to Lua
 */
int Script::text_surface_api_set_text(lua_State* l) {

  TextSurface& text_surface = check_text_surface(l, 1);
  const std::string& text = luaL_checkstring(l, 2);
  text_surface.set_text(text);

  return 0;
}

