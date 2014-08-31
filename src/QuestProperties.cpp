/*
 * Copyright (C) 2006-2014 Christopho, Solarus - http://www.solarus-games.org
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
#include "QuestProperties.h"
#include "lowlevel/FileTools.h"
#include "lowlevel/Video.h"
#include "lowlevel/Rectangle.h"
#include "lowlevel/Debug.h"
#include "lua/LuaTools.h"
#include <lua.hpp>
#include <sstream>
#include <string>

namespace solarus {

namespace {

/**
 * \brief Version of the quest.
 */
std::string solarus_required_version;

/**
 * \brief Checks that the quest is compatible with the current version of
 * Solarus.
 * \param solarus_required_version Version of the quest.
 */
void check_version_compatibility(const std::string& solarus_required_version) {

  if (solarus_required_version.empty()) {
    Debug::die("No Solarus version is specified in your quest.dat file!");
  }

  // TODO check the syntax of the version string

  int dot_index_1 = solarus_required_version.find('.');
  std::istringstream iss(solarus_required_version.substr(0, dot_index_1));
  int required_major_version = 0;
  iss >> required_major_version;

  int dot_index_2 = solarus_required_version.find('.', dot_index_1 + 1);
  std::istringstream iss2(solarus_required_version.substr(dot_index_1 + 1, dot_index_2));
  int required_minor_version = 0;
  iss2 >> required_minor_version;

  // The third digit of the version (patch) is ignored because compatibility
  // is not broken by patches.

  // For now, we assume that any mismatch of major or minor version (first
  // and second digits) breaks compatibility and we just die.
  // This may change in the future, because all minor versions won't
  // necessarily break compatibility.
  if (required_major_version != SOLARUS_MAJOR_VERSION ||
      required_minor_version != SOLARUS_MINOR_VERSION) {
    std::ostringstream oss;
    oss << "This quest is made for Solarus " << required_major_version << "."
        << required_minor_version << ".x but you are running Solarus "
        << SOLARUS_VERSION;
    Debug::die(oss.str());
  }
}

}

/**
 * \brief Creates the quest properties loader.
 * \param main_loop The quest main loop.
 */
QuestProperties::QuestProperties(MainLoop& /* main_loop */) {

}

/**
 * \brief Destructor.
 */
QuestProperties::~QuestProperties() {

}

/**
 * \brief Reads the quest properties data file quest.dat and applies
 * these properties to the quest.
 */
void QuestProperties::load() {

  // Read the quest properties file.
  const std::string file_name("quest.dat");
  lua_State* l = luaL_newstate();
  size_t size = 0;
  char* buffer = nullptr;
  FileTools::data_file_open_buffer(file_name, &buffer, &size);
  int load_result = luaL_loadbuffer(l, buffer, size, file_name.c_str());

  if (load_result != 0) {
    // Syntax error in quest.dat.
    Debug::die(std::string("Failed to load quest.dat: ") + lua_tostring(l, -1));
  }
  else {

    lua_register(l, "quest", l_quest);
    if (lua_pcall(l, 0, 0, 0) != 0) {
      // Loading quest.dat failed.
      // There may be a syntax error, or this is a quest for Solarus 0.9.
      // There was no version number at that time.

      if (std::string(buffer).find("[info]")) {
        // Quest format of Solarus 0.9.
        Debug::die(std::string("This quest is made for Solarus 0.9 but you are running Solarus ")
            + SOLARUS_VERSION);
      }
      else {
        // Runtime error in quest.dat.
        Debug::die(std::string("Failed to parse quest.dat: ") + lua_tostring(l, -1));
      }
      lua_pop(l, 1);
    }
  }

  FileTools::data_file_close_buffer(buffer);
  lua_close(l);
}

int QuestProperties::l_quest(lua_State* l) {

  // Retrieve the quest properties from the table parameter.
  luaL_checktype(l, 1, LUA_TTABLE);
  const std::string& solarus_required_version =
      LuaTools::opt_string_field(l, 1, "solarus_version", "");
  check_version_compatibility(solarus_required_version);
  const std::string& quest_write_dir =
      LuaTools::opt_string_field(l, 1, "write_dir", "");
  const std::string& title_bar =
      LuaTools::opt_string_field(l, 1, "title_bar", "");
  const std::string& normal_quest_size_string =
      LuaTools::opt_string_field(l, 1, "normal_quest_size", "320x240");
  const std::string& min_quest_size_string =
      LuaTools::opt_string_field(l, 1, "min_quest_size", normal_quest_size_string);
  const std::string& max_quest_size_string =
      LuaTools::opt_string_field(l, 1, "max_quest_size", normal_quest_size_string);

  FileTools::set_quest_write_dir(quest_write_dir);
  if (!title_bar.empty()) {
    Video::set_window_title(title_bar);
  }

  Rectangle normal_quest_size, min_quest_size, max_quest_size;
  bool success = Video::parse_size(normal_quest_size_string, normal_quest_size);
  if (!success) {
    LuaTools::arg_error(l, 1, std::string(
        "Bad field 'normal_quest_size' (not a valid size string: '")
        + normal_quest_size_string + "')");
  }

  success = Video::parse_size(min_quest_size_string, min_quest_size);
  if (!success) {
    LuaTools::arg_error(l, 1, std::string(
        "Bad field 'min_quest_size' (not a valid size string: '")
        + min_quest_size_string + "')");
  }

  success = Video::parse_size(max_quest_size_string, max_quest_size);
  if (!success) {
    LuaTools::arg_error(l, 1, std::string(
        "Bad field 'max_quest_size' (not a valid size string: '")
        + max_quest_size_string + "')");
  }

  if (normal_quest_size.get_width() < min_quest_size.get_width()
      || normal_quest_size.get_height() < min_quest_size.get_height()
      || normal_quest_size.get_width() > max_quest_size.get_width()
      || normal_quest_size.get_height() > max_quest_size.get_height()) {
    LuaTools::arg_error(l, 1, "Invalid range of quest sizes");
  }

  Video::set_quest_size_range(
      normal_quest_size,
      min_quest_size,
      max_quest_size);

  return 0;
}

}

