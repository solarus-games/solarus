/*
 * Copyright (C) 2006-2018 Christopho, Solarus - http://www.solarus-games.org
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
#include "solarus/lua/LuaContext.h"
#include "solarus/lua/LuaTools.h"
#include "solarus/lowlevel/Video.h"
#include "solarus/lowlevel/SoftwareVideoMode.h"
#include "solarus/lowlevel/Size.h"
#include "solarus/CurrentQuest.h"
#include <lua.hpp>

namespace Solarus {

/**
 * Name of the Lua table representing the video module.
 */
const std::string LuaContext::video_module_name = "sol.video";

/**
 * \brief Initializes the video features provided to Lua.
 */
void LuaContext::register_video_module() {

  std::vector<luaL_Reg> functions = {
      { "get_window_title", video_api_get_window_title },
      { "set_window_title", video_api_set_window_title },
      { "get_mode", video_api_get_mode },
      { "set_mode", video_api_set_mode },
      { "switch_mode", video_api_switch_mode },
      { "is_mode_supported", video_api_is_mode_supported },
      { "get_modes", video_api_get_modes },
      { "is_fullscreen", video_api_is_fullscreen },
      { "set_fullscreen", video_api_set_fullscreen },
      { "is_cursor_visible", video_api_is_cursor_visible },
      { "set_cursor_visible", video_api_set_cursor_visible },
      { "get_quest_size", video_api_get_quest_size },
      { "get_window_size", video_api_get_window_size },
      { "set_window_size", video_api_set_window_size },
      { "reset_window_size", video_api_reset_window_size }
  };
  if (CurrentQuest::is_format_at_least({ 1, 6 })) {
    functions.insert(functions.end(), {
      { "get_shader", video_api_get_shader },
      { "set_shader", video_api_set_shader},
    });
  }
  register_functions(video_module_name, functions);
}

/**
 * \brief Implementation of sol.video.get_window_title().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::video_api_get_window_title(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const std::string& window_title =
        Video::get_window_title();

    push_string(l, window_title);
    return 1;
  });
}

/**
 * \brief Implementation of sol.video.set_window_title().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::video_api_set_window_title(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const std::string& window_title = LuaTools::check_string(l, 1);

    Video::set_window_title(window_title);
    return 1;
  });
}

/**
 * \brief Implementation of sol.video.get_mode().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::video_api_get_mode(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {

    get_lua_context(l).warning_deprecated(
        { 1, 6 },
        "sol.video.get_mode()",
        "Use sol.video.get_shader() instead."
    );

    const SoftwareVideoMode& mode = Video::get_video_mode();

    push_string(l, mode.get_name());
    return 1;
  });
}

/**
 * \brief Implementation of sol.video.set_mode().
 * \param l the Lua context that is calling this function
 * \return Number of values to return to Lua.
 */
int LuaContext::video_api_set_mode(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {

    get_lua_context(l).warning_deprecated(
        { 1, 6 },
        "sol.video.set_mode()",
        "Use sol.video.set_shader() instead."
    );

    std::string mode_name = LuaTools::check_string(l, 1);
    const SoftwareVideoMode* mode = Video::get_video_mode_by_name(mode_name);

    if (mode != nullptr && Video::get_video_mode().get_name() != mode_name) {
      Video::set_video_mode(*mode);
    }

    return 0;
  });
}

/**
 * \brief Implementation of sol.video.switch_mode().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::video_api_switch_mode(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {

    get_lua_context(l).warning_deprecated(
        { 1, 6 },
        "sol.video.switch_mode()",
        "Use sol.video.get_shader() instead."
    );

    Video::switch_video_mode();

    return 0;
  });
}

/**
 * \brief Implementation of sol.video.get_modes().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::video_api_get_modes(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {

    get_lua_context(l).warning_deprecated(
        { 1, 6 },
        "sol.video.get_modes()",
        "Use sol.main.get_resource_ids(\"shader\") instead."
    );

    const std::vector<const SoftwareVideoMode*>& modes =
        Video::get_video_modes();

    lua_newtable(l);

    int i = 1;
    for (const SoftwareVideoMode* mode: modes) {
      push_string(l, mode->get_name());
      lua_rawseti(l, -2, i);
      ++i;
    }

    return 1;
  });
}

/**
 * \brief Implementation of sol.video.is_mode_supported().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::video_api_is_mode_supported(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {

    get_lua_context(l).warning_deprecated(
        { 1, 6 },
        "sol.video.is_mode_supported()",
        "Use sol.shader.create() instead."
    );

    std::string mode_name = LuaTools::check_string(l, 1);
    const SoftwareVideoMode* mode = Video::get_video_mode_by_name(mode_name);

    bool supported = mode != nullptr && Video::is_mode_supported(*mode);

    lua_pushboolean(l, supported);
    return 1;
  });
}

/**
 * \brief Implementation of sol.video.is_fullscreen().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::video_api_is_fullscreen(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    bool fullscreen = Video::is_fullscreen();

    lua_pushboolean(l, fullscreen);
    return 1;
  });
}

/**
 * \brief Implementation of sol.video.set_fullscreen().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::video_api_set_fullscreen(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    bool fullscreen = LuaTools::opt_boolean(l, 1, true);

    Video::set_fullscreen(fullscreen);

    return 0;
  });
}

/**
 * \brief Implementation of sol.video.is_cursor_visible().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::video_api_is_cursor_visible(lua_State *l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    bool visible_cursor = Video::is_cursor_visible();

    lua_pushboolean(l, visible_cursor);
    return 1;
  });
}

/**
 * \brief Implementation of sol.video.set_fullscreen().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::video_api_set_cursor_visible(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    bool visible_cursor = LuaTools::opt_boolean(l, 1, true);

    Video::set_cursor_visible(visible_cursor);

    return 0;
  });
}

/**
 * \brief Implementation of sol.video.get_quest_size().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::video_api_get_quest_size(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const Size& quest_size = Video::get_quest_size();

    lua_pushinteger(l, quest_size.width);
    lua_pushinteger(l, quest_size.height);
    return 2;
  });
}

/**
 * \brief Implementation of sol.video.get_window_size().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::video_api_get_window_size(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const Size& window_size = Video::get_window_size();

    lua_pushinteger(l, window_size.width);
    lua_pushinteger(l, window_size.height);
    return 2;
  });
}

/**
 * \brief Implementation of sol.video.set_window_size().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::video_api_set_window_size(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    int width = LuaTools::check_int(l, 1);
    int height = LuaTools::check_int(l, 2);

    if (width <= 0) {
      LuaTools::arg_error(l, 1, "Window width must be positive");
    }
    if (height <= 0) {
      LuaTools::arg_error(l, 2, "Window height must be positive");
    }

    Video::set_window_size(Size(width, height));

    return 0;
  });
}

/**
 * \brief Implementation of sol.video.reset_window_size().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::video_api_reset_window_size(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
\
    get_lua_context(l).warning_deprecated(
        { 1, 6 },
        "sol.video.reset_window_size()",
        "Use sol.video.set_window_size() instead."
    );

    Video::reset_window_size();

    return 0;
  });
}

/**
 * \brief Implementation of sol.video.get_shader().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::video_api_get_shader(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {

    const ShaderPtr& shader = Video::get_shader();

    if (shader == nullptr) {
      lua_pushnil(l);
    }
    else {
      push_shader(l, *shader);
    }
    return 1;
  });
}

/**
 * \brief Implementation of sol.video.set_shader().
 * \param l the Lua context that is calling this function
 * \return Number of values to return to Lua.
 */
int LuaContext::video_api_set_shader(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {

    ShaderPtr shader = nullptr;
    if (!lua_isnil(l, 1)) {
      if (is_shader(l, 1)) {
        shader = check_shader(l, 1);
      }
      else {
        LuaTools::type_error(l, 2, "shader or nil");
      }
    }

    Video::set_shader(shader);

    return 0;
  });
}

}

