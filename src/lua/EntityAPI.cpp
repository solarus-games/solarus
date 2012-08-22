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
#include "entities/Hero.h"
#include "entities/NPC.h"
#include "entities/Chest.h"
#include "entities/Block.h"
#include "entities/Switch.h"
#include "entities/Door.h"
#include "entities/Enemy.h"
#include "entities/Sensor.h"
#include "entities/ShopItem.h"
#include "entities/Pickable.h"
#include "entities/MapEntities.h"
#include "movements/Movement.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"
#include "lowlevel/Geometry.h"
#include "Game.h"
#include "Map.h"
#include "Sprite.h"

const std::string LuaContext::entity_module_name = "sol.entity";
const std::string LuaContext::entity_hero_module_name = "sol.entity.hero";
const std::string LuaContext::entity_npc_module_name = "sol.entity.npc";
const std::string LuaContext::entity_chest_module_name = "sol.entity.chest";
const std::string LuaContext::entity_block_module_name = "sol.entity.block";
const std::string LuaContext::entity_switch_module_name = "sol.entity.switch";
const std::string LuaContext::entity_door_module_name = "sol.entity.door";
const std::string LuaContext::entity_pickable_module_name = "sol.entity.pickable";
const std::string LuaContext::entity_enemy_module_name = "sol.entity.enemy";

const char* LuaContext::enemy_attack_names[] = {
  "sword",
  "thrown_item",
  "explosion",
  "arrow",
  "hookshot",
  "boomerang",
  "fire",
  "script",
  NULL
};

const char* LuaContext::enemy_hurt_style_names[] = {
  "normal",
  "monster",
  "boss",
  NULL
};

const char* LuaContext::enemy_obstacle_behavior_names[] = {
  "normal",
  "flying",
  "swimming",
  NULL
};

const char* LuaContext::transition_style_names[] = {
  "immediate",
  "fade",
  "scrolling",
  NULL
};

/**
 * @brief Initializes the map entity features provided to Lua.
 */
void LuaContext::register_entity_module() {

  // Methods common to all entity types.
  static const luaL_Reg common_methods[] = {
      { "get_map", entity_api_get_map },
      { "get_name", entity_api_get_name },
      { "exists", entity_api_exists },
      { "remove", entity_api_remove },
      { "is_enabled", entity_api_is_enabled },
      { "set_enabled", entity_api_set_enabled },
      { "get_size", entity_api_get_size },
      { "get_origin", entity_api_get_origin },
      { "get_position", entity_api_get_position },
      { "get_distance", entity_api_get_distance },
      { "get_angle", entity_api_get_angle},
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
      { "teleport", hero_api_teleport },
      { "set_visible", hero_api_set_visible },
      { "get_direction", hero_api_get_direction },
      { "set_direction", hero_api_set_direction },
      { "set_position", hero_api_set_position },
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
      { "start_movement", entity_api_start_movement },
      { "stop_movement", entity_api_stop_movement },
      { "get_sprite", entity_api_get_sprite },
      { "set_position", entity_api_set_position },
      { NULL, NULL }
  };
  register_functions(entity_npc_module_name, common_methods);
  register_type(entity_npc_module_name, npc_methods,
      common_metamethods);

  // Chest.
  static const luaL_Reg chest_methods[] = {
      { "is_open", chest_api_is_open },
      { "set_open", chest_api_set_open },
      { NULL, NULL }
  };
  register_functions(entity_chest_module_name, common_methods);
  register_type(entity_chest_module_name, chest_methods,
      common_metamethods);

  // Block.
  static const luaL_Reg block_methods[] = {
      { "block_reset", block_api_reset },
      { "block_set_position", entity_api_set_position },
      { NULL, NULL }
  };
  register_functions(entity_block_module_name, common_methods);
  register_type(entity_block_module_name, block_methods,
      common_metamethods);

  // Switch.
  static const luaL_Reg switch_methods[] = {
      { "is_activated", switch_api_is_activated },
      { "set_activated", switch_api_set_activated },
      { "set_locked", switch_api_set_locked },
      { NULL, NULL }
  };
  register_functions(entity_switch_module_name, common_methods);
  register_type(entity_switch_module_name, switch_methods,
      common_metamethods);

  // Door.
  static const luaL_Reg door_methods[] = {
      { "is_open", door_api_is_open },
      { NULL, NULL }
  };
  register_functions(entity_door_module_name, common_methods);
  register_type(entity_door_module_name, door_methods,
      common_metamethods);

  // Pickable.
  static const luaL_Reg pickable_methods[] = {
      { "get_sprite", entity_api_get_sprite },
      { "set_position", entity_api_set_position },
      { "get_movement", entity_api_get_movement },
      { "start_movement", entity_api_start_movement },
      { "stop_movement", entity_api_stop_movement },
      { "set_layer_independent_collisions", entity_api_set_layer_independent_collisions },
      { "get_followed_entity", pickable_api_get_followed_entity },
      { "get_falling_height", pickable_api_get_falling_height },
      { "get_treasure", pickable_api_get_treasure },
      { NULL, NULL }

  };
  register_functions(entity_pickable_module_name, common_methods);
  register_type(entity_pickable_module_name, pickable_methods,
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
      { "set_layer_independent_collisions", entity_api_set_layer_independent_collisions },
      { "set_treasure", enemy_api_set_treasure },
      { "set_no_treasure", enemy_api_set_no_treasure },
      { "set_random_treasure", enemy_api_set_random_treasure },
      { "get_obstacle_behavior", enemy_api_get_obstacle_behavior },
      { "set_obstacle_behavior", enemy_api_set_obstacle_behavior },
      { "get_optimization_distance", entity_api_get_optimization_distance },
      { "set_optimization_distance", entity_api_set_optimization_distance },
      { "set_size", entity_api_set_size },
      { "set_origin", entity_api_set_origin },
      { "set_position", entity_api_set_position },
      { "test_obstacles", entity_api_test_obstacles },
      { "snap_to_grid", entity_api_snap_to_grid },
      { "get_movement", entity_api_get_movement },
      { "start_movement", entity_api_start_movement },
      { "stop_movement", entity_api_stop_movement },
      { "restart", enemy_api_restart },
      { "hurt", enemy_api_hurt },
      { "get_sprite", entity_api_get_sprite },
      { "create_sprite", entity_api_create_sprite },
      { "remove_sprite", entity_api_remove_sprite },
      { "create_enemy", enemy_api_create_enemy },
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
      || is_userdata(l, index, entity_pickable_module_name)
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
 * @brief Implementation of \ref lua_api_entity_exists.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::entity_api_exists(lua_State* l) {

  MapEntity& entity = check_entity(l, 1);

  lua_pushboolean(l, !entity.is_being_removed());
  return 1;
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
 * @brief Implementation of \ref lua_api_entity_set_size.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::entity_api_set_size(lua_State* l) {

  MapEntity& entity = check_entity(l, 1);
  int width = luaL_checkinteger(l, 2);
  int height = luaL_checkinteger(l, 3);

  entity.set_size(width, height);

  return 0;
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
 * @brief Implementation of \ref lua_api_entity_set_origin.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::entity_api_set_origin(lua_State* l) {

  MapEntity& entity = check_entity(l, 1);
  int x = luaL_checkinteger(l, 2);
  int y = luaL_checkinteger(l, 3);

  entity.set_origin(x, y);

  return 0;
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
 * @brief Implementation of \ref lua_api_entity_set_position.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::entity_api_set_position(lua_State* l) {

  MapEntity& entity = check_entity(l, 1);
  int x = luaL_checkinteger(l, 2);
  int y = luaL_checkinteger(l, 3);
  int layer = -1;
  if (lua_gettop(l) >= 4) {
    layer = luaL_checkinteger(l, 4);
  }

  entity.set_xy(x, y);

  if (layer != -1) {
    MapEntities& entities = entity.get_map().get_entities();
    entities.set_entity_layer(entity, Layer(layer));
  }
  entity.check_collision_with_detectors(false);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_entity_snap_to_grid.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::entity_api_snap_to_grid(lua_State* l) {

  MapEntity& entity = check_entity(l, 1);

  entity.set_aligned_to_grid();

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_entity_get_distance.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::entity_api_get_distance(lua_State* l) {

  MapEntity& entity = check_entity(l, 1);
  int distance;
  if (lua_gettop(l) >= 3) {
    int x = luaL_checknumber(l, 2);
    int y = luaL_checknumber(l, 3);
    distance = entity.get_distance(x, y);
  }
  else {
    MapEntity& other_entity = check_entity(l, 2);
    distance = entity.get_distance(other_entity);
  }

  lua_pushinteger(l, distance);
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_entity_get_angle.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::entity_api_get_angle(lua_State* l) {

  MapEntity& entity = check_entity(l, 1);
  double angle;
  if (lua_gettop(l) >= 3) {
    int x = luaL_checknumber(l, 2);
    int y = luaL_checknumber(l, 3);
    angle = entity.get_angle(x, y);
  }
  else {
    MapEntity& other_entity = check_entity(l, 2);
    angle = entity.get_angle(other_entity);
  }

  lua_pushnumber(l, angle);
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_entity_get_sprite.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::entity_api_get_sprite(lua_State* l) {

  MapEntity& entity = check_entity(l, 1);

  if (entity.has_sprite()) {
    push_sprite(l, entity.get_sprite());
  }
  else {
    lua_pushnil(l);
  }
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_entity_create_sprite.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::entity_api_create_sprite(lua_State* l) {

  MapEntity& entity = check_entity(l, 1);
  const std::string& animation_set_id = luaL_checkstring(l, 2);

  Sprite& sprite = entity.create_sprite(animation_set_id, true);

  push_userdata(l, sprite);
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_entity_remove_sprite.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::entity_api_remove_sprite(lua_State* l) {

  MapEntity& entity = check_entity(l, 1);

  if (lua_gettop(l) >= 2) {
    Sprite& sprite = check_sprite(l, 2);
    entity.remove_sprite(sprite);
  }
  else if (entity.has_sprite()) {
    Sprite& sprite = entity.get_sprite();
    entity.remove_sprite(sprite);
  }

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_entity_get_movement.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::entity_api_get_movement(lua_State* l) {

  MapEntity& entity = check_entity(l, 1);

  Movement* movement = entity.get_movement();
  if (movement == NULL) {
    lua_pushnil(l);
  }
  else {
    push_userdata(l, *movement);
  }

  return 1;
}

/**
 * @brief Implementation of \ref lua_api_entity_start_movement.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::entity_api_start_movement(lua_State* l) {

  MapEntity& entity = check_entity(l, 1);
  Movement& movement = check_movement(l, 2);

  movement.set_suspended(false);
  entity.clear_movement();
  entity.set_movement(&movement);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_entity_stop_movement.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::entity_api_stop_movement(lua_State* l) {

  MapEntity& entity = check_entity(l, 1);

  entity.clear_movement();

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_entity_has_layer_independent_collisions.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::entity_api_has_layer_independent_collisions(lua_State* l) {

  MapEntity& entity = check_entity(l, 1);

  bool independent = false;
  if (entity.is_detector()) {
    Detector& detector = static_cast<Detector&>(entity);
    independent = detector.has_layer_independent_collisions();
  }

  lua_pushboolean(l, independent);
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_entity_set_layer_independent_collisions.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::entity_api_set_layer_independent_collisions(lua_State* l) {

  MapEntity& entity = check_entity(l, 1);
  bool independent = true;
  if (lua_gettop(l) >= 2) {
    independent = lua_toboolean(l, 2);
  }

  if (entity.is_detector()) {
    Detector& detector = static_cast<Detector&>(entity);
    detector.set_layer_independent_collisions(independent);
  }

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_entity_test_obstacles.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::entity_api_test_obstacles(lua_State* l) {

  MapEntity& entity = check_entity(l, 1);
  int dx = luaL_checkinteger(l, 2);
  int dy = luaL_checkinteger(l, 3);

  Rectangle bounding_box = entity.get_bounding_box();
  bounding_box.add_xy(dx, dy);

  lua_pushboolean(l, entity.get_map().test_collision_with_obstacles(
      entity.get_layer(), bounding_box, entity));
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_entity_get_optimization_distance.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::entity_api_get_optimization_distance(lua_State* l) {

  MapEntity& entity = check_entity(l, 1);

  lua_pushinteger(l, entity.get_optimization_distance());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_entity_set_optimization_distance.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::entity_api_set_optimization_distance(lua_State* l) {

  MapEntity& entity = check_entity(l, 1);
  int distance = luaL_checkinteger(l, 2);

  entity.set_optimization_distance(distance);

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
 * @brief Implementation of \ref lua_api_hero_teleport.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::hero_api_teleport(lua_State* l) {

  Hero& hero = check_hero(l, 1);
  MapId map_id = luaL_checkinteger(l, 2);
  const std::string& destination_name = luaL_checkstring(l, 3);
  int transition_style = Transition::FADE;
  if (lua_gettop(l) >= 4) {
    transition_style = luaL_checkoption(l, 4, NULL, transition_style_names);
  }

  hero.get_game().set_current_map(map_id, destination_name,
      Transition::Style(transition_style));

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

  hero.set_xy(x, y);
  if (layer != -1) {
    MapEntities& entities = hero.get_map().get_entities();
    entities.set_entity_layer(hero, Layer(layer));
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

  sw.set_locked(locked);

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
 * pickable and returns it.
 * @param l A Lua context.
 * @param index An index in the stack.
 * @return The pickable.
 */
Pickable& LuaContext::check_pickable(lua_State* l, int index) {
  return static_cast<Pickable&>(check_userdata(l, index, entity_pickable_module_name));
}

/**
 * @brief Pushes an pickable userdata onto the stack.
 * @param l A Lua context.
 * @param pickable A pickable treasure.
 */
void LuaContext::push_pickable(lua_State* l, Pickable& pickable) {
  push_userdata(l, pickable);
}

/**
 * @brief Implementation of \ref lua_api_pickable_get_followed_entity.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::pickable_api_get_followed_entity(lua_State* l) {

  Pickable& pickable = check_pickable(l, 1);

  MapEntity* followed_entity = pickable.get_entity_followed();

  if (followed_entity != NULL) {
    push_entity(l, *followed_entity);
  }
  else {
    lua_pushnil(l);
  }
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_pickable_get_falling_height.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::pickable_api_get_falling_height(lua_State* l) {

  Pickable& pickable = check_pickable(l, 1);

  lua_pushinteger(l, pickable.get_falling_height());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_pickable_get_treasure.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::pickable_api_get_treasure(lua_State* l) {

  Pickable& pickable = check_pickable(l, 1);
  const Treasure& treasure = pickable.get_treasure();

  lua_pushstring(l, treasure.get_item_name().c_str());
  lua_pushinteger(l, treasure.get_variant());
  lua_pushinteger(l, treasure.get_savegame_variable());
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

/**
 * @brief Implementation of \ref lua_api_enemy_get_life.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_get_life(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);

  lua_pushinteger(l, enemy.get_life());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_enemy_set_life.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_set_life(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);
  int life = luaL_checkinteger(l, 2);

  enemy.set_life(life);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_enemy_add_life.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_add_life(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);
  int points = luaL_checkinteger(l, 2);

  enemy.set_life(enemy.get_life() + points);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_enemy_remove_life.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_remove_life(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);
  int points = luaL_checkinteger(l, 2);

  enemy.set_life(enemy.get_life() - points);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_enemy_get_damage.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_get_damage(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);

  lua_pushinteger(l, enemy.get_damage());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_enemy_set_damage.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_set_damage(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);
  int damage = luaL_checkinteger(l, 2);

  enemy.set_damage(damage);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_enemy_get_magic_damage.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_get_magic_damage(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);

  lua_pushinteger(l, enemy.get_magic_damage());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_enemy_set_magic_damage.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_set_magic_damage(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);
  int magic_damage = luaL_checkinteger(l, 2);

  enemy.set_magic_damage(magic_damage);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_enemy_is_pushed_back_when_hurt.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_is_pushed_back_when_hurt(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);

  lua_pushboolean(l, enemy.get_pushed_back_when_hurt());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_enemy_set_pushed_back_when_hurt.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_set_pushed_back_when_hurt(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);
  bool push_back = lua_toboolean(l, 2);

  enemy.set_pushed_back_when_hurt(push_back);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_enemy_get_push_hero_on_sword.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_get_push_hero_on_sword(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);

  lua_pushboolean(l, enemy.get_push_hero_on_sword());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_enemy_set_push_hero_on_sword.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_set_push_hero_on_sword(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);
  bool push = lua_toboolean(l, 2);

  enemy.set_push_hero_on_sword(push);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_enemy_get_can_hurt_hero_running.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_get_can_hurt_hero_running(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);

  lua_pushboolean(l, enemy.get_can_hurt_hero_running());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_enemy_set_can_hurt_hero_running.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_set_can_hurt_hero_running(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);
  bool can_hurt_hero_running = lua_toboolean(l, 2);

  enemy.set_can_hurt_hero_running(can_hurt_hero_running);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_enemy_get_hurt_style.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_get_hurt_style(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);

  Enemy::HurtStyle hurt_style = enemy.get_hurt_style();

  lua_pushstring(l, enemy_hurt_style_names[hurt_style]);
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_enemy_set_hurt_style.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_set_hurt_style(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);
  int hurt_style = luaL_checkoption(l, 2, NULL, enemy_hurt_style_names);

  enemy.set_hurt_style(Enemy::HurtStyle(hurt_style));

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_enemy_get_can_attack.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_get_can_attack(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);

  lua_pushboolean(l, enemy.get_can_attack());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_enemy_set_can_attack.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_set_can_attack(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);
  bool can_attack = lua_toboolean(l, 2);

  enemy.set_can_attack(can_attack);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_enemy_get_minimum_shield_needed.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_get_minimum_shield_needed(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);

  int shield_level = enemy.get_minimum_shield_needed();

  lua_pushinteger(l, shield_level);
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_enemy_set_minimum_shield_needed.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_set_minimum_shield_needed(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);
  int shield_level = luaL_checkinteger(l, 2);

  enemy.set_minimum_shield_needed(shield_level);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_enemy_set_attack_consequence.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_set_attack_consequence(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);
  int attack = luaL_checkoption(l, 2, NULL, enemy_attack_names);

  if (lua_isnumber(l, 3)) {
    int life_points = luaL_checkinteger(l, 3);
    Debug::check_assertion(life_points > 0, StringConcat()
        << "Invalid attack consequence: " << life_points);
    enemy.set_attack_consequence(EnemyAttack(attack), EnemyReaction::HURT, life_points);
  }
  else {
    // TODO: simplify or encapsulate the C++ part of specifying attack consequences
    // (but the important thing is that the Lua API is easy to use)
    const std::string& reaction_name = lua_tostring(l, 3);
    EnemyReaction::ReactionType reaction = EnemyReaction::get_reaction_by_name(reaction_name);
    enemy.set_attack_consequence(EnemyAttack(attack), reaction);
  }

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_enemy_set_attack_consequence_sprite.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_set_attack_consequence_sprite(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);
  Sprite& sprite = check_sprite(l, 2);
  int attack = luaL_checkoption(l, 3, NULL, enemy_attack_names);

  if (lua_isnumber(l, 4)) {
    int life_points = luaL_checkinteger(l, 4);
    Debug::check_assertion(life_points > 0, StringConcat()
        << "Invalid attack consequence: " << life_points);
    enemy.set_attack_consequence_sprite(sprite, EnemyAttack(attack), EnemyReaction::HURT, life_points);
  }
  else {
    const std::string& reaction_name = lua_tostring(l, 4);
    EnemyReaction::ReactionType reaction = EnemyReaction::get_reaction_by_name(reaction_name);
    enemy.set_attack_consequence_sprite(sprite, EnemyAttack(attack), reaction);
  }

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_enemy_set_default_attack_consequences.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_set_default_attack_consequences(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);

  enemy.set_default_attack_consequences();

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_enemy_set_default_attack_consequences_sprite.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_set_default_attack_consequences_sprite(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);
  Sprite& sprite = check_sprite(l, 2);

  enemy.set_default_attack_consequences_sprite(sprite);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_enemy_set_invincible.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_set_invincible(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);

  enemy.set_no_attack_consequences();

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_enemy_set_invincible_sprite.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_set_invincible_sprite(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);
  Sprite& sprite = check_sprite(l, 2);

  enemy.set_no_attack_consequences_sprite(sprite);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_enemy_set_treasure.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_set_treasure(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);
  const std::string& item_name = luaL_checkstring(l, 2);
  int variant = luaL_checkinteger(l, 3);
  int savegame_variable = luaL_checkinteger(l, 4);

  Treasure treasure(enemy.get_game(), item_name, variant, savegame_variable);
  enemy.set_treasure(treasure);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_enemy_set_no_treasure.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_set_no_treasure(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);

  Treasure treasure(enemy.get_game(), "_none", 1, -1);
  enemy.set_treasure(treasure);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_enemy_set_random_treasure.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_set_random_treasure(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);

  Treasure treasure(enemy.get_game(), "_random", 1, -1);
  enemy.set_treasure(treasure);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_enemy_get_obstacle_behavior.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_get_obstacle_behavior(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);

  Enemy::ObstacleBehavior behavior = enemy.get_obstacle_behavior();

  lua_pushstring(l, enemy_obstacle_behavior_names[behavior]);
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_enemy_set_obstacle_behavior.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_set_obstacle_behavior(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);
  int behavior = luaL_checkoption(l, 2, NULL, enemy_obstacle_behavior_names);

  enemy.set_obstacle_behavior(Enemy::ObstacleBehavior(behavior));

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_enemy_restart.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_restart(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);

  enemy.restart();

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_enemy_hurt.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_hurt(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);
  int life_points = luaL_checkinteger(l, 2);

  if (enemy.is_in_normal_state() && !enemy.is_invulnerable()) {
    Hero& hero = enemy.get_map().get_entities().get_hero();
    enemy.set_attack_consequence(ATTACK_SCRIPT, EnemyReaction::HURT, life_points);
    enemy.try_hurt(ATTACK_SCRIPT, hero, NULL);
  }

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_enemy_create_enemy.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::enemy_api_create_enemy(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);
  const std::string& name = luaL_checkstring(l, 2);
  const std::string& breed = luaL_checkstring(l, 3);
  int x = luaL_checkinteger(l, 4);
  int y = luaL_checkinteger(l, 5);
  int layer;
  if (lua_gettop(l) >= 6) {
    layer = luaL_checkinteger(l, 6);
  }
  else {
    layer = enemy.get_layer();
  }

  x += enemy.get_x();
  y += enemy.get_y();

  Game& game = enemy.get_game();
  MapEntities& entities = enemy.get_map().get_entities();
  Treasure treasure = Treasure(game, "_random", 1, -1);
  Enemy* other_enemy = (Enemy*) Enemy::create(game, breed, Enemy::RANK_NORMAL,
      -1, name, Layer(layer), x, y, 0, treasure);
  other_enemy->set_optimization_distance(enemy.get_optimization_distance());
  entities.add_entity(other_enemy);
  other_enemy->restart();

  return 0;
}


/**
 * @brief Calls the on_obtaining_treasure() method of a Lua hero.
 * @param hero The hero.
 * @param treasure A treasure the hero is about to obtain.
 */
void LuaContext::hero_on_obtaining_treasure(Hero& hero, const Treasure& treasure) {

  push_hero(l, hero);
  on_obtaining_treasure(treasure);
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_obtained_treasure() method of a Lua hero.
 * @param hero The hero.
 * @param treasure The treasure just obtained.
 */
void LuaContext::hero_on_obtained_treasure(Hero& hero, const Treasure& treasure) {

  push_hero(l, hero);
  on_obtained_treasure(treasure);
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_victory_finished() method of a Lua hero.
 * @param hero The hero.
 */
void LuaContext::hero_on_victory_finished(Hero& hero) {

  push_hero(l, hero);
  on_victory_finished();
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_activated() method of a Lua switch.
 * @param sw A switch.
 */
void LuaContext::switch_on_activated(Switch& sw) {

  push_switch(l, sw);
  on_activated();
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_inactivated() method of a Lua switch.
 * @param sw A switch.
 */
void LuaContext::switch_on_inactivated(Switch& sw) {

  push_switch(l, sw);
  on_inactivated();
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_left() method of a Lua switch.
 * @param sw A switch.
 */
void LuaContext::switch_on_left(Switch& sw) {

  push_switch(l, sw);
  on_left();
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_activated() method of a Lua sensor.
 * @param sensor A sensor.
 */
void LuaContext::sensor_on_activated(Sensor& sensor) {

  push_entity(l, sensor);
  on_activated();
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_activated_repeat() method of a Lua sensor.
 * @param sensor A sensor.
 */
void LuaContext::sensor_on_activated_repeat(Sensor& sensor) {

  push_entity(l, sensor);
  on_activated_repeat();
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_collision_explosion() method of a Lua sensor.
 * @param sensor A sensor.
 */
void LuaContext::sensor_on_collision_explosion(Sensor& sensor) {

  push_entity(l, sensor);
  on_collision_explosion();
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_movement_finished() method of a Lua NPC.
 * @param npc An NPC.
 */
void LuaContext::npc_on_movement_finished(NPC& npc) {

  push_npc(l, npc);
  on_movement_finished();
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_interaction() method of a Lua NPC.
 * @param npc An NPC.
 */
void LuaContext::npc_on_interaction(NPC& npc) {

  push_npc(l, npc);
  on_interaction();
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_interaction_item() method of a Lua NPC.
 * @param item_used The equipment item used.
 * @return \c true if an interaction occurred.
 */
bool LuaContext::npc_on_interaction_item(NPC& npc, EquipmentItem& item_used) {

  push_npc(l, npc);
  bool result = on_interaction_item(item_used);
  lua_pop(l, 1);
  return result;
}

/**
 * @brief Calls the on_collision_fire() method of a Lua NPC.
 * @param npc An NPC.
 */
void LuaContext::npc_on_collision_fire(NPC& npc) {

  push_npc(l, npc);
  on_collision_fire();
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_empty() method of a Lua chest.
 * @param chest A chest.
 * @return \c true if the on_empty() method is defined.
 */
bool LuaContext::chest_on_empty(Chest& chest) {

  push_chest(l, chest);
  bool result = on_empty();
  lua_pop(l, 1);
  return result;
}

/**
 * @brief Calls the on_buying() method of a Lua shop item.
 * @param shop_item A shop item.
 * @return \c true if the player is allowed to buy the item.
 */
bool LuaContext::shop_item_on_buying(ShopItem& shop_item) {

  push_entity(l, shop_item);
  bool result = on_buying();
  lua_pop(l, 1);
  return result;
}

/**
 * @brief Calls the on_bought() method of a Lua shop item.
 * @param shop_item A shop item.
 */
void LuaContext::shop_item_on_bought(ShopItem& shop_item) {

  push_entity(l, shop_item);
  on_bought();
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_open() method of a Lua door.
 * @param door A door.
 */
void LuaContext::door_on_open(Door& door) {

  push_door(l, door);
  on_open();
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_closed() method of a Lua door.
 * @param door A door.
 */
void LuaContext::door_on_closed(Door& door) {

  push_door(l, door);
  on_closed();
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_moved() method of a Lua block.
 * @param block a block.
 */
void LuaContext::block_on_moved(Block& block) {

  push_block(l, block);
  on_moved();
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_update() method of a Lua enemy.
 * @param enemy An enemy.
 */
void LuaContext::enemy_on_update(Enemy& enemy) {

  push_enemy(l, enemy);
  on_update();
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_suspended() method of a Lua enemy.
 * @param enemy An enemy.
 * @param suspended true if the enemy is suspended.
 */
void LuaContext::enemy_on_suspended(Enemy& enemy, bool suspended) {

  push_enemy(l, enemy);
  on_suspended(suspended);
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_created() method of a Lua enemy.
 * @param enemy An enemy.
 */
void LuaContext::enemy_on_created(Enemy& enemy) {

  push_enemy(l, enemy);
  on_created();
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_enabled() method of a Lua enemy.
 * @param enemy An enemy.
 */
void LuaContext::enemy_on_enabled(Enemy& enemy) {

  push_enemy(l, enemy);
  on_enabled();
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_disabled() method of a Lua enemy.
 * @param enemy An enemy.
 */
void LuaContext::enemy_on_disabled(Enemy& enemy) {

  push_enemy(l, enemy);
  on_disabled();
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_restarted() method of a Lua enemy.
 * @param enemy An enemy.
 */
void LuaContext::enemy_on_restarted(Enemy& enemy) {

  push_enemy(l, enemy);
  on_restarted();
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_pre_draw() method of a Lua enemy.
 * @param enemy An enemy.
 */
void LuaContext::enemy_on_pre_draw(Enemy& enemy) {

  push_enemy(l, enemy);
  on_pre_draw();
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_post_draw() method of a Lua enemy.
 * @param enemy An enemy.
 */
void LuaContext::enemy_on_post_draw(Enemy& enemy) {

  push_enemy(l, enemy);
  on_post_draw();
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_position_changed() method of a Lua enemy.
 * @param enemy An enemy.
 * @param xy The new coordinates.
 * @param layer The new layer.
 */
void LuaContext::enemy_on_position_changed(
    Enemy& enemy, const Rectangle& xy, Layer layer) {

  push_enemy(l, enemy);
  on_position_changed(xy, layer);
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_obstacle_reached() method of a Lua enemy.
 * @param enemy An enemy.
 */
void LuaContext::enemy_on_obstacle_reached(Enemy& enemy) {

  push_enemy(l, enemy);
  on_obstacle_reached();
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_movement_changed() method of a Lua enemy.
 * @param enemy An enemy.
 * @param movement Its movement.
 */
void LuaContext::enemy_on_movement_changed(Enemy& enemy,
    Movement& movement) {

  push_enemy(l, enemy);
  on_movement_changed(movement);
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_movement_finished() method of a Lua enemy.
 * @param enemy An enemy.
 */
void LuaContext::enemy_on_movement_finished(Enemy& enemy) {

  push_enemy(l, enemy);
  on_movement_finished();
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_sprite_animation_finished() method of a Lua enemy.
 * @param enemy An enemy.
 * @param sprite A sprite whose animation has just finished.
 * @param animation Name of the animation finished.
 */
void LuaContext::enemy_on_sprite_animation_finished(Enemy& enemy,
    Sprite& sprite, const std::string& animation) {

  push_enemy(l, enemy);
  on_sprite_animation_finished(sprite, animation);
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_sprite_frame_changed() method of a Lua enemy.
 * @param enemy An enemy.
 * @param sprite A sprite whose animation frame has just changed.
 * @param animation Name of the sprite animation.
 * @param frame The new frame.
 */
void LuaContext::enemy_on_sprite_frame_changed(Enemy& enemy,
    Sprite& sprite, const std::string& animation, int frame) {

  push_enemy(l, enemy);
  on_sprite_frame_changed(sprite, animation, frame);
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_collision_enemy() method of a Lua enemy.
 * @param enemy An enemy.
 * @param other_enemy Another enemy colliding with the first one.
 * @param other_sprite Colliding sprite of the other enemy.
 * @param this_sprite Colliding sprite of the first enemy.
 */
void LuaContext::enemy_on_collision_enemy(Enemy& enemy,
    Enemy& other_enemy, Sprite& other_sprite, Sprite& this_sprite) {

  push_enemy(l, enemy);
  on_collision_enemy(other_enemy, other_sprite, this_sprite);
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_custom_attack_received() method of a Lua enemy.
 * @param enemy An enemy.
 * @param attack The attack received.
 * @param sprite The sprite that receives the attack if any.
 */
void LuaContext::enemy_on_custom_attack_received(Enemy& enemy,
    EnemyAttack attack, Sprite* sprite) {

  push_enemy(l, enemy);
  on_custom_attack_received(attack, sprite);
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_hurt() method of a Lua enemy.
 * @param enemy An enemy.
 * @param attack The attack received.
 * @param life_lost Number of life points just lost.
 */
void LuaContext::enemy_on_hurt(Enemy& enemy, EnemyAttack attack, int life_lost) {

  push_enemy(l, enemy);
  on_hurt(attack, life_lost);
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_dying() method of a Lua enemy.
 * @param enemy An enemy.
 */
void LuaContext::enemy_on_dying(Enemy& enemy) {

  push_enemy(l, enemy);
  on_dying();
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_dead() method of a Lua enemy.
 * @param enemy An enemy.
 */
void LuaContext::enemy_on_dead(Enemy& enemy) {

  push_enemy(l, enemy);
  on_dead();
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_immobilized() method of a Lua enemy.
 * @param enemy An enemy.
 */
void LuaContext::enemy_on_immobilized(Enemy& enemy) {

  push_enemy(l, enemy);
  on_immobilized();
  lua_pop(l, 1);
}

