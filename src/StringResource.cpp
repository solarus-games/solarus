/*
 * Copyright (C) 2009-2011 Christopho, Solarus - http://www.solarus-engine.org
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
 * @brief Constructor.
 */
StringResource::StringResource() {

}

/**
 * @brief Destructor.
 */
StringResource::~StringResource() {

}

/**
 * @brief Initializes the text resource by loading all strings.
 *
 * The strings are loaded from the language-specific file "text/strings.dat"
 * and stored into memory for future access by get_string().
 */
void StringResource::initialize() {

  strings.clear();
  std::istream &file = FileTools::data_file_open("text/strings.dat", true);
  std::string line;

  // read each line
  int i = 0;
  while (std::getline(file, line)) {

    i++;

    // ignore empty lines or lines starting with '#'
    if (line.size() == 0 || line[0] == '#') {
      continue;
    }
 
    // get the key
    std::string key = line.substr(0, line.find_first_of(" \t"));

    // get the value
    size_t index = line.find_last_of("\t");
    Debug::check_assertion(index != std::string::npos && index + 1 < line.size(),
      StringConcat() << "strings.dat, line " << i << ": cannot read string value for key '" << key << "'");
    strings[key] = line.substr(index + 1);
  }

  FileTools::data_file_close(file);
}

/**
 * @brief Closes the text resource.
 */
void StringResource::quit() {
  strings.clear();
}

/**
 * @brief Returns a string stored in the language-specific file "text/strings.dat".
 * @param key id of the string to retrieve
 * @return the corresponding localized string
 */
const std::string& StringResource::get_string(const std::string& key) {

  Debug::check_assertion(strings.count(key) > 0, StringConcat() << "Cannot find string with key '" << key << "'");
  return strings[key];
}

