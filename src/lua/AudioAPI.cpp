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
#include "lua/Script.h"
#include "lowlevel/Sound.h"
#include "lowlevel/Music.h"
#include <lua.hpp>

const char* Script::audio_module_name = "sol.audio";

/**
 * @brief Initializes the audio features provided to Lua.
 */
void Script::initialize_audio_module() {

  static const luaL_Reg methods[] = {
      { "play_sound", audio_api_play_sound },
      { "play_music", audio_api_play_music },
      { "stop_music", audio_api_stop_music },
      { NULL, NULL }
  };

  luaL_register(l, audio_module_name, methods);
}

/**
 * @brief Plays a sound.
 *
 * - Argument 1 (string): name of the sound
 *
 * @param l the Lua context that is calling this function
 */
int Script::audio_api_play_sound(lua_State *l) {

  const SoundId& sound_id = luaL_checkstring(l, 1);

  Sound::play(sound_id);

  return 0;
}

/**
 * @brief Plays a music.
 *
 * - Argument 1 (string): name of the music without extension
 * (possibly "none" or "same")
 *
 * @param l the Lua context that is calling this function
 */
int Script::audio_api_play_music(lua_State *l) {

  Script& script = get_script(l);
  const MusicId& music_id = luaL_checkstring(l, 1);

  Music::play(music_id);
  script.music_played = true;

  return 0;
}

/**
 * @brief Stops playing any music.
 * @param l the Lua context that is calling this function
 */
int Script::audio_api_stop_music(lua_State* l) {

  Script& script = get_script(l);

  Music::play(Music::none);
  script.music_played = true;

  return 0;
}

