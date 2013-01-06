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
#include "lowlevel/Sound.h"
#include "lowlevel/Music.h"
#include <lua.hpp>

const std::string LuaContext::audio_module_name = "sol.audio";

/**
 * @brief Initializes the audio features provided to Lua.
 */
void LuaContext::register_audio_module() {

  static const luaL_Reg functions[] = {
      { "play_sound", audio_api_play_sound },
      { "preload_sounds", audio_api_preload_sounds },
      { "play_music", audio_api_play_music },
      { "stop_music", audio_api_stop_music },
      { "get_sound_volume", audio_api_get_sound_volume },
      { "set_sound_volume", audio_api_set_sound_volume },
      { "get_music_volume", audio_api_get_music_volume },
      { "set_music_volume", audio_api_set_music_volume },
      { NULL, NULL }
  };
  register_functions(audio_module_name, functions);
}

/**
 * @brief Implementation of \ref lua_api_audio_play_sound.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::audio_api_play_sound(lua_State *l) {

  const std::string& sound_id = luaL_checkstring(l, 1);
  Sound::play(sound_id);
  return 0;
}

/**
 * @brief Implementation of \ref lua_api_audio_preload_sounds.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::audio_api_preload_sounds(lua_State *l) {

  Sound::load_all();
  return 0;
}


/**
 * @brief Implementation of \ref lua_api_audio_play_music.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::audio_api_play_music(lua_State *l) {

  const std::string& music_id = luaL_checkstring(l, 1);

  Music::play(music_id);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_audio_stop_music.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::audio_api_stop_music(lua_State* l) {

  Music::play(Music::none);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_audio_get_sound_volume.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::audio_api_get_sound_volume(lua_State* l) {

  lua_pushinteger(l, Sound::get_volume());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_audio_set_sound_volume.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::audio_api_set_sound_volume(lua_State* l) {

  int volume = int(luaL_checkinteger(l, 1));

  Sound::set_volume(volume);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_audio_get_music_volume.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::audio_api_get_music_volume(lua_State* l) {

  lua_pushinteger(l, Music::get_volume());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_audio_set_music_volume.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::audio_api_set_music_volume(lua_State* l) {

  int volume = int(luaL_checkinteger(l, 1));

  Music::set_volume(volume);

  return 0;
}

