/*
 * Copyright (C) 2006-2012 Christopho, Solarus - http://www.solarus-games.org
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
#include "lowlevel/Debug.h"

const std::string Configuration::group_name = "configuration";

/**
 * @brief Returns the configuration file name.
 * @return The configuration file name, relative to the Solarus write directory.
 */
const std::string Configuration::get_file_name() {

  const std::string& quest_write_dir = FileTools::get_quest_write_dir();
  Debug::check_assertion(!quest_write_dir.empty(), "The quest write directory was not set");
  return quest_write_dir + "/config.ini";
}

/**
 * @brief Returns a string value from the configuration file.
 * @param key name of the value to get
 * @param default_value the value to return if the configuration file does not exist or does not have the information
 * @return the value as a string
 */
const std::string Configuration::get_value(const std::string& key, const std::string &default_value) {

  const std::string& file_name = get_file_name();
  std::string value = default_value;

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
 * @brief Returns an integer value from the configuration file.
 * @param key name of the value to get
 * @param default_value the value to return if the configuration file does not exist or does not have the information
 * @return the value as an integer
 */
int Configuration::get_value(const std::string& key, int default_value) {

  const std::string& file_name = get_file_name();
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
 * @brief Saves an integer value into the configuration file.
 * @param key name of the value to set
 * @param value the value
 */
void Configuration::set_value(const std::string& key, const std::string &value) {

  IniFile ini(get_file_name(), IniFile::WRITE);
  ini.set_group(group_name);
  ini.set_string_value(key, value);
  ini.save();
}

/**
 * @brief Saves a string value into the configuration file.
 * @param key name of the value to set
 * @param value the value
 */
void Configuration::set_value(const std::string& key, int value) {

  IniFile ini(get_file_name(), IniFile::WRITE);
  ini.set_group(group_name);
  ini.set_integer_value(key, value);
  ini.save();
}

