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
#include "solarus/core/Debug.h"
#include "solarus/core/StringResources.h"
#include "solarus/lua/LuaTools.h"
#include <ostream>
#include <sstream>

namespace Solarus {

/**
 * \brief Creates an empty string resources.
 */
StringResources::StringResources() {
}

/**
 * \brief Clears all strings.
 */
void StringResources::clear() {
  strings.clear();
}

/**
 * \brief Returns all strings.
 * \return The strings indexed by their key.
 */
const std::map<std::string, std::string>& StringResources::get_strings() const {
  return strings;
}

/**
 * \brief Returns whether there exists a string with the specified key.
 * \param key The key to test.
 * \return \c true if a string exists with this key.
 */
bool StringResources::has_string(const std::string& key) const {
  return strings.find(key) != strings.end();
}

/**
 * \brief Returns the string with the specified key.
 * \param key A string key. It must exist.
 * \return The string with this key.
 */
const std::string& StringResources::get_string(
    const std::string& key) const {

  const auto& it = strings.find(key);
  Debug::check_assertion(it != strings.end(),
    std::string("No such string: '") + key + "'");

  return it->second;
}

/**
 * \brief Returns the string with the specified key.
 *
 * Non-const version.
 *
 * \param key A string key. It must exist.
 * \return The string with this key.
 */
std::string& StringResources::get_string(const std::string& key) {

  const auto& it = strings.find(key);
  Debug::check_assertion(it != strings.end(),
    std::string("No such string: '") + key + "'");

  return it->second;
}

/**
 * \brief Adds a string.
 * \param key Key of the new string.
 * \param string The string to add.
 * \return \c true in case of success.
 */
bool StringResources::add_string(
    const std::string& key, const std::string& string) {

  const auto& result = strings.emplace(key, string);
  if (!result.second) {
    // Insertion failed: the id already exists.
    return false;
  }

  return true;
}

/**
 * \brief Removes a string.
 * \param key Key of the string to remove.
 * \return \c true in case of success.
 */
bool StringResources::remove_string(const std::string& key) {
  return strings.erase(key) > 0;
}

/**
 * \brief Changes the key of a string.
 * \param old_key Old key of the string.
 * \param new_key New key to set.
 * \return \c true in case of success.
 * In case of failure, the old string is unchanged.
 */
bool StringResources::set_string_key(
    const std::string& old_key, const std::string& new_key) {

  if (!has_string(old_key)) {
    // No string was found with the old id.
    return false;
  }

  if (has_string(new_key)) {
    // The new key is already used.
    return false;
  }

  std::string string = get_string(old_key);
  remove_string(old_key);
  add_string(new_key, string);

  return true;
}

/**
 * \brief Function called by Lua to add a dialog to the resource.
 *
 * - Argument 1 (table): properties of the dialog.
 *
 * \param l the Lua context that is calling this function
 * \return Number of values to return to Lua.
 */
int StringResources::l_text(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    lua_getfield(l, LUA_REGISTRYINDEX, "strings");
    StringResources& strings = *static_cast<StringResources*>(
          lua_touserdata(l, -1));
    lua_pop(l, 1);

    LuaTools::check_type(l, 1, LUA_TTABLE);

    const std::string key = LuaTools::check_string_field(l, 1, "key");
    const std::string value = LuaTools::check_string_field(l, 1, "value");

    strings.add_string(key, value);

    return 0;
  });
}

/**
 * \copydoc LuaData::import_from_lua
 */
bool StringResources::import_from_lua(lua_State* l) {

  lua_pushlightuserdata(l, this);
  lua_setfield(l, LUA_REGISTRYINDEX, "strings");
  lua_register(l, "text", l_text);
  if (lua_pcall(l, 0, 0, 0) != 0) {
    Debug::error(std::string("Error in string data file: ")
                 + lua_tostring(l, -1));
    lua_pop(l, 1);
    return false;
  }

  return true;
}

/**
 * \copydoc LuaData::export_to_lua
 */
bool StringResources::export_to_lua(std::ostream& out) const {

  for (const auto kvp : strings) {
    const std::string& key = kvp.first;
    const std::string& value = kvp.second;

    out << "text{ key = \"" << escape_string(key) <<
        "\", value = \"" << escape_string(value) << "\" }\n";
  }

  return true;
}

}
