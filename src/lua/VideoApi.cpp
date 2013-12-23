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
#include "lua/LuaContext.h"
#include "lowlevel/Video.h"
#include "lowlevel/VideoMode.h"
#include "lowlevel/Rectangle.h"
#include <lua.hpp>

namespace solarus {

const std::string LuaContext::video_module_name = "sol.video";

/**
 * \brief Initializes the video features provided to Lua.
 */
void LuaContext::register_video_module() {

  static const luaL_Reg functions[] = {
      { "get_window_title", video_api_get_window_title },
      { "set_window_title", video_api_set_window_title },
      { "get_mode", video_api_get_mode },
      { "set_mode", video_api_set_mode },
      { "switch_mode", video_api_switch_mode },
      { "is_mode_supported", video_api_is_mode_supported },
      { "get_modes", video_api_get_modes },
      { "is_fullscreen", video_api_is_fullscreen },
      { "set_fullscreen", video_api_set_fullscreen },
      { "get_quest_size", video_api_get_quest_size },
      { NULL, NULL }
  };
  register_functions(video_module_name, functions);
}

/**
 * \brief Implementation of sol.video.get_window_title().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::video_api_get_window_title(lua_State *l) {

  const std::string& window_title =
    Video::get_window_title();

  push_string(l, window_title);
  return 1;
}

/**
 * \brief Implementation of sol.video.set_window_title().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::video_api_set_window_title(lua_State *l) {

  const std::string& window_title = luaL_checkstring(l, 1);

  Video::set_window_title(window_title);
  return 1;
}

/**
 * \brief Implementation of sol.video.get_mode().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::video_api_get_mode(lua_State *l) {

  const VideoMode& mode = Video::get_video_mode();

  push_string(l, mode.get_name());
  return 1;
}

/**
 * \brief Implementation of sol.video.set_mode().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::video_api_set_mode(lua_State *l) {

  std::string mode_name = luaL_checkstring(l, 1);
  const VideoMode* mode = Video::get_video_mode_by_name(mode_name);

  if (mode != NULL && Video::get_video_mode().get_name() != mode_name) {
    Video::set_video_mode(*mode);
  }

  return 0;
}

/**
 * \brief Implementation of sol.video.switch_mode().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::video_api_switch_mode(lua_State *l) {

  Video::switch_video_mode();

  return 0;
}

/**
 * \brief Implementation of sol.video.get_modes().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::video_api_get_modes(lua_State* l) {

  const std::vector<const VideoMode*>& modes =
    Video::get_video_modes();

  lua_newtable(l);

  std::vector<const VideoMode*>::const_iterator it;
  int i = 1;
  for (it = modes.begin(); it != modes.end(); ++it) {
    const VideoMode* mode = *it;
    push_string(l, mode->get_name());
    lua_rawseti(l, -2, i);
    ++i;
  }

  return 1;
}

/**
 * \brief Implementation of sol.video.is_mode_supported().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::video_api_is_mode_supported(lua_State *l) {

  std::string mode_name = luaL_checkstring(l, 1);
  const VideoMode* mode = Video::get_video_mode_by_name(mode_name);

  bool supported = mode != NULL && Video::is_mode_supported(*mode);

  lua_pushboolean(l, supported);
  return 1;
}

/**
 * \brief Implementation of sol.video.is_fullscreen().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::video_api_is_fullscreen(lua_State *l) {

  bool fullscreen = Video::is_fullscreen();

  lua_pushboolean(l, fullscreen);
  return 1;
}

/**
 * \brief Implementation of sol.video.set_fullscreen().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::video_api_set_fullscreen(lua_State *l) {

  bool fullscreen = true;  // true if unspecified.
  if (lua_gettop(l) >= 1) {
    fullscreen = lua_toboolean(l, 1);
  }

  Video::set_fullscreen(fullscreen);

  return 0;
}

/**
 * \brief Implementation of sol.video.get_quest_size().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::video_api_get_quest_size(lua_State* l) {

  const Rectangle& quest_size = Video::get_quest_size();

  lua_pushinteger(l, quest_size.get_width());
  lua_pushinteger(l, quest_size.get_height());
  return 2;
}

}

