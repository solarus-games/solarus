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
#include "solarus/StringResourceData.h"
#include <map>

namespace Solarus {

const std::string StringResource::file_name = "text/strings.dat";
std::map<std::string, std::string> StringResource::strings;

/**
 * \brief Initializes the text resource by loading all strings.
 *
 * The strings are loaded from the language-specific file "text/strings.dat"
 * and stored into memory for future access by get_string().
 */
void StringResource::initialize() {

  strings.clear();

  StringResourceData data;
  const std::string& buffer = QuestFiles::data_file_read(file_name, true);
  bool success = data.import_from_buffer(buffer);
  if (success) {
    // Get the imported data.
    for (const auto& kvp : data.get_strings()) {
      strings.emplace(kvp.first, kvp.second);
    }
  } else {
    Debug::error(std::string("Failed to load strings file '") + file_name
        + "' for language '" + Language::get_language());
  }
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

