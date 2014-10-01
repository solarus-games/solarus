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
#include "solarus/QuestResourceList.h"
#include "solarus/lowlevel/FileTools.h"
#include "solarus/lowlevel/Debug.h"
#include "solarus/lua/LuaTools.h"
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

  using ResourceMap = std::map<std::string, std::string>;
  std::map<ResourceType, ResourceMap> resource_maps;

  /**
   * \brief Implementation of the resource() function.
   * \param l The Lua state of the quest resource file.
   * \return Number of values to return to Lua.
   */
  int l_resource_element(lua_State* l) {

    return LuaTools::exception_boundary_handle(l, [&] {
      ResourceType resource_type =
          LuaTools::check_enum<ResourceType>(l, 1, resource_type_names);
      const std::string& id = LuaTools::check_string_field(l, 2, "id");
      const std::string& description = LuaTools::check_string_field(l, 2, "description");

      resource_maps[resource_type][id] = description;

      return 0;
    });
  }

}

/**
 * \brief Reads the quest resource list file data file project_db.dat and stores
 * the list of resources.
 */
void QuestResourceList::initialize() {

  // Read the quest resource list file.
  const std::string& file_name = "project_db.dat";
  lua_State* l = luaL_newstate();
  const std::string& buffer = FileTools::data_file_read(file_name);
  luaL_loadbuffer(l, buffer.data(), buffer.size(), file_name.c_str());

  // We register only one C function for all resource types.
  lua_register(l, "resource", l_resource_element);
  for (const std::string& resource_type_name: resource_type_names) {
    std::ostringstream oss;
    oss << "function " << resource_type_name << "(t) resource('"
      << resource_type_name << "', t) end";
    luaL_dostring(l, oss.str().c_str());
  }

  if (lua_pcall(l, 0, 0, 0) != 0) {
    Debug::die(std::string("Failed to load quest resource list 'project_db.dat': ") + lua_tostring(l, -1));
    lua_pop(l, 1);
  }

  lua_close(l);
}

/**
 * \brief Clears the loaded quest resource list.
 */
void QuestResourceList::quit() {

  resource_maps.clear();
}

/**
 * \brief Returns whether there exists an element with the specified ID.
 * \param resource_type A type of resource.
 * \param id The ID to look for.
 * \return \c true if there exists an element with the specified ID in this
 * resource type.
 */
bool QuestResourceList::exists(ResourceType resource_type, const std::string& id) {

  return resource_maps[resource_type].find(id) != resource_maps[resource_type].end();
}

/**
 * \brief Returns the list of element IDs of the specified resource type.
 * \param resource_type A type of resource.
 * \return The IDs of all declared element of this type, in their declaration
 * order.
 */
const std::map<std::string, std::string>&
    QuestResourceList::get_elements(ResourceType resource_type) {

  return resource_maps[resource_type];
}

/**
 * \brief Returns the name of a resource type.
 * \param resource_type A resource type.
 * \return The name of this resource type.
 */
const std::string& QuestResourceList::get_resource_type_name(ResourceType resource_type) {

  return resource_type_names[static_cast<int>(resource_type)];
}

/**
 * \brief Returns a resource type given its Lua name.
 * \param resource_type_name Lua name of a resource type. It must be valid.
 * \return The corresponding resource_type.
 */
ResourceType QuestResourceList::get_resource_type_by_name(
    const std::string& resource_type_name
) {
  for (const auto& kvp: resource_maps) {
    if (get_resource_type_name(kvp.first) == resource_type_name) {
      return kvp.first;
    }
  }

  Debug::die(std::string("Unknown resource type: ") + resource_type_name);
}

}

