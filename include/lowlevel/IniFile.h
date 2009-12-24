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
#ifndef ZSDX_INI_FILE_H
#define ZSDX_INI_FILE_H

#include "Common.h"
#include "SDL_Config/SDL_config_lib.h"
#include "SDL/SDL.h"

/**
 * This class provides functions to read and write an ini-like file.
 * It encapsulates the library used to manage ini files (SDL_Config for now).
 */
class IniFile {

  public:

    /**
     * Possible modes when opening ini files.
     */
    enum Mode {
      READ,
      WRITE
    };

  private:

    // file management
    std::string file_name; /**< name of the ini file we are working on, relative to the data location */
    Mode mode;             /**< the mode used to open the file */
    CFG_File ini;          /**< the SDL_Config ini data encapsulated */
    SDL_RWops *rw;         /**< the IO object containing the data */

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
    std::string get_string_value(const std::string &key, const std::string &default_value);

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

