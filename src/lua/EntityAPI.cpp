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
#include "entities/Enemy.h"

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

  // Methods common to all entity types.
  static const luaL_Reg common_methods[] = {
      { "get_map", entity_api_get_map },
      { "get_name", entity_api_get_name },
      { "get_size", entity_api_get_size },
      { "get_origin", entity_api_get_origin },
      { "get_position", entity_api_get_position },
      { "is_enabled", entity_api_is_enabled },
      { "set_enabled", entity_api_set_enabled },
      { "remove", entity_api_remove },
      { NULL, NULL }
  };
  static const luaL_Reg common_metamethods[] = {
      { "__eq", userdata_meta_eq },
      { "__gc", userdata_meta_gc },
      { "__newindex", userdata_meta_newindex_as_table },
      { "__index", userdata_meta_index_as_table },
      { NULL, NULL }
  };
  register_functions(entity_module_name, common_methods);
  register_type(entity_module_name, common_methods,
      common_metamethods);

  // Hero.
  static const luaL_Reg hero_methods[] = {
      { "freeze", entity_hero_api_freeze },
      { "unfreeze", entity_hero_api_unfreeze },
      { "set_map", entity_hero_api_set_map },
      { "set_visible", entity_hero_api_set_visible },
      { "get_direction", entity_hero_api_get_direction },
      { "set_direction", entity_hero_api_set_direction },
      { "set_position", entity_hero_api_set_position },
      { "align_on_sensor", entity_hero_api_align_on_sensor },
      { "save_solid_ground", entity_hero_api_save_solid_ground },
      { "reset_solid_ground", entity_hero_api_reset_solid_ground },
      { "walk", entity_hero_api_walk },
      { "start_jumping", entity_hero_api_start_jumping },
      { "start_treasure", entity_hero_api_start_treasure },
      { "start_victory_sequence", entity_hero_api_start_victory_sequence },
      { "start_boomerang", entity_hero_api_start_boomerang },
      { "start_bow", entity_hero_api_start_bow },
      { "start_hookshot", entity_hero_api_start_hookshot },
      { "start_running", entity_hero_api_start_running },
      { "start_hurt", entity_hero_api_start_hurt },
      { NULL, NULL }
  };
  register_functions(entity_hero_module_name, common_methods);
  register_type(entity_hero_module_name, hero_methods,
      common_metamethods);

  // Non-playing character.
  static const luaL_Reg npc_methods[] = {
      { "start_movement", entity_npc_api_start_movement },
      { "stop_movement", entity_npc_api_stop_movement },
      { "get_sprite", entity_npc_api_get_sprite },
      { "set_position", entity_npc_api_set_position }
  };
  register_functions(entity_npc_module_name, common_methods);
  register_type(entity_npc_module_name, npc_methods,
      common_metamethods);

  // Chest.
  static const luaL_Reg chest_methods[] = {
      { "is_open", entity_chest_api_is_open },
      { "set_open", entity_chest_api_set_open },
  };
  register_functions(entity_chest_module_name, common_methods);
  register_type(entity_chest_module_name, chest_methods,
      common_metamethods);

  // Block.
  static const luaL_Reg block_methods[] = {
      { "block_reset", entity_block_api_reset },
      { "block_set_position", entity_block_api_set_position },
  };
  register_functions(entity_block_module_name, common_methods);
  register_type(entity_block_module_name, block_methods,
      common_metamethods);

  // Switch.
  static const luaL_Reg switch_methods[] = {
      { "is_activated", entity_switch_api_is_activated },
      { "set_activated", entity_switch_api_set_activated },
      { "set_locked", entity_switch_api_set_locked },
  };
  register_functions(entity_switch_module_name, common_methods);
  register_type(entity_switch_module_name, switch_methods,
      common_metamethods);

  // Door.
  static const luaL_Reg door_methods[] = {
      { "open", entity_door_api_open },
      { "close", entity_door_api_close },
      { "is_open", entity_door_api_is_open },
      { "set_open", entity_door_api_set_open },

  };
  register_functions(entity_door_module_name, common_methods);
  register_type(entity_door_module_name, door_methods,
      common_metamethods);

  // Enemy.
  static const luaL_Reg enemy_methods[] = {
      { "get_life", entity_enemy_api_get_life },
      { "set_life", entity_enemy_api_set_life },
      { "add_life", entity_enemy_api_add_life },
      { "remove_life", entity_enemy_api_remove_life },
      { "get_damage", entity_enemy_api_get_damage },
      { "set_damage", entity_enemy_api_set_damage },
      { "get_magic_damage", entity_enemy_api_get_magic_damage },
      { "set_magic_damage", entity_enemy_api_set_magic_damage },
      { "is_pushed_back_when_hurt", entity_enemy_api_is_pushed_back_when_hurt },
      { "set_pushed_back_when_hurt", entity_enemy_api_set_pushed_back_when_hurt },
      { "get_push_hero_on_sword", entity_enemy_api_get_push_hero_on_sword },
      { "set_push_hero_on_sword", entity_enemy_api_set_push_hero_on_sword },
      { "get_can_hurt_hero_running", entity_enemy_api_get_can_hurt_hero_running },
      { "set_can_hurt_hero_running", entity_enemy_api_set_can_hurt_hero_running },
      { "get_hurt_style", entity_enemy_api_get_hurt_style },
      { "set_hurt_style", entity_enemy_api_set_hurt_style },
      { "get_can_attack", entity_enemy_api_get_can_attack },
      { "set_can_attack", entity_enemy_api_set_can_attack },
      { "get_minimum_shield_needed", entity_enemy_api_get_minimum_shield_needed },
      { "set_minimum_shield_needed", entity_enemy_api_set_minimum_shield_needed },
      { "set_attack_consequence", entity_enemy_api_set_attack_consequence },
      { "set_attack_consequence_sprite", entity_enemy_api_set_attack_consequence_sprite },
      { "set_default_attack_consequences", entity_enemy_api_set_default_attack_consequences },
      { "set_default_attack_consequences_sprite", entity_enemy_api_set_default_attack_consequences_sprite },
      { "set_invincible", entity_enemy_api_set_invincible },
      { "set_invincible_sprite", entity_enemy_api_set_invincible_sprite },
      { "set_layer_independent_collisions", entity_enemy_api_set_layer_independent_collisions },
      { "set_treasure", entity_enemy_api_set_treasure },
      { "set_no_treasure", entity_enemy_api_set_no_treasure },
      { "set_random_treasure", entity_enemy_api_set_random_treasure },
      { "get_obstacle_behavior", entity_enemy_api_get_obstacle_behavior },
      { "set_obstacle_behavior", entity_enemy_api_set_obstacle_behavior },
      { "get_optimization_distance", entity_enemy_api_get_optimization_distance },
      { "set_optimization_distance", entity_enemy_api_set_optimization_distance },
      { "set_size", entity_enemy_api_set_size },
      { "set_origin", entity_enemy_api_set_origin },
      { "set_position", entity_enemy_api_set_position },
      { "get_distance_to_hero", entity_enemy_api_get_distance_to_hero },
      { "get_angle_to_hero", entity_enemy_api_get_angle_to_hero },
      { "test_obstacles", entity_enemy_api_test_obstacles },
      { "snap_to_grid", entity_enemy_api_snap_to_grid },
      { "get_movement", entity_enemy_api_get_movement },
      { "start_movement", entity_enemy_api_start_movement },
      { "stop_movement", entity_enemy_api_stop_movement },
      { "restart", entity_enemy_api_restart },
      { "hurt", entity_enemy_api_hurt },
      { "create_sprite", entity_enemy_api_create_sprite },
      { "remove_sprite", entity_enemy_api_remove_sprite },
      { "get_sprite", entity_enemy_api_get_sprite },
      { "create_son", entity_enemy_api_create_son },
      { "get_father", entity_enemy_api_get_father },
      { "send_message", entity_enemy_api_send_message },
      { NULL, NULL }
  };
  register_functions(entity_enemy_module_name, common_methods);
  register_type(entity_enemy_module_name, enemy_methods,
      common_metamethods);

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

/**
 * @brief Implementation of \ref lua_api_entity_get_map.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::entity_api_get_map(lua_State* l) {

  MapEntity& entity = check_entity(l, 1);

  push_map(l, entity.get_map());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_entity_get_name.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::entity_api_get_name(lua_State* l) {

  MapEntity& entity = check_entity(l, 1);

  lua_pushstring(l, entity.get_name().c_str());
  return 1;
}


/**
 * @brief Implementation of \ref lua_api_entity_get_size.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::entity_api_get_size(lua_State* l) {

  MapEntity& entity = check_entity(l, 1);

  lua_pushinteger(l, entity.get_width());
  lua_pushinteger(l, entity.get_height());
  return 2;
}

/**
 * @brief Implementation of \ref lua_api_entity_get_origin.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::entity_api_get_origin(lua_State* l) {

  MapEntity& entity = check_entity(l, 1);

  const Rectangle& origin = entity.get_origin();

  lua_pushinteger(l, origin.get_x());
  lua_pushinteger(l, origin.get_y());
  return 2;
}

/**
 * @brief Implementation of \ref lua_api_entity_get_position
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::entity_api_get_position(lua_State* l) {

  MapEntity& entity = check_entity(l, 1);

  lua_pushinteger(l, entity.get_x());
  lua_pushinteger(l, entity.get_y());
  lua_pushinteger(l, entity.get_layer());
  return 3;
}

/**
 * @brief Implementation of \ref lua_api_entity_is_enabled.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::entity_api_is_enabled(lua_State* l) {

  MapEntity& entity = check_entity(l, 1);

  lua_pushboolean(l, entity.is_enabled());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_entity_is_enabled.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::entity_api_set_enabled(lua_State* l) {

  MapEntity& entity = check_entity(l, 1);
  bool enabled = true;  // true if unspecified.
  if (lua_isboolean(l, 2)) {
    enabled = lua_toboolean(l, 2);
  }

  entity.set_enabled(enabled);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_entity_remove.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::entity_api_remove(lua_State* l) {

  MapEntity& entity = check_entity(l, 1);

  entity.remove_from_map();

  return 0;
}

/**
 * @brief Checks that the userdata at the specified index of the stack is an
 * enemy and returns it.
 * @param l A Lua context.
 * @param index An index in the stack.
 * @return The enemy.
 */
Enemy& LuaContext::check_enemy(lua_State* l, int index) {
  return static_cast<Enemy&>(check_userdata(l, index, entity_enemy_module_name));
}

/**
 * @brief Pushes an enemy userdata onto the stack.
 * @param l A Lua context.
 * @param enemy An enemy.
 */
void LuaContext::push_enemy(lua_State* l, Enemy& enemy) {
  push_userdata(l, enemy);
}


