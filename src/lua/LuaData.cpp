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
#include "solarus/lowlevel/Debug.h"
#include "solarus/lowlevel/QuestFiles.h"
#include "solarus/lua/LuaData.h"
#include <lua.hpp>
#include <cstdio>
#include <fstream>
#include <ostream>
#include <sstream>

namespace Solarus {

/**
 * \brief Imports a Lua data file from memory to this object.
 * \param[in] buffer A memory area with the content of a data file
 * encoded in UTF-8.
 * \param[in] file_name Name of a file to use in error messages.
 * \return \c true in case of success, \c false if the file could not be loaded.
 */
bool LuaData::import_from_buffer(
    const std::string& buffer,
    const std::string& file_name
) {
  // Read the file.
  lua_State* l = luaL_newstate();
  if (luaL_loadbuffer(l, buffer.data(), buffer.size(), file_name.c_str()) != 0) {
    Debug::error(std::string("Failed to load data file: ") + lua_tostring(l, -1));
    lua_pop(l, 1);
    return false;
  }

  bool success = import_from_lua(l);
  lua_close(l);
  return success;
}

/**
 * \brief Imports a Lua data file from the filesystem to this object.
 *
 * Unlike import_from_quest_file(), this function acts on a regular file on the
 * filesystem, independently of any notion of quest search path.
 *
 * \param[in] file_name Path of the file to load.
 * The file must be encoded in UTF-8.
 * \return \c true in case of success, \c false if the file could not be loaded.
 */
bool LuaData::import_from_file(const std::string& file_name) {

  lua_State* l = luaL_newstate();
  if (luaL_loadfile(l, file_name.c_str()) != 0) {
    Debug::error(std::string("Failed to load data file '") + file_name + "': " + lua_tostring(l, -1));
    lua_pop(l, 1);
    return false;
  }

  bool success = import_from_lua(l);
  lua_close(l);
  return success;
}

/**
 * \brief Imports a Lua data file from the current quest to this object.
 *
 * This function loads a file in the search path of the current quest.
 * The actual file might be located in the physical quest data directory,
 * in the quest write directory or in the quest data archive (see QuestFiles).
 * This function does the search for you.
 *
 * \param[in] quest_file_name Path of the file to load, relative to the quest
 * data path.
 * \param[in] language_specific \c true to search in the language-specific
 * directory of the current language.
 * The file must be encoded in UTF-8.
 * \return \c true in case of success, \c false if the file could not be loaded.
 */
bool LuaData::import_from_quest_file(
    const std::string& quest_file_name,
    bool language_specific
) {
  if (!QuestFiles::data_file_exists(quest_file_name, language_specific)) {
    Debug::error(std::string("Cannot find quest file '") + quest_file_name + "'");
    return false;
  }

  const std::string& buffer = QuestFiles::data_file_read(
      quest_file_name, language_specific
  );
  return import_from_buffer(buffer, quest_file_name);
}

/**
 * \brief Saves this object into memory as Lua.
 * \param[out] buffer The buffer to write.
 * Text will be encoded in UTF-8.
 * \return \c true in case of success, \c false if the data
 * could not be exported.
 */
bool LuaData::export_to_buffer(std::string& buffer) const {

  std::ostringstream oss;
  if (!export_to_lua(oss)) {
    return false;
  }

  buffer = oss.str();
  return true;
}

/**
 * \brief Saves the data into a Lua file.
 * \param[in] file_name Path of the file to save.
 * The file will be encoded in UTF-8.
 * \return \c true in case of success, \c false if the data
 * could not be exported.
 */
bool LuaData::export_to_file(const std::string& file_name) const {

  // Work on a temporary file to keep the initial one intact in case of failure.
  std::string tmp_file_name = file_name + ".solarus_tmp";
  std::ofstream tmp_out(tmp_file_name, std::ofstream::binary);
  if (!tmp_out) {
    return false;
  }

  if (!export_to_lua(tmp_out)) {
    std::remove(tmp_file_name.c_str());
    return false;
  }
  tmp_out.flush();
  tmp_out.close();

  std::ifstream in(tmp_file_name);
  if (!in) {
    return false;
  }
  std::ofstream out(file_name);
  if (!out) {
    return false;
  }
  out << in.rdbuf();

  in.close();
  std::remove(tmp_file_name.c_str());
  out.flush();
  return true;
}

/**
 * \fn LuaData::import_from_lua
 * \brief Loads data from a Lua chunk.
 * \param l A Lua state with the chunk loaded.
 * \return \c true in case of success, \c false if there was a Lua error
 * while executing the chunk.
 */

/**
 * \brief Saves this data as Lua into a stream.
 * \param out The stream to write.
 * \return \c true in case of success, \c false if the data
 * could not be exported.
 */
bool LuaData::export_to_lua(std::ostream& /* out */) const {

  // Exporting is optional. Not implemented by default.
  return false;
}

/**
 * \brief Protects a string so that it can safely be enclosed in double quotes.
 *
 * Characters \ and " are protected by a backslash.
 *
 * Note that there is no unescape_string() function because the Lua parser
 * already replaces \\ and \".
 *
 * \param value The string value to escape.
 * \return The escaped value.
 */
std::string LuaData::escape_string(std::string value) {

  for (size_t i = 0; i < value.size(); ++i) {
    if (value[i] == '\\') {
      value.replace(i, 1, "\\\\");
      ++i;
    }
    else if (value[i] == '"') {
      value.replace(i, 1, "\\\"");
      ++i;
    }
  }

  return value;
}

/**
 * \brief Protects a string so that it can safely be enclosed between [[ and ]].
 *
 * Characters \, [ and ] are protected by a backslash.
 *
 * \param value The string value to escape.
 * \return The escaped value.
 */
std::string LuaData::escape_multiline_string(std::string value) {

  for (size_t i = 0; i < value.size(); ++i) {
    if (value[i] == '\\') {
      value.replace(i, 1, "\\\\");
      ++i;
    }
    else if (value[i] == '[') {
      value.replace(i, 1, "\\[");
      ++i;
    }
    else if (value[i] == ']') {
      value.replace(i, 1, "\\]");
      ++i;
    }
  }

  return value;
}

/**
 * \brief Restores an original string after a call to escape_multiline_string().
 * \param value The string value to unescape.
 * \return The unescaped value.
 */
std::string LuaData::unescape_multiline_string(std::string value) {

  // The Lua parsing step already replaced \\.
  for (int i = 0; i < (int) (value.size() - 1); ++i) {
    if (value[i] == '\\' && value[i + 1] == '[') {
      value.replace(i, 2, "[");
    }
    else if (value[i] == '\\' && value[i + 1] == ']') {
      value.replace(i, 2, "]");
    }
  }

  return value;
}

}
