/*
 * Copyright (C) 2006-2014 Christopho, Solarus - http://www.solarus-games.org
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
#include "solarus/lowlevel/Debug.h"
#include "solarus/lua/LuaData.h"
#include <lua.hpp>
#include <fstream>
#include <sstream>

namespace Solarus {
namespace LuaData {

/**
 * \brief Loads a data file from memory.
 * \tparam Type of data.
 * It must have a method <tt>import_from_lua(lua_State* l)</tt>.
 * \param[out] data The data to fill.
 * \param[in] buffer A memory area with the content of a data file
 * encoded in UTF-8.
 * \return \c true in case of success, \c false if the file could not be loaded.
 */
template<typename Data>
bool import_from_buffer(Data& data, const std::string& buffer) {

  // Read the file.
  lua_State* l = luaL_newstate();
  if (luaL_loadbuffer(l, buffer.data(), buffer.size(), "data file") != 0) {
    Debug::error(std::string("Failed to load data file: ") + lua_tostring(l, -1));
    lua_pop(l, 1);
    return false;
  }

  bool success = data.import_from_lua(l);
  lua_close(l);
  return success;
}

/**
 * \brief Loads a data file from the filesystem.
 * \tparam Type of data.
 * It must have a method <tt>import_from_lua(lua_State* l)</tt>.
 * \param[out] data The data to fill.
 * \param[in] file_name Path of the file to load.
 * The file must be encoded in UTF-8.
 * \return \c true in case of success, \c false if the file could not be loaded.
 */
template<typename Data>
bool import_from_file(Data& data, const std::string& file_name) {

  lua_State* l = luaL_newstate();
  if (luaL_loadfile(l, file_name.c_str()) != 0) {
    Debug::error(std::string("Failed to load data file '") + file_name + "': " + lua_tostring(l, -1));
    lua_pop(l, 1);
    return false;
  }

  bool success = data.import_from_lua(l);
  lua_close(l);
  return success;
}

/**
 * \brief Saves this data file into memory.
 * \tparam Type of data.
 * It must have a method <tt>export_to_lua(std::ostream& out)</tt>.
 * \param[in] data The data to export.
 * \param[out] buffer The buffer to write.
 * Text will be encoded in UTF-8.
 * \return \c true in case of success, \c false if the data
 * could not be exported.
 */
template<typename Data>
bool export_to_buffer(const Data& data, std::string& buffer) {

  std::ostringstream oss;
  if (!data.export_to_lua(oss)) {
    return false;
  }

  buffer = oss.str();
  return true;
}

/**
 * \brief Saves the data into a file.
 * \tparam Type of data.
 * It must have a method <tt>export_to_lua(std::ostream& out)</tt>.
 * \param[in] data The data to export.
 * \param[in] file_name Path of the file to save.
 * The file will be encoded in UTF-8.
 * \return \c true in case of success, \c false if the data
 * could not be exported.
 */
template<typename Data>
bool export_to_file(const Data& data, const std::string& file_name) {

  std::ofstream out(file_name);
  if (!out) {
    return false;
  }

  if (!data.export_to_lua(out)) {
    return false;
  }

  return true;
}

}
}
