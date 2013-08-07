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
#include "QuestResourceList.h"
#include "lowlevel/FileTools.h"
#include "lowlevel/Debug.h"
#include "lua/LuaContext.h"

namespace {

  const std::string resource_type_names[] = {
    "map",
    "tileset",
    "sprite",
    "music",
    "sound",
    "item",
    "enemy",
    "language",
    "",  // Sentinel for Lua.
  };

  std::vector<QuestResourceList::Element> resource_elements[QuestResourceList::RESOURCE_NB];

  /**
   * \brief Implement of the resource() function.
   * \param l The Lua state of the quest resource file.
   * \return Number of values to return to Lua.
   */
  int l_resource_element(lua_State* l) {

    QuestResourceList::ResourceType resource_type =
        LuaContext::check_enum<QuestResourceList::ResourceType>(l, 1, resource_type_names);
    const std::string& id = LuaContext::check_string_field(l, 2, "id");
    const std::string& description = LuaContext::check_string_field(l, 2, "description");

    resource_elements[resource_type].push_back(std::make_pair(id, description));

    return 0;
  }
}

/**
 * \brief Reads the quest resource list file data file quest.lua and stores
 * the list of resources.
 */
void QuestResourceList::initialize() {

  // Read the quest resource list file.
  const std::string& file_name = "project_db.dat";
  lua_State* l = luaL_newstate();
  size_t size;
  char* buffer;
  FileTools::data_file_open_buffer(file_name, &buffer, &size);
  luaL_loadbuffer(l, buffer, size, file_name.c_str());
  FileTools::data_file_close_buffer(buffer);

  // We register only one C function for all resource types.
  lua_register(l, "resource", l_resource_element);
  for (int i = 0; i < RESOURCE_NB; ++i) {
    std::ostringstream oss;
    oss << "function " << resource_type_names[i] << "(t) resource('"
      << resource_type_names[i] << "', t) end";
    luaL_dostring(l, oss.str().c_str());
  }

  if (lua_pcall(l, 0, 0, 0) != 0) {
    Debug::die(std::string("Failed to load quest resource list 'project_db.dat': ") + lua_tostring(l, -1));
    lua_pop(l, 1);
  }

  lua_close(l);
}

/**
 * \brief Destroys what was created by initialize().
 */
void QuestResourceList::quit() {

  for (int i = 0; i < RESOURCE_NB; ++i) {
    resource_elements[i].clear();
  }
}

/**
 * \brief Returns the list of element IDs of the specified resource type.
 * \param resource_type A type of resource.
 * \return The IDs of all declared element of this type.
 */
const std::vector<QuestResourceList::Element>&
    QuestResourceList::get_elements(ResourceType resource_type) {

  Debug::check_assertion(resource_type >= 0 && resource_type < RESOURCE_NB,
      "Invalid resource type");
  return resource_elements[resource_type];
}

