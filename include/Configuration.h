/*
 * Copyright (C) 2009 Christopho, Solarus - http://www.solarus-engine.org
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

#ifndef SOLARUS_CONFIGURATION_H
#define SOLARUS_CONFIGURATION_H

#include "Common.h"

/**
 * @brief Stores the global configuration of the program.
 *
 * This class loads and saves the global configuration of the program, such as the language,
 * the screen resolution and the audio volume.
 */
class Configuration {

  private:

    static const std::string file_name;     /**< name of the file that stores the configuration in an ini-like way */
    static const std::string group_name;    /**< name of the configuration group in the file */

  public:

    static const std::string get_value(const std::string &key, const std::string &default_value);
    static int get_value(const std::string &key, int default_value);

    static void set_value(const std::string &key, const std::string &value);
    static void set_value(const std::string &key, int value);
};

#endif

