/*
 * Copyright (C) 2009 Christopho, Solarus - http://www.solarus-engine.org
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

std::map<std::string, std::string> StringResource::strings;

/**
 * Constructor.
 */
StringResource::StringResource(void) {

}

/**
 * Destructor.
 */
StringResource::~StringResource(void) {

}

/**
 * Initializes the text resource by loading all string of the language-specific file "text/strings.dat"
 * into memory for future access by get_string().
 */
void StringResource::initialize(void) {

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
    if (index == std::string::npos || index + 1 >= line.size()) {
      DIE("strings.dat, line " << i << ": cannot read string value for key '" << key << "'");
    }
    strings[key] = line.substr(index + 1);
  }

  FileTools::data_file_close(file);
}

/**
 * Closes the text resource.
 */
void StringResource::quit(void) {

}

/**
 * Returns a string stored in the language-specific file "text/strings.dat".
 * @param key id of the string to retrieve
 */
const std::string &StringResource::get_string(const std::string &key) {

  const std::string &value = strings[key];
  if (value == "") {
    DIE("Cannot find string with key '" << key << "'");
  }

  return value;
}

