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
#include <fstream>
#include <sstream>

namespace Solarus {

namespace {

  const std::map<ResourceType, std::string> resource_type_names = {
      { ResourceType::MAP, "map" },
      { ResourceType::TILESET, "tileset" },
      { ResourceType::SPRITE, "sprite" },
      { ResourceType::MUSIC, "music" },
      { ResourceType::SOUND, "sound" },
      { ResourceType::ITEM, "item" },
      { ResourceType::ENEMY, "enemy" },
      { ResourceType::ENTITY, "entity" },
      { ResourceType::LANGUAGE, "language" },
      { ResourceType::FONT, "font" }
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
          LuaTools::check_enum(l, 1, resource_type_names);
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
 * \brief Adds a resource element to the list.
 * \param resource_type A type of resource.
 * \param id Id of the element to add.
 * \param description Description the element to add.
 * \return \c true if the element was added, \c false if an element with
 * this id already exists.
 */
bool QuestResources::add(
    ResourceType resource_type,
    const std::string& id,
    const std::string& description
) {
  ResourceMap& resource = get_elements(resource_type);
  auto result = resource.insert(std::make_pair(id, description));
  return result.second;
}

/**
 * \brief Removes a resource element from the list.
 * \param resource_type A type of resource.
 * \param id Id of the element to remove.
 * \return \c true if the element was removed, \c false if such an element
 * did not exist.
 */
bool QuestResources::remove(
    ResourceType resource_type,
    const std::string& id
) {
  ResourceMap& resource = get_elements(resource_type);
  return resource.erase(id) > 0;
}

/**
 * \brief Changes the id of a resource element from the list.
 * \param resource_ A type of resource.
 * \param old_id Id of the element to change.
 * \param new_id The new id to set.
 * \return \c true in case of success, \c false if the old id does not
 * exist or if the new id already exists.
 */
bool QuestResources::rename(
    ResourceType resource_type,
    const std::string& old_id,
    const std::string& new_id
) {
  if (!exists(resource_type, old_id)) {
    return false;
  }
  if (exists(resource_type, new_id)) {
    return false;
  }
  const std::string& description = get_description(resource_type, old_id);
  remove(resource_type, old_id);
  add(resource_type, new_id, description);
  return true;
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
 * \return \c true in case of success, \c false if such an element does not
 * exist.
 */
bool QuestResources::set_description(
    ResourceType resource_type,
    const std::string& id,
    const std::string& description
) {
  if (!exists(resource_type, id)) {
    return false;
  }

  ResourceMap& resource = get_elements(resource_type);
  resource[id] = description;
  return true;
}

/**
 * \brief Returns the list of existing resource types and their names.
 * \return The resource type names.
 */
const std::map<ResourceType, std::string>& QuestResources::get_resource_type_names() {
  return resource_type_names;
}

/**
 * \brief Returns the name of a resource type.
 * \param resource_type A resource type.
 * \return The name of this resource type.
 */
const std::string& QuestResources::get_resource_type_name(ResourceType resource_type) {

  return resource_type_names.at(resource_type);
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
  for (const auto& kvp: resource_type_names) {
    if (kvp.second == resource_type_name) {
      return kvp.first;
    }
    ++i;
  }

  Debug::die(std::string("Unknown resource type: ") + resource_type_name);
  return ResourceType();
}

/**
 * \brief Loads resources from a project_db.dat chunk.
 * \param l A Lua state with the chunk loaded.
 * \return \c true in case of success, \c false if there was a Lua error
 * while executing the chunk.
 */
bool QuestResources::import_from_lua(lua_State* l) {

  lua_pushlightuserdata(l, this);
  lua_setfield(l, LUA_REGISTRYINDEX, "resources");

  // We register only one C function for all resource types.
  lua_register(l, "resource", l_resource_element);
  for (const auto& kvp: resource_type_names) {
    std::ostringstream oss;
    oss << "function " << kvp.second << "(t) resource('"
      << kvp.second << "', t) end";
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
 * \brief Saves this quest resource list into a stream.
 * \param out The stream to write.
 * \return \c true in case of success, \c false if the data
 * could not be exported.
 */
bool QuestResources::export_to_lua(std::ostream& out) const {

  // Save each resource.
  for (const auto& kvp: resource_type_names) {

    const ResourceMap& resource = get_elements(kvp.first);
    for (const auto& element: resource) {

      const std::string& id = element.first;
      const std::string& description = element.second;

      out << kvp.second
          << "{ id = \""
          << id
          << "\", description = \""
          << description
          << "\" }\n";
    }
    out << "\n";
  }

  return true;
}

}

