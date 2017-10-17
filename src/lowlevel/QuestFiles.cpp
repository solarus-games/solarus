/*
 * Copyright (C) 2006-2016 Christopho, Solarus - http://www.solarus-games.org
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
#include "solarus/lowlevel/Debug.h"
#include "solarus/lowlevel/QuestFiles.h"
#include "solarus/lua/LuaContext.h"
#include "solarus/Arguments.h"
#include "solarus/CurrentQuest.h"
#include "solarus/QuestProperties.h"
#include <physfs.h>
#include <fstream>
#include <cstdlib>  // exit(), mkstemp(), tmpnam()
#include <cstdio>   // remove()
#ifdef HAVE_UNISTD_H
#  include <unistd.h>  // close()
#endif

#if defined(SOLARUS_OSX) || defined(SOLARUS_IOS)
#   include "solarus/lowlevel/apple/AppleInterface.h"
#endif

namespace Solarus {

namespace QuestFiles {

namespace {

/**
 * \brief Path of the data/ directory, the data.solarus archive
 * or the data.solarus.zip archive,
 * absolute or relative to the current directory. */
std::string quest_path_;

/**
 * \brief Directory where the engine can write files,
 * relative to the user's home.
 */
std::string solarus_write_dir_;

/**<
 * \brief Write directory of the current quest,
 * relative to solarus_write_dir.
 */
std::string quest_write_dir_;

/**
 * \brief Name of all temporary files created.
 */
std::vector<std::string> temporary_files_;

/**
 * \brief Sets the directory where the engine can write files.
 *
 * Initially, this directory is set to the preprocessor constant
 * SOLARUS_WRITE_DIR (by default ".solarus").
 * You normally don't need to change this, it should have been set correctly
 * at compilation time to a value that depends on the target system.
 *
 * \param solarus_write_dir The directory where the engine can write files,
 * relative to the base write directory.
 */
void set_solarus_write_dir(const std::string& solarus_write_dir) {

  // This setting never changes at runtime.
  // Allowing to change it would be complex and we don't need that.
  Debug::check_assertion(solarus_write_dir_.empty(),
      "The Solarus write directory is already set");

  solarus_write_dir_ = solarus_write_dir;

  // First check that we can write in a directory.
  if (!PHYSFS_setWriteDir(get_base_write_dir().c_str())) {
     Debug::die(std::string("Cannot write in user directory '")
         + get_base_write_dir().c_str() + "': " + PHYSFS_getLastError());
  }

  // Create the directory.
  PHYSFS_mkdir(solarus_write_dir.c_str());

  const std::string& full_write_dir = get_base_write_dir() + "/" + solarus_write_dir;
  if (!PHYSFS_setWriteDir(full_write_dir.c_str())) {
    Debug::die(std::string("Cannot set Solarus write directory to '")
        + full_write_dir + "': " + PHYSFS_getLastError());
  }

  // The quest subdirectory may be new, create it if needed.
  if (!quest_write_dir_.empty()) {
    set_quest_write_dir(quest_write_dir_);
  }
}

} // Anonymous namespace

/**
 * \brief Opens a quest.
 *
 * Only one quest can be open at a time.
 * If a quest is already open, it is automatically closed.
 *
 * \param program_name Program name passed as first command-line argument.
 * Necessary for the quest file management system.
 * \param quest_path Path of the quest to open.
 * \return \c true if the quest was found.
 */
SOLARUS_API bool open_quest(const std::string& program_name, const std::string& quest_path) {

  if (is_open()) {
    close_quest();
  }

  if (program_name.empty()) {
    PHYSFS_init(nullptr);
  }
  else {
    PHYSFS_init(program_name.c_str());
  }

  quest_path_ = quest_path;
  std::string dir_quest_path = quest_path + "/data";
  std::string archive_quest_path_1 = quest_path + "/data.solarus";
  std::string archive_quest_path_2 = quest_path + "/data.solarus.zip";

  const std::string& base_dir = PHYSFS_getBaseDir();
  PHYSFS_addToSearchPath(dir_quest_path.c_str(), 1);   // data directory
  PHYSFS_addToSearchPath(archive_quest_path_1.c_str(), 1); // data.solarus archive
  PHYSFS_addToSearchPath(archive_quest_path_2.c_str(), 1); // data.solarus.zip archive
  PHYSFS_addToSearchPath((base_dir + "/" + dir_quest_path).c_str(), 1);
  PHYSFS_addToSearchPath((base_dir + "/" + archive_quest_path_1).c_str(), 1);
  PHYSFS_addToSearchPath((base_dir + "/" + archive_quest_path_2).c_str(), 1);

  // Set the engine root write directory.
  set_solarus_write_dir(SOLARUS_WRITE_DIR);

  if (!quest_exists()) {
    return false;
  }

  // Set the quest write directory.
  CurrentQuest::initialize();
  set_quest_write_dir(CurrentQuest::get_properties().get_quest_write_dir());

  return true;
}

/**
 * \brief Closes the quest.
 */
SOLARUS_API void close_quest() {

  if (!is_open()) {
    return;
  }

  CurrentQuest::quit();

  remove_temporary_files();

  quest_path_ = "";
  solarus_write_dir_ = "";
  quest_write_dir_ = "";

  PHYSFS_deinit();
}

/**
 * @brief Returns whether a quest is open.
 * @return @c true if a quest is open, even if missing.
 */
SOLARUS_API bool is_open() {

  return !quest_path_.empty();
}

/**
 * \brief Returns the path of the quest, relative to the current directory.
 * \return Path of the data/ directory, the data.solarus archive or the
 * data.solarus.zip archive, relative to the current directory.
 */
SOLARUS_API const std::string& get_quest_path() {
  return quest_path_;
}

/**
 * @brief Returns whether there is a quest in the directory QuestFiles
 * were initialized with.
 *
 * A quest is considered to be found if there is at a least quest.dat file.
 *
 * @return @c true if the quest exists.
 */
SOLARUS_API bool quest_exists() {

  return data_file_exists("quest.dat");
}

/**
 * \brief Returns the physical location of a data file.
 *
 * This function is not often necessary since the whole point of this class
 * is to access files independently of their real file location
 * (in the quest write directory, in the data directory or in the data archive).
 *
 * \param file_name The file to look for.
 * \return Where it is actually located, or LOCATION_NONE if not found.
 */
SOLARUS_API DataFileLocation data_file_get_location(
    const std::string& file_name) {

  const char* path_ptr = PHYSFS_getRealDir(file_name.c_str());
  std::string path = path_ptr == nullptr ? "" : path_ptr;
  if (path.empty()) {
    // File does not exist.
    return DataFileLocation::LOCATION_NONE;
  }

  if (!get_quest_write_dir().empty() && path == PHYSFS_getWriteDir()) {
    return DataFileLocation::LOCATION_WRITE_DIRECTORY;
  }

  if (path.rfind("data") == path.size() - 4) {
    return DataFileLocation::LOCATION_DATA_DIRECTORY;
  }

  if (path.rfind("data.solarus") == path.size() - 12
      || path.rfind("data.solarus.zip") == path.size() - 16) {
    return DataFileLocation::LOCATION_DATA_ARCHIVE;
  }

  Debug::die(std::string("Unexpected search path element: " + path));
}

/**
 * \brief Returns whether a file exists in the quest data directory or
 * in Solarus write directory, and is not a directory.
 * \param file_name A file name relative to the quest data directory,
 * to the current language directory or to Solarus write directory.
 * \param language_specific \c true if the file is relative to the current
 * language directory.
 * \return \c true if this file exists and is not a directory.
 */
SOLARUS_API bool data_file_exists(const std::string& file_name,
    bool language_specific) {

  std::string full_file_name;
  if (language_specific) {
    if (CurrentQuest::get_language().empty()) {
      return false;
    }
    full_file_name = std::string("languages/") +
        CurrentQuest::get_language() + "/" + file_name;
  }
  else {
    full_file_name = file_name;
  }

  return PHYSFS_exists(full_file_name.c_str()) && !PHYSFS_isDirectory(full_file_name.c_str());
}

/**
 * \brief Opens a data file an loads its content into memory.
 * \param file_name Name of the file to open.
 * \param language_specific \c true if the file is specific to the current language.
 * \return The content of the file.
 */
SOLARUS_API std::string data_file_read(
    const std::string& file_name,
    bool language_specific
) {
  std::string full_file_name;
  if (language_specific) {
    Debug::check_assertion(!CurrentQuest::get_language().empty(),
        std::string("Cannot open language-specific file '") + file_name
        + "': no language was set"
    );
    full_file_name = std::string("languages/") +
        CurrentQuest::get_language() + "/" + file_name;
  }
  else {
    full_file_name = file_name;
  }

  // open the file
  Debug::check_assertion(PHYSFS_exists(full_file_name.c_str()),
      std::string("Data file '") + full_file_name + "' does not exist"
  );
  PHYSFS_file* file = PHYSFS_openRead(full_file_name.c_str());
  Debug::check_assertion(file != nullptr,
      std::string("Cannot open data file '") + full_file_name + "'"
  );

  // load it into memory
  size_t size =  static_cast<size_t>(PHYSFS_fileLength(file));
  std::vector<char> buffer(size);

  PHYSFS_read(file, buffer.data(), 1, (PHYSFS_uint32) size);
  PHYSFS_close(file);

  return std::string(buffer.data(), size);
}

/**
 * \brief Saves a buffer into a data file.
 * \param file_name Name of the file to write, relative to Solarus write directory.
 * \param buffer The buffer to save.
 *
 */
SOLARUS_API void data_file_save(
    const std::string& file_name,
    const std::string& buffer
) {
  // open the file to write
  PHYSFS_file* file = PHYSFS_openWrite(file_name.c_str());
  if (file == nullptr) {
    Debug::die(std::string("Cannot open file '") + file_name
        + "' for writing: " + PHYSFS_getLastError()
    );
  }

  // save the memory buffer
  if (PHYSFS_write(file, buffer.data(), (PHYSFS_uint32) buffer.size(), 1) == -1) {
    Debug::die(std::string("Cannot write file '") + file_name + "': "
        + PHYSFS_getLastError());
  }
  PHYSFS_close(file);
}

/**
 * \brief Removes a file from the write directory.
 * \param file_name Name of the file to delete, relative to the Solarus
 * write directory.
 * \return \c true in case of success.
 */
SOLARUS_API bool data_file_delete(const std::string& file_name) {

  if (!PHYSFS_delete(file_name.c_str())) {
    return false;
  }

  return true;
}

/**
 * \brief Creates a directory in the write directory.
 * \param dir_name Name of the directory to delete, relative to the Solarus
 * write directory.
 * \return \c true in case of success.
 */
SOLARUS_API bool data_file_mkdir(const std::string& dir_name) {

  if (!PHYSFS_mkdir(dir_name.c_str())) {
    return false;
  }

  return true;
}

/**
 * \brief Enumerate files of a directory.
 *
 * Symbolic links are never returned.
 *
 * \param dir_path Name of the directory to list, relative to the quest data
 * directory.
 * \param list_files Whether regular files should be included in the result.
 * \param list_directories Whether directories should be included in the result.
 * \return The files in this directory.
 */
SOLARUS_API std::vector<std::string> data_files_enumerate(
    const std::string& dir_path,
    bool list_files,
    bool list_directories
) {

  std::vector<std::string> result;

  if (PHYSFS_exists(dir_path.c_str())) {
    char** files = PHYSFS_enumerateFiles(dir_path.c_str());

    for (char** file = files; *file != nullptr; file++) {
      bool is_directory = PHYSFS_isDirectory((dir_path + "/" + *file).c_str());

      if (!PHYSFS_isSymbolicLink(*file)
          && ((list_files && !is_directory)
              || (list_directories && is_directory))) {
        result.push_back(std::string(*file));
      }
    }

    PHYSFS_freeList(files);
  }

  return result;
}

/**
 * \brief Returns the directory where the engine can write files.
 * \returns The directory where the engine can write files, relative to the
 * base write directory.
 */
SOLARUS_API const std::string& get_solarus_write_dir() {
  return solarus_write_dir_;
}

/**
 * \brief Returns the subdirectory where files specific to the quest are
 * saved, like savegames and configuration files.
 * \return The quest write directory, relative to the Solarus write directory,
 * or an empty string if it has not been set yet.
 */
SOLARUS_API const std::string& get_quest_write_dir() {
  return quest_write_dir_;
}

/**
 * \brief Sets the subdirectory where files specific to the quest are
 * saved, like savegames and configuration files.
 *
 * You have to call this function before loading or saving savegames and
 * configuration files.
 * This directory should typically be named like your quest, to be sure other
 * quests will not interfere.
 *
 * \param quest_write_dir The quest write directory, relative to the Solarus
 * write directory.
 */
SOLARUS_API void set_quest_write_dir(const std::string& quest_write_dir) {

  if (!quest_write_dir_.empty()) {
    // There was already a previous quest subdirectory: remove it from the
    // search path.
    PHYSFS_removeFromSearchPath(PHYSFS_getWriteDir());
  }

  quest_write_dir_ = quest_write_dir;

  // Reset the write directory to the Solarus directory
  // so that we can create the new quest subdirectory.
  std::string full_write_dir = get_base_write_dir() + "/" + solarus_write_dir_;
  if (!PHYSFS_setWriteDir(full_write_dir.c_str())) {
    Debug::die(std::string("Cannot set Solarus write directory to '")
        + full_write_dir + "': " + PHYSFS_getLastError());
  }

  if (!quest_write_dir.empty()) {
    // Create the quest subdirectory (if not existing)
    // in the Solarus write directory.
    PHYSFS_mkdir(quest_write_dir.c_str());

    // Set the write directory to this new place.
    full_write_dir = get_base_write_dir() + "/" + solarus_write_dir_ + "/" + quest_write_dir;
    PHYSFS_setWriteDir(full_write_dir.c_str());

    // Also allow the quest to read savegames, settings and data files there.
    PHYSFS_addToSearchPath(PHYSFS_getWriteDir(), 0);
  }
}

/**
 * \brief Returns the absolute path of the quest write directory.
 */
SOLARUS_API std::string get_full_quest_write_dir() {
  return get_base_write_dir() + "/" + get_solarus_write_dir() + "/" + get_quest_write_dir();
}

/**
 * \brief Returns the privileged base write directory, depending on the OS.
 * \return The base write directory.
 */
SOLARUS_API std::string get_base_write_dir() {

#if defined(SOLARUS_OSX) || defined(SOLARUS_IOS)
  return get_user_application_support_directory();
#else
  return std::string(PHYSFS_getUserDir());
#endif
}

/**
 * \brief Creates a temporary file with the specified content and closes it.
 * \param content Content of the file to create.
 * \return Full name of the file created, or an empty string in case of failure.
 */
SOLARUS_API std::string create_temporary_file(const std::string& content) {

  // Determine the name of our temporary file.
  std::string file_name;

#ifdef HAVE_MKSTEMP
  // mkstemp+close is safer than tmpnam, but POSIX only.
  char name_template[] = "/tmp/solarus.XXXXXX";
  int file_descriptor = mkstemp(name_template);
  if (file_descriptor == -1) {
    // Failure.
    return "";
  }
  close(file_descriptor);
  file_name = name_template;
#else
  file_name = std::tmpnam(nullptr);
#endif

  std::ofstream out(file_name.c_str());

  if (!out) {
    return "";
  }

  // File successfully created.
  temporary_files_.push_back(file_name);

  if (!content.empty()) {
    out.write(content.data(), content.size());
    if (!out) {
      file_name = "";
    }
  }

  out.close();

  return file_name;
}

/**
 * \brief Deletes all files previously created with create_temporary_file().
 * \return \c true in case of success, \c false if at least one file could not
 * be deleted.
 */
SOLARUS_API bool remove_temporary_files() {

  bool success = true;
  for (const std::string& file_name: temporary_files_) {
    if (std::remove(file_name.c_str()) != 0) {
      success = false;
    }
  }
  temporary_files_.clear();

  return success;
}

}  // namespace QuestFiles

}  // namespace Solarus

