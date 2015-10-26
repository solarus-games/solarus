/*
 * Copyright (C) 2006-2015 Christopho, Solarus - http://www.solarus-games.org
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
#include "solarus/lowlevel/QuestFiles.h"
#include "solarus/lowlevel/Size.h"
#include "solarus/lowlevel/Video.h"
#include "solarus/lua/LuaTools.h"
#include <lua.hpp>
#include <ostream>

namespace Solarus {

namespace {

/**
 * \brief Function called by Lua when parsing the quest{ } part of the file.
 */
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
 * \copydoc LuaData::import_from_lua
 */
bool QuestProperties::import_from_lua(lua_State* l) {

  lua_pushlightuserdata(l, this);
  lua_pushcclosure(l, l_quest, 1);
  lua_setglobal(l, "quest");
  if (lua_pcall(l, 0, 0, 0) != 0) {
    // Runtime error in quest.dat.
    Debug::error(std::string("Failed to parse quest.dat: ") + lua_tostring(l, -1));
    return false;
  }

  return true;
}

/**
 * \copydoc LuaData::export_to_lua
 */
bool QuestProperties::export_to_lua(std::ostream& out) const {

  out << "quest{\n"
      << "  solarus_version = \"" << solarus_version << "\",\n"
      << "  write_dir = \"" << escape_string(quest_write_dir) << "\",\n"
      << "  title_bar = \"" << escape_string(title_bar) << "\",\n"
      << "  normal_quest_size = \"" << normal_quest_size.width << 'x' << normal_quest_size.height << "\",\n"
      << "  min_quest_size = \"" << min_quest_size.width << 'x' << min_quest_size.height << "\",\n"
      << "  max_quest_size = \"" << max_quest_size.width << 'x' << max_quest_size.height << "\",\n"
      << "}\n";

  return true;
}

/**
 * \brief Returns the Solarus compatibility version of the quest.
 * \return The "solarus_version" value.
 */
std::string QuestProperties::get_solarus_version() const {
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
std::string QuestProperties::get_quest_write_dir() const {
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
std::string QuestProperties::get_title_bar() const {
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
Size QuestProperties::get_normal_quest_size() const {
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
Size QuestProperties::get_min_quest_size() const {
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
Size QuestProperties::get_max_quest_size() const {
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
