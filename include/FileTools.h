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
#ifndef ZSDX_FILE_TOOLS_H
#define ZSDX_FILE_TOOLS_H

#include "Common.h"

/**
 * This class provides some functions to open and analyse a data file.
 * It encapsulates how the access to data files is done.
 *
 * The most important function here is data_file_add_prefix, which
 * takes as paramater a relative data file path and returns its real path
 * depending on how the data files are stored: as normal files, in an archive,
 * in the memory itself... This class is the only one which knows how to access
 * a data file.
 */
class FileTools {
 
  public: // TODO private

    static const std::string data_file_get_prefix(void);
    static const std::string data_file_add_prefix(const std::string &file_name);

  public:

    static SDL_RWops * get_data_rw(const std::string &file_name);
    static SDL_RWops * get_data_rw(const std::string &file_name, std::string mode);
 
    static std::istream & data_file_open(const std::string &file_name);
    static void data_file_close(const std::istream &data_file);
    static void data_file_open_buffer(const std::string &file_name, char **buffer, size_t *size);
    static void data_file_close_buffer(char *buffer);

    static SDL_Surface *open_image(const std::string &file_name);

    static void read(std::istream &is, int &value);
    static void read(std::istream &is, Uint32 &value);
    static void read(std::istream &is, std::string &value);
};

#endif

