/*
 * Copyright (C) 2006-2012 Christopho, Solarus - http://www.solarus-games.org
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
#include <lua.hpp>
#include "lua/LuaContext.h"
#include "entities/MapEntity.h"

const std::string LuaContext::entity_module_name = "sol.entity";
const std::string LuaContext::entity_hero_module_name = "sol.entity.hero";
const std::string LuaContext::entity_npc_module_name = "sol.entity.npc";
const std::string LuaContext::entity_chest_module_name = "sol.entity.chest";
const std::string LuaContext::entity_block_module_name = "sol.entity.block";
const std::string LuaContext::entity_switch_module_name = "sol.entity.switch";
const std::string LuaContext::entity_door_module_name = "sol.entity.door";

/**
 * @brief Initializes the map entity features provided to Lua.
 */
void LuaContext::register_entity_module() {

  // TODO
}

/**
 * @brief Checks that the userdata at the specified index of the stack is an
 * entity and returns it.
 * @param l A Lua context.
 * @param index An index in the stack.
 * @return The entity.
 */
MapEntity& LuaContext::check_entity(lua_State* l, int index) {

  MapEntity** entity = NULL;

  if (is_userdata(l, index, entity_module_name)
      || is_userdata(l, index, entity_hero_module_name)
      || is_userdata(l, index, entity_npc_module_name)
      || is_userdata(l, index, entity_chest_module_name)
      || is_userdata(l, index, entity_block_module_name)
      || is_userdata(l, index, entity_switch_module_name)
      || is_userdata(l, index, entity_door_module_name)
      || is_userdata(l, index, entity_enemy_module_name)) {
    entity = static_cast<MapEntity**>(lua_touserdata(l, index));
  }
  else {
    luaL_typerror(l, index, "entity");
  }

  return **entity;
}

/**
 * @brief Pushes an enemy userdata onto the stack.
 * @param l A Lua context.
 * @param enemy An enemy.
 */
void LuaContext::push_entity(lua_State* l, MapEntity& entity) {
  push_userdata(l, entity);
}


