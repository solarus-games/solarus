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
#include "solarus/lua/LuaContext.h"
#include "solarus/lua/LuaTools.h"
#include "solarus/lowlevel/QuestFiles.h"

namespace Solarus {

/**
 * Name of the Lua table representing the file module.
 */
const std::string LuaContext::file_module_name = "sol.file";

/**
 * \brief Initializes the file features provided to Lua.
 */
void LuaContext::register_file_module() {

  const std::vector<luaL_Reg> functions = {
      { "open", file_api_open },
      { "exists", file_api_exists },
      { "remove", file_api_remove },
      { "mkdir", file_api_mkdir }
  };
  register_functions(file_module_name, functions);

  // Store the original io.open function in the registry.
  // We will need to access it from sol.file.open().
                                  // --
  lua_getglobal(l, "io");
                                  // io
  lua_getfield(l, -1, "open");
                                  // io open
  Debug::check_assertion(lua_isfunction(l, -1), "Could not find io.open");
  lua_setfield(l, LUA_REGISTRYINDEX, "io.open");
                                  // io
  lua_pop(l, 1);
                                  // --
}

/**
 * \brief Implementation of sol.file.open().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::file_api_open(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const std::string& file_name = LuaTools::check_string(l, 1);
    const std::string& mode = LuaTools::opt_string(l, 2, "r");

    const bool writing = mode != "r" && mode != "rb";

    // file_name is relative to the data directory, the data archive or the
    // quest write directory.
    // Let's determine the real file name and pass it to io.open().
    std::string real_file_name;
    if (writing) {

      // Writing a file.
      if (QuestFiles::get_quest_write_dir().empty()) {
        LuaTools::error(l,
            "Cannot open file in writing: no write directory was specified in quest.dat");
      }

      real_file_name = QuestFiles::get_full_quest_write_dir() + "/" + file_name;
    }
    else {
      // Reading a file.
      QuestFiles::DataFileLocation location = QuestFiles::data_file_get_location(file_name);

      switch (location) {

      case QuestFiles::DataFileLocation::LOCATION_NONE:
        // Not found.
        lua_pushnil(l);
        push_string(l, std::string("Cannot find file '") + file_name
            + "' in the quest write directory, in data/, data.solarus or in data.solarus.zip");
        return 2;

      case QuestFiles::DataFileLocation::LOCATION_WRITE_DIRECTORY:
        // Found in the quest write directory.
        real_file_name = QuestFiles::get_full_quest_write_dir() + "/" + file_name;
        break;

      case QuestFiles::DataFileLocation::LOCATION_DATA_DIRECTORY:
        // Found in the data directory.
        real_file_name = QuestFiles::get_quest_path() + "/data/" + file_name;
        break;

      case QuestFiles::DataFileLocation::LOCATION_DATA_ARCHIVE:
      {
        // Found in the data archive.
        // To call io.open(), we need a regular file, so let's create
        // a temporary one.
        const std::string& buffer = QuestFiles::data_file_read(file_name);
        real_file_name = QuestFiles::create_temporary_file(buffer);
        break;
      }
      }
    }

    // Call io.open.
    lua_getfield(l, LUA_REGISTRYINDEX, "io.open");
    push_string(l, real_file_name);
    push_string(l, mode);

    bool called = LuaTools::call_function(l, 2, 2, "io.open");
    if (!called) {
      LuaTools::error(l, "Unexpected error: failed to call io.open()");
    }

    return 2;
  });
}

/**
 * \brief Implementation of sol.file.exists().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::file_api_exists(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const std::string& file_name = LuaTools::check_string(l, 1);

    lua_pushboolean(l, QuestFiles::data_file_exists(file_name, false));

    return 1;
  });
}

/**
 * \brief Implementation of sol.file.remove().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::file_api_remove(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const std::string& file_name = LuaTools::check_string(l, 1);

    bool success = QuestFiles::data_file_delete(file_name);

    if (!success) {
      lua_pushnil(l);
      push_string(l, std::string("Failed to delete file '") + file_name + "'");
      return 2;
    }

    lua_pushboolean(l, true);
    return 1;
  });
}

/**
 * \brief Implementation of sol.file.mkdir().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::file_api_mkdir(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const std::string& dir_name = LuaTools::check_string(l, 1);

    bool success = QuestFiles::data_file_mkdir(dir_name);

    if (!success) {
      lua_pushnil(l);
      push_string(l, std::string("Failed to create directory '") + dir_name + "'");
      return 2;
    }

    lua_pushboolean(l, true);
    return 1;
  });
}

}

