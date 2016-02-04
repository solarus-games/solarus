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

#ifndef SOLARUS_SETTINGS_H
#define SOLARUS_SETTINGS_H

#include "solarus/Common.h"
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

  template<typename T>
  using OptionalSetting = std::pair<T, bool>;

  Settings();

  bool load(const std::string& file_name);
  bool save(const std::string& file_name);

  void set_from_quest();
  void apply_to_quest();

private:

  OptionalSetting<const VideoMode*> video_mode;  /**< Video mode if any. TODO name instead of pointer */
  OptionalSetting<bool> fullscreen               /**< Fullscreen flag if any. */;
  OptionalSetting<int> sound_volume;             /**< Sound volume if any (0 to 100). */
  OptionalSetting<int> music_volume;             /**< Music volume if any (0 to 100). */
  OptionalSetting<std::string> language;         /**< Language code if any. */
  OptionalSetting<bool> joypad_enabled;          /**< Joypad flag if any. */

};

}  // namespace Solarus

#endif

