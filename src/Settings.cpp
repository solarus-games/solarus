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
#include "Settings.h"
#include "lowlevel/FileTools.h"
#include "lowlevel/Language.h"
#include "lowlevel/VideoManager.h"
#include "lowlevel/Sound.h"
#include "lowlevel/Music.h"
#include "lowlevel/InputEvent.h"
#include "lowlevel/Debug.h"
#include <lua.hpp>
#include <sstream>

/**
 * \brief Attempts to load the built-in settings from a file.
 * \param file_name Settings file to read, relative to the quest write directory.
 * \return true if settings were successfully loaded and applied.
 */
bool Settings::load(const std::string& file_name) {

  const std::string& quest_write_dir = FileTools::get_quest_write_dir();
  Debug::check_assertion(!quest_write_dir.empty(),
      "Cannot load settings: no quest write directory was specified in quest.dat");

  if (!FileTools::data_file_exists(file_name)) {
    return false;
  }

  // Read the settings as a Lua data file.
  lua_State* l = luaL_newstate();
  size_t size;
  char* buffer;
  FileTools::data_file_open_buffer(file_name, &buffer, &size);
  int load_result = luaL_loadbuffer(l, buffer, size, file_name.c_str());
  FileTools::data_file_close_buffer(buffer);

  if (load_result != 0 || lua_pcall(l, 0, 0, 0) != 0) {
    lua_pop(l, 1);
    lua_close(l);
    return false;
  }

  // The syntax of the settings file is valid. Read its values now.

  // Video mode.
  lua_getglobal(l, "video_mode");
  if (lua_isstring(l, 1)) {
    const std::string& mode_name = lua_tostring(l, 1);
    VideoManager::VideoMode mode = VideoManager::get_video_mode_by_name(mode_name);
    if (mode != VideoManager::NO_MODE && mode != VideoManager::get_instance()->get_video_mode()) {
      VideoManager::get_instance()->set_video_mode(mode);
    }
  }
  lua_pop(l, 1);

  // Sound volume.
  lua_getglobal(l, "sound_volume");
  if (lua_isnumber(l, 1)) {
    int sound_volume = int(lua_tointeger(l, 1));
    Sound::set_volume(sound_volume);
  }
  lua_pop(l, 1);

  // Music volume.
  lua_getglobal(l, "music_volume");
  if (lua_isnumber(l, 1)) {
    int music_volume = int(lua_tointeger(l, 1));
    Music::set_volume(music_volume);
  }
  lua_pop(l, 1);

  // Language.
  lua_getglobal(l, "language");
  if (lua_isstring(l, 1)) {
    const std::string& language = lua_tostring(l, 1);
    if (Language::has_language(language)) {
      Language::set_language(language);
    }
  }
  lua_pop(l, 1);

  // Joystick.
  lua_getglobal(l, "joypad_enabled");
  if (lua_isboolean(l, 1)) {
    bool joypad_enabled = lua_toboolean(l, 1);
    InputEvent::set_joypad_enabled(joypad_enabled);
  }

  lua_close(l);

  return true;
}

/**
 * \brief Attempts to save the built-in settings to a file.
 * \param file_name Settings file to write, relative to the quest write directory.
 * \return true if settings were successfully saved.
 */
bool Settings::save(const std::string& file_name) {

  const std::string& quest_write_dir = FileTools::get_quest_write_dir();
  Debug::check_assertion(!quest_write_dir.empty(),
      "Cannot save settings: no quest write directory was specified in quest.dat");

  std::ostringstream oss;
  VideoManager::VideoMode video_mode = VideoManager::get_instance()->get_video_mode();
  oss << "video_mode = \"" << VideoManager::video_mode_names[video_mode] << "\"\n";
  oss << "sound_volume = " << Sound::get_volume() << "\n";
  oss << "music_volume = " << Music::get_volume() << "\n";
  if (!Language::get_language().empty()) {
    oss << "language = \"" << Language::get_language() << "\"\n";
  }
  oss << "joypad_enabled = " << (InputEvent::is_joypad_enabled() ? "true" : "false") << "\n";

  const std::string& text = oss.str();
  FileTools::data_file_save_buffer(file_name, text.c_str(), text.size());
  return true;
}

