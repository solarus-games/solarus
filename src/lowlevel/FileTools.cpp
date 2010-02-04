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
#include "lowlevel/FileTools.h"
#include <physfs.h>

std::string FileTools::language_code = "en";
std::map<std::string, std::string> FileTools::languages;

/**
 * Initializes the file tools.
 * The command-line flags recognized are:
 * -datapath=some/path/to/data specifies the data path to use (overides the one given at compilation time if any)
 * -language=lg sets the language to use, where lg is the code of a language supported by the game
 * @param argc number of command-line arguments
 * @param argv command line arguments
 */
void FileTools::initialize(int argc, char **argv) {
  PHYSFS_init(argv[0]);

  // parse the flags
  const std::string datapath_flag = "-datapath=";
  std::string datapath_arg = "";
  const std::string language_flag = "-language=";
  std::string language_arg = "";
  for (argv++; argc > 1; argv++, argc--) {
    std::string arg = *argv;
    if (arg.find(datapath_flag) == 0) {
      datapath_arg = arg.substr(datapath_flag.size());
    }
    if (arg.find(language_flag) == 0) {
      language_arg = arg.substr(language_flag.size());
    }
  }

  // we set the write directory to a ".solarus" subdirectory of the user home

  // first, create the ".solarus" directory
   if (!PHYSFS_setWriteDir(PHYSFS_getUserDir())) {
    DIE("Cannot write in user directory:" << PHYSFS_getLastError());
  }
  std::string app_dir = ".solarus";
  PHYSFS_mkdir(app_dir.c_str());

  // then set this directory as the write directory
  std::string write_dir = (std::string) PHYSFS_getUserDir() + app_dir;
  if (!PHYSFS_setWriteDir(write_dir.c_str())) {
    DIE("Cannot set write dir '" << write_dir << "': " << PHYSFS_getLastError());
  }
 
  // set the search path for data files
  std::string datapath = ".";

#ifdef DATAPATH
  // by default, use the datapath defined during the build process
  datapath = std::string(DATAPATH);
#endif
  // if a command-line argument was specified, use it instead
  if (datapath_arg != "") {
    datapath = datapath_arg;
  }
  // now, datapath may be the path defined as command-line argument,
  // the path defined during the build process, or the current directory
  // if nothing was defined

  std::string debug_datapath = datapath + "/data"; // in debug mode, read directly from the data directory
  std::string release_datapath = datapath + "/data.solarus"; // in release mode, only read from the data.solarus archive

  PHYSFS_addToSearchPath(PHYSFS_getWriteDir(), 1);     // directory for writing files (savegames and configuration file)
#if SOLARUS_DEBUG_LEVEL >= 1
  PHYSFS_addToSearchPath(debug_datapath.c_str(), 1);   // data directory
#endif
  PHYSFS_addToSearchPath(release_datapath.c_str(), 1); // data.solarus archive

  // load the list of languages
  initialize_languages(language_arg);
}

/**
 * Quits the file tools.
 */
void FileTools::quit(void) {
  PHYSFS_deinit();
}

/**
 * Loads the list of available languages.
 * @param initial_language an initial language to set (empty to set the default one)
 */
void FileTools::initialize_languages(const std::string &initial_language) {
  // TODO load this from some external file
  languages["en"] = "English";
  languages["fr"] = "Français";
  languages["de"] = "Deutsch";
  languages["nl"] = "Nederlands";

  if (initial_language != "") {
    set_language(initial_language);
  }
  else {
    set_language("fr"); // default
  }
}

/**
 * Sets the current language.
 * The language-specific data will be loaded from the directory of this language.
 * @param language code of the language
 */
void FileTools::set_language(const std::string &language) {
  FileTools::language_code = language;
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
 * Opens in reading a text file in the Solarus data directory.
 * The file name is relative to the Solarus data directory.
 * The program is stopped with an error message if the file cannot be open.
 * Don't forget to close the stream with data_file_close().
 * @param file_name name of the file to open
 * @param language_specific true if the file is specific to the current language
 * @return the input stream
 */
std::istream & FileTools::data_file_open(const std::string &file_name, bool language_specific) {

  size_t size;
  char *buffer;
  data_file_open_buffer(file_name, &buffer, &size, language_specific);

  // create an input stream
  std::istringstream *is = new std::istringstream(std::string(buffer, size));
  data_file_close_buffer(buffer);
  return *is;
}

/**
 * Closes a text file previously open with data_file_open().
 * @param data_file the input stream to close
 */
void FileTools::data_file_close(const std::istream &data_file) {
  delete &data_file;
}

/**
 * Opens a data file an loads its content into a buffer.
 * @param file_name name of the file to open
 * @param buffer the buffer to load
 * @param size number of bytes to read
 * @param language_specific true if the file is specific to the current language
 */
void FileTools::data_file_open_buffer(const std::string &file_name, char **buffer, size_t *size, bool language_specific) {

  std::string full_file_name;
  if (language_specific) {
    full_file_name = (std::string) "languages/" + language_code + "/" + file_name;
  }
  else {
    full_file_name = file_name;
  }

  // open the file
  if (!PHYSFS_exists(full_file_name.c_str())) {
    DIE("Data file " << full_file_name << " does not exist");
  }
  PHYSFS_file* file = PHYSFS_openRead(full_file_name.c_str());
  if (file == NULL) {
    DIE("Cannot open data file " << full_file_name);
  }

  // load it into memory
  *size = PHYSFS_fileLength(file);

  *buffer = new char[*size];
  if (buffer == NULL) {
    DIE("Cannot allocate memory to read file " << full_file_name);
  }

  PHYSFS_read(file, *buffer, 1, *size);
  PHYSFS_close(file);
}

/**
 * Saves a buffer into a data file.
 * @param file_name name of the file to write
 * @param buffer the buffer to save
 * @param size number of bytes to write
 *
 */
void FileTools::data_file_save_buffer(const std::string &file_name, const char *buffer, size_t size) {

  // open the file to write
  PHYSFS_file *file = PHYSFS_openWrite(file_name.c_str());
  if (file == NULL) {
    DIE("Cannot open file '" << file_name << "' for writing: " << PHYSFS_getLastError());
  }
 
  // save the memory buffer 
  if (PHYSFS_write(file, buffer, size, 1) == -1) {
    DIE("Cannot write file '" << file_name << "': " << PHYSFS_getLastError());
  }
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
void FileTools::read(std::istream &is, uint32_t &value) {
  int v;
  read(is, v);
  if (v < 0) {
    DIE("Positive integer value expected from input stream");
  }
  value = (uint32_t) v;
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

