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
#include "lowlevel/FileTools.h"
#include <physfs.h>

/**
 * Initializes the file tools.
 * @param argc number of command line arguments
 * @param argv command line arguments
 */
void FileTools::initialize(int argc, char **argv) {
  PHYSFS_init(argv[0]);

  // we set the write directory to a ".zsdx" subdirectory of the user home

  // first, create the ".zsdx" directory
   if (!PHYSFS_setWriteDir(PHYSFS_getUserDir())) {
    DIE("Cannot write in user directory:" << PHYSFS_getLastError());
  }
  std::string app_dir = ".zsdx";
  PHYSFS_mkdir(app_dir.c_str());

  // then set this directory as the write directory
  std::string write_dir = (std::string) PHYSFS_getUserDir() + app_dir;
  if (!PHYSFS_setWriteDir(write_dir.c_str())) {
    DIE("Cannot set write dir '" << write_dir << "': " << PHYSFS_getLastError());
  }
 
  // set the search path
  std::string data_path = ".";
  const std::string flag = "-datapath=";
  for (argv++; argc > 1; argv++, argc--) {
    std::string arg = *argv;
    if (arg.find(flag) == 0) {
      data_path = arg.substr(flag.size());
      argc = 0;
    }
  }
  data_path = data_path + "/data.zsdx";

  PHYSFS_addToSearchPath(PHYSFS_getWriteDir(), 1);
#if ZSDX_DEBUG_LEVEL >= 1
  PHYSFS_addToSearchPath("data", 1);
#endif
  PHYSFS_addToSearchPath(data_path.c_str(), 1);
}

/**
 * Quits the file tools.
 */
void FileTools::quit(void) {
  PHYSFS_deinit();
}

/**
 * Returns whether a file exists in the search path.
 * @param file_name a file name relative to a directory from the search path
 * @return true if this file exists
 */
bool FileTools::data_file_exists(const std::string &file_name) {
  return PHYSFS_exists(file_name.c_str());
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

  if (rw == NULL) {
    DIE("Cannot open data file " << file_name);
  }
  return rw;
}

/**
 * Returns an SDL_RWops object corresponding to an empty buffer
 * that yon can then use for writing a new data file.
 * @param size size of the buffer to create
 */
SDL_RWops * FileTools::data_file_new_rw(size_t size) {

  char *buffer = new char[size];
  memset(buffer, 0, size);
  SDL_RWops *rw = SDL_RWFromMem(buffer, size);
  return rw;
}

/**
 * Writes the data from an SDL_RWops source into a file.
 * @param rw the SDL_RWops source, created with data_file_open_rw() or data_file_new_rw().
 * @param file_name name of the file to write, relative to the write directory
 */
void FileTools::data_file_save_rw(SDL_RWops *rw, const std::string &file_name) {

  // open the file to write
  PHYSFS_file *file = PHYSFS_openWrite(file_name.c_str());
  if (file == NULL) {
    DIE("Cannot open file '" << file_name << "' for writing: " << PHYSFS_getLastError());
  }
 
  // read the memory buffer created by data_file_open_rw() or data_file_new_rw()
  size_t size = rw->hidden.mem.here - rw->hidden.mem.base;
  if (PHYSFS_write(file, rw->hidden.mem.base, size, 1) == -1) {
    DIE("Cannot write file '" << file_name << "': " << PHYSFS_getLastError());
  }
  PHYSFS_close(file);
}

/**
 * Frees an SDL_RWops object previously created with data_file_open_rw()
 * or data_file_new_rw().
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
 * Removes a file from the write directory.
 * @param file_name name of the file to delete
 */
void FileTools::data_file_delete(const std::string &file_name) {
  PHYSFS_delete(file_name.c_str());
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

