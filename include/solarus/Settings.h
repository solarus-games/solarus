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

/**
 * \brief Loads and saves the built-in settings of the quest.
 *
 * The settings include the language, the video mode and the audio volume.
 */
class Settings {

  public:

    static bool load(const std::string& file_name);
    static bool save(const std::string& file_name);

};

}

#endif

