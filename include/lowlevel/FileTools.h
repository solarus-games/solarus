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
#ifndef SOLARUS_FILE_TOOLS_H
#define SOLARUS_FILE_TOOLS_H

#include "Common.h"

/**
 * This class provides some functions to open and analyze a data file.
 * It encapsulates how the access to data files is done.
 * This class is the only one that knows how to access a data file,
 * and the only one that calls the PHYSFS library to get data files from
 * the data archive when necessary.
 */
class FileTools {

  public:

    static void initialize(int argc, char **argv);
    static void quit(void);

    static bool data_file_exists(const std::string &file_name);
    static std::istream & data_file_open(const std::string &file_name);
    static void data_file_close(const std::istream &data_file);
    static void data_file_open_buffer(const std::string &file_name, char **buffer, size_t *size);
    static void data_file_save_buffer(const std::string &file_name, const char *buffer, size_t size);
    static void data_file_close_buffer(char *buffer);
    static void data_file_delete(const std::string &file_name);

    static void read(std::istream &is, int &value);
    static void read(std::istream &is, uint32_t &value);
    static void read(std::istream &is, std::string &value);
};

#endif

