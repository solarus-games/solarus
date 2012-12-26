/*
 * Copyright (C) 2009-2011 Christopho, Solarus - http://www.solarus-engine.org
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
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"
#include "lowlevel/IniFile.h"
#include "Configuration.h"
#include "StringResource.h"
#include "DialogResource.h"
#include <physfs.h>
#include <lua.hpp>

#if defined(SOLARUS_USE_OSX_INTERFACE)
#   include "lowlevel/osx/OSXInterface.h"
#endif

std::string FileTools::language_code;
std::string FileTools::default_language_code;
std::map<std::string, std::string> FileTools::languages;

/**
 * @brief Initializes the file tools.
 * @param argc number of command-line arguments
 * @param argv command line arguments
 */
void FileTools::initialize(int argc, char** argv) {

  PHYSFS_init(argv[0]);

  // look for the language options
  const std::string language_flag = "-language=";
  std::string language_arg = "";
  for (int i = 1; i < argc; i++) {
    std::string arg = argv[i];
    if (arg.find(language_flag) == 0) {
      language_arg = arg.substr(language_flag.size());
    }
  }

  // set the quest path, by default as defined during the build process
  std::string quest_path = SOLARUS_DEFAULT_QUEST;

  // if a command-line argument was specified, use it instead
  if (argc > 1 && argv[argc - 1][0] != '-') {
    // the last parameter is not an option: it is the quest path
    quest_path = argv[argc - 1];
  }

  // now, quest_path may be the path defined as command-line argument,
  // the path defined during the build process, or the current directory
  // if nothing was specified

  std::string dir_quest_path = quest_path + "/data";
  std::string archive_quest_path = quest_path + "/data.solarus";

  const std::string& base_dir = PHYSFS_getBaseDir();
  PHYSFS_addToSearchPath(dir_quest_path.c_str(), 1);   // data directory
  PHYSFS_addToSearchPath(archive_quest_path.c_str(), 1); // data.solarus archive
  PHYSFS_addToSearchPath((base_dir + "/" + dir_quest_path).c_str(), 1);
  PHYSFS_addToSearchPath((base_dir + "/" + archive_quest_path).c_str(), 1);

  // check the existence of the quest path
  if (!FileTools::data_file_exists("quest.dat")) {
    Debug::die(StringConcat() << "No quest was found in the directory '" << quest_path
        << "'. To specify your quest's path, run: "
        << argv[0] << " path/to/quest");
  }

  // then set the write directory to a "SOLARUS_WRITE_DIR/quest_dir" subdirectory of the user home

  // first, create the directory
   if (!PHYSFS_setWriteDir(PHYSFS_getUserDir())) {
     Debug::die(StringConcat() << "Cannot write in user directory:" << PHYSFS_getLastError());
  }
  IniFile ini("quest.dat", IniFile::READ);
  ini.set_group("info");

    //TODO adapt on solarus1.0.0 branch
#if !defined(SOLARUS_USE_OSX_INTERFACE)
    std::string write_dir = (std::string) PHYSFS_getUserDir();
#else
    std::string write_dir =  std::string( getUserApplicationSupportDirectory() );
#endif
  PHYSFS_mkdir(write_dir.c_str());

  // then set this directory as the write directory
  write_dir += (std::string) SOLARUS_WRITE_DIR + "/" + ini.get_string_value("write_dir");
  if (!PHYSFS_setWriteDir(write_dir.c_str())) {
    Debug::die(StringConcat() << "Cannot set write dir '" << write_dir << "': " << PHYSFS_getLastError());
  }
  PHYSFS_addToSearchPath(PHYSFS_getWriteDir(), 0);     // directory for writing files (savegames and configuration file)

  // load the list of languages
  initialize_languages(language_arg);
}

/**
 * @brief Quits the file tools.
 */
void FileTools::quit() {

  DialogResource::quit();
  StringResource::quit();
  PHYSFS_deinit();
}

/**
 * @brief Loads the list of available languages.
 * @param arg_language the language specified as command-line argument
 * (or an empty string if not specified)
 */
void FileTools::initialize_languages(const std::string& arg_language) {

  static const std::string file_name = "languages/languages.lua";

  // read the languages file
  lua_State* l = lua_open();
  size_t size;
  char* buffer;
  FileTools::data_file_open_buffer(file_name, &buffer, &size);
  luaL_loadbuffer(l, buffer, size, file_name.c_str());
  FileTools::data_file_close_buffer(buffer);

  lua_register(l, "language", l_language);
  if (lua_pcall(l, 0, 0, 0) != 0) {
    Debug::die(StringConcat() << "Failed to load language file: "
        << lua_tostring(l, -1));
    lua_pop(l, 1);
  }

  lua_close(l);

  // set a language
  if (arg_language.size() != 0) {
    set_language(arg_language);
  }
  else {
    std::string config_language = Configuration::get_value("language", "");
    if (config_language.size() != 0) {
      set_language(config_language);
    }
  }
}

/**
 * @brief Function called by Lua to add a language.

 * - Argument 1 (table): properties of the language (keys must be code and
 * name).
 *
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int FileTools::l_language(lua_State* l) {

  luaL_checktype(l, 1, LUA_TTABLE);

  std::string code;
  std::string name;

  // traverse the table, looking for properties
  lua_pushnil(l); // first key
  while (lua_next(l, 1) != 0) {

    const std::string& key = luaL_checkstring(l, -2);
    if (key == "code") {
      code = luaL_checkstring(l, -1);
    }
    else if (key == "name") {
      name = luaL_checkstring(l, -1);
    }
    else if (key == "default") {
      bool is_default = lua_toboolean(l, -1);
      if (is_default) {
        default_language_code = language_code;
      }
    }
    else {
      luaL_error(l, (StringConcat() << "Invalid key '" << key << "'").c_str());
    }
    lua_pop(l, 1); // pop the value, let the key for the iteration
  }

  if (code.empty()) {
    luaL_error(l, "The code of this language is missing");
  }

  if (name.empty()) {
    luaL_error(l, "The name of this language is missing");
  }

  languages[code] = name;

  return 0;
}

/**
 * @brief Sets the current language.
 *
 * The language-specific data will be loaded from the directory of this language.
 * This function must be called before the first language-specific file is loaded.
 *
 * @param language_code code of the language
 */
void FileTools::set_language(const std::string& language_code) {

  Debug::check_assertion(languages[language_code].size() > 0,
      StringConcat() << "Unknown language '" << language_code << "'");
  FileTools::language_code = language_code;
  Configuration::set_value("language", language_code);
  StringResource::initialize();
  DialogResource::initialize();
}

/**
 * @brief Returns the current language.
 *
 * The language-specific data are be loaded from the directory of this language.
 *
 * @return code of the language, or an empty string if no language is set
 */
const std::string& FileTools::get_language() {
  return language_code;
}

/**
 * @brief Returns the default language.
 *
 * This default language is indicated in the languages file (languages/languages.lua).
 * It can be used to pick a language without user interaction, but you still have
 * to call set_language() otherwise no initial language is set.
 *
 * @return code of the default language, or an empty string if the languages file
 * does not specify a default language
 */
const std::string& FileTools::get_default_language() {
  return default_language_code;
}


/**
 * @brief Returns the list of available languages.
 * @return the available languages (mapping of language codes to language names)
 */
const std::map<std::string, std::string>& FileTools::get_languages() {
  return languages;
}

/**
 * @brief Returns whether a file exists in the search path.
 * @param file_name a file name relative to a directory from the search path
 * @return true if this file exists
 */
bool FileTools::data_file_exists(const std::string& file_name) {
  return PHYSFS_exists(file_name.c_str());
}

/**
 * @brief Opens in reading a text file in the Solarus data directory.
 *
 * The file name is relative to the Solarus data directory.
 * The program is stopped with an error message if the file cannot be open.
 * Don't forget to close the stream with data_file_close().
 *
 * @param file_name name of the file to open
 * @param language_specific true if the file is specific to the current language
 * @return the input stream
 */
std::istream& FileTools::data_file_open(const std::string& file_name,
    bool language_specific) {

  size_t size;
  char* buffer;
  data_file_open_buffer(file_name, &buffer, &size, language_specific);

  // create an input stream
  std::istringstream* is = new std::istringstream(std::string(buffer, size));
  data_file_close_buffer(buffer);
  return *is;
}

/**
 * @brief Closes a text file previously open with data_file_open().
 * @param data_file the input stream to close
 */
void FileTools::data_file_close(const std::istream& data_file) {
  delete &data_file;
}

/**
 * @brief Opens a data file an loads its content into a buffer.
 * @param file_name name of the file to open
 * @param buffer the buffer to load
 * @param size number of bytes to read
 * @param language_specific true if the file is specific to the current language
 */
void FileTools::data_file_open_buffer(const std::string& file_name, char** buffer,
    size_t* size, bool language_specific) {

  std::string full_file_name;
  if (language_specific) {
    full_file_name = (std::string) "languages/" + language_code + "/" + file_name;
  }
  else {
    full_file_name = file_name;
  }

  // open the file
  Debug::check_assertion(PHYSFS_exists(full_file_name.c_str()), StringConcat()
      << "Data file " << full_file_name << " does not exist");
  PHYSFS_file* file = PHYSFS_openRead(full_file_name.c_str());
  Debug::check_assertion(file != NULL, StringConcat()
      << "Cannot open data file " << full_file_name);

  // load it into memory
  *size = PHYSFS_fileLength(file);

  *buffer = new char[*size];
  Debug::check_assertion(buffer != NULL, StringConcat()
      << "Cannot allocate memory to read file " << full_file_name);

  PHYSFS_read(file, *buffer, 1, *size);
  PHYSFS_close(file);
}

/**
 * @brief Saves a buffer into a data file.
 * @param file_name name of the file to write
 * @param buffer the buffer to save
 * @param size number of bytes to write
 *
 */
void FileTools::data_file_save_buffer(const std::string& file_name,
    const char* buffer, size_t size) {

  // open the file to write
  PHYSFS_file *file = PHYSFS_openWrite(file_name.c_str());
  Debug::check_assertion(file != NULL, StringConcat()
      << "Cannot open file '" << file_name << "' for writing: "
      << PHYSFS_getLastError());
 
  // save the memory buffer 
  if (PHYSFS_write(file, buffer, size, 1) == -1) {
    Debug::die(StringConcat() << "Cannot write file '" << file_name
        << "': " << PHYSFS_getLastError());
  }
  PHYSFS_close(file);
}

/**
 * @brief Closes a data buffer previously open with data_file_open_buffer().
 * @param buffer the buffer to close
 */
void FileTools::data_file_close_buffer(char* buffer) {

  delete[] buffer;
}
 
/**
 * @brief Removes a file from the write directory.
 * @param file_name name of the file to delete
 */
void FileTools::data_file_delete(const std::string& file_name) {

  PHYSFS_delete(file_name.c_str());
}

/**
 * @brief Reads an integer value from an input stream.
 *
 * Stops the program on an error message if the read fails.
 *
 * @param is an input stream
 * @param value the value read
 */
void FileTools::read(std::istream& is, int& value) {

  if (!(is >> value)) {
    Debug::die("Cannot read integer from input stream");
  }
}

/**
 * @brief Reads an integer value from an input stream.
 *
 * Stops the program on an error message if the read fails.
 *
 * @param is an input stream
 * @param value the value read
 */
void FileTools::read(std::istream& is, uint32_t& value) {

  int v;
  read(is, v);
  Debug::check_assertion(v >= 0, "Positive integer value expected from input stream");
  value = (uint32_t) v;
}

/**
 * @brief Reads a string value from an input stream.
 *
 * Stops the program on an error message if the read fails.
 *
 * @param is an input stream
 * @param value the value read
 */
void FileTools::read(std::istream& is, std::string& value) {

  if (!(is >> value)) {
    Debug::die("Cannot read string from input stream");
  }
}

