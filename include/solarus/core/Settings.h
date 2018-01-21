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
#ifndef SOLARUS_SETTINGS_H
#define SOLARUS_SETTINGS_H

#include "solarus/core/Common.h"
#include <map>
#include <string>

namespace Solarus {

class VideoMode;

/**
 * \brief Stores built-in settings of the quest.
 *
 * Settings include the language, the video mode and the audio volume.
 */
class SOLARUS_API Settings {

public:

  Settings();

  bool load(const std::string& file_name);
  bool save(const std::string& file_name);

  void set_from_quest();
  void apply_to_quest();

  std::pair<std::string, bool> get_string(const std::string& key);
  std::pair<int, bool> get_integer(const std::string& key);
  std::pair<bool, bool> get_boolean(const std::string& key);

  void set_string(const std::string& key, const std::string& value);
  void set_integer(const std::string& key, int value);
  void set_boolean(const std::string& key, bool value);
  void unset(const std::string& key);
  void clear();

  static const std::string key_video_mode;  // TODO convert to a shader value
  static const std::string key_fullscreen;
  static const std::string key_sound_volume;
  static const std::string key_music_volume;
  static const std::string key_language;
  static const std::string key_joypad_enabled;

private:

  std::map<std::string, std::string> entries;

};

}  // namespace Solarus

#endif

