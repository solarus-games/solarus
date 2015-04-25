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
#ifndef SOLARUS_QUEST_FILES_H
#define SOLARUS_QUEST_FILES_H

#include "solarus/Common.h"
#include <string>
#include <vector>

struct lua_State;

namespace Solarus {

class Arguments;

/**
 * \brief Handles access to data files of the current quest.
 *
 * This class provides some functions to open and analyze a data file.
 * It encapsulates how the access to data files is done.
 * This class is the only one that knows how to access a data file of the
 * current quest (including the language-specific ones)
 * and is the only one that calls the PHYSFS library to get data files from
 * the data archive when necessary.
 */
class SOLARUS_API QuestFiles {

  public:

    /**
     * \brief Describes the possible physical locations of a data file
     * of the quest.
     */
    enum DataFileLocation {
      LOCATION_NONE,
      LOCATION_DATA_DIRECTORY,
      LOCATION_DATA_ARCHIVE,
      LOCATION_WRITE_DIRECTORY,
    };

    // Initialization.
    static void initialize(const Arguments& args);
    static void quit();

    // Reading data files of the quest.
    static const std::string& get_quest_path();
    static DataFileLocation data_file_get_location(
        const std::string& file_name);
    static bool data_file_exists(
        const std::string& file_name,
        bool language_specific = false
    );
    static std::string data_file_read(
        const std::string& file_name,
        bool language_specific = false
    );
    static void data_file_save(
        const std::string& file_name,
        const std::string& buffer
    );
    static bool data_file_delete(const std::string& file_name);
    static bool data_file_mkdir(const std::string& dir_name);
    static std::vector<std::string> data_files_enumerate(
        const std::string& dir_path,
        bool list_files = true,
        bool list_directories = true
    );

    // Writing files.
    static std::string get_base_write_dir();
    static const std::string& get_solarus_write_dir();
    static const std::string& get_quest_write_dir();
    static void set_quest_write_dir(const std::string& quest_write_dir);
    static std::string get_full_quest_write_dir();

    // Temporary files.
    static std::string create_temporary_file(const std::string& content);
    static bool remove_temporary_files();

  private:

    static void set_solarus_write_dir(const std::string& solarus_write_dir);

    static std::string quest_path;                       /**< Path of the data/ directory, the data.solarus archive
                                                          * or the data.solarus.zip archive,
                                                          * relative to the current directory. */
    static std::string solarus_write_dir;                /**< Directory where the engine can write files, relative to the user's home. */
    static std::string quest_write_dir;                  /**< Write directory of the current quest, relative to solarus_write_dir. */

    static std::vector<std::string> temporary_files;     /**< Name of all temporary files created. */

};

}

#endif

