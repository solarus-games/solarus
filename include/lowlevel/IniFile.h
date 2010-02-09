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
#ifndef SOLARUS_INI_FILE_H
#define SOLARUS_INI_FILE_H

#include "Common.h"
#include "simpleini/SimpleIni.h"

/**
 * This class provides functions to read and write an ini-like file.
 * It encapsulates the library used to manage ini files (SimpleIni for now).
 */
class IniFile {

  public:

    /**
     * Possible modes when opening ini files.
     */
    enum Mode {
      READ,             /**< read the ini file from the data directory */
      READ_LANGUAGE,    /**< read the ini file from the language specific directory */
      WRITE             /**< write the ini file */
    };

  private:

    // file management
    std::string file_name;                        /**< name of the ini file we are working on, relative to the data location */
    Mode mode;                                    /**< the mode used to open the file */
    std::string group;                            /**< the group currently selected in the ini file */

    CSimpleIniA ini;                              /**< the library-dependent object encapsulated */
    CSimpleIniA::TNamesDepend groups;             /**< the groups currently traversed by a group iteration */
    CSimpleIniA::TNamesDepend::iterator iterator; /**< the iteration */

  public:

    // creation and destruction
    IniFile(const std::string &file_name, Mode mode);
    ~IniFile(void);

    // reading values
    bool has_group(const std::string &group);
    void set_group(const std::string &group);
    const std::string & get_group(void); 
    int get_integer_value(const std::string &key, int default_value);
    bool get_boolean_value(const std::string &key, bool default_value);
    const std::string get_string_value(const std::string &key, const std::string &default_value);
    const std::string get_string_value(const std::string &key);

    // writing values
    void set_integer_value(const std::string &key, int value);
    void set_boolean_value(const std::string &key, bool value);
    void set_string_value(const std::string &key, const std::string &value);

    void save(void);

    // group iterations
    void start_group_iteration(void);
    bool has_more_groups(void);
    void next_group(void);

};

#endif

