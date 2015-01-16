/*
 * Copyright (C) 2006-2015 Christopho, Solarus - http://www.solarus-games.org
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
#include "solarus/lowlevel/Sound.h"
#include "solarus/lowlevel/Music.h"
#include <lua.hpp>
#include <sstream>

namespace Solarus {

/**
 * Name of the Lua table representing the audio module.
 */
const std::string LuaContext::audio_module_name = "sol.audio";

/**
 * \brief Initializes the audio features provided to Lua.
 */
void LuaContext::register_audio_module() {

  static const luaL_Reg functions[] = {
      { "get_sound_volume", audio_api_get_sound_volume },
      { "set_sound_volume", audio_api_set_sound_volume },
      { "play_sound", audio_api_play_sound },
      { "preload_sounds", audio_api_preload_sounds },
      { "get_music_volume", audio_api_get_music_volume },
      { "set_music_volume", audio_api_set_music_volume },
      { "play_music", audio_api_play_music },
      { "stop_music", audio_api_stop_music },
      { "get_music", audio_api_get_music },
      { "get_music_format", audio_api_get_music_format },
      { "get_music_num_channels", audio_api_get_music_num_channels },
      { "get_music_channel_volume", audio_api_get_music_channel_volume },
      { "set_music_channel_volume", audio_api_set_music_channel_volume },
      { "get_music_tempo", audio_api_get_music_tempo },
      { "set_music_tempo", audio_api_set_music_tempo },
      { nullptr, nullptr }
  };
  register_functions(audio_module_name, functions);
}

/**
 * \brief Implementation of sol.audio.get_sound_volume().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::audio_api_get_sound_volume(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    lua_pushinteger(l, Sound::get_volume());
    return 1;
  });
}

/**
 * \brief Implementation of sol.audio.set_sound_volume().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::audio_api_set_sound_volume(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    int volume = LuaTools::check_int(l, 1);
    Sound::set_volume(volume);

    return 0;
  });
}

/**
 * \brief Implementation of sol.audio.play_sound().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::audio_api_play_sound(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const std::string& sound_id = LuaTools::check_string(l, 1);

    if (!Sound::exists(sound_id)) {
      LuaTools::error(l, std::string("No such sound: '") + sound_id + "'");
    }
    Sound::play(sound_id);

    return 0;
  });
}

/**
 * \brief Implementation of sol.audio.preload_sounds().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::audio_api_preload_sounds(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Sound::load_all();
    return 0;
  });
}

/**
 * \brief Implementation of sol.audio.get_music_volume().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::audio_api_get_music_volume(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    lua_pushinteger(l, Music::get_volume());
    return 1;
  });
}

/**
 * \brief Implementation of sol.audio.set_music_volume().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::audio_api_set_music_volume(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    int volume = LuaTools::check_int(l, 1);

    Music::set_volume(volume);

    return 0;
  });
}

/**
 * \brief Implementation of sol.audio.play_music().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::audio_api_play_music(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const std::string& music_id = LuaTools::opt_string(l, 1, "");
    bool loop = true;  // true by default, unless there is a callback.
    ScopedLuaRef callback_ref;
    if (lua_gettop(l) >= 2) {
      if (lua_isboolean(l, 2)) {
        // There is a loop parameter.
        loop = lua_toboolean(l, 2);
      }
      else {
        // There is a callback parameter.
        loop = false;  // No loop when there is a callback.
        callback_ref = LuaTools::check_function(l, 2);
      }
    }

    if (music_id.empty()) {
      // nil music: stop playing any music.
      Music::stop_playing();
    }
    else {
      if (!Music::exists(music_id)) {
        // Could not find the specified music.
        LuaTools::error(l, std::string("No such music: '") + music_id + "'");
      }

      // Valid music file name.
      Music::play(music_id, loop, callback_ref);
    }
    return 0;
  });
}

/**
 * \brief Implementation of sol.audio.stop_music().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::audio_api_stop_music(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Music::stop_playing();

    return 0;
  });
}

/**
 * \brief Implementation of sol.audio.get_music().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::audio_api_get_music(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const std::string& music_id = Music::get_current_music_id();

    if (music_id == Music::none) {
      lua_pushnil(l);
    }
    else {
      push_string(l, music_id);
    }
    return 1;
  });
}

/**
 * \brief Implementation of sol.audio.get_music_format().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::audio_api_get_music_format(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const Music::Format format = Music::get_format();

    if (format == Music::NO_FORMAT) {
      // No music is playing.
      lua_pushnil(l);
    }
    else {
      push_string(l, Music::format_names[format]);
    }
    return 1;
  });
}

/**
 * \brief Implementation of sol.audio.get_music_num_channels().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::audio_api_get_music_num_channels(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    if (Music::get_format() != Music::IT) {
      lua_pushnil(l);
    }
    else {
      lua_pushinteger(l, Music::get_num_channels());
    }
    return 1;
  });
}

/**
 * \brief Implementation of sol.audio.get_music_channel_volume().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::audio_api_get_music_channel_volume(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    int channel = LuaTools::check_int(l, 1);

    if (Music::get_format() != Music::IT) {
      lua_pushnil(l);
    }
    else {
      if (channel < 0 || channel >= Music::get_num_channels()) {
        std::ostringstream oss;
        oss << "Invalid channel number: " << channel;
        LuaTools::arg_error(l, 1, oss.str());
      }
      lua_pushinteger(l, Music::get_channel_volume(channel));
    }
    return 1;
  });
}

/**
 * \brief Implementation of sol.audio.set_music_channel_volume().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::audio_api_set_music_channel_volume(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    int channel = LuaTools::check_int(l, 1);
    int volume = LuaTools::check_int(l, 2);

    if (Music::get_format() != Music::IT) {
      lua_pushboolean(l, false);
    }
    else {
      if (channel < 0 || channel >= Music::get_num_channels()) {
        std::ostringstream oss;
        oss << "Invalid channel number: " << channel;
        LuaTools::arg_error(l, 1, oss.str());
      }
      Music::set_channel_volume(channel, volume);
      lua_pushboolean(l, true);
    }
    return 1;
  });
}

/**
 * \brief Implementation of sol.audio.get_music_tempo().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::audio_api_get_music_tempo(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    if (Music::get_format() != Music::IT) {
      lua_pushnil(l);
    }
    else {
      lua_pushinteger(l, Music::get_tempo());
    }
    return 1;
  });
}

/**
 * \brief Implementation of sol.audio.set_music_tempo().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::audio_api_set_music_tempo(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    int tempo = LuaTools::check_int(l, 1);

    if (Music::get_format() != Music::IT) {
      lua_pushboolean(l, false);
    }
    else {
      Music::set_tempo(tempo);
      lua_pushboolean(l, true);
    }
    return 1;
  });
}

}

