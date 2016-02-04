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
#include "solarus/lowlevel/QuestFiles.h"
#include "solarus/CurrentQuest.h"
#include "solarus/lowlevel/Video.h"
#include "solarus/lowlevel/VideoMode.h"
#include "solarus/lowlevel/Sound.h"
#include "solarus/lowlevel/Music.h"
#include "solarus/lowlevel/InputEvent.h"
#include "solarus/lowlevel/Debug.h"
#include <lua.hpp>
#include <sstream>

namespace Solarus {

/**
 * \brief Creates empty settings.
 */
Settings::Settings() :
  video_mode(nullptr, false),
  fullscreen(false, false),
  sound_volume(0, false),
  music_volume(0, false),
  language("", false),
  joypad_enabled(false, false) {

}

/**
 * \brief Attempts to load the built-in settings from a file.
 * \param file_name Settings file to read, relative to the quest write directory.
 * \return \c true if settings were successfully loaded and applied.
 */
bool Settings::load(const std::string& file_name) {

  const std::string& quest_write_dir = QuestFiles::get_quest_write_dir();
  if (quest_write_dir.empty()) {
    Debug::error("Cannot load settings: no write directory was specified in quest.dat");
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

  // Video mode.
  lua_getglobal(l, "video_mode");
  if (lua_isstring(l, 1)) {
    const std::string& mode_name = lua_tostring(l, 1);
    if (mode_name != "" && mode_name != Video::get_video_mode().get_name()) {
      const VideoMode* video_mode = Video::get_video_mode_by_name(mode_name);
      if (video_mode != nullptr) {  // Valid video mode.
        this->video_mode = std::make_pair(video_mode, true);
      }
      else {
        this->video_mode = std::make_pair(nullptr, false);
      }
    }
  }
  lua_pop(l, 1);

  // Fullscreen.
  lua_getglobal(l, "fullscreen");
  if (lua_isboolean(l, 1)) {
    bool fullscreen = lua_toboolean(l, 1);
    this->fullscreen = std::make_pair(fullscreen, true);
  }
  else {
    this->fullscreen = std::make_pair(false, false);
  }
  lua_pop(l, 1);

  // Sound volume.
  lua_getglobal(l, "sound_volume");
  if (lua_isnumber(l, 1)) {
    int sound_volume = int(lua_tointeger(l, 1));
    this->sound_volume = std::make_pair(sound_volume, true);
  }
  else {
    this->sound_volume = std::make_pair(0, false);
  }
  lua_pop(l, 1);

  // Music volume.
  lua_getglobal(l, "music_volume");
  if (lua_isnumber(l, 1)) {
    int music_volume = int(lua_tointeger(l, 1));
    this->music_volume = std::make_pair(music_volume, true);
  }
  else {
    this->music_volume = std::make_pair(0, false);
  }
  lua_pop(l, 1);

  // Language.
  lua_getglobal(l, "language");
  if (lua_isstring(l, 1)) {
    const std::string& language = lua_tostring(l, 1);
    this->language = std::make_pair(language, true);
  }
  else {
    this->language = std::make_pair("", false);
  }
  lua_pop(l, 1);

  // Joystick.
  lua_getglobal(l, "joypad_enabled");
  if (lua_isboolean(l, 1)) {
    bool joypad_enabled = lua_toboolean(l, 1);
    this->joypad_enabled = std::make_pair(joypad_enabled, true);
  }
  else {
    this->joypad_enabled = std::make_pair(false, false);
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

  if (video_mode.second) {
    Debug::check_assertion(video_mode.first != nullptr, "Missing video mode");
    oss << "video_mode = \"" << video_mode.first->get_name() << "\"\n";
  }

  if (fullscreen.second) {
    oss << "fullscreen = " << (fullscreen.first ? "true" : "false") << "\n";
  }

  if (sound_volume.second) {
    oss << "sound_volume = " << sound_volume.first << "\n";
  }

  if (music_volume.second) {
    oss << "music_volume = " << music_volume.first << "\n";
  }

  if (language.second) {
    Debug::check_assertion(!language.first.empty(), "Missing language");
    oss << "language = \"" << language.first << "\"\n";
  }

  if (joypad_enabled.second) {
    oss << "joypad_enabled = " << (joypad_enabled.first ? "true" : "false") << "\n";
  }

  const std::string& text = oss.str();
  QuestFiles::data_file_save(file_name, text);
  return true;
}

/**
 * \brief Sets these settings to the ones of the currently running quest.
 */
void Settings::set_from_quest() {

  /* TODO
  set_video_mode(std::make_pair(&Video::get_video_mode(), true));
  set_fullscreen(std::make_pair(Video::is_fullscreen(), true));
  set_sound_volume(std::make_pair(Sound::get_volume()));
  set_music_volume(std::make_pair(Music::get_volume()));
  set_joypad_enabled(std::make_pair(InputEvent::is_joypad_enabled()));

  if (CurrentQuest::get_language().empty()) {
    set_language(std::make_pair("", false));
  }
  else {
    set_language(std::make_pair(CurrentQuest::get_language()));
  }
  */
}

/**
 * \brief Applies these settings to the currently running quest.
 */
void Settings::apply_to_quest() {

  // Video mode.
  if (video_mode.second) {
    Debug::check_assertion(video_mode.first != nullptr, "Missing video mode");
    Video::set_video_mode(*video_mode.first);
  }

  // Fullscreen.
  if (fullscreen.second) {
    Video::set_fullscreen(fullscreen.first);
  }

  // Sound volume.
  if (sound_volume.second) {
    Sound::set_volume(sound_volume.first);
  }

  // Music volume.
  if (music_volume.second) {
    Music::set_volume(music_volume.first);
  }

  // Language.
  if (language.second) {
    if (CurrentQuest::has_language(language.first)) {
      CurrentQuest::set_language(language.first);
    }
  }

  // Joystick.
  if (joypad_enabled.second) {
    InputEvent::set_joypad_enabled(joypad_enabled.first);
  }
}

}  // namespace Solarus
