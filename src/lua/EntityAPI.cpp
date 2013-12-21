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
#include "lua/LuaContext.h"
#include "entities/Hero.h"
#include "entities/NPC.h"
#include "entities/Chest.h"
#include "entities/Block.h"
#include "entities/Switch.h"
#include "entities/Door.h"
#include "entities/Enemy.h"
#include "entities/Sensor.h"
#include "entities/Separator.h"
#include "entities/ShopTreasure.h"
#include "entities/Pickable.h"
#include "entities/CustomEntity.h"
#include "entities/MapEntities.h"
#include "movements/Movement.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"
#include "lowlevel/Geometry.h"
#include "lowlevel/Sound.h"
#include "Game.h"
#include "Map.h"
#include "Sprite.h"
#include "EquipmentItem.h"

namespace solarus {

const std::string LuaContext::entity_module_name = "sol.entity";
const std::string LuaContext::entity_hero_module_name = "sol.entity.hero";
const std::string LuaContext::entity_npc_module_name = "sol.entity.npc";
const std::string LuaContext::entity_chest_module_name = "sol.entity.chest";
const std::string LuaContext::entity_block_module_name = "sol.entity.block";
const std::string LuaContext::entity_switch_module_name = "sol.entity.switch";
const std::string LuaContext::entity_door_module_name = "sol.entity.door";
const std::string LuaContext::entity_shop_treasure_module_name = "sol.entity.shop_treasure";
const std::string LuaContext::entity_pickable_module_name = "sol.entity.pickable";
const std::string LuaContext::entity_enemy_module_name = "sol.entity.enemy";
const std::string LuaContext::entity_custom_module_name = "sol.entity.custom";

// TODO move this to Enemy
const std::string LuaContext::enemy_attack_names[] = {
  "sword",
  "thrown_item",
  "explosion",
  "arrow",
  "hookshot",
  "boomerang",
  "fire",
  "script",
  ""  // Sentinel.
};

const std::string LuaContext::enemy_hurt_style_names[] = {
  "normal",
  "monster",
  "boss",
  ""  // Sentinel.
};

const std::string LuaContext::enemy_obstacle_behavior_names[] = {
  "normal",
  "flying",
  "swimming",
  ""  // Sentinel.
};

// TODO move this to Transition
const std::string LuaContext::transition_style_names[] = {
  "immediate",
  "fade",
  "scrolling",
  ""  // Sentinel.
};

/**
 * \brief Initializes the map entity features provided to Lua.
 */
void LuaContext::register_entity_module() {

  // Methods common to all entity types.
  static const luaL_Reg common_methods[] = {
      { "get_type", entity_api_get_type },
      { "get_map", entity_api_get_map },
      { "get_game", entity_api_get_game },
      { "get_name", entity_api_get_name },
      { "exists", entity_api_exists },
      { "remove", entity_api_remove },
      { "is_enabled", entity_api_is_enabled },
      { "set_enabled", entity_api_set_enabled },
      { "get_size", entity_api_get_size },
      { "get_origin", entity_api_get_origin },
      { "get_position", entity_api_get_position },
      { "set_position", entity_api_set_position },
      { "get_center_position", entity_api_get_center_position },
      { "snap_to_grid", entity_api_snap_to_grid },
      { "get_distance", entity_api_get_distance },
      { "get_angle", entity_api_get_angle },
      { "get_direction4_to", entity_api_get_direction4_to },
      { "get_direction8_to", entity_api_get_direction8_to },
      { "get_optimization_distance", entity_api_get_optimization_distance },
      { "set_optimization_distance", entity_api_set_optimization_distance },
      { "is_in_same_region", entity_api_is_in_same_region },
      { "test_obstacles", entity_api_test_obstacles },
      { "is_visible", entity_api_is_visible },
      { "set_visible", entity_api_set_visible },
      { "get_movement", entity_api_get_movement },
      { "stop_movement", entity_api_stop_movement },
      { NULL, NULL }
  };
  static const luaL_Reg common_metamethods[] = {
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
      { "teleport", hero_api_teleport },
      { "get_direction", hero_api_get_direction },
      { "set_direction", hero_api_set_direction },
      { "get_walking_speed", hero_api_get_walking_speed },
      { "set_walking_speed", hero_api_set_walking_speed },
      { "save_solid_ground", hero_api_save_solid_ground },
      { "reset_solid_ground", hero_api_reset_solid_ground },
      { "get_state", hero_api_get_state },
      { "freeze", hero_api_freeze },
      { "unfreeze", hero_api_unfreeze },
      { "walk", hero_api_walk },  // TODO use the more general movement:start
      { "start_jumping", hero_api_start_jumping },
      { "start_treasure", hero_api_start_treasure },
      { "start_victory", hero_api_start_victory},
      { "start_item", hero_api_start_item },
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
      { "get_sprite", entity_api_get_sprite },
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
      { "reset", block_api_reset },
      { "is_pushable", block_api_is_pushable },
      { "set_pushable", block_api_set_pushable },
      { "is_pullable", block_api_is_pullable },
      { "set_pullable", block_api_set_pullable },
      { "get_maximum_moves", block_api_get_maximum_moves },
      { "set_maximum_moves", block_api_set_maximum_moves },
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
      { "is_opening", door_api_is_opening },
      { "is_closed", door_api_is_closed },
      { "is_closing", door_api_is_closing },
      { NULL, NULL }
  };
  register_functions(entity_door_module_name, common_methods);
  register_type(entity_door_module_name, door_methods,
      common_metamethods);

  // Shop treasure.
  static const luaL_Reg shop_treasure_methods[] = {
      { NULL, NULL }
  };
  register_functions(entity_shop_treasure_module_name, common_methods);
  register_type(entity_shop_treasure_module_name, shop_treasure_methods,
      common_metamethods);

  // Pickable.
  static const luaL_Reg pickable_methods[] = {
      { "get_sprite", entity_api_get_sprite },
      { "has_layer_independent_collisions", entity_api_has_layer_independent_collisions },
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
      { "get_breed", enemy_api_get_breed },
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
      { "has_layer_independent_collisions", entity_api_has_layer_independent_collisions },
      { "set_layer_independent_collisions", entity_api_set_layer_independent_collisions },
      { "set_treasure", enemy_api_set_treasure },
      { "is_traversable", enemy_api_is_traversable },
      { "set_traversable", enemy_api_set_traversable },
      { "get_obstacle_behavior", enemy_api_get_obstacle_behavior },
      { "set_obstacle_behavior", enemy_api_set_obstacle_behavior },
      { "set_size", entity_api_set_size },
      { "set_origin", entity_api_set_origin },
      { "restart", enemy_api_restart },
      { "hurt", enemy_api_hurt },
      { "immobilize", enemy_api_immobilize },
      { "get_sprite", entity_api_get_sprite },
      { "create_sprite", entity_api_create_sprite },
      { "remove_sprite", entity_api_remove_sprite },
      { "create_enemy", enemy_api_create_enemy },
      { NULL, NULL }
  };
  register_functions(entity_enemy_module_name, common_methods);
  register_type(entity_enemy_module_name, enemy_methods,
      common_metamethods);

  // Custom entity.
  static const luaL_Reg custom_entity_methods[] = {
      { "get_model", custom_entity_api_get_model },
      { NULL, NULL }
  };
  register_functions(entity_custom_module_name, common_methods);
  register_type(entity_custom_module_name, custom_entity_methods,
          common_metamethods);
}

/**
 * \brief Returns whether a value is a userdata of type entity.
 * \param l A Lua context.
 * \param index An index in the stack.
 * \return true if the value at this index is a entity.
 */
bool LuaContext::is_entity(lua_State* l, int index) {
  return is_userdata(l, index, entity_module_name)
      || is_hero(l, index)
      || is_npc(l, index)
      || is_chest(l, index)
      || is_block(l, index)
      || is_switch(l, index)
      || is_door(l, index)
      || is_pickable(l, index)
      || is_enemy(l, index)
      || is_shop_treasure(l, index);
}

/**
 * \brief Checks that the userdata at the specified index of the stack is an
 * entity and returns it.
 * \param l A Lua context.
 * \param index An index in the stack.
 * \return The entity.
 */
MapEntity& LuaContext::check_entity(lua_State* l, int index) {

  MapEntity** entity = NULL;

  if (is_entity(l, index)) {
    entity = static_cast<MapEntity**>(lua_touserdata(l, index));
  }
  else {
    luaL_typerror(l, index, "entity");
  }

  return **entity;
}

/**
 * \brief Pushes an entity userdata onto the stack.
 *
 * If the entity or its map does not exist anymore, pushes nil.
 *
 * \param l A Lua context.
 * \param entity An entity.
 */
void LuaContext::push_entity(lua_State* l, MapEntity& entity) {

  push_userdata(l, entity);
}

/**
 * \brief Implementation of entity:get_type().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::entity_api_get_type(lua_State* l) {

  MapEntity& entity = check_entity(l, 1);

  push_string(l, entity.get_type_name());
  return 1;
}

/**
 * \brief Implementation of entity:get_map().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::entity_api_get_map(lua_State* l) {

  MapEntity& entity = check_entity(l, 1);

  push_map(l, entity.get_map());
  return 1;
}

/**
 * \brief Implementation of entity:get_game().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::entity_api_get_game(lua_State* l) {

  MapEntity& entity = check_entity(l, 1);

  push_game(l, entity.get_game().get_savegame());
  return 1;
}

/**
 * \brief Implementation of entity:get_name().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::entity_api_get_name(lua_State* l) {

  MapEntity& entity = check_entity(l, 1);

  const std::string& name = entity.get_name();
  if (name.empty()) {
    lua_pushnil(l);
  }
  else {
    push_string(l, name);
  }
  return 1;
}

/**
 * \brief Implementation of entity:exists().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::entity_api_exists(lua_State* l) {

  MapEntity& entity = check_entity(l, 1);

  lua_pushboolean(l, !entity.is_being_removed());
  return 1;
}

/**
 * \brief Implementation of entity:remove().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::entity_api_remove(lua_State* l) {

  MapEntity& entity = check_entity(l, 1);

  entity.remove_from_map();

  return 0;
}

/**
 * \brief Implementation of entity:is_enabled().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::entity_api_is_enabled(lua_State* l) {

  MapEntity& entity = check_entity(l, 1);

  lua_pushboolean(l, entity.is_enabled());
  return 1;
}

/**
 * \brief Implementation of entity:set_enabled().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::entity_api_set_enabled(lua_State* l) {

  MapEntity& entity = check_entity(l, 1);
  bool enabled = true;
  if (lua_gettop(l) >= 2) {
    enabled = lua_toboolean(l, 2);
  }

  entity.set_enabled(enabled);

  return 0;
}

/**
 * \brief Implementation of entity:get_size().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::entity_api_get_size(lua_State* l) {

  MapEntity& entity = check_entity(l, 1);

  lua_pushinteger(l, entity.get_width());
  lua_pushinteger(l, entity.get_height());
  return 2;
}

/**
 * \brief Implementation of enemy:set_size().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::entity_api_set_size(lua_State* l) {

  MapEntity& entity = check_entity(l, 1);
  int width = luaL_checkint(l, 2);
  int height = luaL_checkint(l, 3);

  if (width < 0 || width % 8 != 0) {
    arg_error(l, 2, StringConcat() <<
        "Invalid width: " << width << ": should be a positive multiple of 8");
  }
  if (height < 0 || height % 8 != 0) {
    arg_error(l, 3, StringConcat() <<
        "Invalid height: " << height << ": should be a positive multiple of 8");
  }

  entity.set_size(width, height);

  return 0;
}

/**
 * \brief Implementation of entity:get_origin().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::entity_api_get_origin(lua_State* l) {

  MapEntity& entity = check_entity(l, 1);

  const Rectangle& origin = entity.get_origin();

  lua_pushinteger(l, origin.get_x());
  lua_pushinteger(l, origin.get_y());
  return 2;
}

/**
 * \brief Implementation of enemy:set_origin().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::entity_api_set_origin(lua_State* l) {

  MapEntity& entity = check_entity(l, 1);
  int x = luaL_checkint(l, 2);
  int y = luaL_checkint(l, 3);

  entity.set_origin(x, y);

  return 0;
}

/**
 * \brief Implementation of entity:get_position().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::entity_api_get_position(lua_State* l) {

  MapEntity& entity = check_entity(l, 1);

  lua_pushinteger(l, entity.get_x());
  lua_pushinteger(l, entity.get_y());
  lua_pushinteger(l, entity.get_layer());
  return 3;
}

/**
 * \brief Implementation of entity:set_position().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::entity_api_set_position(lua_State* l) {

  MapEntity& entity = check_entity(l, 1);
  int x = luaL_checkint(l, 2);
  int y = luaL_checkint(l, 3);
  int layer = -1;
  if (lua_gettop(l) >= 4) {
    layer = luaL_checkint(l, 4);
  }

  entity.set_xy(x, y);
  if (layer != -1) {
    if (layer < LAYER_LOW || layer >= LAYER_NB) {
      arg_error(l, 4, StringConcat() << "Invalid layer: " << layer);
    }
    MapEntities& entities = entity.get_map().get_entities();
    entities.set_entity_layer(entity, Layer(layer));
  }
  entity.notify_position_changed();

  return 0;
}

/**
 * \brief Implementation of entity:get_center_position().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::entity_api_get_center_position(lua_State* l) {

  MapEntity& entity = check_entity(l, 1);

  const Rectangle& center_point = entity.get_center_point();
  lua_pushinteger(l, center_point.get_x());
  lua_pushinteger(l, center_point.get_y());
  return 2;
}

/**
 * \brief Implementation of entity:snap_to_grid().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::entity_api_snap_to_grid(lua_State* l) {

  MapEntity& entity = check_entity(l, 1);

  entity.set_aligned_to_grid();

  return 0;
}

/**
 * \brief Implementation of entity:get_distance().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
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
 * \brief Implementation of entity:get_angle().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
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
 * \brief Implementation of entity:get_direction4_to().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::entity_api_get_direction4_to(lua_State* l) {

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

  // Convert from radians.
  int direction4 = (angle + Geometry::PI_OVER_4) / Geometry::PI_OVER_2;

  // Normalize.
  direction4 = (direction4 + 4) % 4;

  lua_pushnumber(l, direction4);
  return 1;
}

/**
 * \brief Implementation of entity:get_direction8_to().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::entity_api_get_direction8_to(lua_State* l) {

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

  // Convert from radians.
  int direction8 = (angle + Geometry::PI_OVER_4 / 2) / Geometry::PI_OVER_4;

  // Normalize.
  direction8 = (direction8 + 8) % 8;

  lua_pushnumber(l, direction8);
  return 1;
}

/**
 * \brief Implementation of
 * npc:get_sprite(), pickable:get_sprite() and enemy:get_sprite().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
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
 * \brief Implementation of enemy:create_sprite().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::entity_api_create_sprite(lua_State* l) {

  MapEntity& entity = check_entity(l, 1);
  const std::string& animation_set_id = luaL_checkstring(l, 2);

  Sprite& sprite = entity.create_sprite(animation_set_id, true);

  push_sprite(l, sprite);
  return 1;
}

/**
 * \brief Implementation of enemy:remove_sprite().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
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
 * \brief Implementation of entity:is_visible().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::entity_api_is_visible(lua_State* l) {

  MapEntity& entity = check_entity(l, 1);

  lua_pushboolean(l, entity.is_visible());
  return 1;
}

/**
 * \brief Implementation of entity:set_visible().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::entity_api_set_visible(lua_State* l) {

  MapEntity& entity = check_entity(l, 1);
  bool visible = true;
  if (lua_gettop(l) >= 2) {
    visible = lua_toboolean(l, 2);
  }

  entity.set_visible(visible);

  return 0;
}

/**
 * \brief Implementation of entity:get_movement().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
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
 * \brief Implementation of entity:stop_movement().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::entity_api_stop_movement(lua_State* l) {

  MapEntity& entity = check_entity(l, 1);

  entity.clear_movement();

  return 0;
}

/**
 * \brief Implementation of
 * pickable:has_layer_independent_collisions() and
 * enemy:has_layer_independent_collisions().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
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
 * \brief Implementation of
 * pickable:set_layer_independent_collisions() and
 * enemy:set_layer_independent_collisions().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
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
 * \brief Implementation of entity:test_obstacles().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::entity_api_test_obstacles(lua_State* l) {

  MapEntity& entity = check_entity(l, 1);
  int dx = luaL_checkint(l, 2);
  int dy = luaL_checkint(l, 3);

  Rectangle bounding_box = entity.get_bounding_box();
  bounding_box.add_xy(dx, dy);

  lua_pushboolean(l, entity.get_map().test_collision_with_obstacles(
      entity.get_layer(), bounding_box, entity));
  return 1;
}

/**
 * \brief Implementation of entity:get_optimization_distance().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::entity_api_get_optimization_distance(lua_State* l) {

  MapEntity& entity = check_entity(l, 1);

  lua_pushinteger(l, entity.get_optimization_distance());
  return 1;
}

/**
 * \brief Implementation of entity:set_optimization_distance().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::entity_api_set_optimization_distance(lua_State* l) {

  MapEntity& entity = check_entity(l, 1);
  int distance = luaL_checkint(l, 2);

  entity.set_optimization_distance(distance);

  return 0;
}

/**
 * \brief Implementation of entity:is_in_same_region().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::entity_api_is_in_same_region(lua_State* l) {

  MapEntity& entity = check_entity(l, 1);
  MapEntity& other_entity = check_entity(l, 2);

  lua_pushboolean(l, entity.is_in_same_region(other_entity));
  return 1;
}

/**
 * \brief Returns whether a value is a userdata of type hero.
 * \param l A Lua context.
 * \param index An index in the stack.
 * \return true if the value at this index is a hero.
 */
bool LuaContext::is_hero(lua_State* l, int index) {
  return is_userdata(l, index, entity_hero_module_name);
}

/**
 * \brief Checks that the userdata at the specified index of the stack is a
 * hero and returns it.
 * \param l A Lua context.
 * \param index An index in the stack.
 * \return The hero.
 */
Hero& LuaContext::check_hero(lua_State* l, int index) {
  return static_cast<Hero&>(check_userdata(l, index, entity_hero_module_name));
}

/**
 * \brief Pushes a hero userdata onto the stack.
 * \param l A Lua context.
 * \param hero A hero.
 */
void LuaContext::push_hero(lua_State* l, Hero& hero) {
  push_userdata(l, hero);
}

/**
 * \brief Implementation of hero:teleport().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::hero_api_teleport(lua_State* l) {

  Hero& hero = check_hero(l, 1);
  const std::string& map_id = luaL_checkstring(l, 2);
  const std::string& destination_name = luaL_optstring(l, 3, "");
  Transition::Style transition_style = opt_enum<Transition::Style>(
      l, 4, transition_style_names, Transition::FADE);

  // TODO don't allow side destinations and scrolling?

  hero.get_game().set_current_map(map_id, destination_name, transition_style);

  return 0;
}

/**
 * \brief Implementation of hero:get_direction().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::hero_api_get_direction(lua_State* l) {

  Hero& hero = check_hero(l, 1);

  lua_pushinteger(l, hero.get_animation_direction());
  return 1;
}

/**
 * \brief Implementation of hero:set_direction().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::hero_api_set_direction(lua_State* l) {

  Hero& hero = check_hero(l, 1);
  int direction = luaL_checkint(l, 2);

  hero.set_animation_direction(direction);

  return 0;
}

/**
 * \brief Implementation of hero:get_walking_speed().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::hero_api_get_walking_speed(lua_State* l) {

  Hero& hero = check_hero(l, 1);

  lua_pushinteger(l, hero.get_normal_walking_speed());
  return 1;
}

/**
 * \brief Implementation of hero:set_walking_speed().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::hero_api_set_walking_speed(lua_State* l) {

  Hero& hero = check_hero(l, 1);
  int normal_walking_speed = luaL_checkint(l, 2);

  hero.set_normal_walking_speed(normal_walking_speed);

  return 0;
}

/**
 * \brief Implementation of hero:save_solid_ground().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::hero_api_save_solid_ground(lua_State* l) {

  Hero& hero = check_hero(l, 1);
  int x, y, layer;
  if (lua_gettop(l) >= 2) {
    x = luaL_checkint(l, 2);
    y = luaL_checkint(l, 3);
    layer = luaL_checkint(l, 4);
    if (layer < LAYER_LOW || layer >= LAYER_NB) {
      arg_error(l, 4, StringConcat() << "Invalid layer: " << layer);
    }
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
 * \brief Implementation of hero:reset_solid_ground().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::hero_api_reset_solid_ground(lua_State* l) {

  Hero& hero = check_hero(l, 1);

  hero.reset_target_solid_ground_coords();

  return 0;
}

/**
 * \brief Implementation of hero:get_state().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::hero_api_get_state(lua_State* l) {

  Hero& hero = check_hero(l, 1);

  push_string(l, hero.get_state_name());
  return 1;
}

/**
 * \brief Implementation of hero:freeze().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::hero_api_freeze(lua_State* l) {

  Hero& hero = check_hero(l, 1);

  hero.start_freezed();

  return 0;
}

/**
 * \brief Implementation of hero:unfreeze().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::hero_api_unfreeze(lua_State* l) {

  Hero& hero = check_hero(l, 1);

  hero.start_free();

  return 0;
}

/**
 * \brief Implementation of hero:walk().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
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
 * \brief Implementation of hero:start_jumping().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::hero_api_start_jumping(lua_State* l) {

  Hero& hero = check_hero(l, 1);
  int direction = luaL_checkint(l, 2);
  int length = luaL_checkint(l, 3);
  bool ignore_obstacles = lua_toboolean(l, 4) != 0;

  hero.start_jumping(direction, length, ignore_obstacles, false);

  return 0;
}

/**
 * \brief Implementation of hero:start_treasure().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::hero_api_start_treasure(lua_State* l) {

  Hero& hero = check_hero(l, 1);
  const std::string& item_name = luaL_checkstring(l, 2);
  int variant = luaL_optint(l, 3, 1);
  const std::string& savegame_variable = luaL_optstring(l, 4, "");

  if (!savegame_variable.empty() && !is_valid_lua_identifier(savegame_variable)) {
    arg_error(l, 4, std::string(
        "savegame variable identifier expected, got '") +
        savegame_variable + "'");
  }

  if (!hero.get_game().get_equipment().item_exists(item_name)) {
    arg_error(l, 2, std::string("No such item: '") + item_name + "'");
  }

  Treasure treasure(hero.get_game(), item_name, variant, savegame_variable);
  if (treasure.is_found()) {
    arg_error(l, 4, "This treasure is already found");
  }

  int callback_ref = LUA_REFNIL;
  if (lua_gettop(l) >= 5) {
    luaL_checktype(l, 5, LUA_TFUNCTION);
    lua_settop(l, 5);
    callback_ref = luaL_ref(l, LUA_REGISTRYINDEX);
  }

  hero.start_treasure(treasure, callback_ref);

  return 0;
}

/**
 * \brief Implementation of hero:start_victory().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::hero_api_start_victory(lua_State* l) {

  Hero& hero = check_hero(l, 1);
  int callback_ref = LUA_REFNIL;
  if (lua_gettop(l) >= 2) {
    luaL_checktype(l, 2, LUA_TFUNCTION);
    lua_settop(l, 2);
    callback_ref = luaL_ref(l, LUA_REGISTRYINDEX);
  }

  hero.start_victory(callback_ref);

  return 0;
}

/**
 * \brief Implementation of hero:start_item().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::hero_api_start_item(lua_State* l) {

  Hero& hero = check_hero(l, 1);
  EquipmentItem& item = check_item(l, 2);

  if (hero.can_start_item(item)) {
    hero.start_item(item);
  }

  return 0;
}

/**
 * \brief Implementation of hero:start_boomerang().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::hero_api_start_boomerang(lua_State* l) {

  Hero& hero = check_hero(l, 1);
  int max_distance = luaL_checkint(l, 2);
  int speed = luaL_checkint(l, 3);
  const std::string& tunic_preparing_animation = luaL_checkstring(l, 4);
  const std::string& sprite_name = luaL_checkstring(l, 5);

  hero.start_boomerang(max_distance, speed,
      tunic_preparing_animation, sprite_name);

  return 0;
}

/**
 * \brief Implementation of hero:start_bow().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::hero_api_start_bow(lua_State* l) {

  Hero& hero = check_hero(l, 1);

  hero.start_bow();

  return 0;
}

/**
 * \brief Implementation of hero:start_hookshot().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::hero_api_start_hookshot(lua_State* l) {

  Hero& hero = check_hero(l, 1);

  hero.start_hookshot();

  return 0;
}

/**
 * \brief Implementation of hero:start_running().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::hero_api_start_running(lua_State* l) {

  Hero& hero = check_hero(l, 1);

  hero.start_running();

  return 0;
}

/**
 * \brief Implementation of hero:start_hurt().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::hero_api_start_hurt(lua_State* l) {

  Hero& hero = check_hero(l, 1);
  int source_x = luaL_checkint(l, 2);
  int source_y = luaL_checkint(l, 3);
  int life_points = luaL_checkint(l, 4);
  int magic_points = luaL_checkint(l, 5);

  hero.hurt(Rectangle(source_x, source_y),
      life_points, magic_points);

  return 0;
}

/**
 * \brief Notifies Lua that the hero is brandishing a treasure.
 *
 * Lua then manages the treasure's dialog.
 *
 * \param treasure The treasure being brandished.
 * \param callback_ref Lua ref to a function to call when the
 * treasure's dialog finishes (possibly LUA_REFNIL).
 */
void LuaContext::notify_hero_brandish_treasure(
    const Treasure& treasure, int callback_ref) {

  // This is getting tricky. We will define our own dialog callback
  // that will do some work and call callback_ref.
  std::ostringstream oss;
  oss << "_treasure." << treasure.get_item_name() << "." << treasure.get_variant();
  const std::string& dialog_id = oss.str();

  push_item(l, treasure.get_item());
  lua_pushinteger(l, treasure.get_variant());
  push_string(l, treasure.get_savegame_variable());
  if (callback_ref == LUA_REFNIL) {
    lua_pushnil(l);
  }
  else {
    push_callback(callback_ref);
    cancel_callback(callback_ref);  // Now we have the callback as a regular function, so remove the ref.
  }
  lua_pushcclosure(l, l_treasure_dialog_finished, 4);
  int dialog_callback_ref = create_ref();

  treasure.get_game().start_dialog(dialog_id, LUA_REFNIL, dialog_callback_ref);
}

/**
 * \brief Callback function executed after the dialog of obtaining a treasure.
 *
 * Upvalues: item, variant, savegame variable, callback/nil.
 *
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::l_treasure_dialog_finished(lua_State* l) {

  LuaContext& lua_context = get_lua_context(l);

  // The treasure's dialog is over.
  EquipmentItem& item = lua_context.check_item(l, lua_upvalueindex(1));
  int treasure_variant = luaL_checkint(l, lua_upvalueindex(2));
  const std::string& treasure_savegame_variable =
      luaL_checkstring(l, lua_upvalueindex(3));
  lua_pushvalue(l, lua_upvalueindex(4));

  // Check upvalues. Any error here would be the fault of the C++ side
  // because the user cannot call this function.
  Debug::check_assertion(item.get_game() != NULL,
      "Equipment item without game");

  Debug::check_assertion(lua_isnil(l, -1) || lua_isfunction(l, -1),
        "Expected function or nil for treasure callback");

  Game& game = *item.get_game();
  Hero& hero = game.get_hero();
  const Treasure treasure(game, item.get_name(), treasure_variant, treasure_savegame_variable);

  // Notify the Lua item and the Lua map.
  if (!lua_isnil(l, -1)) {
    // There is a user callback for this treasure.
    lua_context.call_function(0, 0, "treasure callback");
  }
  lua_context.item_on_obtained(item, treasure);
  lua_context.map_on_obtained_treasure(game.get_current_map(), treasure);

  if (hero.is_brandishing_treasure()) {
    // The script may have changed the hero's state.
    // If not, stop the treasure state.
    hero.start_free();
  }

  return 0;
}

/**
 * \brief Returns whether a value is a userdata of type NPC.
 * \param l A Lua context.
 * \param index An index in the stack.
 * \return true if the value at this index is an NPC.
 */
bool LuaContext::is_npc(lua_State* l, int index) {
  return is_userdata(l, index, entity_npc_module_name);
}

/**
 * \brief Checks that the userdata at the specified index of the stack is an
 * NPC and returns it.
 * \param l A Lua context.
 * \param index An index in the stack.
 * \return The NPC.
 */
NPC& LuaContext::check_npc(lua_State* l, int index) {
  return static_cast<NPC&>(check_userdata(l, index, entity_npc_module_name));
}

/**
 * \brief Pushes an NPC userdata onto the stack.
 * \param l A Lua context.
 * \param npc An NPC.
 */
void LuaContext::push_npc(lua_State* l, NPC& npc) {
  push_userdata(l, npc);
}

/**
 * \brief Returns whether a value is a userdata of type chest.
 * \param l A Lua context.
 * \param index An index in the stack.
 * \return true if the value at this index is a chest.
 */
bool LuaContext::is_chest(lua_State* l, int index) {
  return is_userdata(l, index, entity_chest_module_name);
}

/**
 * \brief Checks that the userdata at the specified index of the stack is a
 * chest and returns it.
 * \param l A Lua context.
 * \param index An index in the stack.
 * \return The chest.
 */
Chest& LuaContext::check_chest(lua_State* l, int index) {
  return static_cast<Chest&>(check_userdata(l, index, entity_chest_module_name));
}

/**
 * \brief Pushes a chest userdata onto the stack.
 * \param l A Lua context.
 * \param chest A chest.
 */
void LuaContext::push_chest(lua_State* l, Chest& chest) {
  push_userdata(l, chest);
}

/**
 * \brief Implementation of chest:is_open().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::chest_api_is_open(lua_State* l) {

  Chest& chest = check_chest(l, 1);

  lua_pushboolean(l, chest.is_open());
  return 1;
}

/**
 * \brief Implementation of chest:set_open().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::chest_api_set_open(lua_State* l) {

  Chest& chest = check_chest(l, 1);
  bool open = true;
  if (lua_gettop(l) >= 2) {
    open = lua_toboolean(l, 2);
  }

  chest.set_open(open);

  return 0;
}

/**
 * \brief Returns whether a value is a userdata of type block.
 * \param l A Lua context.
 * \param index An index in the stack.
 * \return true if the value at this index is a block.
 */
bool LuaContext::is_block(lua_State* l, int index) {
  return is_userdata(l, index, entity_block_module_name);
}

/**
 * \brief Checks that the userdata at the specified index of the stack is a
 * block and returns it.
 * \param l A Lua context.
 * \param index An index in the stack.
 * \return The block.
 */
Block& LuaContext::check_block(lua_State* l, int index) {
  return static_cast<Block&>(check_userdata(l, index, entity_block_module_name));
}

/**
 * \brief Pushes a block userdata onto the stack.
 * \param l A Lua context.
 * \param block A block.
 */
void LuaContext::push_block(lua_State* l, Block& block) {
  push_userdata(l, block);
}

/**
 * \brief Implementation of block:reset().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::block_api_reset(lua_State* l) {

  Block& block = check_block(l, 1);

  block.reset();

  return 0;
}

/**
 * \brief Implementation of block:is_pushable().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::block_api_is_pushable(lua_State* l) {

  const Block& block = check_block(l, 1);

  lua_pushboolean(l, block.is_pushable());
  return 1;
}

/**
 * \brief Implementation of block:set_pushable().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::block_api_set_pushable(lua_State* l) {

  Block& block = check_block(l, 1);
  bool pushable = true;
  if (lua_gettop(l) >= 2) {
    pushable = lua_toboolean(l, 2);
  }

  block.set_pushable(pushable);

  return 0;
}

/**
 * \brief Implementation of block:is_pullable().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::block_api_is_pullable(lua_State* l) {

  const Block& block = check_block(l, 1);

  lua_pushboolean(l, block.is_pullable());
  return 1;
}

/**
 * \brief Implementation of block:set_pullable().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::block_api_set_pullable(lua_State* l) {

  Block& block = check_block(l, 1);
  bool pullable = true;
  if (lua_gettop(l) >= 2) {
    pullable = lua_toboolean(l, 2);
  }

  block.set_pullable(pullable);

  return 0;
}

/**
 * \brief Implementation of block:get_maximum_moves().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::block_api_get_maximum_moves(lua_State* l) {

  const Block& block = check_block(l, 1);

  const int maximum_moves = block.get_maximum_moves();

  if (maximum_moves == 2) {
    // 2 means no maximum in the side C++ side (for now).
    lua_pushnil(l);
  }
  else {
    lua_pushinteger(l, maximum_moves);
  }
  return 1;
}

/**
 * \brief Implementation of block:set_maximum_moves().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::block_api_set_maximum_moves(lua_State* l) {

  Block& block = check_block(l, 1);
  if (lua_type(l, 2) != LUA_TNUMBER && lua_type(l, 2) != LUA_TNIL) {
    luaL_typerror(l, 2, "number or nil");
  }


  if (lua_isnumber(l, 2)) {
    const int maximum_moves = luaL_checkint(l, 2);
    if (maximum_moves < 0 || maximum_moves > 1) {
      arg_error(l, 2, "maximum_moves should be 0, 1 or nil");
    }
    block.set_maximum_moves(maximum_moves);
  }
  else if (lua_isnil(l, 2)) {
    // 2 means no maximum in C++.
    block.set_maximum_moves(2);
  }



  return 0;
}

/**
 * \brief Returns whether a value is a userdata of type switch.
 * \param l A Lua context.
 * \param index An index in the stack.
 * \return true if the value at this index is a switch.
 */
bool LuaContext::is_switch(lua_State* l, int index) {
  return is_userdata(l, index, entity_switch_module_name);
}

/**
 * \brief Checks that the userdata at the specified index of the stack is a
 * switch and returns it.
 * \param l A Lua context.
 * \param index An index in the stack.
 * \return The switch.
 */
Switch& LuaContext::check_switch(lua_State* l, int index) {
  return static_cast<Switch&>(check_userdata(l, index, entity_switch_module_name));
}

/**
 * \brief Pushes a switch userdata onto the stack.
 * \param l A Lua context.
 * \param sw A switch.
 */
void LuaContext::push_switch(lua_State* l, Switch& sw) {
  push_userdata(l, sw);
}

/**
 * \brief Implementation of switch:is_activated().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::switch_api_is_activated(lua_State* l) {

  Switch& sw = check_switch(l, 1);

  lua_pushboolean(l, sw.is_activated());
  return 1;
}

/**
 * \brief Implementation of switch:set_activated().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::switch_api_set_activated(lua_State* l) {

  Switch& sw = check_switch(l, 1);
  bool activated = true;
  if (lua_gettop(l) >= 2) {
    activated = lua_toboolean(l, 2);
  }

  sw.set_activated(activated);

  return 0;
}

/**
 * \brief Implementation of switch:set_locked().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::switch_api_set_locked(lua_State* l) {

  Switch& sw = check_switch(l, 1);
  bool locked = true;
  if (lua_gettop(l) >= 2) {
    locked = lua_toboolean(l, 2);
  }

  sw.set_locked(locked);

  return 0;
}

/**
 * \brief Returns whether a value is a userdata of type door.
 * \param l A Lua context.
 * \param index An index in the stack.
 * \return true if the value at this index is a door.
 */
bool LuaContext::is_door(lua_State* l, int index) {
  return is_userdata(l, index, entity_door_module_name);
}

/**
 * \brief Checks that the userdata at the specified index of the stack is a
 * door and returns it.
 * \param l A Lua context.
 * \param index An index in the stack.
 * \return The door.
 */
Door& LuaContext::check_door(lua_State* l, int index) {
  return static_cast<Door&>(check_userdata(l, index, entity_door_module_name));
}

/**
 * \brief Pushes a door userdata onto the stack.
 * \param l A Lua context.
 * \param door A door.
 */
void LuaContext::push_door(lua_State* l, Door& door) {
  push_userdata(l, door);
}

/**
 * \brief Implementation of door:is_open().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::door_api_is_open(lua_State* l) {

  Door& door = check_door(l, 1);

  lua_pushboolean(l, door.is_open());
  return 1;
}

/**
 * \brief Implementation of door:is_opening().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::door_api_is_opening(lua_State* l) {

  Door& door = check_door(l, 1);

  lua_pushboolean(l, door.is_opening());
  return 1;
}

/**
 * \brief Implementation of door:is_closed().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::door_api_is_closed(lua_State* l) {

  Door& door = check_door(l, 1);

  lua_pushboolean(l, door.is_closed());
  return 1;
}

/**
 * \brief Implementation of door:is_closing().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::door_api_is_closing(lua_State* l) {

  Door& door = check_door(l, 1);

  lua_pushboolean(l, door.is_closing());
  return 1;
}

/**
 * \brief Returns whether a value is a userdata of type shop treasure.
 * \param l A Lua context.
 * \param index An index in the stack.
 * \return true if the value at this index is a shop treasure.
 */
bool LuaContext::is_shop_treasure(lua_State* l, int index) {
  return is_userdata(l, index, entity_shop_treasure_module_name);
}

/**
 * \brief Checks that the userdata at the specified index of the stack is a
 * shop treasure and returns it.
 * \param l A Lua context.
 * \param index An index in the stack.
 * \return The shop treasure.
 */
ShopTreasure& LuaContext::check_shop_treasure(lua_State* l, int index) {
  return static_cast<ShopTreasure&>(
      check_userdata(l, index, entity_shop_treasure_module_name));
}

/**
 * \brief Pushes a shop treasure userdata onto the stack.
 * \param l A Lua context.
 * \param shop_treasure A shop treasure.
 */
void LuaContext::push_shop_treasure(lua_State* l, ShopTreasure& shop_treasure) {
  push_userdata(l, shop_treasure);
}

/**
 * \brief Notifies Lua that the hero interacts with a shop treasure.
 *
 * Lua then manages the dialogs shown to the player.
 *
 * \param shop_treasure A shop treasure.
 */
void LuaContext::notify_shop_treasure_interaction(ShopTreasure& shop_treasure) {

  push_shop_treasure(l, shop_treasure);
  lua_pushcclosure(l, l_shop_treasure_description_dialog_finished, 1);
  int callback_ref = create_ref();

  shop_treasure.get_game().start_dialog(shop_treasure.get_dialog_id(), LUA_REFNIL, callback_ref);
}

/**
 * \brief Callback function executed after the description dialog of
 * a shop treasure.
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::l_shop_treasure_description_dialog_finished(lua_State* l) {

  LuaContext& lua_context = get_lua_context(l);

  // The description message has just finished.
  // The shop treasure is the first upvalue.
  ShopTreasure& shop_treasure = lua_context.check_shop_treasure(l, lua_upvalueindex(1));
  Game& game = shop_treasure.get_game();

  if (shop_treasure.is_being_removed()) {
    // The shop treasure was removed during the dialog.
    return 0;
  }

  lua_pushinteger(l, shop_treasure.get_price());
  int price_ref = lua_context.create_ref();

  push_shop_treasure(l, shop_treasure);
  lua_pushcclosure(l, l_shop_treasure_question_dialog_finished, 1);
  int callback_ref = lua_context.create_ref();

  game.start_dialog("_shop.question", price_ref, callback_ref);

  return 0;
}

/**
 * \brief Callback function executed after the question dialog of
 * a shop treasure.
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::l_shop_treasure_question_dialog_finished(lua_State* l) {

  LuaContext& lua_context = get_lua_context(l);

  // The "do you want to buy?" question has just been displayed.
  // The shop treasure is the first upvalue.
  ShopTreasure& shop_treasure = check_shop_treasure(l, lua_upvalueindex(1));

  if (shop_treasure.is_being_removed()) {
    // The shop treasure was removed during the dialog.
    return 0;
  }

  // The first parameter is the answer.
  bool wants_to_buy = lua_isboolean(l, 1) && lua_toboolean(l, 1);

  Game& game = shop_treasure.get_game();
  if (wants_to_buy) {

    // The player wants to buy the item.
    Equipment& equipment = game.get_equipment();
    const Treasure& treasure = shop_treasure.get_treasure();
    EquipmentItem& item = treasure.get_item();

    if (equipment.get_money() < shop_treasure.get_price()) {
      // not enough rupees
      Sound::play("wrong");
      game.start_dialog("_shop.not_enough_money");
    }
    else if (item.has_amount() && item.get_amount() >= item.get_max_amount()) {
      // the player already has the maximum amount of this item
      Sound::play("wrong");
      game.start_dialog("_shop.amount_full");
    }
    else {

      bool can_buy = lua_context.shop_treasure_on_buying(shop_treasure);
      if (can_buy) {

        // give the treasure
        equipment.remove_money(shop_treasure.get_price());

        game.get_hero().start_treasure(treasure, LUA_REFNIL);
        if (treasure.is_saved()) {
          shop_treasure.remove_from_map();
          game.get_savegame().set_boolean(treasure.get_savegame_variable(), true);
        }
        lua_context.shop_treasure_on_bought(shop_treasure);
      }
    }
  }
  return 0;
}

/**
 * \brief Returns whether a value is a userdata of type pickable.
 * \param l A Lua context.
 * \param index An index in the stack.
 * \return true if the value at this index is a pickable.
 */
bool LuaContext::is_pickable(lua_State* l, int index) {
  return is_userdata(l, index, entity_pickable_module_name);
}

/**
 * \brief Checks that the userdata at the specified index of the stack is an
 * pickable and returns it.
 * \param l A Lua context.
 * \param index An index in the stack.
 * \return The pickable.
 */
Pickable& LuaContext::check_pickable(lua_State* l, int index) {
  return static_cast<Pickable&>(check_userdata(l, index, entity_pickable_module_name));
}

/**
 * \brief Pushes an pickable userdata onto the stack.
 * \param l A Lua context.
 * \param pickable A pickable treasure.
 */
void LuaContext::push_pickable(lua_State* l, Pickable& pickable) {
  push_userdata(l, pickable);
}

/**
 * \brief Implementation of pickable:get_followed_entity().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
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
 * \brief Implementation of pickable:get_falling_height().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::pickable_api_get_falling_height(lua_State* l) {

  Pickable& pickable = check_pickable(l, 1);

  lua_pushinteger(l, pickable.get_falling_height());
  return 1;
}

/**
 * \brief Implementation of pickable:get_treasure().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::pickable_api_get_treasure(lua_State* l) {

  Pickable& pickable = check_pickable(l, 1);
  const Treasure& treasure = pickable.get_treasure();

  push_string(l, treasure.get_item_name());
  lua_pushinteger(l, treasure.get_variant());
  lua_pushstring(l, treasure.get_savegame_variable().c_str());
  return 1;
}

/**
 * \brief Returns whether a value is a userdata of type enemy.
 * \param l A Lua context.
 * \param index An index in the stack.
 * \return true if the value at this index is an enemy.
 */
bool LuaContext::is_enemy(lua_State* l, int index) {
  return is_userdata(l, index, entity_enemy_module_name);
}

/**
 * \brief Checks that the userdata at the specified index of the stack is an
 * enemy and returns it.
 * \param l A Lua context.
 * \param index An index in the stack.
 * \return The enemy.
 */
Enemy& LuaContext::check_enemy(lua_State* l, int index) {
  return static_cast<Enemy&>(check_userdata(l, index, entity_enemy_module_name));
}

/**
 * \brief Returns whether a value is a userdata of type custom entity.
 * \param l A Lua context.
 * \param index An index in the stack.
 * \return true if the value at this index is a custom entity.
 */
bool LuaContext::is_custom_entity(lua_State* l, int index) {
  return is_userdata(l, index, entity_custom_module_name);
}

/**
 * \brief Checks that the userdata at the specified index of the stack is a
 * custom entity and returns it.
 * \param l A Lua context.
 * \param index An index in the stack.
 * \return The custom entity.
 */
CustomEntity& LuaContext::check_custom_entity(lua_State* l, int index) {
  return static_cast<CustomEntity&>(check_userdata(l, index, entity_custom_module_name));
}

/**
 * \brief Pushes an enemy userdata onto the stack.
 * \param l A Lua context.
 * \param enemy An enemy.
 */
void LuaContext::push_enemy(lua_State* l, Enemy& enemy) {
  push_userdata(l, enemy);
}

/**
 * \brief Implementation of enemy:get_breed().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::enemy_api_get_breed(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);

  push_string(l, enemy.get_breed());
  return 1;
}

/**
 * \brief Implementation of enemy:get_life().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::enemy_api_get_life(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);

  lua_pushinteger(l, enemy.get_life());
  return 1;
}

/**
 * \brief Implementation of enemy:set_life().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::enemy_api_set_life(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);
  int life = luaL_checkint(l, 2);

  enemy.set_life(life);

  return 0;
}

/**
 * \brief Implementation of enemy:add_life().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::enemy_api_add_life(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);
  int points = luaL_checkint(l, 2);

  enemy.set_life(enemy.get_life() + points);

  return 0;
}

/**
 * \brief Implementation of enemy:remove_life().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::enemy_api_remove_life(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);
  int points = luaL_checkint(l, 2);

  enemy.set_life(enemy.get_life() - points);

  return 0;
}

/**
 * \brief Implementation of enemy:get_damage().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::enemy_api_get_damage(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);

  lua_pushinteger(l, enemy.get_damage());
  return 1;
}

/**
 * \brief Implementation of enemy:set_damage().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::enemy_api_set_damage(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);
  int damage = luaL_checkint(l, 2);

  enemy.set_damage(damage);

  return 0;
}

/**
 * \brief Implementation of enemy:get_magic_damage().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::enemy_api_get_magic_damage(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);

  lua_pushinteger(l, enemy.get_magic_damage());
  return 1;
}

/**
 * \brief Implementation of enemy:set_magic_damage().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::enemy_api_set_magic_damage(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);
  int magic_damage = luaL_checkint(l, 2);

  enemy.set_magic_damage(magic_damage);

  return 0;
}

/**
 * \brief Implementation of enemy:is_pushed_back_when_hurt().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::enemy_api_is_pushed_back_when_hurt(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);

  lua_pushboolean(l, enemy.get_pushed_back_when_hurt());
  return 1;
}

/**
 * \brief Implementation of enemy:set_pushed_back_when_hurt().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::enemy_api_set_pushed_back_when_hurt(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);
  bool push_back = true;
  if (lua_gettop(l) >= 2) {
    push_back = lua_toboolean(l, 2);
  }

  enemy.set_pushed_back_when_hurt(push_back);

  return 0;
}

/**
 * \brief Implementation of enemy:get_push_hero_on_sword().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::enemy_api_get_push_hero_on_sword(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);

  lua_pushboolean(l, enemy.get_push_hero_on_sword());
  return 1;
}

/**
 * \brief Implementation of enemy:set_push_hero_on_sword().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::enemy_api_set_push_hero_on_sword(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);
  bool push = true;
  if (lua_gettop(l) >= 2) {
    push = lua_toboolean(l, 2);
  }

  enemy.set_push_hero_on_sword(push);

  return 0;
}

/**
 * \brief Implementation of enemy:get_can_hurt_hero_running().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::enemy_api_get_can_hurt_hero_running(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);

  lua_pushboolean(l, enemy.get_can_hurt_hero_running());
  return 1;
}

/**
 * \brief Implementation of enemy:set_can_hurt_hero_running().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::enemy_api_set_can_hurt_hero_running(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);
  bool can_hurt_hero_running = true;
  if (lua_gettop(l) >= 2) {
    can_hurt_hero_running = lua_toboolean(l, 2);
  }

  enemy.set_can_hurt_hero_running(can_hurt_hero_running);

  return 0;
}

/**
 * \brief Implementation of enemy:get_hurt_style().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::enemy_api_get_hurt_style(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);

  Enemy::HurtStyle hurt_style = enemy.get_hurt_style();

  push_string(l, enemy_hurt_style_names[hurt_style]);
  return 1;
}

/**
 * \brief Implementation of enemy:set_hurt_style().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::enemy_api_set_hurt_style(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);
  Enemy::HurtStyle hurt_style = check_enum<Enemy::HurtStyle>(
      l, 2, enemy_hurt_style_names);

  enemy.set_hurt_style(hurt_style);

  return 0;
}

/**
 * \brief Implementation of enemy:get_can_attack().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::enemy_api_get_can_attack(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);

  lua_pushboolean(l, enemy.get_can_attack());
  return 1;
}

/**
 * \brief Implementation of enemy:set_can_attack().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::enemy_api_set_can_attack(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);
  bool can_attack = true;
  if (lua_gettop(l) >= 2) {
    can_attack = lua_toboolean(l, 2);
  }

  enemy.set_can_attack(can_attack);

  return 0;
}

/**
 * \brief Implementation of enemy:get_minimum_shield_needed().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::enemy_api_get_minimum_shield_needed(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);

  int shield_level = enemy.get_minimum_shield_needed();

  lua_pushinteger(l, shield_level);
  return 1;
}

/**
 * \brief Implementation of enemy:set_minimum_shield_needed().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::enemy_api_set_minimum_shield_needed(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);
  int shield_level = luaL_checkint(l, 2);

  enemy.set_minimum_shield_needed(shield_level);

  return 0;
}

/**
 * \brief Implementation of enemy:set_attack_consequence().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::enemy_api_set_attack_consequence(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);
  EnemyAttack attack = check_enum<EnemyAttack>(l, 2, enemy_attack_names);

  if (lua_isnumber(l, 3)) {
    int life_points = luaL_checkint(l, 3);
    if (life_points < 0) {
      LuaContext::arg_error(l, 3, StringConcat() <<
          "Invalid life points number for attack consequence: '"
          << life_points << "'");
    }
    enemy.set_attack_consequence(attack, EnemyReaction::HURT, life_points);
  }
  else {
    EnemyReaction::ReactionType reaction = check_enum<EnemyReaction::ReactionType>(
        l, 3, EnemyReaction::reaction_names);
    enemy.set_attack_consequence(attack, reaction);
  }

  return 0;
}

/**
 * \brief Implementation of enemy:set_attack_consequence_sprite().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::enemy_api_set_attack_consequence_sprite(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);
  Sprite& sprite = check_sprite(l, 2);
  EnemyAttack attack = check_enum<EnemyAttack>(l, 3, enemy_attack_names);

  if (lua_isnumber(l, 4)) {
    int life_points = luaL_checkint(l, 4);
    if (life_points < 0) {
      LuaContext::arg_error(l, 4, StringConcat() <<
          "Invalid life points number for attack consequence: '"
          << life_points << "'");
    }
    enemy.set_attack_consequence_sprite(sprite, attack, EnemyReaction::HURT, life_points);
  }
  else {
    EnemyReaction::ReactionType reaction = check_enum<EnemyReaction::ReactionType>(
        l, 4, EnemyReaction::reaction_names);
    enemy.set_attack_consequence_sprite(sprite, attack, reaction);
  }

  return 0;
}

/**
 * \brief Implementation of enemy:set_default_attack_consequences().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::enemy_api_set_default_attack_consequences(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);

  enemy.set_default_attack_consequences();

  return 0;
}

/**
 * \brief Implementation of enemy:set_default_attack_consequences_sprite().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::enemy_api_set_default_attack_consequences_sprite(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);
  Sprite& sprite = check_sprite(l, 2);

  enemy.set_default_attack_consequences_sprite(sprite);

  return 0;
}

/**
 * \brief Implementation of enemy:set_invincible().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::enemy_api_set_invincible(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);

  enemy.set_no_attack_consequences();

  return 0;
}

/**
 * \brief Implementation of enemy:set_invincible_sprite().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::enemy_api_set_invincible_sprite(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);
  Sprite& sprite = check_sprite(l, 2);

  enemy.set_no_attack_consequences_sprite(sprite);

  return 0;
}

/**
 * \brief Implementation of enemy:set_treasure().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::enemy_api_set_treasure(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);
  std::string item_name, savegame_variable;
  int variant = 1;

  if (lua_gettop(l) >= 2 && !lua_isnil(l, 2)) {
    item_name = luaL_checkstring(l, 2);
  }
  if (lua_gettop(l) >= 3 && !lua_isnil(l, 3)) {
    variant = luaL_checkint(l, 3);
  }
  if (lua_gettop(l) >= 4 && !lua_isnil(l, 4)) {
    savegame_variable = luaL_checkstring(l, 4);
  }

  if (!savegame_variable.empty() && !is_valid_lua_identifier(savegame_variable)) {
    LuaContext::arg_error(l, 4, StringConcat() <<
        "savegame variable identifier expected, got '" <<
        savegame_variable << "'");
  }

  Treasure treasure(enemy.get_game(), item_name, variant, savegame_variable);
  enemy.set_treasure(treasure);

  return 0;
}

/**
 * \brief Implementation of enemy:is_traversable().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::enemy_api_is_traversable(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);

  lua_pushboolean(l, enemy.is_traversable());
  return 1;
}

/**
 * \brief Implementation of enemy:set_traversable().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::enemy_api_set_traversable(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);

  bool traversable = true;
  if (lua_gettop(l) >= 2) {
    traversable = lua_toboolean(l, 2);
  }

  enemy.set_traversable(traversable);

  return 0;
}

/**
 * \brief Implementation of enemy:get_obstacle_behavior().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::enemy_api_get_obstacle_behavior(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);

  Enemy::ObstacleBehavior behavior = enemy.get_obstacle_behavior();

  push_string(l, enemy_obstacle_behavior_names[behavior]);
  return 1;
}

/**
 * \brief Implementation of enemy:set_obstacle_behavior().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::enemy_api_set_obstacle_behavior(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);
  Enemy::ObstacleBehavior behavior = check_enum<Enemy::ObstacleBehavior>(
      l, 2, enemy_obstacle_behavior_names);

  enemy.set_obstacle_behavior(behavior);

  return 0;
}

/**
 * \brief Implementation of enemy:restart().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::enemy_api_restart(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);

  enemy.restart();

  return 0;
}

/**
 * \brief Implementation of enemy:hurt().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::enemy_api_hurt(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);
  int life_points = luaL_checkint(l, 2);

  if (enemy.is_in_normal_state() && !enemy.is_invulnerable()) {
    Hero& hero = enemy.get_map().get_entities().get_hero();
    enemy.set_attack_consequence(ATTACK_SCRIPT, EnemyReaction::HURT, life_points);
    enemy.try_hurt(ATTACK_SCRIPT, hero, NULL);
  }

  return 0;
}

/**
 * \brief Implementation of enemy:immobilize().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::enemy_api_immobilize(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);

  if (enemy.is_in_normal_state() && !enemy.is_invulnerable()) {
    Hero& hero = enemy.get_map().get_entities().get_hero();
    enemy.set_attack_consequence(ATTACK_SCRIPT, EnemyReaction::IMMOBILIZED, 0);
    enemy.try_hurt(ATTACK_SCRIPT, hero, NULL);
  }

  return 0;
}

/**
 * \brief Implementation of enemy:create_enemy().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::enemy_api_create_enemy(lua_State* l) {

  Enemy& enemy = check_enemy(l, 1);
  luaL_checktype(l, 2, LUA_TTABLE);
  const std::string& name = opt_string_field(l, 2, "name", "");
  int layer = opt_int_field(l, 2, "layer", enemy.get_layer());
  int x = opt_int_field(l, 2, "x", 0);
  int y = opt_int_field(l, 2, "y", 0);
  int direction = opt_int_field(l, 2, "direction", 3);
  const std::string& breed = check_string_field(l, 2, "breed");
  Enemy::Rank rank = Enemy::Rank(opt_int_field(l, 2, "rank", 0));
  const std::string& savegame_variable = opt_string_field(l, 2, "savegame_variable", "");
  const std::string& treasure_name = opt_string_field(l, 2, "treasure_name", "");
  int treasure_variant = opt_int_field(l, 2, "treasure_variant", 1);
  const std::string& treasure_savegame_variable = opt_string_field(l, 2, "treasure_savegame_variable", "");

  if (layer < LAYER_LOW || layer >= LAYER_NB) {
    arg_error(l, 2, StringConcat() << "Invalid layer: " << layer);
  }

  if (!savegame_variable.empty() && !is_valid_lua_identifier(savegame_variable)) {
    LuaContext::arg_error(l, 2, StringConcat() <<
        "Bad field 'savegame_variable' (invalid savegame variable identifier '" <<
        savegame_variable << "'");
  }

  if (!treasure_savegame_variable.empty() && !is_valid_lua_identifier(treasure_savegame_variable)) {
    LuaContext::arg_error(l, 2, StringConcat() <<
        "Bad field 'treasure_savegame_variable' (invalid savegame variable identifier '" <<
        treasure_savegame_variable << "'");
  }

  // Make x and y relative to the existing enemy.
  x += enemy.get_x();
  y += enemy.get_y();

  // Create the new enemy.
  Map& map = enemy.get_map();
  Game& game = map.get_game();
  MapEntity* entity = Enemy::create(
      game,
      breed,
      rank,
      savegame_variable,
      name,
      Layer(layer),
      x,
      y,
      direction,
      Treasure(game, treasure_name, treasure_variant, treasure_savegame_variable));

  if (entity == NULL) {
    // The enemy is saved as already dead.
    lua_pushnil(l);
    return 1;
  }

  entity->set_optimization_distance(enemy.get_optimization_distance());
  map.get_entities().add_entity(entity);

  if (entity->get_type() == ENTITY_ENEMY) {  // Because it may also be a pickable treasure.
    (static_cast<Enemy*>(entity))->restart();
  }

  push_entity(l, *entity);
  return 1;
}

/**
 * \brief Pushes a custom entity userdata onto the stack.
 * \param l A Lua context.
 * \param custom_entity A custom entity.
 */
void LuaContext::push_custom_entity(lua_State* l, CustomEntity& entity) {
  push_userdata(l, entity);
}

/**
 * \brief Implementation of custom_entity:get_model().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::custom_entity_api_get_model(lua_State* l) {

  CustomEntity& entity = check_custom_entity(l, 1);

  push_string(l, entity.get_model());
  return 1;
}

/**
 * \brief Calls the on_removed() method of a Lua map entity if it is defined.
 *
 * Also stops timers associated to the entity.
 *
 * \param entity A map entity.
 */
void LuaContext::entity_on_removed(MapEntity& entity) {

  if (!entity.is_known_to_lua()) {
    return;
  }

  push_entity(l, entity);
  if (userdata_has_field(entity, "on_removed")) {
    on_removed();
  }
  remove_timers(-1);  // Stop timers associated to this entity.
  lua_pop(l, 1);
}

/**
 * \brief Calls the on_position_changed() method of a Lua map entity.
 *
 * Does nothing if the method is not defined.
 *
 * \param entity A map entity.
 * \param xy The new coordinates.
 * \param layer The new layer.
 */
void LuaContext::entity_on_position_changed(
    MapEntity& entity, const Rectangle& xy, Layer layer) {

  if (!userdata_has_field(entity, "on_position_changed")) {
    return;
  }

  push_entity(l, entity);
  on_position_changed(xy, layer);
  lua_pop(l, 1);
}

/**
 * \brief Calls the on_obstacle_reached() method of a Lua map entity.
 *
 * Does nothing if the method is not defined.
 *
 * \param entity A map entity.
 * \param movement The movement that reached an obstacle.
 */
void LuaContext::entity_on_obstacle_reached(
    MapEntity& entity, Movement& movement) {

  if (!userdata_has_field(entity, "on_obstacle_reached")) {
    return;
  }

  push_entity(l, entity);
  on_obstacle_reached(movement);
  lua_pop(l, 1);
}

/**
 * \brief Calls the on_movement_changed() method of a Lua map entity.
 *
 * Does nothing if the method is not defined.
 *
 * \param entity A map entity.
 * \param movement Its movement.
 */
void LuaContext::entity_on_movement_changed(
    MapEntity& entity, Movement& movement) {

  if (!userdata_has_field(entity, "on_movement_changed")) {
    return;
  }

  push_entity(l, entity);
  on_movement_changed(movement);
  lua_pop(l, 1);
}

/**
 * \brief Calls the on_movement_finished() method of a Lua map entity.
 *
 * Does nothing if the method is not defined.
 *
 * \param entity A map entity.
 */
void LuaContext::entity_on_movement_finished(MapEntity& entity) {

  if (!userdata_has_field(entity, "on_movement_finished")) {
    return;
  }

  push_entity(l, entity);
  on_movement_finished();
  lua_pop(l, 1);
}

/**
 * \brief Calls the on_state_changed() method of a Lua hero.
 *
 * Does nothing if the method is not defined.
 *
 * \param hero The hero.
 * \param state_name A name describing the new state.
 */
void LuaContext::hero_on_state_changed(
    Hero& hero, const std::string& state_name) {

  if (!userdata_has_field(hero, "on_state_changed")) {
    return;
  }

  push_hero(l, hero);
  on_state_changed(state_name);
  lua_pop(l, 1);
}

/**
 * \brief Calls the on_interaction() method of a Lua NPC.
 *
 * Does nothing if the method is not defined.
 *
 * \param npc An NPC.
 */
void LuaContext::npc_on_interaction(NPC& npc) {

  if (!userdata_has_field(npc, "on_interaction")) {
    return;
  }

  push_npc(l, npc);
  on_interaction();
  lua_pop(l, 1);
}

/**
 * \brief Calls the on_interaction_item() method of a Lua NPC.
 *
 * Does nothing if the method is not defined.
 *
 * \param npc An NPC.
 * \param item_used The equipment item used.
 * \return \c true if an interaction occurred.
 */
bool LuaContext::npc_on_interaction_item(NPC& npc, EquipmentItem& item_used) {

  if (!userdata_has_field(npc, "on_interaction_item")) {
    return false;
  }

  push_npc(l, npc);
  bool result = on_interaction_item(item_used);
  lua_pop(l, 1);
  return result;
}

/**
 * \brief Calls the on_collision_fire() method of a Lua NPC.
 *
 * Does nothing if the method is not defined.
 *
 * \param npc An NPC.
 */
void LuaContext::npc_on_collision_fire(NPC& npc) {

  if (!userdata_has_field(npc, "on_collision_fire")) {
    return;
  }

  push_npc(l, npc);
  on_collision_fire();
  lua_pop(l, 1);
}

/**
 * \brief Calls the on_moving() method of a Lua block.
 *
 * Does nothing if the method is not defined.
 *
 * \param block a block.
 */
void LuaContext::block_on_moving(Block& block) {

  if (!userdata_has_field(block, "on_moving")) {
    return;
  }

  push_block(l, block);
  on_moving();
  lua_pop(l, 1);
}

/**
 * \brief Calls the on_moved() method of a Lua block.
 *
 * Does nothing if the method is not defined.
 *
 * \param block a block.
 */
void LuaContext::block_on_moved(Block& block) {

  if (!userdata_has_field(block, "on_moved")) {
    return;
  }

  push_block(l, block);
  on_moved();
  lua_pop(l, 1);
}

/**
 * \brief Calls the on_empty() method of a Lua chest.
 *
 * Does nothing if the method is not defined.
 *
 * \param chest A chest.
 * \return \c true if the on_empty() method is defined.
 */
bool LuaContext::chest_on_empty(Chest& chest) {

  if (!userdata_has_field(chest, "on_empty")) {
    return false;
  }

  push_chest(l, chest);
  bool exists = on_empty();
  lua_pop(l, 1);
  return exists;
}

/**
 * \brief Calls the on_activated() method of a Lua switch.
 *
 * Does nothing if the method is not defined.
 *
 * \param sw A switch.
 */
void LuaContext::switch_on_activated(Switch& sw) {

  if (!userdata_has_field(sw, "on_activated")) {
    return;
  }

  push_switch(l, sw);
  on_activated();
  lua_pop(l, 1);
}

/**
 * \brief Calls the on_inactivated() method of a Lua switch.
 *
 * Does nothing if the method is not defined.
 *
 * \param sw A switch.
 */
void LuaContext::switch_on_inactivated(Switch& sw) {

  if (!userdata_has_field(sw, "on_inactivated")) {
    return;
  }

  push_switch(l, sw);
  on_inactivated();
  lua_pop(l, 1);
}

/**
 * \brief Calls the on_left() method of a Lua switch.
 *
 * Does nothing if the method is not defined.
 *
 * \param sw A switch.
 */
void LuaContext::switch_on_left(Switch& sw) {

  if (!userdata_has_field(sw, "on_left")) {
    return;
  }

  push_switch(l, sw);
  on_left();
  lua_pop(l, 1);
}

/**
 * \brief Calls the on_activated() method of a Lua sensor.
 *
 * Does nothing if the method is not defined.
 *
 * \param sensor A sensor.
 */
void LuaContext::sensor_on_activated(Sensor& sensor) {

  if (!userdata_has_field(sensor, "on_activated")) {
    return;
  }

  push_entity(l, sensor);
  on_activated();
  lua_pop(l, 1);
}

/**
 * \brief Calls the on_activated_repeat() method of a Lua sensor.
 *
 * Does nothing if the method is not defined.
 *
 * \param sensor A sensor.
 */
void LuaContext::sensor_on_activated_repeat(Sensor& sensor) {

  if (!userdata_has_field(sensor, "on_activated_repeat")) {
    return;
  }

  push_entity(l, sensor);
  on_activated_repeat();
  lua_pop(l, 1);
}

/**
 * \brief Calls the on_left() method of a Lua sensor.
 * \param sensor A sensor.
 */
void LuaContext::sensor_on_left(Sensor& sensor) {

  if (!userdata_has_field(sensor, "on_left")) {
    return;
  }

  push_entity(l, sensor);
  on_left();
  lua_pop(l, 1);
}

/**
 * \brief Calls the on_collision_explosion() method of a Lua sensor.
 *
 * Does nothing if the method is not defined.
 *
 * \param sensor A sensor.
 */
void LuaContext::sensor_on_collision_explosion(Sensor& sensor) {

  if (!userdata_has_field(sensor, "on_collision_explosion")) {
    return;
  }

  push_entity(l, sensor);
  on_collision_explosion();
  lua_pop(l, 1);
}

/**
 * \brief Calls the on_activating() method of a Lua separator.
 *
 * Does nothing if the method is not defined.
 *
 * \param separator A separator.
 * \param direction4 Direction of the traversal.
 */
void LuaContext::separator_on_activating(Separator& separator, int direction4) {

  if (!userdata_has_field(separator, "on_activating")) {
    return;
  }

  push_entity(l, separator);
  on_activating(direction4);
  lua_pop(l, 1);
}

/**
 * \brief Calls the on_activated() method of a Lua separator.
 *
 * Does nothing if the method is not defined.
 *
 * \param separator A separator.
 * \param direction4 Direction of the traversal.
 */
void LuaContext::separator_on_activated(Separator& separator, int direction4) {

  if (!userdata_has_field(separator, "on_activated")) {
    return;
  }

  push_entity(l, separator);
  on_activated(direction4);
  lua_pop(l, 1);
}

/**
 * \brief Calls the on_opened() method of a Lua door.
 *
 * Does nothing if the method is not defined.
 *
 * \param door A door.
 */
void LuaContext::door_on_opened(Door& door) {

  if (!userdata_has_field(door, "on_opened")) {
    return;
  }

  push_door(l, door);
  on_opened();
  lua_pop(l, 1);
}

/**
 * \brief Calls the on_closed() method of a Lua door.
 *
 * Does nothing if the method is not defined.
 *
 * \param door A door.
 */
void LuaContext::door_on_closed(Door& door) {

  if (!userdata_has_field(door, "on_closed")) {
    return;
  }

  push_door(l, door);
  on_closed();
  lua_pop(l, 1);
}

/**
 * \brief Calls the on_buying() method of a Lua shop treasure.
 *
 * Does nothing if the method is not defined.
 *
 * \param shop_treasure A shop treasure.
 * \return \c true if the player is allowed to buy the treasure.
 */
bool LuaContext::shop_treasure_on_buying(ShopTreasure& shop_treasure) {

  if (!userdata_has_field(shop_treasure, "on_buying")) {
    return true;
  }

  push_shop_treasure(l, shop_treasure);
  bool result = on_buying();
  lua_pop(l, 1);
  return result;
}

/**
 * \brief Calls the on_bought() method of a Lua shop treasure.
 *
 * Does nothing if the method is not defined.
 *
 * \param shop_treasure A shop treasure.
 */
void LuaContext::shop_treasure_on_bought(ShopTreasure& shop_treasure) {

  if (!userdata_has_field(shop_treasure, "on_bought")) {
    return;
  }

  push_shop_treasure(l, shop_treasure);
  on_bought();
  lua_pop(l, 1);
}

/**
 * \brief Calls the on_update() method of a Lua enemy.
 *
 * Does nothing if the method is not defined.
 *
 * \param enemy An enemy.
 */
void LuaContext::enemy_on_update(Enemy& enemy) {

  // This particular method is tried so often that we want to save optimize
  // the std::string construction.
  static const std::string method_name = "on_update";
  if (!userdata_has_field(enemy, method_name)) {
    return;
  }

  push_enemy(l, enemy);
  on_update();
  lua_pop(l, 1);
}

/**
 * \brief Calls the on_suspended() method of a Lua enemy.
 *
 * Does nothing if the method is not defined.
 *
 * \param enemy An enemy.
 * \param suspended true if the enemy is suspended.
 */
void LuaContext::enemy_on_suspended(Enemy& enemy, bool suspended) {

  if (!userdata_has_field(enemy, "on_suspended")) {
    return;
  }

  push_enemy(l, enemy);
  on_suspended(suspended);
  lua_pop(l, 1);
}

/**
 * \brief Calls the on_created() method of a Lua enemy.
 *
 * Does nothing if the method is not defined.
 *
 * \param enemy An enemy.
 */
void LuaContext::enemy_on_created(Enemy& enemy) {

  if (!userdata_has_field(enemy, "on_created")) {
    return;
  }

  push_enemy(l, enemy);
  on_created();
  lua_pop(l, 1);
}

/**
 * \brief Calls the on_enabled() method of a Lua enemy.
 *
 * Does nothing if the method is not defined.
 *
 * \param enemy An enemy.
 */
void LuaContext::enemy_on_enabled(Enemy& enemy) {

  if (!userdata_has_field(enemy, "on_enabled")) {
    return;
  }

  push_enemy(l, enemy);
  on_enabled();
  lua_pop(l, 1);
}

/**
 * \brief Calls the on_disabled() method of a Lua enemy.
 *
 * Does nothing if the method is not defined.
 *
 * \param enemy An enemy.
 */
void LuaContext::enemy_on_disabled(Enemy& enemy) {

  if (!userdata_has_field(enemy, "on_disabled")) {
    return;
  }

  push_enemy(l, enemy);
  on_disabled();
  lua_pop(l, 1);
}

/**
 * \brief Calls the on_restarted() method of a Lua enemy if it is defined.
 *
 * Also stops timers associated to the entity.
 *
 * \param enemy An enemy.
 */
void LuaContext::enemy_on_restarted(Enemy& enemy) {

  if (!enemy.is_known_to_lua()) {
    return;
  }

  push_enemy(l, enemy);
  remove_timers(-1);  // Stop timers associated to this enemy.
  if (userdata_has_field(enemy, "on_restarted")) {
    on_restarted();
  }
  lua_pop(l, 1);
}

/**
 * \brief Calls the on_pre_draw() method of a Lua enemy.
 *
 * Does nothing if the method is not defined.
 *
 * \param enemy An enemy.
 */
void LuaContext::enemy_on_pre_draw(Enemy& enemy) {

  if (!userdata_has_field(enemy, "on_pre_draw")) {
    return;
  }

  push_enemy(l, enemy);
  on_pre_draw();
  lua_pop(l, 1);
}

/**
 * \brief Calls the on_post_draw() method of a Lua enemy.
 *
 * Does nothing if the method is not defined.
 *
 * \param enemy An enemy.
 */
void LuaContext::enemy_on_post_draw(Enemy& enemy) {

  if (!userdata_has_field(enemy, "on_post_draw")) {
    return;
  }

  push_enemy(l, enemy);
  on_post_draw();
  lua_pop(l, 1);
}

/**
 * \brief Calls the on_collision_enemy() method of a Lua enemy.
 *
 * Does nothing if the method is not defined.
 *
 * \param enemy An enemy.
 * \param other_enemy Another enemy colliding with the first one.
 * \param other_sprite Colliding sprite of the other enemy.
 * \param this_sprite Colliding sprite of the first enemy.
 */
void LuaContext::enemy_on_collision_enemy(Enemy& enemy,
    Enemy& other_enemy, Sprite& other_sprite, Sprite& this_sprite) {

  if (!userdata_has_field(enemy, "on_collision_enemy")) {
    return;
  }

  push_enemy(l, enemy);
  on_collision_enemy(other_enemy, other_sprite, this_sprite);
  lua_pop(l, 1);
}

/**
 * \brief Calls the on_custom_attack_received() method of a Lua enemy.
 *
 * Does nothing if the method is not defined.
 *
 * \param enemy An enemy.
 * \param attack The attack received.
 * \param sprite The sprite that receives the attack if any.
 */
void LuaContext::enemy_on_custom_attack_received(Enemy& enemy,
    EnemyAttack attack, Sprite* sprite) {

  if (!userdata_has_field(enemy, "on_custom_attack_received")) {
    return;
  }

  push_enemy(l, enemy);
  on_custom_attack_received(attack, sprite);
  lua_pop(l, 1);
}

/**
 * \brief Calls the on_hurt() method of a Lua enemy if it is defined.
 *
 * Also stops timers associated to the enemy.
 *
 * \param enemy An enemy.
 * \param attack The attack received.
 * \param life_lost Number of life points just lost.
 */
void LuaContext::enemy_on_hurt(Enemy& enemy, EnemyAttack attack, int life_lost) {

  if (!enemy.is_known_to_lua()) {
    return;
  }

  push_enemy(l, enemy);
  remove_timers(-1);  // Stop timers associated to this enemy.
  if (userdata_has_field(enemy, "on_hurt")) {
    on_hurt(attack, life_lost);
  }
  lua_pop(l, 1);
}

/**
 * \brief Calls the on_dying() method of a Lua enemy if it is defined.
 *
 * Also stops timers associated to the enemy.
 *
 * \param enemy An enemy.
 */
void LuaContext::enemy_on_dying(Enemy& enemy) {

  if (!enemy.is_known_to_lua()) {
    return;
  }

  push_enemy(l, enemy);
  remove_timers(-1);  // Stop timers associated to this enemy.
  if (userdata_has_field(enemy, "on_dying")) {
    on_dying();
  }
  lua_pop(l, 1);
}

/**
 * \brief Calls the on_dead() method of a Lua enemy.
 *
 * Does nothing if the method is not defined.
 *
 * \param enemy An enemy.
 */
void LuaContext::enemy_on_dead(Enemy& enemy) {

  if (!userdata_has_field(enemy, "on_dead")) {
    return;
  }

  push_enemy(l, enemy);
  on_dead();
  lua_pop(l, 1);
}

/**
 * \brief Calls the on_immobilized() method of a Lua enemy if it is defined.
 *
 * Also stops timers associated to the enemy.
 *
 * \param enemy An enemy.
 */
void LuaContext::enemy_on_immobilized(Enemy& enemy) {

  if (!enemy.is_known_to_lua()) {
    return;
  }

  push_enemy(l, enemy);
  remove_timers(-1);  // Stop timers associated to this enemy.
  if (userdata_has_field(enemy, "on_immobilized")) {
    on_immobilized();
  }
  lua_pop(l, 1);
}

}

