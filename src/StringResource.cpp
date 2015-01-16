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
#include "solarus/StringResource.h"
#include "solarus/Language.h"
#include "solarus/lowlevel/QuestFiles.h"
#include "solarus/lua/LuaTools.h"
#include <map>

namespace Solarus {

namespace {

std::map<std::string, std::string> strings;

/**
 * \brief Function called by Lua to add a dialog to the resource.
 *
 * - Argument 1 (table): properties of the dialog.
 *
 * \param l the Lua context that is calling this function
 * \return Number of values to return to Lua.
 */
int l_text(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    LuaTools::check_type(l, 1, LUA_TTABLE);

    const std::string key = LuaTools::check_string_field(l, 1, "key");
    const std::string value = LuaTools::check_string_field(l, 1, "value");

    strings[key] = value;

    return 0;
  });
}

}

/**
 * \brief Initializes the text resource by loading all strings.
 *
 * The strings are loaded from the language-specific file "text/strings.dat"
 * and stored into memory for future access by get_string().
 */
void StringResource::initialize() {

  strings.clear();

  const std::string file_name("text/strings.dat");
  lua_State* l = luaL_newstate();
  const std::string& buffer = QuestFiles::data_file_read(file_name, true);
  int load_result = luaL_loadbuffer(l, buffer.data(), buffer.size(), file_name.c_str());

  if (load_result != 0) {
    Debug::error(std::string("Failed to load strings file '") + file_name
        + "' for language '" + Language::get_language() + "': "
        + lua_tostring(l, -1));
    lua_pop(l, 1);
  }
  else {
    lua_register(l, "text", l_text);
    if (lua_pcall(l, 0, 0, 0) != 0) {
      Debug::error(std::string("Failed to load strings file '") + file_name
          + "' for language '" + Language::get_language() + "': "
          + lua_tostring(l, -1));
      lua_pop(l, 1);
    }
  }

  lua_close(l);
}

/**
 * \brief Closes the text resource.
 */
void StringResource::quit() {
  strings.clear();
}

/**
 * \brief Returns whether a string exists in the language-specific file
 * "text/strings.dat" for the current language.
 * \param key Id of a string.
 * \return true if the string exists.
 */
bool StringResource::exists(const std::string& key) {

  return strings.find(key) != strings.end();
}

/**
 * \brief Returns a string stored in the language-specific file
 * "text/strings.dat" for the current language.
 * \param key Id of the string to retrieve. It must exist.
 * \return The corresponding localized string.
 */
const std::string& StringResource::get_string(const std::string& key) {

  Debug::check_assertion(exists(key),
      std::string("Cannot find string with key '") + key + "'"
  );
  return strings[key];
}

}

