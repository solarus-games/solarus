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
const std::string LuaContext::entity_enemy_module_name = "sol.entity.enemy";

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
      { "freeze", hero_api_freeze },
      { "unfreeze", hero_api_unfreeze },
      { "set_map", hero_api_set_map },
      { "set_visible", hero_api_set_visible },
      { "get_direction", hero_api_get_direction },
      { "set_direction", hero_api_set_direction },
      { "set_position", hero_api_set_position },
      { "align_on_sensor", hero_api_align_on_sensor },
      { "save_solid_ground", hero_api_save_solid_ground },
      { "reset_solid_ground", hero_api_reset_solid_ground },
      { "walk", hero_api_walk },
      { "start_jumping", hero_api_start_jumping },
      { "start_treasure", hero_api_start_treasure },
      { "start_victory", hero_api_start_victory},
      { "start_boomerang", hero_api_start_boomerang },
      { "start_bow", hero_api_start_bow },
      { "start_hookshot", hero_api_start_hookshot },
      { "start_running", hero_api_start_running },
      { "start_hurt", hero_api_start_hurt },
      { NULL, NULL }
  };
  register_functions(entity_hero_module_name, common_methods);
  register_type(entity_hero_module_name, hero_methods,
      common_metamethods);

  // Non-playing character.
  static const luaL_Reg npc_methods[] = {
      { "start_movement", npc_api_start_movement },
      { "stop_movement", npc_api_stop_movement },
      { "get_sprite", npc_api_get_sprite },
      { "set_position", npc_api_set_position }
  };
  register_functions(entity_npc_module_name, common_methods);
  register_type(entity_npc_module_name, npc_methods,
      common_metamethods);

  // Chest.
  static const luaL_Reg chest_methods[] = {
      { "is_open", chest_api_is_open },
      { "set_open", chest_api_set_open },
  };
  register_functions(entity_chest_module_name, common_methods);
  register_type(entity_chest_module_name, chest_methods,
      common_metamethods);

  // Block.
  static const luaL_Reg block_methods[] = {
      { "block_reset", block_api_reset },
      { "block_set_position", block_api_set_position },
  };
  register_functions(entity_block_module_name, common_methods);
  register_type(entity_block_module_name, block_methods,
      common_metamethods);

  // Switch.
  static const luaL_Reg switch_methods[] = {
      { "is_activated", switch_api_is_activated },
      { "set_activated", switch_api_set_activated },
      { "set_locked", switch_api_set_locked },
  };
  register_functions(entity_switch_module_name, common_methods);
  register_type(entity_switch_module_name, switch_methods,
      common_metamethods);

  // Door.
  static const luaL_Reg door_methods[] = {
      { "open", door_api_open },
      { "close", door_api_close },
      { "is_open", door_api_is_open },
      { "set_open", door_api_set_open },

  };
  register_functions(entity_door_module_name, common_methods);
  register_type(entity_door_module_name, door_methods,
      common_metamethods);

  // Enemy.
  static const luaL_Reg enemy_methods[] = {
      { "get_life", enemy_api_get_life },
      { "set_life", enemy_api_set_life },
      { "add_life", enemy_api_add_life },
      { "remove_life", enemy_api_remove_life },
      { "get_damage", enemy_api_get_damage },
      { "set_damage", enemy_api_set_damage },
      { "get_magic_damage", enemy_api_get_magic_damage },
      { "set_magic_damage", enemy_api_set_magic_damage },
      { "is_pushed_back_when_hurt", enemy_api_is_pushed_back_when_hurt },
      { "set_pushed_back_when_hurt", enemy_api_set_pushed_back_when_hurt },
      { "get_push_hero_on_sword", enemy_api_get_push_hero_on_sword },
      { "set_push_hero_on_sword", enemy_api_set_push_hero_on_sword },
      { "get_can_hurt_hero_running", enemy_api_get_can_hurt_hero_running },
      { "set_can_hurt_hero_running", enemy_api_set_can_hurt_hero_running },
      { "get_hurt_style", enemy_api_get_hurt_style },
      { "set_hurt_style", enemy_api_set_hurt_style },
      { "get_can_attack", enemy_api_get_can_attack },
      { "set_can_attack", enemy_api_set_can_attack },
      { "get_minimum_shield_needed", enemy_api_get_minimum_shield_needed },
      { "set_minimum_shield_needed", enemy_api_set_minimum_shield_needed },
      { "set_attack_consequence", enemy_api_set_attack_consequence },
      { "set_attack_consequence_sprite", enemy_api_set_attack_consequence_sprite },
      { "set_default_attack_consequences", enemy_api_set_default_attack_consequences },
      { "set_default_attack_consequences_sprite", enemy_api_set_default_attack_consequences_sprite },
      { "set_invincible", enemy_api_set_invincible },
      { "set_invincible_sprite", enemy_api_set_invincible_sprite },
      { "set_layer_independent_collisions", enemy_api_set_layer_independent_collisions },
      { "set_treasure", enemy_api_set_treasure },
      { "set_no_treasure", enemy_api_set_no_treasure },
      { "set_random_treasure", enemy_api_set_random_treasure },
      { "get_obstacle_behavior", enemy_api_get_obstacle_behavior },
      { "set_obstacle_behavior", enemy_api_set_obstacle_behavior },
      { "get_optimization_distance", enemy_api_get_optimization_distance },
      { "set_optimization_distance", enemy_api_set_optimization_distance },
      { "set_size", enemy_api_set_size },
      { "set_origin", enemy_api_set_origin },
      { "set_position", enemy_api_set_position },
      { "get_distance_to_hero", enemy_api_get_distance_to_hero },
      { "get_angle_to_hero", enemy_api_get_angle_to_hero },
      { "test_obstacles", enemy_api_test_obstacles },
      { "snap_to_grid", enemy_api_snap_to_grid },
      { "get_movement", enemy_api_get_movement },
      { "start_movement", enemy_api_start_movement },
      { "stop_movement", enemy_api_stop_movement },
      { "restart", enemy_api_restart },
      { "hurt", enemy_api_hurt },
      { "create_sprite", enemy_api_create_sprite },
      { "remove_sprite", enemy_api_remove_sprite },
      { "get_sprite", enemy_api_get_sprite },
      { "create_son", enemy_api_create_son },
      { "get_father", enemy_api_get_father },
      { "send_message", enemy_api_send_message },
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
  bool enabled = true;
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
 * @brief Checks that the userdata at the specified index of the stack is a
 * hero and returns it.
 * @param l A Lua context.
 * @param index An index in the stack.
 * @return The hero.
 */
Hero& LuaContext::check_hero(lua_State* l, int index) {
  return static_cast<Hero&>(check_userdata(l, index, entity_hero_module_name));
}

/**
 * @brief Pushes a hero userdata onto the stack.
 * @param l A Lua context.
 * @param hero A hero.
 */
void LuaContext::push_hero(lua_State* l, Hero& hero) {
  push_userdata(l, hero);
}

/**
 * @brief Implementation of \ref lua_api_hero_set_map.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::hero_api_set_map(lua_State* l) {

  Hero& hero = check_hero(l, 1);
  MapId map_id = luaL_checkinteger(l, 2);
  const std::string& destination_name = luaL_checkstring(l, 3);
  Transition::Style transition_style = Transition::Style(luaL_checkinteger(l, 4));
  // TODO give names to transitions for Lua

  hero.get_game().set_current_map(map_id, destination_name, transition_style);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_hero_set_visible.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::hero_api_set_visible(lua_State* l) {

  Hero& hero = check_hero(l, 1);
  bool visible = true;
  if (lua_isboolean(l, 2)) {
    visible = lua_toboolean(l, 2);
  }

  hero.set_visible(visible);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_hero_get_direction.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::hero_api_get_direction(lua_State* l) {

  Hero& hero = check_hero(l, 1);

  lua_pushinteger(l, hero.get_animation_direction());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_hero_set_direction.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::hero_api_set_direction(lua_State* l) {

  Hero& hero = check_hero(l, 1);
  int direction = luaL_checkinteger(l, 2);

  hero.set_animation_direction(direction);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_hero_set_position.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::hero_api_set_position(lua_State* l) {

  Hero& hero = check_hero(l, 1);
  int x = luaL_checkinteger(l, 2);
  int y = luaL_checkinteger(l, 3);
  int layer = -1;
  if (lua_gettop(l) >= 4) {
    layer = luaL_checkinteger(l, 4);
  }

  Hero& hero = map.get_entities().get_hero();
  hero.set_xy(x, y);
  if (layer != -1) {
    map.get_entities().set_entity_layer(hero, Layer(layer));
  }
  hero.check_position();

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_hero_save_solid_ground.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::hero_api_save_solid_ground(lua_State* l) {

  Hero& hero = check_hero(l, 1);
  int x, y, layer;
  if (lua_gettop(l) >= 2) {
    x = luaL_checkinteger(l, 2);
    y = luaL_checkinteger(l, 3);
    layer = luaL_checkinteger(l, 4);
  }
  else {
    x = hero.get_x();
    y = hero.get_y();
    layer = hero.get_layer();
  }

  hero.set_target_solid_ground_coords(Rectangle(x, y), Layer(layer));

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_hero_reset_solid_ground.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::hero_api_reset_solid_ground(lua_State* l) {

  Hero& hero = check_hero(l, 1);

  hero.reset_target_solid_ground_coords();

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_hero_freeze.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::hero_api_freeze(lua_State* l) {

  Hero& hero = check_hero(l, 1);

  hero.start_freezed();

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_hero_unfreeze.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::hero_api_unfreeze(lua_State* l) {

  Hero& hero = check_hero(l, 1);

  hero.start_free();

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_hero_walk.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::hero_api_walk(lua_State* l) {

  Hero& hero = check_hero(l, 1);
  const std::string& path = luaL_checkstring(l, 2);
  bool loop = lua_toboolean(l, 3) != 0;
  bool ignore_obstacles = lua_toboolean(l, 4) != 0;

  hero.start_forced_walking(path, loop, ignore_obstacles);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_hero_start_jumping.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::hero_api_start_jumping(lua_State* l) {

  Hero& hero = check_hero(l, 1);
  int direction = luaL_checkinteger(l, 2);
  int length = luaL_checkinteger(l, 3);
  bool ignore_obstacles = lua_toboolean(l, 4) != 0;

  hero.start_jumping(direction, length, ignore_obstacles, false);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_hero_start_treasure.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::hero_api_start_treasure(lua_State* l) {

  Hero& hero = check_hero(l, 1);
  const std::string &item_name = luaL_checkstring(l, 2);
  int variant = luaL_checkinteger(l, 3);
  int savegame_variable = luaL_checkinteger(l, 4);

  hero.start_treasure(
      Treasure(hero.get_game(), item_name, variant, savegame_variable));

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_hero_start_victory.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::hero_api_start_victory(lua_State* l) {

  Hero& hero = check_hero(l, 1);

  hero.start_victory();

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_hero_start_boomerang.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::hero_api_start_boomerang(lua_State* l) {

  Hero& hero = check_hero(l, 1);
  int max_distance = luaL_checkinteger(l, 2);
  int speed = luaL_checkinteger(l, 3);
  const std::string& tunic_preparing_animation = luaL_checkstring(l, 4);
  const std::string& sprite_name = luaL_checkstring(l, 5);

  hero.start_boomerang(max_distance, speed,
      tunic_preparing_animation, sprite_name);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_hero_start_bow.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::hero_api_start_bow(lua_State* l) {

  Hero& hero = check_hero(l, 1);

  hero.start_bow();

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_hero_start_hookshot.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::hero_api_start_hookshot(lua_State* l) {

  Hero& hero = check_hero(l, 1);

  hero.start_hookshot();

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_hero_start_running.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::hero_api_start_running(lua_State* l) {

  Hero& hero = check_hero(l, 1);

  hero.start_running();

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_hero_start_hurt.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::hero_api_start_hurt(lua_State* l) {

  Hero& hero = check_hero(l, 1);
  int source_x = luaL_checkinteger(l, 2);
  int source_y = luaL_checkinteger(l, 3);
  int life_points = luaL_checkinteger(l, 4);
  int magic_points = luaL_checkinteger(l, 5);

  hero.hurt(Rectangle(source_x, source_y),
      life_points, magic_points);

  return 0;
}

/**
 * @brief Checks that the userdata at the specified index of the stack is an
 * NPC and returns it.
 * @param l A Lua context.
 * @param index An index in the stack.
 * @return The NPC.
 */
NPC& LuaContext::check_npc(lua_State* l, int index) {
  return static_cast<NPC&>(check_userdata(l, index, entity_npc_module_name));
}

/**
 * @brief Pushes an NPC userdata onto the stack.
 * @param l A Lua context.
 * @param npc An NPC.
 */
void LuaContext::push_npc(lua_State* l, NPC& npc) {
  push_userdata(l, npc);
}

/**
 * @brief Implementation of \ref lua_api_npc_start_movement.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::npc_api_start_movement(lua_State* l) {

  NPC& npc = check_npc(l, 1);
  Movement& movement = check_movement(l, 2);

  movement.set_suspended(false);
  npc.clear_movement();
  npc.set_movement(&movement);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_npc_stop_movement.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::npc_api_stop_movement(lua_State* l) {

  NPC& npc = check_npc(l, 1);

  npc.clear_movement();

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_npc_set_position.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::npc_api_set_position(lua_State* l) {

  NPC& npc = check_npc(l, 1);
  int x = luaL_checkinteger(l, 2);
  int y = luaL_checkinteger(l, 3);
  int layer = -1;
  if (lua_gettop(l) >= 4) {
    layer = luaL_checkinteger(l, 4);
  }

  npc.set_xy(x, y);
  if (layer != -1) {
    npc.get_entities().set_entity_layer(*npc, Layer(layer));
  }

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_npc_get_sprite.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::npc_api_get_sprite(lua_State* l) {

  NPC& npc = check_npc(l, 1);

  push_sprite(l, npc.get_sprite());
  return 1;
}

/**
 * @brief Checks that the userdata at the specified index of the stack is a
 * chest and returns it.
 * @param l A Lua context.
 * @param index An index in the stack.
 * @return The chest.
 */
Chest& LuaContext::check_chest(lua_State* l, int index) {
  return static_cast<Chest&>(check_userdata(l, index, entity_chest_module_name));
}

/**
 * @brief Pushes a chest userdata onto the stack.
 * @param l A Lua context.
 * @param chest A chest.
 */
void LuaContext::push_chest(lua_State* l, Chest& chest) {
  push_userdata(l, chest);
}

/**
 * @brief Implementation of \ref lua_api_chest_is_open.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::chest_api_is_open(lua_State* l) {

  Chest& chest = check_chest(l, 1);

  lua_pushboolean(l, chest.is_open());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_chest_set_open.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::chest_api_set_open(lua_State* l) {

  Chest& chest = check_chest(l, 1);
  bool open = true;
  if (lua_isboolean(l, 2)) {
    open = lua_toboolean(l, 2);
  }

  chest.set_open(open);

  return 0;
}

/**
 * @brief Checks that the userdata at the specified index of the stack is a
 * block and returns it.
 * @param l A Lua context.
 * @param index An index in the stack.
 * @return The block.
 */
Block& LuaContext::check_block(lua_State* l, int index) {
  return static_cast<Block&>(check_userdata(l, index, entity_block_module_name));
}

/**
 * @brief Pushes a block userdata onto the stack.
 * @param l A Lua context.
 * @param block A block.
 */
void LuaContext::push_block(lua_State* l, Block& block) {
  push_userdata(l, block);
}

/**
 * @brief Implementation of \ref lua_api_block_reset.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::block_api_reset(lua_State* l) {

  Block& block = check_block(l, 1);

  block.reset();

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_block_set_position.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::block_api_set_position(lua_State* l) {

  Block& block = check_block(l, 1);
  int x = luaL_checkinteger(l, 2);
  int y = luaL_checkinteger(l, 3);
  int layer = -1;
  if (lua_gettop(l) >= 4) {
    layer = luaL_checkinteger(l, 4);
  }

  block.set_xy(x, y);

  if (layer != -1) {
    block.get_entities().set_entity_layer(*block, Layer(layer));
  }
  block.check_collision_with_detectors(false);

  return 0;
}

/**
 * @brief Checks that the userdata at the specified index of the stack is a
 * switch and returns it.
 * @param l A Lua context.
 * @param index An index in the stack.
 * @return The switch.
 */
Switch& LuaContext::check_switch(lua_State* l, int index) {
  return static_cast<Switch&>(check_userdata(l, index, entity_switch_module_name));
}

/**
 * @brief Pushes a switch userdata onto the stack.
 * @param l A Lua context.
 * @param sw A switch.
 */
void LuaContext::push_switch(lua_State* l, Switch& sw) {
  push_userdata(l, sw);
}

/**
 * @brief Implementation of \ref lua_api_switch_is_activated.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::switch_api_is_activated(lua_State* l) {

  Switch& sw = check_switch(l, 1);

  lua_pushboolean(l, sw.is_activated());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_switch_set_activated.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::switch_api_set_activated(lua_State* l) {

  Switch& sw = check_switch(l, 1);
  bool activated = true;
  if (lua_isboolean(l, 2)) {
    activated = lua_toboolean(l, 2);
  }

  sw.set_activated(activated);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_switch_set_locked.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::switch_api_set_locked(lua_State* l) {

  Switch& sw = check_switch(l, 1);
  bool locked = true;
  if (lua_isboolean(l, 2)) {
    locked = lua_toboolean(l, 2);
  }

  sw.set_locked(lock);

  return 0;
}

/**
 * @brief Checks that the userdata at the specified index of the stack is a
 * door and returns it.
 * @param l A Lua context.
 * @param index An index in the stack.
 * @return The door.
 */
Door& LuaContext::check_door(lua_State* l, int index) {
  return static_cast<Door&>(check_userdata(l, index, entity_door_module_name));
}

/**
 * @brief Pushes a door userdata onto the stack.
 * @param l A Lua context.
 * @param door A door.
 */
void LuaContext::push_door(lua_State* l, Door& door) {
  push_userdata(l, door);
}

/**
 * @brief Implementation of \ref lua_api_door_is_open.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::door_api_is_open(lua_State* l) {

  Door& door = check_door(l, 1);

  lua_pushboolean(l, door.is_open());
  return 1;
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


