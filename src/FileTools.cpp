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
#include "FileTools.h"
#include <physfs.h>

/**
 * @brief Converts the parameter as a <code>char*</code> constant string.
 * @param x the text to convert
 */
#define STRING(x) STRING2(x)
#define STRING2(x) #x

/**
 * Initializes the file tools.
 */
void FileTools::initialize(int argc, char **argv) {
  PHYSFS_init(argv[0]);
#if ZSDX_DEBUG_LEVEL >= 1
  PHYSFS_addToSearchPath("./data", 1);
#endif
  PHYSFS_addToSearchPath("./data.zip", 1);
}

/**
 * Quits the file tools.
 */
void FileTools::quit(void) {
  PHYSFS_deinit();
}

/**
 * Returns the SDL_RWops object corresponding to the specified file name.
 * @param file_name a file name relative to the data directory
 */
SDL_RWops * FileTools::data_file_open_rw(const std::string &file_name) {

  size_t size;
  char *buffer;
  data_file_open_buffer(file_name, &buffer, &size);
  SDL_RWops *rw = SDL_RWFromMem(buffer, size);
//  SDL_RWops *rw = SDL_RWFromZZIP(full_file_name.c_str(), mode.c_str());

  if (rw == NULL) {
    DIE("Cannot open data file " << file_name);
  }
  return rw;
}

/**
 * Frees an SDL_RWops object previously create with data_file_open_rw().
 * @param rw the object to free
 */
void FileTools::data_file_close_rw(SDL_RWops *rw) {
  delete[] rw->hidden.mem.base;
  SDL_FreeRW(rw);
}

/**
 * Opens in reading a text file in the ZSDX data directory.
 * The file name is relative to the ZSDX data directory.
 * The program is stopped with an error message if the file cannot be open.
 * Don't forget to close the stream with data_file_close().
 * @param file_name name of the file to open
 * @return the input stream
 */
std::istream & FileTools::data_file_open(const std::string &file_name) {

  size_t size;
  char *buffer;
  data_file_open_buffer(file_name, &buffer, &size);

  // create an input stream
  std::istringstream *is = new std::istringstream(std::string(buffer, size));
  data_file_close_buffer(buffer);
  return *is;
}

/**
 * Closes a text file previously open with data_file_open().
 * @param data file the input stream to close
 */
void FileTools::data_file_close(const std::istream &data_file) {
  delete &data_file;
}

/**
 * Opens a data file an loads its content into a buffer.
 * @param file_name name of the file to open
 * @param buffer the buffer to load
 * @param size number of bytes read
 */
void FileTools::data_file_open_buffer(const std::string &file_name, char **buffer, size_t *size) {

  // open the file
  if (!PHYSFS_exists(file_name.c_str())) {
    DIE("Data file " << file_name << " does not exist");
  }
  PHYSFS_file* file = PHYSFS_openRead(file_name.c_str());
  if (file == NULL) {
    DIE("Cannot open data file " << file_name);
  }

  // load it into memory
  *size = PHYSFS_fileLength(file);

  *buffer = new char[*size];
  if (buffer == NULL) {
    DIE("Cannot allocate memory to read file " << file_name);
  }

  PHYSFS_read(file, *buffer, 1, *size);
  PHYSFS_close(file);
}

/**
 * Closes a data buffer previously open with data_file_open_buffer().
 * @param buffer the buffer to close
 */
void FileTools::data_file_close_buffer(char *buffer) {
  delete[] buffer;
}

/**
 * Loads an image file.
 * The file name is relative to the ZSDX data directory.
 * The program is stopped with an error message if the image cannot be loaded.
 * @param file_name name of the image file to open
 * @return the file
 */
SDL_Surface *FileTools::open_image(const std::string &file_name) {

  SDL_RWops *rw = data_file_open_rw(file_name);
  SDL_Surface *image = IMG_Load_RW(rw, 0);
  data_file_close_rw(rw);

  if (image == NULL) {
    DIE("Cannot load image '" << file_name << "'");
  }

  return image;
}

/**
 * Reads an integer value from an input stream.
 * Stops the program on an error message if the read fails.
 * @param is an input stream
 * @param value the value read
 */
void FileTools::read(std::istream &is, int &value) {
  if (!(is >> value)) {
    DIE("Cannot read integer from input stream");
  }
}

/**
 * Reads an integer value from an input stream.
 * Stops the program on an error message if the read fails.
 * @param is an input stream
 * @param value the value read
 */
void FileTools::read(std::istream &is, Uint32 &value) {
  int v;
  read(is, v);
  if (v < 0) {
    DIE("Positive integer value expected from input stream");
  }
  value = (Uint32) v;
}

/**
 * Reads a string value from an input stream.
 * Stops the program on an error message if the read fails.
 * @param is an input stream
 * @param value the value read
 */
void FileTools::read(std::istream &is, std::string &value) {
  if (!(is >> value)) {
    DIE("Cannot read string from input stream");
  }
}

