/*
 * Copyright (C) 2006-2013 Christopho, Solarus - http://www.solarus-games.org
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
#include "StringResource.h"
#include "lowlevel/FileTools.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"

std::map<std::string, std::string> StringResource::strings;

/**
 * \brief Constructor.
 */
StringResource::StringResource() {

}

/**
 * \brief Destructor.
 */
StringResource::~StringResource() {

}

/**
 * \brief Initializes the text resource by loading all strings.
 *
 * The strings are loaded from the language-specific file "text/strings.dat"
 * and stored into memory for future access by get_string().
 */
void StringResource::initialize() {

  strings.clear();
  std::istream& file = FileTools::data_file_open("text/strings.dat", true);
  std::string line;

  // read each line
  int i = 0;
  while (std::getline(file, line)) {

    i++;

    // ignore empty lines or lines starting with '#'
    if (line.size() == 0 || line[0] == '\r' || line[0] == '#') {
      continue;
    }
 
    // get the key
    size_t index = line.find_first_of(" \t");
    Debug::check_assertion(index != std::string::npos,
	StringConcat() << "strings.dat, line " << i
	<< ": invalid line (expected a key and a value)");
    std::string key = line.substr(0, index);

    // get the value
    do {
      index++;
    } while (index < line.size()
	&& (line[index] == ' ' || line[index] == '\t' || line[index] == '\r'));

    Debug::check_assertion(index < line.size(),
      StringConcat() << "strings.dat, line " << i
      << ": the value of key '" << key << "' is missing");

    std::string value = line.substr(index);

    if (value[value.size() - 1] == '\r') {
      // If the file has DOS line endings, remove the trailing '\r'.
      value = value.substr(0, value.size() - 1);
    }

    strings[key] = value;
  }

  FileTools::data_file_close(file);
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

  Debug::check_assertion(exists(key), StringConcat()
      << "Cannot find string with key '" << key << "'");
  return strings[key];
}

