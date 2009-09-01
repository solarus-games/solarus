/*
 * Copyright (C) 2009 Christopho, Zelda Solarus - http://www.zelda-solarus.com
 * 
 * Zelda: Mystery of Solarus DX is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Zelda: Mystery of Solarus DX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "TextResource.h"
#include "FileTools.h"

/**
 * Returns a string stored in the file "text/strings.zsd".
 * @param id id of the string to retrieve
 */
std::string TextResource::get_string(const std::string &id) {

  std::string result = "";
  std::istream &file = FileTools::data_file_open("text/strings.zsd");
  std::string line;
  bool found = false;

  // read each line until we find the id
  while (std::getline(file, line) && !found) {

    if (line.size() == 0 || line[0] == '#') {
      continue;
    }

    std::string current_id;
    std::istringstream iss(line);
    FileTools::read(iss, current_id);

    if (current_id == id) {
      found = true;
      result = line.substr(current_id.size() + 1);
    }
  }

  FileTools::data_file_close(file);

  if (!found) {
    DIE("Cannot find string with id '" << id << "'");
  }

  return result;
}

