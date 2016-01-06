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
 * \brief Attempts to load the built-in settings from a file.
 * \param file_name Settings file to read, relative to the quest write directory.
 * \return true if settings were successfully loaded and applied.
 */
bool Settings::load(const std::string& file_name) {

  const std::string& quest_write_dir = QuestFiles::get_quest_write_dir();
  Debug::check_assertion(!quest_write_dir.empty(),
      "Cannot load settings: no quest write directory was specified in quest.dat");

  if (!QuestFiles::data_file_exists(file_name)) {
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
        Video::set_video_mode(*video_mode);
      }
    }
  }
  lua_pop(l, 1);

  // Fullscreen.
  lua_getglobal(l, "fullscreen");
  if (lua_isboolean(l, 1)) {
    bool fullscreen = lua_toboolean(l, 1);
    Video::set_fullscreen(fullscreen);
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
    if (CurrentQuest::has_language(language)) {
      CurrentQuest::set_language(language);
    }
  }
  lua_pop(l, 1);

  // Joystick.
  lua_getglobal(l, "joypad_enabled");
  if (lua_isboolean(l, 1)) {
    bool joypad_enabled = lua_toboolean(l, 1);
    InputEvent::set_joypad_enabled(joypad_enabled);
  }
  lua_pop(l, 1);

  lua_close(l);

  return true;
}

/**
 * \brief Attempts to save the built-in settings to a file.
 * \param file_name Settings file to write, relative to the quest write directory.
 * \return true if settings were successfully saved.
 */
bool Settings::save(const std::string& file_name) {

  const std::string& quest_write_dir = QuestFiles::get_quest_write_dir();
  Debug::check_assertion(!quest_write_dir.empty(),
      "Cannot save settings: no quest write directory was specified in quest.dat");

  std::ostringstream oss;
  const VideoMode& video_mode = Video::get_video_mode();
  oss << "video_mode = \"" << video_mode.get_name() << "\"\n";
  oss << "fullscreen = " << (Video::is_fullscreen() ? "true" : "false") << "\n";
  oss << "sound_volume = " << Sound::get_volume() << "\n";
  oss << "music_volume = " << Music::get_volume() << "\n";
  if (!CurrentQuest::get_language().empty()) {
    oss << "language = \"" << CurrentQuest::get_language() << "\"\n";
  }
  oss << "joypad_enabled = " << (InputEvent::is_joypad_enabled() ? "true" : "false") << "\n";

  const std::string& text = oss.str();
  QuestFiles::data_file_save(file_name, text);
  return true;
}

}

