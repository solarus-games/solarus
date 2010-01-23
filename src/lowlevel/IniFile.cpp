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
#include "lowlevel/IniFile.h"
#include "lowlevel/FileTools.h"
#include <SDL/SDL.h>

/**
 * Creates an object to read or write an ini file.
 * Opens the specified file.
 * @param file_name name of the file to write, relative to the data location
 * @param mode mode to open the file: READ or WRITE
 */
IniFile::IniFile(const std::string &file_name, Mode mode):
  file_name(file_name), mode(mode) {

  if (mode == READ) {
    // read the ini file
    rw = FileTools::data_file_open_rw(file_name);

    if (CFG_OpenFile_RW(rw, &ini) != CFG_OK) {
      DIE("Cannot load the ini file '" << file_name << "': " << CFG_GetError());
    }
  }

  else {
    // open an empty file
    rw = NULL;
    if (CFG_OpenFile(NULL, &ini) != CFG_OK) {
      DIE("Cannot open empty ini object for writing: " << CFG_GetError());
    }
  }
}

/**
 * Closes the ini file and destroys the object.
 */
IniFile::~IniFile(void) {

  if (mode == READ) {
    // close the file
    FileTools::data_file_close_rw(rw);
    CFG_CloseFile(&ini);
  }
}

/**
 * Saves the ini file into the file specified with the constructor.
 */
void IniFile::save(void) {

  if (mode != WRITE) {
    DIE("Cannot save ini file: the mode should be WRITE");
  }

  // save the data into the rw stream
  rw = FileTools::data_file_new_rw(64000);
  if (CFG_SaveFile_RW(rw) != CFG_OK) {
    DIE("Cannot save ini data: " << CFG_GetError());
  }

  // then save the rw stream into the real file
  FileTools::data_file_save_rw(rw, file_name);
  FileTools::data_file_close_rw(rw);
  CFG_CloseFile(&ini);
}

/**
 * Returns whether the specified group exists.
 * @param group name of the group to check
 * @return true if this group exists
 */
bool IniFile::has_group(const std::string &group) {
  return (CFG_SelectGroup(group.c_str(), 0) == CFG_OK);
}

/**
 * Sets the current group, that will be used for the next operations.
 * If the file mode is WRITE and the group does not exist, it is created.
 * If the file mode is READ and the group does not exist, a fatal error is raised.
 * @param group name of the group to set (an empty name means the global group)
 */
void IniFile::set_group(const std::string &group) {

  const char *c_group = group.c_str();
  if (c_group[0] == '\0') {
    c_group = NULL; // global group
  }

  bool create_group = (mode == WRITE);
  if (CFG_SelectGroup(c_group, create_group ? 1 : 0) == CFG_ERROR) {
    DIE("Cannot select group '" << group << "' in ini file: '" << file_name << "': " << CFG_GetError());
  }
}

/**
 * Returns the name of the current group.
 * This name was typically set by set_group() or next_group().
 * @return name of the current group (an empty name means the global group)
 */
const std::string & IniFile::get_group(void) {

  static std::string group;
  group = CFG_GetSelectedGroupName();
  return group;
}

/**
 * Returns the integer value corresponding to the specified key in the current group.
 * @param key the key
 * @param default value a default value to return if the key does not exist
 */
int IniFile::get_integer_value(const std::string &key, int default_value) {
  return CFG_ReadInt(key.c_str(), default_value);
}

/**
 * Returns the boolean value corresponding to the specified key in the current group.
 * @param key the key
 * @param default value a default value to return if the key does not exist
 */
bool IniFile::get_boolean_value(const std::string &key, bool default_value) {
  return CFG_ReadBool(key.c_str(), default_value ? 1 : 0) != 0;
}
/**
 * Returns the string value corresponding to the specified key in the current group.
 * @param key the key
 * @param default value a default value to return if the key does not exist
 */
std::string IniFile::get_string_value(const std::string &key, const std::string &default_value) {
 return CFG_ReadText(key.c_str(), default_value.c_str());
}

/**
 * Sets an integer value in the current group.
 * @param key the key
 * @param value the new value to set for that key
 */
void IniFile::set_integer_value(const std::string &key, int value) {
  CFG_WriteInt(key.c_str(), value);
}

/**
 * Sets a boolean value in the current group.
 * @param key the key
 * @param value the new value to set for that key
 */
void IniFile::set_boolean_value(const std::string &key, bool value) {
  CFG_WriteBool(key.c_str(), value ? 1 : 0);
}

/**
 * Sets a string value in the current group.
 * @param key the key
 * @param value the new value to set for that key
 */
void IniFile::set_string_value(const std::string &key, const std::string &value) {
  CFG_WriteText(key.c_str(), value.c_str());
}


/**
 * Starts an iteration over the groups of this ini file.
 * While has_more_groups() returns true, call next_group() to get the next element of your iteration.
 */
void IniFile::start_group_iteration(void) {
  CFG_StartGroupIteration(CFG_SORT_ORIGINAL); 
}

/**
 * During a group iteration, returns whether there are at least one group remaining.
 * @return true if the group iteration can continue (i.e. you can call next_group)
 */
bool IniFile::has_more_groups(void) {
 return !CFG_IsLastGroup(); 
}

/**
 * Selects the next group during a group iteration.
 * You don't have to call set_group() to select this group, it is already done by this function.
 * To know the group name, call get_group().
 */
void IniFile::next_group(void) {
  CFG_SelectNextGroup();
}


