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
#include "solarus/QuestProperties.h"
#include "solarus/lowlevel/Debug.h"
#include "solarus/lowlevel/FileTools.h"
#include "solarus/lowlevel/Size.h"
#include "solarus/lowlevel/Video.h"
#include "solarus/lua/LuaTools.h"
#include <lua.hpp>
#include <string>

namespace Solarus {

namespace {

int l_quest(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {

    QuestProperties& properties = *(static_cast<QuestProperties*>(
        lua_touserdata(l, lua_upvalueindex(1))
    ));

    // Retrieve the quest properties from the table parameter.
    LuaTools::check_type(l, 1, LUA_TTABLE);
    const std::string& solarus_version =
        LuaTools::opt_string_field(l, 1, "solarus_version", "");
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

    properties.set_solarus_version(solarus_version);
    properties.set_quest_write_dir(quest_write_dir);
    properties.set_title_bar(title_bar);

    Size normal_quest_size, min_quest_size, max_quest_size;
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

    if (normal_quest_size.width < min_quest_size.width
        || normal_quest_size.height < min_quest_size.height
        || normal_quest_size.width > max_quest_size.width
        || normal_quest_size.height > max_quest_size.height) {
      LuaTools::arg_error(l, 1, "Invalid range of quest sizes");
    }

    properties.set_normal_quest_size(normal_quest_size);
    properties.set_min_quest_size(min_quest_size);
    properties.set_max_quest_size(max_quest_size);

    return 0;
  });
}

}

/**
 * \brief Creates quest properties.
 */
QuestProperties::QuestProperties() {
}

/**
 * \brief Load properties from a quest.dat.
 */
void QuestProperties::load() {

  // Read the quest properties file.
  const std::string file_name("quest.dat");
  lua_State* l = luaL_newstate();
  const std::string& buffer = FileTools::data_file_read(file_name);
  int load_result = luaL_loadbuffer(l, buffer.data(), buffer.size(), file_name.c_str());

  if (load_result != 0) {
    // Syntax error in quest.dat.
    // Loading quest.dat failed.
    // There may be a syntax error, or this is a quest for Solarus 0.9.
    // There was no version number at that time.

    if (std::string(buffer).find("[info]")) {
      // Quest format of Solarus 0.9.
      Debug::die(std::string("This quest is made for Solarus 0.9 but you are running Solarus ")
          + SOLARUS_VERSION);
    }
    else {
      Debug::die(std::string("Failed to load quest.dat: ") + lua_tostring(l, -1));
    }
  }
  else {
    lua_pushlightuserdata(l, this);
    lua_pushcclosure(l, l_quest, 1);
    lua_setglobal(l, "quest");
    if (lua_pcall(l, 0, 0, 0) != 0) {
      // Runtime error in quest.dat.
      Debug::die(std::string("Failed to parse quest.dat: ") + lua_tostring(l, -1));
    }
    lua_pop(l, 1);
  }

  lua_close(l);
}

/**
 * \brief Returns the Solarus compatibility version of the quest.
 * \return The "solarus_version" value.
 */
std::string QuestProperties::get_solarus_version() {
  return solarus_version;
}

/**
 * \brief Sets the Solarus compatibility version of the quest.
 * \param solarus_version The "solarus_version" value.
 */
void QuestProperties::set_solarus_version(const std::string& solarus_version) {
  this->solarus_version = solarus_version;
}

/**
 * \brief Returns the quest write directory.
 * \return The "write_dir" value.
 */
std::string QuestProperties::get_quest_write_dir() {
  return quest_write_dir;
}

/**
 * \brief Sets the quest write directory.
 * \param quest_write_dir The "write_dir" value.
 */
void QuestProperties::set_quest_write_dir(const std::string& quest_write_dir) {
  this->quest_write_dir = quest_write_dir;
}

/**
 * \brief Returns the title of the game window.
 * \return The "title_bar" value.
 */
std::string QuestProperties::get_title_bar() {
  return title_bar;
}

/**
 * \brief Sets the title of game windows for the quest.
 * \param title_bar The "title_bar" value.
 */
void QuestProperties::set_title_bar(const std::string& title_bar) {
  this->title_bar = title_bar;
}

/**
 * \brief Returns the default quest size.
 * \return The "normal_quest_size" value.
 */
Size QuestProperties::get_normal_quest_size() {
  return normal_quest_size;
}

/**
 * \brief Sets the default quest size.
 * \param normal_quest_size The "normal_quest_size" value.
 */
void QuestProperties::set_normal_quest_size(const Size& normal_quest_size) {
  this->normal_quest_size = normal_quest_size;
}

/**
 * \brief Returns the minimum quest size.
 * \return The "min_quest_size" value.
 */
Size QuestProperties::get_min_quest_size() {
  return min_quest_size;
}

/**
 * \brief Sets the minimum quest size.
 * \param min_quest_size The "min_quest_size" value.
 */
void QuestProperties::set_min_quest_size(const Size& min_quest_size) {
  this->min_quest_size = min_quest_size;
}

/**
 * \brief Returns the maximum quest size.
 * \return The "max_quest_size" value.
 */
Size QuestProperties::get_max_quest_size() {
  return max_quest_size;
}

/**
 * \brief Sets the minimum quest size.
 * \param max_quest_size The "max_quest_size" value.
 */
void QuestProperties::set_max_quest_size(const Size& max_quest_size) {
  this->max_quest_size = max_quest_size;
}

}
