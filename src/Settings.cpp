/*
 * Copyright (C) 2006-2016 Christopho, Solarus - http://www.solarus-games.org
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
#include "solarus/Settings.h"
#include "solarus/CurrentQuest.h"
#include "solarus/lowlevel/Debug.h"
#include "solarus/lowlevel/QuestFiles.h"
#include "solarus/lowlevel/InputEvent.h"
#include "solarus/lowlevel/Logger.h"
#include "solarus/lowlevel/Music.h"
#include "solarus/lowlevel/Sound.h"
#include "solarus/lowlevel/String.h"
#include "solarus/lowlevel/Video.h"
#include "solarus/lowlevel/VideoMode.h"
#include <lua.hpp>
#include <sstream>

namespace Solarus {

const std::string Settings::key_video_mode = "video_mode";
const std::string Settings::key_fullscreen = "fullscreen";
const std::string Settings::key_sound_volume = "sound_volume";
const std::string Settings::key_music_volume = "music_volume";
const std::string Settings::key_language = "language";
const std::string Settings::key_joypad_enabled = "joypad_enabled";

/**
 * \brief Creates empty settings.
 */
Settings::Settings() :
  entries() {

}

/**
 * \brief Returns a string value if any.
 * \param key Key of the entry to get.
 * \return The value if any and a boolean indicating if it exists.
 */
std::pair<std::string, bool> Settings::get_string(const std::string& key) {

  const auto& it = entries.find(key);
  if (it == entries.end()) {
    return std::make_pair("", false);
  }

  return std::make_pair(it->second, true);
}

/**
 * \brief Returns an integer value if any.
 * \param key Key of the entry to get.
 * \return The value if any and a boolean indicating if it exists.
 */
std::pair<int, bool> Settings::get_integer(const std::string& key) {

  const auto& it = entries.find(key);
  if (it == entries.end()) {
    return std::make_pair(0, false);
  }

  int value = 0;
  const std::string string_value = it->second;
  std::istringstream iss(string_value);
  iss >> value;
  return std::make_pair(value, true);
}

/**
 * \brief Returns a boolean value if any.
 * \param key Key of the entry to get.
 * \return The value if any and a boolean indicating if it exists.
 */
std::pair<bool, bool> Settings::get_boolean(const std::string& key) {

  const auto& it = entries.find(key);
  if (it == entries.end()) {
    return std::make_pair(false, false);
  }

  return std::make_pair(it->second == "true", true);
}

/**
 * \brief Sets a string value.
 * \param key Key of the value to set.
 * \param value The new value.
 */
void Settings::set_string(const std::string& key, const std::string& value) {

  entries[key] = value;
}

/**
 * \brief Sets an integer value.
 * \param key Key of the value to set.
 * \param value The new value.
 */
void Settings::set_integer(const std::string& key, int value) {

  entries[key] = String::to_string(value);
}

/**
 * \brief Sets an boolean value.
 * \param key Key of the value to set.
 * \param value The new value.
 */
void Settings::set_boolean(const std::string& key, bool value) {

  entries[key] = value ? "true" : "false";
}

/**
 * \brief Unsets a value.
 * \param key Key of the value to unset.
 */
void Settings::unset(const std::string& key) {

  entries.erase(key);
}

/**
 * \brief Unsets all values.
 */
void Settings::clear() {

  entries.clear();
}

/**
 * \brief Attempts to load the built-in settings from a file.
 * \param file_name Settings file to read, relative to the quest write directory.
 * \return \c true if settings were successfully loaded and applied.
 */
bool Settings::load(const std::string& file_name) {

  const std::string& quest_write_dir = QuestFiles::get_quest_write_dir();
  if (quest_write_dir.empty()) {
    Debug::error("Cannot load settings: no quest write directory was specified in quest.dat");
    return false;
  }

  if (!QuestFiles::data_file_exists(file_name)) {
    Debug::error("Cannot find settings file '" + file_name + "'");
    return false;
  }

  // Read the settings as a Lua data file.
  lua_State* l = luaL_newstate();
  const std::string& buffer = QuestFiles::data_file_read(file_name);
  int load_result = luaL_loadbuffer(l, buffer.data(), buffer.size(), file_name.c_str());

  if (load_result != 0 || lua_pcall(l, 0, 0, 0) != 0) {
    Debug::error(std::string("Cannot read settings file '")
        + file_name + "': " + lua_tostring(l, -1)
    );
    lua_pop(l, 1);
    lua_close(l);
    return false;
  }

  // The syntax of the settings file is valid. Read its values now.
  entries.clear();

  // Video mode.
  lua_getglobal(l, "video_mode");
  if (lua_isstring(l, 1)) {
    const std::string& mode_name = lua_tostring(l, 1);
    if (!mode_name.empty()) {
      entries[key_video_mode] = mode_name;
    }
  }
  lua_pop(l, 1);

  // Fullscreen.
  lua_getglobal(l, "fullscreen");
  if (lua_isboolean(l, 1)) {
    bool fullscreen = lua_toboolean(l, 1);
    entries[key_fullscreen] = fullscreen ? "true" : "false";
  }
  lua_pop(l, 1);

  // Sound volume.
  lua_getglobal(l, "sound_volume");
  if (lua_isnumber(l, 1)) {
    const std::string& sound_volume = lua_tostring(l, 1);
    entries[key_sound_volume] = sound_volume;
  }
  lua_pop(l, 1);

  // Music volume.
  lua_getglobal(l, "music_volume");
  if (lua_isnumber(l, 1)) {
    const std::string& music_volume = lua_tostring(l, 1);
    entries[key_music_volume] = music_volume;
  }
  lua_pop(l, 1);

  // Language.
  lua_getglobal(l, "language");
  if (lua_isstring(l, 1)) {
    const std::string& language = lua_tostring(l, 1);
    entries[key_language] = language;
  }
  lua_pop(l, 1);

  // Joystick.
  lua_getglobal(l, "joypad_enabled");
  if (lua_isboolean(l, 1)) {
    bool joypad_enabled = lua_toboolean(l, 1);
    entries[key_joypad_enabled] = joypad_enabled ? "true" : "false";
  }
  lua_pop(l, 1);

  lua_close(l);

  return true;
}

/**
 * \brief Attempts to save the built-in settings to a file.
 * \param file_name Settings file to write, relative to the quest write directory.
 * \return \c true if settings were successfully saved.
 */
bool Settings::save(const std::string& file_name) {

  const std::string& quest_write_dir = QuestFiles::get_quest_write_dir();
  if (quest_write_dir.empty()) {
    Debug::error("Cannot save settings: no quest write directory was specified in quest.dat");
  }

  std::ostringstream oss;

  auto it = entries.find(key_video_mode);
  if (it != entries.end()) {
    oss << it->first << " = \"" << it->second << "\"\n";
  }

  it = entries.find(key_fullscreen);
  if (it != entries.end()) {
    oss << it->first << " = " << it->second << "\n";
  }

  it = entries.find(key_sound_volume);
  if (it != entries.end()) {
    oss << it->first << " = " << it->second << "\n";
  }

  it = entries.find(key_music_volume);
  if (it != entries.end()) {
    oss << it->first << " = " << it->second << "\n";
  }

  it = entries.find(key_language);
  if (it != entries.end()) {
    oss << it->first << " = \"" << it->second << "\"\n";
  }

  it = entries.find(key_joypad_enabled);
  if (it != entries.end()) {
    oss << it->first << " = " << it->second << "\n";
  }

  const std::string& text = oss.str();
  QuestFiles::data_file_save(file_name, text);
  return true;
}

/**
 * \brief Sets these settings to the ones of the currently running quest.
 */
void Settings::set_from_quest() {

  clear();

  if (Video::is_initialized()) {
    set_string(key_video_mode, Video::get_video_mode().get_name());
    set_boolean(key_fullscreen, Video::is_fullscreen());
  }
  if (Sound::is_initialized()) {
    set_integer(key_sound_volume, Sound::get_volume());
    set_integer(key_music_volume, Music::get_volume());
  }
  if (InputEvent::is_initialized()) {
    set_boolean(key_joypad_enabled, InputEvent::is_joypad_enabled());
  }
  if (CurrentQuest::is_initialized()) {
    if (!CurrentQuest::get_language().empty()) {
      set_string(key_language, CurrentQuest::get_language());
    }
  }
}

/**
 * \brief Applies these settings to the currently running quest.
 */
void Settings::apply_to_quest() {

  if (Video::is_initialized()) {
    // Video mode.
    auto video_mode_name = get_string(key_video_mode);
    if (video_mode_name.second) {
      const VideoMode* video_mode = Video::get_video_mode_by_name(video_mode_name.first);
      if (video_mode != nullptr) {
        Video::set_video_mode(*video_mode);
      }
    }

    // Fullscreen.
    auto fullscreen = get_boolean(key_fullscreen);
    if (fullscreen.second) {
      Video::set_fullscreen(fullscreen.first);
    }
  }

  if (Sound::is_initialized()) {
    // Sound volume.
    auto sound_volume = get_integer(key_sound_volume);
    if (sound_volume.second) {
      Sound::set_volume(sound_volume.first);
    }

    // Music volume.
    auto music_volume = get_integer(key_music_volume);
    if (music_volume.second) {
      Music::set_volume(music_volume.first);
    }
}

  if (CurrentQuest::is_initialized()) {
    // Language.
    auto language = get_string(key_language);
    if (language.second) {
      if (CurrentQuest::has_language(language.first)) {
        CurrentQuest::set_language(language.first);
      }
    }
  }

  if (InputEvent::is_initialized()) {
    // Joystick.
    auto joypad_enabled = get_boolean(key_joypad_enabled);
    if (joypad_enabled.second) {
      InputEvent::set_joypad_enabled(joypad_enabled.first);
    }
  }
}

}  // namespace Solarus
