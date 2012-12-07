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
#include "lowlevel/VideoManager.h"
#include <lua.hpp>

const std::string LuaContext::video_module_name = "sol.video";

/**
 * @brief Initializes the video features provided to Lua.
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
      { NULL, NULL }
  };
  register_functions(video_module_name, functions);
}

/**
 * @brief Implementation of \ref lua_api_video_get_window_title.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::video_api_get_window_title(lua_State *l) {

  const std::string& window_title =
    VideoManager::get_instance()->get_window_title();

  push_string(l, window_title);
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_video_set_window_title.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::video_api_set_window_title(lua_State *l) {

  const std::string& window_title = luaL_checkstring(l, 1);

  VideoManager::get_instance()->set_window_title(window_title);
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_video_get_mode.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::video_api_get_mode(lua_State *l) {

  VideoManager::VideoMode mode =
    VideoManager::get_instance()->get_video_mode();

  lua_pushstring(l, VideoManager::video_mode_names[mode]);
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_video_set_mode.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::video_api_set_mode(lua_State *l) {

  int mode = luaL_checkoption(l, 1, NULL, VideoManager::video_mode_names);

  if (VideoManager::get_instance()->get_video_mode() != mode) {
    VideoManager::get_instance()->set_video_mode(
        VideoManager::VideoMode(mode));
  }

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_video_switch_mode.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::video_api_switch_mode(lua_State *l) {

  VideoManager::get_instance()->switch_video_mode();

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_video_get_modes.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::video_api_get_modes(lua_State* l) {

  const std::list<VideoManager::VideoMode> modes =
    VideoManager::get_instance()->get_video_modes();

  lua_newtable(l);

  std::list<VideoManager::VideoMode>::const_iterator it;
  int i = 1;
  for (it = modes.begin(); it != modes.end(); it++) {
    VideoManager::VideoMode mode = *it;
    lua_pushstring(l, VideoManager::video_mode_names[mode]);
    lua_rawseti(l, -2, i);
    ++i;
  }

  return 1;
}

/**
 * @brief Implementation of \ref lua_api_video_is_mode_supported.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::video_api_is_mode_supported(lua_State *l) {

  int mode = luaL_checkoption(l, 1, NULL, VideoManager::video_mode_names);

  bool supported = VideoManager::get_instance()->is_mode_supported(
      VideoManager::VideoMode(mode));

  lua_pushboolean(l, supported);
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_video_is_fullscreen.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::video_api_is_fullscreen(lua_State *l) {

  bool fullscreen = VideoManager::get_instance()->is_fullscreen();

  lua_pushboolean(l, fullscreen);
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_video_set_fullscreen.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::video_api_set_fullscreen(lua_State *l) {

  bool fullscreen = true;  // true if unspecified.
  if (lua_isboolean(l, 1)) {
    fullscreen = lua_toboolean(l, 1);
  }

  VideoManager::get_instance()->set_fullscreen(fullscreen);

  return 0;
}

