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
#include "solarus/lowlevel/Debug.h"
#include "solarus/lowlevel/FileTools.h"
#include "solarus/lua/LuaTools.h"
#include "solarus/QuestResources.h"
#include <map>
#include <sstream>

namespace Solarus {

namespace {

  const std::vector<std::string> resource_type_names = {
    "map",
    "tileset",
    "sprite",
    "music",
    "sound",
    "item",
    "enemy",
    "entity",
    "language",
    "font"
  };

  /**
   * \brief Implementation of the resource() function.
   * \param l The Lua state of the quest resource file.
   * \return Number of values to return to Lua.
   */
  int l_resource_element(lua_State* l) {

    return LuaTools::exception_boundary_handle(l, [&] {

      lua_getfield(l, LUA_REGISTRYINDEX, "resources");
      QuestResources& resources = *(static_cast<QuestResources*>(
          lua_touserdata(l, -1)
      ));
      lua_pop(l, 1);

      ResourceType resource_type =
          LuaTools::check_enum<ResourceType>(l, 1, resource_type_names);
      const std::string& id = LuaTools::check_string_field(l, 2, "id");
      const std::string& description = LuaTools::check_string_field(l, 2, "description");

      resources.add(resource_type, id, description);

      return 0;
    });
  }

}

/**
 * \brief Creates an empty quest resources object.
 */
QuestResources::QuestResources() {

  for (size_t i = 0 ; i < resource_type_names.size(); ++i) {
    resource_maps.insert(std::make_pair(static_cast<ResourceType>(i), ResourceMap()));
  }
}

/**
 * \brief Loads a quest resource list file from memory.
 * \param buffer A memory area with the content of a project_db.dat file.
 * \return \c true in case of success, \c false if the file could not be loaded.
 */
bool QuestResources::load_from_buffer(const std::string& buffer) {

  // Read the quest resource list file.
  lua_State* l = luaL_newstate();
  if (luaL_loadbuffer(l, buffer.data(), buffer.size(), "project_db.dat") != 0) {
    Debug::error(std::string("Failed to load quest resource list 'project_db.dat': ") + lua_tostring(l, -1));
    lua_pop(l, 1);
    return false;
  }

  bool success = parse(l);
  lua_close(l);
  return success;
}

/**
 * \brief Loads a quest resource list from the filesystem.
 * \param file_name Path of the file to load.
 * \return \c true in case of success, \c false if the file could not be loaded.
 */
bool QuestResources::load_from_file(const std::string& file_name) {

  lua_State* l = luaL_newstate();
  if (luaL_loadfile(l, file_name.c_str()) != 0) {
    Debug::error(std::string("Failed to load quest resource list '") + file_name + "': " + lua_tostring(l, -1));
    lua_pop(l, 1);
    return false;
  }

  bool success = parse(l);
  lua_close(l);
  return success;
}

/**
 * \brief Loads resources from a project_db.dat chunk.
 * \param l A Lua state with the chunk loaded.
 * \return \c true in case of success, \c false if there was a Lua error
 * while executing the chunk.
 */
bool QuestResources::parse(lua_State* l) {

  lua_pushlightuserdata(l, this);
  lua_setfield(l, LUA_REGISTRYINDEX, "resources");

  // We register only one C function for all resource types.
  lua_register(l, "resource", l_resource_element);
  for (const std::string& resource_type_name: resource_type_names) {
    std::ostringstream oss;
    oss << "function " << resource_type_name << "(t) resource('"
      << resource_type_name << "', t) end";
    luaL_dostring(l, oss.str().c_str());
  }

  if (lua_pcall(l, 0, 0, 0) != 0) {
    Debug::error(std::string("Failed to load quest resource list 'project_db.dat': ") + lua_tostring(l, -1));
    lua_pop(l, 1);
    return false;
  }

  return true;
}

/**
 * \brief Removes all resource elements.
 */
void QuestResources::clear() {

  for (size_t i = 0 ; i < resource_type_names.size(); ++i) {
    resource_maps[static_cast<ResourceType>(i)].clear();
  }
}

/**
 * \brief Returns whether there exists an element with the specified id.
 * \param resource_type A type of resource.
 * \param id The id to look for.
 * \return \c true if there exists an element with the specified id in this
 * resource type.
 */
bool QuestResources::exists(ResourceType resource_type, const std::string& id) const {

  const ResourceMap& resource = get_elements(resource_type);
  return resource.find(id) != resource.end();
}

/**
 * \brief Returns the list of element IDs of the specified resource type.
 * \param resource_type A type of resource.
 * \return The ids of all declared element of this type
 */
const QuestResources::ResourceMap& QuestResources::get_elements(
    ResourceType resource_type) const {

  return resource_maps.find(resource_type)->second;
}

/**
 * \brief Returns the list of element IDs of the specified resource type.
 * \param resource_type A type of resource.
 * \return The ids of all declared element of this type
 */
QuestResources::ResourceMap& QuestResources::get_elements(
    ResourceType resource_type) {

  return resource_maps.find(resource_type)->second;
}

/**
 * \brief Adds a resource element.
 * \param resource_type A type of resource.
 * \param id Id of the element to add.
 * \param description Description the element to add.
 */
void QuestResources::add(
    ResourceType resource_type,
    const std::string& id,
    const std::string& description
) {
  ResourceMap& resource = get_elements(resource_type);
  resource.insert(std::make_pair(id, description));
}

/**
 * \brief Returns the description of a resource element.
 * \param resource_type A type of resource.
 * \param id Id of the element to get.
 * \return description The element description.
 * Returns an empty string if the element does not exist.
 */
std::string QuestResources::get_description(
    ResourceType resource_type,
    const std::string& id
) const {

  const ResourceMap& resource = get_elements(resource_type);

  const auto& it = resource.find(id);
  if (it == resource.end()) {
    return "";
  }
  return it->second;
}

/**
 * \brief Sets the description of a resource element.
 * \param resource_type A type of resource.
 * \param id Id of the element to modify.
 * \param description The new description.
 */
void QuestResources::set_description(
    ResourceType resource_type,
    const std::string& id,
    const std::string& description
) {
  ResourceMap& resource = get_elements(resource_type);
  resource[id] = description;
}

/**
 * \brief Returns the name of a resource type.
 * \param resource_type A resource type.
 * \return The name of this resource type.
 */
const std::string& QuestResources::get_resource_type_name(ResourceType resource_type) {

  return resource_type_names[static_cast<int>(resource_type)];
}

/**
 * \brief Returns a resource type given its Lua name.
 * \param resource_type_name Lua name of a resource type. It must be valid.
 * \return The corresponding resource_type.
 */
ResourceType QuestResources::get_resource_type_by_name(
    const std::string& resource_type_name
) {
  int i = 0;
  for (const std::string& name: resource_type_names) {
    if (name == resource_type_name) {
      return static_cast<ResourceType>(i);
    }
    ++i;
  }

  Debug::die(std::string("Unknown resource type: ") + resource_type_name);
  return ResourceType();
}

}

