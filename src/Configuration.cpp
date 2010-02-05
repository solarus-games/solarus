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
#include "Configuration.h"
#include "lowlevel/FileTools.h"
#include "lowlevel/IniFile.h"

const std::string Configuration::file_name = "config.ini";
const std::string Configuration::group_name = "configuration";

/**
 * Returns a string value from the configuration file.
 * @param key name of the value to get
 * @param default value the value to return if the configuration file does not exist or does not have the information
 * @return the value as a string
 */
const std::string & Configuration::get_value(const std::string &key, const std::string &default_value) {

  static std::string value;
  value = default_value;

  if (FileTools::data_file_exists(file_name)) {

    IniFile ini(file_name, IniFile::READ);
    if (ini.has_group(group_name)) {
      ini.set_group(group_name);
      value = ini.get_string_value(key, default_value);
    }
  }
  return value;
}

/**
 * Returns an integer value from the configuration file.
 * @param key name of the value to get
 * @param default value the value to return if the configuration file does not exist or does not have the information
 * @return the value as an integer
 */
int Configuration::get_value(const std::string &key, int default_value) {

  int value = default_value;

  if (FileTools::data_file_exists(file_name)) {

    IniFile ini(file_name, IniFile::READ);
    if (ini.has_group(group_name)) {
      ini.set_group(group_name);
      value = ini.get_integer_value(key, default_value);
    }
  }
  return value;
}

/**
 * Saves an integer value into the configuration file.
 * @param key name of the value to set
 * @param value the value
 */
void Configuration::set_value(const std::string &key, const std::string &value) {

  IniFile ini(file_name, IniFile::WRITE);
  ini.set_group(group_name);
  ini.set_string_value(key, value);
  ini.save();
}

/**
 * Saves a string value into the configuration file.
 * @param key name of the value to set
 * @param value the value
 */
void Configuration::set_value(const std::string &key, int value) {

  IniFile ini(file_name, IniFile::WRITE);
  ini.set_group(group_name);
  ini.set_integer_value(key, value);
  ini.save();
}

