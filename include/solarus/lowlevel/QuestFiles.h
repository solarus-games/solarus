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
#ifndef SOLARUS_QUEST_FILES_H
#define SOLARUS_QUEST_FILES_H

#include "solarus/Common.h"
#include <string>
#include <vector>

struct lua_State;

namespace Solarus {

class Arguments;

/**
 * \brief Provides access to data files of the current quest.
 *
 * This namespace provides some functions to read and write data files.
 * It encapsulates how the access to data files is done.
 * This is this only place that accesses data files of the
 * current quest (including the language-specific ones)
 * and that calls the PHYSFS library to get data files from
 * the data archive when necessary.
 */
namespace QuestFiles {

/**
 * \brief Describes the possible physical locations of a data file
 * of the quest.
 */
enum class DataFileLocation {
  LOCATION_NONE,
  LOCATION_DATA_DIRECTORY,
  LOCATION_DATA_ARCHIVE,
  LOCATION_WRITE_DIRECTORY,
};

// Initialization.
SOLARUS_API bool open_quest(
    const std::string& program_name,
    const std::string& quest_path
);
SOLARUS_API void close_quest();
SOLARUS_API bool is_open();

// Reading data files of the quest.
SOLARUS_API const std::string& get_quest_path();
SOLARUS_API bool quest_exists();
SOLARUS_API DataFileLocation data_file_get_location(
    const std::string& file_name);
SOLARUS_API bool data_file_exists(
    const std::string& file_name,
    bool language_specific = false
);
SOLARUS_API std::string data_file_read(
    const std::string& file_name,
    bool language_specific = false
);
SOLARUS_API void data_file_save(
    const std::string& file_name,
    const std::string& buffer
);
SOLARUS_API bool data_file_delete(const std::string& file_name);
SOLARUS_API bool data_file_mkdir(const std::string& dir_name);

// Writing files.
SOLARUS_API std::string get_base_write_dir();
SOLARUS_API const std::string& get_solarus_write_dir();
SOLARUS_API const std::string& get_quest_write_dir();
SOLARUS_API void set_quest_write_dir(const std::string& quest_write_dir);
SOLARUS_API std::string get_full_quest_write_dir();

// Temporary files.
SOLARUS_API std::string create_temporary_file(const std::string& content);
SOLARUS_API bool remove_temporary_files();

}  // namespace QuestFiles

}  // namespace Solarus

#endif

