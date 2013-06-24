/*
 * Copyright (C) 2006-2013 Christopho, Solarus - http://www.solarus-games.org
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
#include "lowlevel/VideoManager.h"
#include "lowlevel/Rectangle.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"
#include "lua/LuaContext.h"
#include <lua.hpp>
#include <sstream>

namespace {

/**
 * @brief Version of the quest.
 */
std::string solarus_required_version;

/**
 * @brief Checks that the quest is compatible with the current version of Solarus.
 */
void check_version_compatibility() {

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
    Debug::die(StringConcat() <<
        "This quest is made for Solarus " << required_major_version << "."
        << required_minor_version << ".x but you are running Solarus "
        << SOLARUS_VERSION);
  }
}

}

/**
 * @brief Creates the quest properties loader.
 * @param main_loop The quest main loop.
 */
QuestProperties::QuestProperties(MainLoop& main_loop):
  main_loop(main_loop) {

}

/**
 * @brief Destructor.
 */
QuestProperties::~QuestProperties() {

}

/**
 * @brief Reads the quest properties data file quest.lua and applies
 * these properties to the quest.
 */
void QuestProperties::load() {

  // Read the quest properties file.
  const std::string& file_name = "quest.dat";
  lua_State* l = luaL_newstate();
  size_t size;
  char* buffer;
  FileTools::data_file_open_buffer(file_name, &buffer, &size);
  luaL_loadbuffer(l, buffer, size, file_name.c_str());
  FileTools::data_file_close_buffer(buffer);

  lua_register(l, "quest", l_quest);
  if (lua_pcall(l, 0, 0, 0) != 0) {
    Debug::die(StringConcat() << "Error: failed to load quest.dat: "
        << lua_tostring(l, -1));
    lua_pop(l, 1);
  }

  check_version_compatibility();

  lua_close(l);
}

int QuestProperties::l_quest(lua_State* l) {

  // Retrieve the quest properties from the table parameter.
  luaL_checktype(l, 1, LUA_TTABLE);
  solarus_required_version = LuaContext::opt_string_field(l, 1, "solarus_version", "");
  const std::string& quest_write_dir =
      LuaContext::opt_string_field(l, 1, "write_dir", "");
  const std::string& title_bar =
      LuaContext::opt_string_field(l, 1, "title_bar", "");
  const std::string& normal_quest_size_string =
      LuaContext::opt_string_field(l, 1, "normal_quest_size", "320x240");
  const std::string& min_quest_size_string =
      LuaContext::opt_string_field(l, 1, "min_quest_size", normal_quest_size_string);
  const std::string& max_quest_size_string =
      LuaContext::opt_string_field(l, 1, "max_quest_size", normal_quest_size_string);

  FileTools::set_quest_write_dir(quest_write_dir);
  if (!title_bar.empty()) {
    VideoManager::get_instance()->set_window_title(title_bar);
  }

  Rectangle normal_quest_size, min_quest_size, max_quest_size;
  bool success = VideoManager::parse_size(normal_quest_size_string, normal_quest_size);
  if (!success) {
    luaL_argerror(l, 1,
        "Bad field 'normal_quest_size' (not a valid size string)");
  }

  success = VideoManager::parse_size(min_quest_size_string, min_quest_size);
  if (!success) {
    luaL_argerror(l, 1,
        "Bad field 'min_quest_size' (not a valid size string)");
  }

  success = VideoManager::parse_size(max_quest_size_string, max_quest_size);
  if (!success) {
    luaL_argerror(l, 1,
        "Bad field 'max_quest_size' (not a valid size string)");
  }

  VideoManager::get_instance()->set_quest_size_range(
      normal_quest_size,
      min_quest_size,
      max_quest_size);

  return 0;
}

