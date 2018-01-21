/*
 * Copyright (C) 2006-2018 Christopho, Solarus - http://www.solarus-games.org
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
#include "solarus/core/QuestProperties.h"
#include "solarus/core/Debug.h"
#include "solarus/core/QuestFiles.h"
#include "solarus/core/Size.h"
#include "solarus/graphics/Video.h"
#include "solarus/lua/LuaTools.h"
#include <lua.hpp>
#include <ostream>
#include <sstream>

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
        LuaTools::check_string_field(l, 1, "solarus_version");
    const std::string& quest_write_dir =
        LuaTools::opt_string_field(l, 1, "write_dir", "");

    // Accept the old title_bar value (1.4 and before) as a quest title
    // so that the quest.dat file of 1.4 quests and earlier
    // is still recognized, even though the quest cannot be run.
    const std::string& title_bar =
        LuaTools::opt_string_field(l, 1, "title_bar", "");
    const std::string& title =
        LuaTools::opt_string_field(l, 1, "title", title_bar);

    const std::string& short_description =
        LuaTools::opt_string_field(l, 1, "short_description", "");
    std::string long_description =
        LuaTools::opt_string_field(l, 1, "long_description", "");
    long_description = LuaData::unescape_multiline_string(long_description);
    const std::string& author =
        LuaTools::opt_string_field(l, 1, "author", "");
    const std::string& quest_version =
        LuaTools::opt_string_field(l, 1, "quest_version", "");
    const std::string& release_date =
        LuaTools::opt_string_field(l, 1, "release_date", "");
    const std::string& website =
        LuaTools::opt_string_field(l, 1, "website", "");

    const std::string& normal_quest_size_string =
        LuaTools::opt_string_field(l, 1, "normal_quest_size", "320x240");
    const std::string& min_quest_size_string =
        LuaTools::opt_string_field(l, 1, "min_quest_size", normal_quest_size_string);
    const std::string& max_quest_size_string =
        LuaTools::opt_string_field(l, 1, "max_quest_size", normal_quest_size_string);

    properties.set_solarus_version(solarus_version);
    properties.set_quest_write_dir(quest_write_dir);
    properties.set_title(title);
    properties.set_short_description(short_description);
    properties.set_long_description(long_description);
    properties.set_author(author);
    properties.set_website(website);
    properties.set_quest_version(quest_version);
    properties.set_release_date(release_date);

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
      << "  title = \"" << escape_string(title) << "\",\n"
      << "  short_description = \"" << escape_string(short_description) << "\",\n"
      << "  long_description = [[\n" << escape_multiline_string(long_description) << "]],\n"
      << "  author = \"" << escape_string(author) << "\",\n"
      << "  quest_version = \"" << escape_string(quest_version) << "\",\n"
      << "  release_date = \"" << escape_string(release_date) << "\",\n"
      << "  website = \"" << escape_string(website) << "\",\n"
      << "  normal_quest_size = \"" << normal_quest_size.width << 'x' << normal_quest_size.height << "\",\n"
      << "  min_quest_size = \"" << min_quest_size.width << 'x' << min_quest_size.height << "\",\n"
      << "  max_quest_size = \"" << max_quest_size.width << 'x' << max_quest_size.height << "\",\n"
      << "}\n\n";

  return true;
}

/**
 * \brief Returns the Solarus compatibility version of the quest.
 * \return The major and minor Solarus version number of the quest.
 * Returns \c { 0, 0 } if unset.
 */
std::pair<int, int> QuestProperties::get_solarus_version_major_minor() const {

  const std::string& quest_version = get_solarus_version();
  if (quest_version.empty()) {
    return { 0, 0 };
  }

  // TODO check the syntax of the version string

  int dot_index_1 = quest_version.find('.');
  std::istringstream iss(quest_version.substr(0, dot_index_1));
  int quest_major_version = 0;
  iss >> quest_major_version;

  int dot_index_2 = quest_version.find('.', dot_index_1 + 1);
  std::istringstream iss2(quest_version.substr(dot_index_1 + 1, dot_index_2));
  int quest_minor_version = 0;
  iss2 >> quest_minor_version;

  return { quest_major_version, quest_minor_version };
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
 * \brief Returns the title of the quest.
 * \return The "title" value.
 */
std::string QuestProperties::get_title() const {
  return title;
}

/**
 * \brief Sets the title of quest.
 * \param title The "title" value.
 */
void QuestProperties::set_title(const std::string& title) {
  this->title = title;
}

/**
 * \brief Returns the one-line description of the quest.
 * \return The "short_description" value.
 */
std::string QuestProperties::get_short_description() const {
  return short_description;
}

/**
 * \brief Sets the one-line description of the quest.
 * \param short_description The "short_description" value.
 */
void QuestProperties::set_short_description(const std::string& short_description) {
  this->short_description = short_description;
}

/**
 * \brief Returns the longer description of the quest.
 * \return The "long_description" value.
 */
std::string QuestProperties::get_long_description() const {
  return long_description;
}

/**
 * \brief Sets the longer description of the quest.
 * \param long_description The "long_description" value.
 */
void QuestProperties::set_long_description(const std::string& long_description) {
  this->long_description = long_description;
}

/**
 * \brief Returns the author of the quest.
 * \return The "author" value.
 */
std::string QuestProperties::get_author() const {
  return author;
}

/**
 * \brief Sets the author of the quest.
 * \param author The "author" value.
 */
void QuestProperties::set_author(const std::string& author) {
  this->author = author;
}

/**
 * \brief Returns the version of the quest.
 * \return The "quest_version" value.
 */
std::string QuestProperties::get_quest_version() const {
  return quest_version;
}

/**
 * \brief Sets the version of the quest.
 * \param quest_version The "quest_version" value.
 */
void QuestProperties::set_quest_version(const std::string& quest_version) {
  this->quest_version = quest_version;
}

/**
 * \brief Returns the release date of the quest.
 * \return The "release_date" value.
 */
std::string QuestProperties::get_release_date() const {
  return release_date;
}

/**
 * \brief Sets the release date of the quest.
 * \param release_date The "release_date" value.
 */
void QuestProperties::set_release_date(const std::string& release_date) {
  this->release_date = release_date;
}

/**
 * \brief Returns the website of the quest.
 * \return The "website" value.
 */
std::string QuestProperties::get_website() const {
  return website;
}

/**
 * \brief Sets the website of the quest.
 * \param website The "website" value.
 */
void QuestProperties::set_website(const std::string& website) {
  this->website = website;
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
