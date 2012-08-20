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
#include "lua/LuaContext.h"
#include "Game.h"
#include "Map.h"
#include "DialogBox.h"
#include "Treasure.h"
#include "entities/MapEntities.h"
#include "entities/Door.h"
#include "entities/NPC.h"
#include "entities/Sensor.h"
#include "entities/Chest.h"
#include "entities/DynamicTile.h"
#include "entities/Stairs.h"
#include "entities/Wall.h"
#include "entities/Block.h"
#include "entities/Switch.h"
#include "entities/Crystal.h"
#include "entities/Teletransporter.h"
#include "entities/Hero.h"
#include "entities/Pickable.h"
#include "entities/Destructible.h"
#include "entities/Bomb.h"
#include "entities/Fire.h"
#include "movements/Movement.h"
#include "lowlevel/Sound.h"
#include "lowlevel/Debug.h"
#include <lua.hpp>

const std::string LuaContext::map_module_name = "sol.map";

/**
 * @brief Initializes the map features provided to Lua.
 */
void LuaContext::register_map_module() {

  static const luaL_Reg methods[] = {
      { "get_game", map_api_get_game },
      { "dialog_start", map_api_dialog_start },
      { "dialog_set_variable", map_api_dialog_set_variable },
      { "dialog_set_style", map_api_dialog_set_style },
      { "set_pause_enabled", map_api_set_pause_enabled },
      { "light_get", map_api_light_get },
      { "light_set", map_api_light_set },
      { "treasure_give", map_api_treasure_give },
      { "camera_move", map_api_camera_move },
      { "draw_sprite", map_api_draw_sprite },
      { "tileset_get", map_api_tileset_get },
      { "tileset_set", map_api_tileset_set },
      { "hero_freeze", map_api_hero_freeze },
      { "hero_unfreeze", map_api_hero_unfreeze },
      { "hero_set_map", map_api_hero_set_map },
      { "hero_set_visible", map_api_hero_set_visible },
      { "hero_get_direction", map_api_hero_get_direction },
      { "hero_set_direction", map_api_hero_set_direction },
      { "hero_get_position", map_api_hero_get_position },
      { "hero_set_position", map_api_hero_set_position },
      { "hero_align_on_sensor", map_api_hero_align_on_sensor },
      { "hero_save_solid_ground", map_api_hero_save_solid_ground },
      { "hero_reset_solid_ground", map_api_hero_reset_solid_ground },
      { "hero_walk", map_api_hero_walk },
      { "hero_start_jumping", map_api_hero_start_jumping },
      { "hero_start_victory_sequence", map_api_hero_start_victory_sequence },
      { "hero_start_boomerang", map_api_hero_start_boomerang },
      { "hero_start_bow", map_api_hero_start_bow },
      { "hero_start_hookshot", map_api_hero_start_hookshot },
      { "hero_start_running", map_api_hero_start_running },
      { "hero_start_hurt", map_api_hero_start_hurt },
      { "npc_is_enabled", map_api_npc_is_enabled },
      { "npc_set_enabled", map_api_npc_set_enabled },
      { "npc_set_group_enabled", map_api_npc_set_group_enabled },
      { "npc_start_movement", map_api_npc_start_movement },
      { "npc_stop_movement", map_api_npc_stop_movement },
      { "npc_get_sprite", map_api_npc_get_sprite },
      { "npc_remove", map_api_npc_remove },
      { "npc_exists", map_api_npc_exists },
      { "npc_get_position", map_api_npc_get_position },
      { "npc_set_position", map_api_npc_set_position },
      { "chest_is_open", map_api_chest_is_open },
      { "chest_set_open", map_api_chest_set_open },
      { "chest_is_enabled", map_api_chest_is_enabled },
      { "chest_set_enabled", map_api_chest_set_enabled },
      { "chest_set_group_enabled", map_api_chest_set_group_enabled },
      { "tile_is_enabled", map_api_tile_is_enabled },
      { "tile_set_enabled", map_api_tile_set_enabled },
      { "tile_set_group_enabled", map_api_tile_set_group_enabled },
      { "stairs_is_enabled", map_api_stairs_is_enabled },
      { "stairs_set_enabled", map_api_stairs_set_enabled },
      { "stairs_set_group_enabled", map_api_stairs_set_group_enabled },
      { "wall_is_enabled", map_api_wall_is_enabled },
      { "wall_set_enabled", map_api_wall_set_enabled },
      { "wall_set_group_enabled", map_api_wall_set_group_enabled },
      { "sensor_is_enabled", map_api_sensor_is_enabled },
      { "sensor_set_enabled", map_api_sensor_set_enabled },
      { "sensor_set_group_enabled", map_api_sensor_set_group_enabled },
      { "jumper_is_enabled", map_api_jumper_is_enabled },
      { "jumper_set_enabled", map_api_jumper_set_enabled },
      { "jumper_set_group_enabled", map_api_jumper_set_group_enabled },
      { "crystal_is_enabled", map_api_crystal_is_enabled },
      { "crystal_set_enabled", map_api_crystal_set_enabled },
      { "crystal_set_group_enabled", map_api_crystal_set_group_enabled },
      { "crystal_get_state", map_api_crystal_get_state },
      { "crystal_set_state", map_api_crystal_set_state },
      { "crystal_change_state", map_api_crystal_change_state },
      { "teletransporter_is_enabled", map_api_teletransporter_is_enabled },
      { "teletransporter_set_enabled", map_api_teletransporter_set_enabled },
      { "teletransporter_set_group_enabled", map_api_teletransporter_set_group_enabled },
      { "block_is_enabled", map_api_block_is_enabled },
      { "block_set_enabled", map_api_block_set_enabled },
      { "block_set_group_enabled", map_api_block_set_group_enabled },
      { "block_reset", map_api_block_reset },
      { "block_reset_all", map_api_block_reset_all },
      { "block_get_position", map_api_block_get_position },
      { "block_set_position", map_api_block_set_position },
      { "shop_item_exists", map_api_shop_item_exists },
      { "shop_item_remove", map_api_shop_item_remove },
      { "switch_is_activated", map_api_switch_is_activated },
      { "switch_set_activated", map_api_switch_set_activated },
      { "switch_set_locked", map_api_switch_set_locked },
      { "switch_is_enabled", map_api_switch_is_enabled },
      { "switch_set_enabled", map_api_switch_set_enabled },
      { "switch_set_group_enabled", map_api_switch_set_group_enabled },
      { "door_open", map_api_door_open },
      { "door_close", map_api_door_close },
      { "door_is_open", map_api_door_is_open },
      { "door_set_open", map_api_door_set_open },
      { "pickable_create", map_api_pickable_create },
      { "destructible_create", map_api_destructible_create },
      { "block_create", map_api_block_create },
      { "bomb_create", map_api_bomb_create },
      { "explosion_create", map_api_explosion_create },
      { "fire_create", map_api_fire_create },
      { "arrow_remove", map_api_arrow_remove },
      { "enemy_create", map_api_enemy_create },
      { "enemy_remove", map_api_enemy_remove },
      { "enemy_remove_group", map_api_enemy_remove_group },
      { "enemy_is_enabled", map_api_enemy_is_enabled },
      { "enemy_set_enabled", map_api_enemy_set_enabled },
      { "enemy_set_group_enabled", map_api_enemy_set_group_enabled },
      { "enemy_is_dead", map_api_enemy_is_dead },
      { "enemy_is_group_dead", map_api_enemy_is_group_dead },
      { "enemy_get_group_count", map_api_enemy_get_group_count },
      { "enemy_get_position", map_api_enemy_get_position },
      { "enemy_set_position", map_api_enemy_set_position },
      { "enemy_set_treasure", map_api_enemy_set_treasure },
      { "enemy_set_no_treasure", map_api_enemy_set_no_treasure },
      { "enemy_set_random_treasure", map_api_enemy_set_random_treasure },
      { "enemy_get_sprite", map_api_enemy_get_sprite },
      { NULL, NULL }
  };
  static const luaL_Reg metamethods[] = {
      { "__eq", userdata_meta_eq },
      { "__gc", userdata_meta_gc },
      { "__newindex", userdata_meta_newindex_as_table },
      { "__index", userdata_meta_index_as_table },
      { NULL, NULL }
  };
  register_type(map_module_name, methods, metamethods);
}

/**
 * @brief Checks that the userdata at the specified index of the stack is a
 * map and returns it.
 * @param l A Lua context.
 * @param index An index in the stack.
 * @return The map.
 */
Map& LuaContext::check_map(lua_State* l, int index) {
  return static_cast<Map&>(check_userdata(l, index, map_module_name));
}

/**
 * @brief Pushes a map userdata onto the stack.
 * @param l A Lua context.
 * @param game A game.
 */
void LuaContext::push_map(lua_State* l, Map& map) {

  push_userdata(l, map);
}

/**
 * @brief __index function of the environment of the map's code.
 *
 * This special __index function allows the map's Lua code to get a map
 * entity like a global value.
 * If an entity exists with the specified name, this entity is returned.
 * Otherwise, we fall back to the usual behavior of global values:
 * a global value with this name (or \c nil) is returned.
 *
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::l_get_map_entity_or_global(lua_State* l) {

  lua_pushvalue(l, lua_upvalueindex(1));  // Because check_map does not like pseudo-indexes.
  Map& map = check_map(l, -1);
  const std::string& name = luaL_checkstring(l, 2);

  MapEntity* entity = map.get_entities().find_entity(name);
  if (entity != NULL) {
    // TODO push_entity(l, entity);
    push_sprite(l, entity->get_sprite());
  }
  else {
    lua_getglobal(l, name.c_str());
  }
  return 1;
}

/**
 * @brief Executes the callback function of a camera movement.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::l_camera_do_callback(lua_State* l) {

  // Execute the function.
  lua_settop(l, 0);
  lua_getfield(l, LUA_REGISTRYINDEX, "sol.camera_function");
  call_function(l, 0, 0, "camera callback");

  // Set a second timer to restore the camera.
  Map& map = get_lua_context(l).get_current_game()->get_current_map();
  push_map(l, map);
  lua_getfield(l, LUA_REGISTRYINDEX, "sol.camera_delay_after");
  lua_pushcfunction(l, l_camera_restore);
  timer_api_start(l);

  return 0;
}

/**
 * @brief Moves the camera back to the hero.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::l_camera_restore(lua_State* l) {

  LuaContext& lua_context = get_lua_context(l);

  lua_context.get_current_game()->get_current_map().restore_camera();

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_get_game.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_get_game(lua_State* l) {

  Map& map = check_map(l, 1);

  push_game(l, map.get_game().get_savegame());

  return 1;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_dialog_start(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& dialog_id = luaL_checkstring(l, 2);

  EquipmentItem* issuer_item = NULL;
  if (lua_gettop(l) >= 3) {
    issuer_item = &check_item(l, 3);
  }

  map.get_game().get_dialog_box().start_dialog(dialog_id, issuer_item);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_dialog_set_variable(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& dialog_id = luaL_checkstring(l, 2);
  const std::string& value = luaL_checkstring(l, 3);

  map.get_game().get_dialog_box().set_variable(dialog_id, value);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_dialog_set_style(lua_State* l) {

  Map& map = check_map(l, 1);
  int style = luaL_checkinteger(l, 2);

  map.get_game().get_dialog_box().set_style(DialogBox::Style(style));

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_set_pause_enabled(lua_State* l) {

  Map& map = check_map(l, 1);
  bool pause_key_available = lua_toboolean(l, 2) != 0;

  map.get_game().set_pause_key_available(pause_key_available);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_light_get(lua_State* l) {

  Map& map = check_map(l, 1);

  int light = map.get_light();

  lua_pushinteger(l, light);
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_light_set(lua_State* l) {

  Map& map = check_map(l, 1);
  int light = luaL_checkinteger(l, 2);

  map.set_light(light);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_camera_move(lua_State* l) {

  Map& map = check_map(l, 1);
  int x = luaL_checkinteger(l, 2);
  int y = luaL_checkinteger(l, 3);
  int speed = luaL_checkinteger(l, 4);
  luaL_checktype(l, 5, LUA_TFUNCTION);

  uint32_t delay_before = 1000;
  uint32_t delay_after = 1000;
  if (lua_gettop(l) >= 6) {
    delay_before = luaL_checkinteger(l, 6);
    if (lua_gettop(l) >= 7) {
      delay_after = luaL_checkinteger(l, 7);
    }
  }
  lua_settop(l, 5); // let the function on top of the stack

  // store the function and the delays
  // TODO store this in the map's table instead of globally
  lua_setfield(l, LUA_REGISTRYINDEX, "sol.camera_function");
  lua_pushinteger(l, delay_before);
  lua_setfield(l, LUA_REGISTRYINDEX, "sol.camera_delay_before");
  lua_pushinteger(l, delay_after);
  lua_setfield(l, LUA_REGISTRYINDEX, "sol.camera_delay_after");

  // start the camera
  map.move_camera(x, y, speed);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_treasure_give(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string &item_name = luaL_checkstring(l, 2);
  int variant = luaL_checkinteger(l, 3);
  int savegame_variable = luaL_checkinteger(l, 4);

  map.get_entities().get_hero().start_treasure(
      Treasure(map.get_game(), item_name, variant, savegame_variable));

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_draw_sprite(lua_State* l) {

  Map& map = check_map(l, 1);
  Sprite& sprite = check_sprite(l, 2);
  int x = luaL_checkinteger(l, 3);
  int y = luaL_checkinteger(l, 4);

  map.draw_sprite(sprite, x, y);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_tileset_get(lua_State* l) {

  Map& map = check_map(l, 1);

  TilesetId id = map.get_tileset_id();

  lua_pushinteger(l, id);
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_tileset_set(lua_State* l) {

  Map& map = check_map(l, 1);
  TilesetId id = luaL_checkinteger(l, 2);

  map.set_tileset(id);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_hero_freeze(lua_State* l) {

  Map& map = check_map(l, 1);

  map.get_entities().get_hero().start_freezed();

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_hero_unfreeze(lua_State* l) {

  Map& map = check_map(l, 1);

  map.get_entities().get_hero().start_free();

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_hero_set_map(lua_State* l) {

  Map& map = check_map(l, 1);
  MapId map_id = luaL_checkinteger(l, 2);
  const std::string &destination_name = luaL_checkstring(l, 3);
  Transition::Style transition_style = Transition::Style(luaL_checkinteger(l, 4));

  map.get_game().set_current_map(map_id, destination_name, transition_style);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_hero_set_visible(lua_State* l) {

  Map& map = check_map(l, 1);
  bool visible = lua_toboolean(l, 2) != 0;

  map.get_entities().get_hero().set_visible(visible);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_hero_get_direction(lua_State* l) {

  Map& map = check_map(l, 1);

  int direction = map.get_entities().get_hero().get_animation_direction();

  lua_pushinteger(l, direction);
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_hero_set_direction(lua_State* l) {

  Map& map = check_map(l, 1);
  int direction = luaL_checkinteger(l, 2);

  map.get_entities().get_hero().set_animation_direction(direction);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_hero_get_position(lua_State* l) {

  Map& map = check_map(l, 1);

  Hero& hero = map.get_entities().get_hero();

  lua_pushinteger(l, hero.get_x());
  lua_pushinteger(l, hero.get_y());
  lua_pushinteger(l, hero.get_layer());
  return 3;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_hero_set_position(lua_State* l) {

  Map& map = check_map(l, 1);
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
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_hero_align_on_sensor(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& name = luaL_checkstring(l, 2);

  MapEntities& entities = map.get_entities();
  MapEntity* sensor = entities.get_entity(name);
  map.get_entities().get_hero().set_xy(sensor->get_xy());

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_hero_save_solid_ground(lua_State* l) {

  Map& map = check_map(l, 1);
  Hero& hero = map.get_entities().get_hero();

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
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_hero_reset_solid_ground(lua_State* l) {

  Map& map = check_map(l, 1);
  Hero& hero = map.get_entities().get_hero();

  hero.reset_target_solid_ground_coords();

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_hero_walk(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& path = luaL_checkstring(l, 2);
  bool loop = lua_toboolean(l, 3) != 0;
  bool ignore_obstacles = lua_toboolean(l, 4) != 0;

  map.get_entities().get_hero().start_forced_walking(path, loop, ignore_obstacles);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_hero_start_jumping(lua_State* l) {

  Map& map = check_map(l, 1);
  int direction = luaL_checkinteger(l, 2);
  int length = luaL_checkinteger(l, 3);
  bool ignore_obstacles = lua_toboolean(l, 4) != 0;

  map.get_entities().get_hero().start_jumping(
      direction, length, ignore_obstacles, false);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_hero_start_victory_sequence(lua_State* l) {

  Map& map = check_map(l, 1);

  map.get_entities().get_hero().start_victory();

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_hero_start_boomerang(lua_State* l) {

  Map& map = check_map(l, 1);
  int max_distance = luaL_checkinteger(l, 2);
  int speed = luaL_checkinteger(l, 3);
  const std::string& tunic_preparing_animation = luaL_checkstring(l, 4);
  const std::string& sprite_name = luaL_checkstring(l, 5);

  map.get_entities().get_hero().start_boomerang(max_distance, speed,
      tunic_preparing_animation, sprite_name);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_hero_start_bow(lua_State* l) {

  Map& map = check_map(l, 1);

  map.get_entities().get_hero().start_bow();

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_hero_start_hookshot(lua_State* l) {

  Map& map = check_map(l, 1);

  map.get_entities().get_hero().start_hookshot();

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_hero_start_running(lua_State* l) {

  Map& map = check_map(l, 1);

  map.get_entities().get_hero().start_running();

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_hero_start_hurt(lua_State* l) {

  Map& map = check_map(l, 1);
  int source_x = luaL_checkinteger(l, 2);
  int source_y = luaL_checkinteger(l, 3);
  int life_points = luaL_checkinteger(l, 4);
  int magic_points = luaL_checkinteger(l, 5);

  map.get_entities().get_hero().hurt(Rectangle(source_x, source_y),
      life_points, magic_points);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_npc_is_enabled(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& name = luaL_checkstring(l, 2);

  MapEntities& entities = map.get_entities();
  MapEntity* npc = entities.get_entity(name);

  lua_pushboolean(l, npc->is_enabled());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_npc_set_enabled(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& name = luaL_checkstring(l, 2);
  bool enabled = lua_toboolean(l, 3);

  MapEntities& entities = map.get_entities();
  MapEntity* npc = entities.get_entity(name);
  npc->set_enabled(enabled);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_npc_set_group_enabled(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& prefix = luaL_checkstring(l, 2);
  bool enable = lua_toboolean(l, 3);

  std::list<MapEntity*> entities =
      map.get_entities().get_entities_with_prefix(NON_PLAYING_CHARACTER, prefix);

  std::list<MapEntity*>::iterator it;
  for (it = entities.begin(); it != entities.end(); it++) {
    (*it)->set_enabled(enable);
  }

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_npc_get_position(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& name = luaL_checkstring(l, 2);

  MapEntities& entities = map.get_entities();
  MapEntity* npc = entities.get_entity(name);

  lua_pushinteger(l, npc->get_x());
  lua_pushinteger(l, npc->get_y());
  lua_pushinteger(l, npc->get_layer());
  return 3;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_npc_set_position(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& name = luaL_checkstring(l, 2);
  int x = luaL_checkinteger(l, 3);
  int y = luaL_checkinteger(l, 4);
  int layer = -1;
  if (lua_gettop(l) >= 5) {
    layer = luaL_checkinteger(l, 5);
  }

  MapEntities& entities = map.get_entities();
  MapEntity* npc = entities.get_entity(name);
  npc->set_xy(x, y);
  if (layer != -1) {
    MapEntities& entities = map.get_entities();
    entities.set_entity_layer(*npc, Layer(layer));
  }

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_npc_start_movement(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& name = luaL_checkstring(l, 2);
  Movement& movement = check_movement(l, 3);

  MapEntities& entities = map.get_entities();
  MapEntity* npc = entities.get_entity(name);

  movement.set_suspended(false);
  npc->clear_movement();
  npc->set_movement(&movement);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_npc_stop_movement(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& name = luaL_checkstring(l, 2);

  MapEntities& entities = map.get_entities();
  MapEntity* npc = entities.get_entity(name);
  npc->clear_movement();

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_npc_get_sprite(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& entity_name = luaL_checkstring(l, 2);

  MapEntities& entities = map.get_entities();
  MapEntity* npc = entities.get_entity(entity_name);

  push_sprite(l, npc->get_sprite());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_npc_remove(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& name = luaL_checkstring(l, 2);

  MapEntities& entities = map.get_entities();
  entities.remove_entity(name);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_npc_exists(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& name = luaL_checkstring(l, 2);

  MapEntities& entities = map.get_entities();
  MapEntity* npc = entities.find_entity(name);

  lua_pushboolean(l, npc != NULL);
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_chest_is_open(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& name = luaL_checkstring(l, 2);

  MapEntities& entities = map.get_entities();
  MapEntity* entity = entities.get_entity(name);
  Debug::check_assertion(entity->get_type() == CHEST,
      "This entity is not a chest");
  Chest* chest = (Chest*) entity;

  lua_pushboolean(l, chest->is_open());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_chest_set_open(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string &name = luaL_checkstring(l, 2);
  bool open = lua_toboolean(l, 3) != 0;

  MapEntities& entities = map.get_entities();
  MapEntity* entity = entities.get_entity(name);
  Debug::check_assertion(entity->get_type() == CHEST,
      "This entity is not a chest");
  Chest* chest = (Chest*) entity;
  chest->set_open(open);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_chest_is_enabled(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& name = luaL_checkstring(l, 2);

  MapEntities& entities = map.get_entities();
  MapEntity* chest = entities.get_entity(name);

  lua_pushboolean(l, chest->is_visible());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_chest_set_enabled(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& name = luaL_checkstring(l, 2);
  bool enabled = lua_toboolean(l, 3) != 0;

  MapEntities& entities = map.get_entities();
  MapEntity* chest = entities.get_entity(name);
  chest->set_visible(enabled);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_chest_set_group_enabled(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& prefix = luaL_checkstring(l, 2);
  bool enable = lua_toboolean(l, 3) != 0;

  std::list<MapEntity*> entities =
      map.get_entities().get_entities_with_prefix(prefix);

  std::list<MapEntity*>::iterator it;
  for (it = entities.begin(); it != entities.end(); it++) {
    (*it)->set_enabled(enable);
  }

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_tile_is_enabled(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& name = luaL_checkstring(l, 2);

  MapEntities& entities = map.get_entities();
  MapEntity* dynamic_tile = entities.get_entity(name);

  lua_pushboolean(l, dynamic_tile->is_enabled());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_tile_set_enabled(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& name = luaL_checkstring(l, 2);
  bool enable = lua_toboolean(l, 3) != 0;

  MapEntities& entities = map.get_entities();
  MapEntity* dynamic_tile = entities.get_entity(name);
  dynamic_tile->set_enabled(enable);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_tile_set_group_enabled(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& prefix = luaL_checkstring(l, 2);
  bool enable = lua_toboolean(l, 3) != 0;

  std::list<MapEntity*> entities =
      map.get_entities().get_entities_with_prefix(prefix);

  std::list<MapEntity*>::iterator it;
  for (it = entities.begin(); it != entities.end(); it++) {
    (*it)->set_enabled(enable);
  }

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_stairs_is_enabled(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& name = luaL_checkstring(l, 2);

  MapEntities &entities = map.get_entities();
  MapEntity* stairs = entities.get_entity(name);

  lua_pushboolean(l, stairs->is_enabled());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_stairs_set_enabled(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& name = luaL_checkstring(l, 2);
  bool enable = lua_toboolean(l, 3) != 0;

  MapEntities& entities = map.get_entities();
  MapEntity* stairs = entities.get_entity(name);
  stairs->set_enabled(enable);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_stairs_set_group_enabled(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& prefix = luaL_checkstring(l, 2);
  bool enable = lua_toboolean(l, 3) != 0;

  std::list<MapEntity*> entities =
      map.get_entities().get_entities_with_prefix(prefix);

  std::list<MapEntity*>::iterator it;
  for (it = entities.begin(); it != entities.end(); it++) {
    (*it)->set_enabled(enable);
  }

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_wall_is_enabled(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& name = luaL_checkstring(l, 2);

  MapEntities& entities = map.get_entities();
  MapEntity* wall = entities.get_entity(name);

  lua_pushboolean(l, wall->is_enabled());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_wall_set_enabled(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& name = luaL_checkstring(l, 2);
  bool enable = lua_toboolean(l, 3) != 0;

  MapEntities& entities = map.get_entities();
  MapEntity* wall = entities.get_entity(name);
  wall->set_enabled(enable);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_wall_set_group_enabled(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& prefix = luaL_checkstring(l, 2);
  bool enable = lua_toboolean(l, 3) != 0;

  std::list<MapEntity*> entities =
      map.get_entities().get_entities_with_prefix(prefix);

  std::list<MapEntity*>::iterator it;
  for (it = entities.begin(); it != entities.end(); it++) {
    (*it)->set_enabled(enable);
  }

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_sensor_is_enabled(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& name = luaL_checkstring(l, 2);

  MapEntities& entities = map.get_entities();
  MapEntity* sensor = entities.get_entity(name);

  lua_pushboolean(l, sensor->is_enabled());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_sensor_set_enabled(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string &name = luaL_checkstring(l, 2);
  bool enable = lua_toboolean(l, 3) != 0;

  MapEntities& entities = map.get_entities();
  MapEntity* sensor = entities.get_entity(name);
  sensor->set_enabled(enable);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_sensor_set_group_enabled(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& prefix = luaL_checkstring(l, 2);
  bool enable = lua_toboolean(l, 3) != 0;

  std::list<MapEntity*> entities =
      map.get_entities().get_entities_with_prefix(prefix);

  std::list<MapEntity*>::iterator it;
  for (it = entities.begin(); it != entities.end(); it++) {
    (*it)->set_enabled(enable);
  }

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_jumper_is_enabled(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& name = luaL_checkstring(l, 2);

  MapEntities& entities = map.get_entities();
  MapEntity* jumper = entities.get_entity(name);

  lua_pushboolean(l, jumper->is_enabled());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_jumper_set_enabled(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& name = luaL_checkstring(l, 2);
  bool enable = lua_toboolean(l, 3) != 0;

  MapEntities& entities = map.get_entities();
  MapEntity* jumper = entities.get_entity(name);
  jumper->set_enabled(enable);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_jumper_set_group_enabled(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& prefix = luaL_checkstring(l, 2);
  bool enable = lua_toboolean(l, 3) != 0;

  std::list<MapEntity*> entities =
      map.get_entities().get_entities_with_prefix(prefix);

  std::list<MapEntity*>::iterator it;
  for (it = entities.begin(); it != entities.end(); it++) {
    (*it)->set_enabled(enable);
  }

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_crystal_is_enabled(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& name = luaL_checkstring(l, 2);

  MapEntities& entities = map.get_entities();
  MapEntity* crystal = entities.get_entity(name);

  lua_pushboolean(l, crystal->is_enabled());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_crystal_set_enabled(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& name = luaL_checkstring(l, 2);
  bool enable = lua_toboolean(l, 3) != 0;

  MapEntities& entities = map.get_entities();
  MapEntity* crystal = entities.get_entity(name);
  crystal->set_enabled(enable);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_crystal_set_group_enabled(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& prefix = luaL_checkstring(l, 2);
  bool enable = lua_toboolean(l, 3) != 0;

  std::list<MapEntity*> entities =
      map.get_entities().get_entities_with_prefix(prefix);

  std::list<MapEntity*>::iterator it;
  for (it = entities.begin(); it != entities.end(); it++) {
    (*it)->set_enabled(enable);
  }

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_crystal_get_state(lua_State* l) {

  Map& map = check_map(l, 1);

  lua_pushboolean(l, map.get_game().get_crystal_state());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_crystal_set_state(lua_State* l) {

  Map& map = check_map(l, 1);
  bool state = lua_toboolean(l, 2);

  Game& game = map.get_game();
  if (game.get_crystal_state() != state) {
    game.change_crystal_state();
  }

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_crystal_change_state(lua_State* l) {

  Map& map = check_map(l, 1);

  map.get_game().change_crystal_state();

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_teletransporter_is_enabled(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& name = luaL_checkstring(l, 2);

  MapEntities& entities = map.get_entities();
  MapEntity* teletransporter = entities.get_entity(name);

  lua_pushboolean(l, teletransporter->is_enabled());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_teletransporter_set_enabled(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& name = luaL_checkstring(l, 2);
  bool enable = lua_toboolean(l, 2);

  MapEntities& entities = map.get_entities();
  MapEntity* teletransporter = entities.get_entity(name);
  teletransporter->set_enabled(enable);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_teletransporter_set_group_enabled(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& prefix = luaL_checkstring(l, 2);
  bool enable = lua_toboolean(l, 3);

  std::list<MapEntity*> entities =
      map.get_entities().get_entities_with_prefix(prefix);

  std::list<MapEntity*>::iterator it;
  for (it = entities.begin(); it != entities.end(); it++) {
    (*it)->set_enabled(enable);
  }

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_block_is_enabled(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& name = luaL_checkstring(l, 2);

  MapEntities& entities = map.get_entities();
  MapEntity* block = entities.get_entity(name);

  lua_pushboolean(l, block->is_enabled());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_block_set_enabled(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& name = luaL_checkstring(l, 2);
  bool enable = lua_toboolean(l, 3);

  MapEntities& entities = map.get_entities();
  MapEntity* block = entities.get_entity(name);
  block->set_enabled(enable);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_block_set_group_enabled(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& prefix = luaL_checkstring(l, 2);
  bool enable = lua_toboolean(l, 3);

  std::list<MapEntity*> entities =
      map.get_entities().get_entities_with_prefix(prefix);

  std::list<MapEntity*>::iterator it;
  for (it = entities.begin(); it != entities.end(); it++) {
    (*it)->set_enabled(enable);
  }

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_block_reset(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& name = luaL_checkstring(l, 2);

  MapEntities& entities = map.get_entities();
  MapEntity* entity = entities.get_entity(name);
  Debug::check_assertion(entity->get_type() == BLOCK,
      "This entity is not a block");
  Block* block = (Block*) entity;
  block->reset();

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_block_reset_all(lua_State* l) {

  Map& map = check_map(l, 1);

  MapEntities& entities = map.get_entities();
  std::list<MapEntity*> blocks = entities.get_entities(BLOCK);
  std::list<MapEntity*>::iterator i;
  for (i = blocks.begin(); i != blocks.end(); i++) {
    ((Block*) (*i))->reset();
  }

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_block_get_position(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& name = luaL_checkstring(l, 2);

  MapEntities& entities = map.get_entities();
  MapEntity* block = entities.get_entity(name);

  lua_pushinteger(l, block->get_x());
  lua_pushinteger(l, block->get_y());
  lua_pushinteger(l, block->get_layer());

  return 3;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_block_set_position(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& name = luaL_checkstring(l, 2);
  int x = luaL_checkinteger(l, 3);
  int y = luaL_checkinteger(l, 4);
  int layer = -1;
  if (lua_gettop(l) >= 5) {
    layer = luaL_checkinteger(l, 5);
  }

  MapEntities& entities = map.get_entities();
  MapEntity* block = entities.get_entity(name);
  block->set_xy(x, y);

  if (layer != -1) {
    MapEntities& entities = map.get_entities();
    entities.set_entity_layer(*block, Layer(layer));
  }
  block->check_collision_with_detectors(false);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_shop_item_exists(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& name = luaL_checkstring(l, 2);

  bool exists = map.get_entities().find_entity(name) != NULL;

  lua_pushboolean(l, exists);
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_shop_item_remove(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& name = luaL_checkstring(l, 2);

  MapEntities& entities = map.get_entities();
  MapEntity* shop_item = entities.find_entity(name);
  if (shop_item != NULL) {
    entities.remove_entity(shop_item);
  }

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_switch_is_activated(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string &name = luaL_checkstring(l, 2);

  MapEntities& entities = map.get_entities();
  MapEntity* entity = entities.get_entity(name);
  Debug::check_assertion(entity->get_type() == SWITCH,
      "This entity is not a switch");
  Switch* sw = (Switch*) entity;

  lua_pushboolean(l, sw->is_activated());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_switch_set_activated(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& name = luaL_checkstring(l, 2);
  bool activate = lua_toboolean(l, 3);

  MapEntities& entities = map.get_entities();
  MapEntity* entity = entities.get_entity(name);
  Debug::check_assertion(entity->get_type() == SWITCH,
      "This entity is not a switch");
  Switch* sw = (Switch*) entity;
  sw->set_activated(activate);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_switch_set_locked(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& name = luaL_checkstring(l, 2);
  bool lock = lua_toboolean(l, 3);

  MapEntities& entities = map.get_entities();
  MapEntity* entity = entities.get_entity(name);
  Debug::check_assertion(entity->get_type() == SWITCH,
      "This entity is not a switch");
  Switch* sw = (Switch*) entity;
  sw->set_locked(lock);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_switch_is_enabled(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& name = luaL_checkstring(l, 2);

  MapEntities& entities = map.get_entities();
  MapEntity* sw = entities.get_entity(name);

  lua_pushboolean(l, sw->is_enabled());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_switch_set_enabled(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& name = luaL_checkstring(l, 2);
  bool enable = lua_toboolean(l, 3);

  MapEntities& entities = map.get_entities();
  MapEntity* sw = entities.get_entity(name);
  sw->set_enabled(enable);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_switch_set_group_enabled(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& prefix = luaL_checkstring(l, 2);
  bool enable = lua_toboolean(l, 3);

  std::list<MapEntity*> entities =
      map.get_entities().get_entities_with_prefix(prefix);

  std::list<MapEntity*>::iterator it;
  for (it = entities.begin(); it != entities.end(); it++) {
    (*it)->set_enabled(enable);
  }

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_door_open(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& prefix = luaL_checkstring(l, 2);

  bool done = false;
  MapEntities& entities = map.get_entities();
  std::list<MapEntity*> doors = entities.get_entities_with_prefix(DOOR, prefix);
  std::list<MapEntity*>::iterator it;
  for (it = doors.begin(); it != doors.end(); it++) {
    Door* door = (Door*) (*it);
    if (!door->is_open() || door->is_changing()) {
      door->open();
      done = true;
    }
  }

  // make sure the sound is played only once even if the script calls
  // this function repeatedly while the door is still changing
  if (done) {
    Sound::play("door_open");
  }

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_door_close(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& prefix = luaL_checkstring(l, 2);

  bool done = false;
  MapEntities& entities = map.get_entities();
  std::list<MapEntity*> doors = entities.get_entities_with_prefix(DOOR, prefix);
  std::list<MapEntity*>::iterator it;
  for (it = doors.begin(); it != doors.end(); it++) {
    Door* door = (Door*) (*it);
    if (door->is_open() || door->is_changing()) {
      door->close();
      done = true;
    }
  }

  // make sure the sound is played only once even if the script calls
  // this function repeatedly while the door is still changing
  if (done) {
    Sound::play("door_closed");
  }

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_door_is_open(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& name = luaL_checkstring(l, 2);

  MapEntities& entities = map.get_entities();
  MapEntity* entity = entities.get_entity(name);
  Debug::check_assertion(entity->get_type() == DOOR,
      "This entity is not a door");
  Door* door = (Door*) entity;

  lua_pushboolean(l, door->is_open());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_door_set_open(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& prefix = luaL_checkstring(l, 2);
  bool open = lua_toboolean(l, 3) != 0;

  MapEntities& entities = map.get_entities();
  std::list<MapEntity*> doors = entities.get_entities_with_prefix(DOOR, prefix);
  std::list<MapEntity*>::iterator it;
  for (it = doors.begin(); it != doors.end(); it++) {
    Door* door = (Door*) (*it);
    door->set_open(open);
  }

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_pickable_create(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string &item_name = luaL_checkstring(l, 2);
  int variant = luaL_checkinteger(l, 3);
  int savegame_variable = luaL_checkinteger(l, 4);
  int x = luaL_checkinteger(l, 5);
  int y = luaL_checkinteger(l, 6);
  Layer layer = Layer(luaL_checkinteger(l, 7));

  Game& game = map.get_game();
  MapEntities& entities = map.get_entities();
  entities.add_entity(Pickable::create(
      game, layer, x, y,
      Treasure(game, item_name, variant, savegame_variable),
      FALLING_MEDIUM, false
      ));

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_destructible_create(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& subtype_name = luaL_checkstring(l, 2);
  int x = luaL_checkinteger(l, 3);
  int y = luaL_checkinteger(l, 4);
  Layer layer = Layer(luaL_checkinteger(l, 5));

  // default properties
  std::string treasure_item = "_random";
  int treasure_variant = 1;
  int treasure_savegame_variable = -1;
  int destruction_callback_ref = LUA_REFNIL;

  if (lua_gettop(l) >= 6) {
    luaL_checktype(l, 6, LUA_TTABLE);

    // traverse the table, looking for properties
    lua_pushnil(l); // first key
    while (lua_next(l, 6) != 0) {

      const std::string& key = luaL_checkstring(l, -2);
      if (key == "treasure_item") {
        treasure_item = luaL_checkstring(l, -1);
      }
      else if (key == "treasure_variant") {
        treasure_variant = luaL_checkinteger(l, -1);
      }
      else if (key == "treasure_savegame_variable") {
        treasure_savegame_variable = luaL_checkinteger(l, -1);
      }
      else if (key == "destruction_callback") {
        // store the callback into the registry
        luaL_argcheck(l, lua_isfunction(l, -1), 6,
            "destruction_callback should be a function");
        destruction_callback_ref = luaL_ref(l, LUA_REGISTRYINDEX);
        lua_pushnil(l);
      }
      lua_pop(l, 1); // pop the value, let the key for the iteration
    }
  }

  Destructible::Subtype subtype =
      Destructible::get_subtype_by_name(subtype_name);
  MapEntities& entities = map.get_entities();
  Destructible* destructible =
      new Destructible(layer, x, y, subtype, Treasure(map.get_game(),
          treasure_item, treasure_variant, treasure_savegame_variable));
  destructible->set_destruction_callback(destruction_callback_ref);
  entities.add_entity(destructible);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_block_create(lua_State* l) {

  Map& map = check_map(l, 1);
  int x = luaL_checkinteger(l, 2);
  int y = luaL_checkinteger(l, 3);
  Layer layer = Layer(luaL_checkinteger(l, 4));
  const std::string& name = luaL_checkstring(l, 5);

  // default properties
  int direction = -1;
  std::string sprite_name = "entities/block";
  bool can_be_pushed = true;
  bool can_be_pulled = false;
  int maximum_moves = 1;

  if (lua_gettop(l) >= 6) {
    luaL_checktype(l, 6, LUA_TTABLE);

    // traverse the table, looking for properties
    lua_pushnil(l); // first key
    while (lua_next(l, 6) != 0) {

      const std::string& key = luaL_checkstring(l, -2);
      if (key == "direction") {
        direction = luaL_checkinteger(l, -1);
      }
      else if (key == "sprite_name") {
        sprite_name = luaL_checkstring(l, -1);
      }
      else if (key == "can_be_pushed") {
        can_be_pushed = lua_toboolean(l, -1) != 0;
      }
      else if (key == "can_be_pulled") {
        can_be_pulled = lua_toboolean(l, -1) != 0;
      }
      else if (key == "maximum_moves") {
        maximum_moves = luaL_checkinteger(l, -1);
      }
      lua_pop(l, 1); // pop the value, let the key for the iteration
    }
  }

  MapEntities& entities = map.get_entities();
  Block* block = new Block(name, layer, x, y, direction, sprite_name,
      can_be_pushed, can_be_pulled, maximum_moves);
  entities.add_entity(block);
  block->check_collision_with_detectors(false);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_bomb_create(lua_State* l) {

  Map& map = check_map(l, 1);
  int x = luaL_checkinteger(l, 2);
  int y = luaL_checkinteger(l, 3);
  Layer layer = Layer(luaL_checkinteger(l, 4));

  MapEntities& entities = map.get_entities();
  entities.add_entity(new Bomb(layer, x, y));

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_explosion_create(lua_State* l) {

  Map& map = check_map(l, 1);
  int x = luaL_checkinteger(l, 2);
  int y = luaL_checkinteger(l, 3);
  Layer layer = Layer(luaL_checkinteger(l, 4));

  MapEntities& entities = map.get_entities();
  entities.add_entity(new Explosion(layer, Rectangle(x, y), true));

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_fire_create(lua_State* l) {

  Map& map = check_map(l, 1);
  int x = luaL_checkinteger(l, 2);
  int y = luaL_checkinteger(l, 3);
  Layer layer = Layer(luaL_checkinteger(l, 4));

  MapEntities& entities = map.get_entities();
  entities.add_entity(new Fire(layer, Rectangle(x, y)));

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_arrow_remove(lua_State* l) {

  Map& map = check_map(l, 1);

  MapEntities& entities = map.get_entities();
  entities.remove_arrows();

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_enemy_create(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& name = luaL_checkstring(l, 2);
  const std::string& breed = luaL_checkstring(l, 3);
  int layer = luaL_checkinteger(l, 4);
  int x = luaL_checkinteger(l, 5);
  int y = luaL_checkinteger(l, 6);

  MapEntities& entities = map.get_entities();
  Treasure treasure(map.get_game(), "_random", 1, -1);
  Enemy* enemy = (Enemy*) Enemy::create(map.get_game(), breed, Enemy::RANK_NORMAL, -1,
      name, Layer(layer), x, y, 0, treasure);
  entities.add_entity(enemy);
  enemy->restart();

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_enemy_remove(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& name = luaL_checkstring(l, 2);

  MapEntities& entities = map.get_entities();
  entities.remove_entity(name);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_enemy_remove_group(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& prefix = luaL_checkstring(l, 2);

  MapEntities& entities = map.get_entities();
  entities.remove_entities_with_prefix(prefix);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_enemy_is_enabled(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& name = luaL_checkstring(l, 2);

  MapEntities& entities = map.get_entities();
  MapEntity* enemy = entities.get_entity(name);

  lua_pushboolean(l, enemy->is_enabled());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_enemy_set_enabled(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& name = luaL_checkstring(l, 2);
  bool enable = lua_toboolean(l, 3);

  MapEntities& entities = map.get_entities();
  MapEntity* enemy = entities.get_entity(name);
  enemy->set_enabled(enable);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_enemy_set_group_enabled(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& prefix = luaL_checkstring(l, 2);
  bool enable = lua_toboolean(l, 3);

  std::list<MapEntity*> entities =
      map.get_entities().get_entities_with_prefix(prefix);

  std::list<MapEntity*>::iterator it;
  for (it = entities.begin(); it != entities.end(); it++) {
    (*it)->set_enabled(enable);
  }

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_enemy_is_dead(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& name = luaL_checkstring(l, 2);

  MapEntities& entities = map.get_entities();
  MapEntity* enemy = entities.find_entity(name);

  lua_pushboolean(l, (enemy == NULL));
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_enemy_is_group_dead(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& prefix = luaL_checkstring(l, 2);

  MapEntities& entities = map.get_entities();
  std::list<MapEntity*> enemies = entities.get_entities_with_prefix(prefix);

  lua_pushboolean(l, enemies.empty());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_enemy_get_group_count(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& prefix = luaL_checkstring(l, 2);

  MapEntities& entities = map.get_entities();
  std::list<MapEntity*> enemies = entities.get_entities_with_prefix(prefix);

  lua_pushinteger(l, enemies.size());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_enemy_get_position(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& name = luaL_checkstring(l, 2);

  MapEntities& entities = map.get_entities();
  MapEntity* enemy = entities.get_entity(name);
  const Rectangle& coordinates = enemy->get_xy();

  lua_pushinteger(l, coordinates.get_x());
  lua_pushinteger(l, coordinates.get_y());
  lua_pushinteger(l, enemy->get_layer());
  return 3;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_enemy_set_position(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& name = luaL_checkstring(l, 2);
  int x = luaL_checkinteger(l, 3);
  int y = luaL_checkinteger(l, 4);
  int layer = -1;
  if (lua_gettop(l) >= 5) {
    layer = luaL_checkinteger(l, 5);
  }

  MapEntities& entities = map.get_entities();
  MapEntity* enemy = entities.get_entity(name);
  enemy->set_xy(x, y);

  if (layer != -1) {
    MapEntities& entities = map.get_entities();
    entities.set_entity_layer(*enemy, Layer(layer));
  }

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_enemy_set_treasure(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& enemy_name = luaL_checkstring(l, 2);
  const std::string& item_name = luaL_checkstring(l, 3);
  int variant = luaL_checkinteger(l, 4);
  int savegame_variable = luaL_checkinteger(l, 5);

  MapEntities& entities = map.get_entities();
  MapEntity* entity = entities.get_entity(enemy_name);
  Debug::check_assertion(entity->get_type() == ENEMY,
      "This entity is not an enemy");
  Enemy* enemy = (Enemy*) entity;
  Treasure treasure(enemy->get_game(), item_name, variant, savegame_variable);
  enemy->set_treasure(treasure);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_enemy_set_no_treasure(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& enemy_name = luaL_checkstring(l, 2);

  MapEntities& entities = map.get_entities();
  MapEntity* entity = entities.get_entity(enemy_name);
  Debug::check_assertion(entity->get_type() == ENEMY,
      "This entity is not an enemy");
  Enemy* enemy = (Enemy*) entity;
  Treasure treasure(enemy->get_game(), "_none", 1, -1);
  enemy->set_treasure(treasure);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_enemy_set_random_treasure(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& enemy_name = luaL_checkstring(l, 2);

  MapEntities& entities = map.get_entities();
  MapEntity* entity = entities.get_entity(enemy_name);
  Debug::check_assertion(entity->get_type() == ENEMY,
      "This entity is not an enemy");
  Enemy* enemy = (Enemy*) entity;
  Treasure treasure(enemy->get_game(), "_random", 1, -1);
  enemy->set_treasure(treasure);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_enemy_get_sprite(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& enemy_name = luaL_checkstring(l, 2);

  MapEntities& entities = map.get_entities();
  MapEntity* enemy = entities.get_entity(enemy_name);

  push_sprite(l, enemy->get_sprite());
  return 1;
}

/**
 * @brief Calls the on_update() method of a Lua map.
 * @param map A map.
 */
void LuaContext::map_on_update(Map& map) {

  push_map(l, map);
  on_update();
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_suspended() method of a Lua map.
 * @param map A map.
 * @param suspended true if the map is suspended.
 */
void LuaContext::map_on_suspended(Map& map, bool suspended) {

  push_map(l, map);
  on_suspended(suspended);
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_started() method of a Lua map.
 * @param map A map.
 * @param destination The destination point used (NULL if it's a special one).
 */
void LuaContext::map_on_started(Map& map, Destination* destination) {

  push_map(l, map);
  on_started(destination);
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_finished() method of a Lua map.
 * @param map A map.
 */
void LuaContext::map_on_finished(Map& map) {

  push_map(l, map);
  on_finished();
  remove_timers(-1);  // Stop timers associated to this map.
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_opening_transition_finished() method of a Lua map.
 * @param map A map.
 * @param destination The destination point used (NULL if it's a special one).
 */
void LuaContext::map_on_opening_transition_finished(Map& map,
    Destination* destination) {

  push_map(l, map);
  on_opening_transition_finished(destination);
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_dialog_started() method of a Lua map.
 * @param map A map.
 * @param dialog_id Id a the dialog just started.
 */
void LuaContext::map_on_dialog_started(Map& map, const std::string& dialog_id) {

  push_map(l, map);
  on_dialog_started(dialog_id);
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_dialog_finished() method of a Lua map.
 * @param map A map.
 * @param dialog_id Id a the dialog just finished.
 * @param answer The answer selected by the player: 0 for the first one,
 * 1 for the second one, -1 if there was no question.
 */
void LuaContext::map_on_dialog_finished(Map& map, const std::string& dialog_id, int answer) {

  push_map(l, map);
  on_dialog_finished(dialog_id, answer);
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_camera_back() method of a Lua map.
 * @param map A map.
 */
void LuaContext::map_on_camera_back(Map& map) {

  push_map(l, map);
  on_camera_back();
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_treasure_obtaining() method of a Lua map.
 * @param map A map.
 * @param treasure A treasure the hero is about to obtain.
 */
void LuaContext::map_on_treasure_obtaining(Map& map, const Treasure& treasure) {

  push_map(l, map);
  on_treasure_obtaining(treasure);
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_treasure_obtaining() method of a Lua map.
 * @param map A map.
 * @param treasure The treasure just obtained.
 */
void LuaContext::map_on_treasure_obtained(Map& map, const Treasure& treasure) {

  push_map(l, map);
  on_treasure_obtained(treasure);
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_switch_activated() method of a Lua map.
 * @param map A map.
 * @param sw A switch.
 */
void LuaContext::map_on_switch_activated(Map& map, Switch& sw) {

  push_map(l, map);
  on_switch_activated(sw);
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_switch_inactivated() method of a Lua map.
 * @param map A map.
 * @param sw A switch.
 */
void LuaContext::map_on_switch_inactivated(Map& map, Switch& sw) {

  push_map(l, map);
  on_switch_inactivated(sw);
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_switch_left() method of a Lua map.
 * @param map A map.
 * @param sw A switch.
 */
void LuaContext::map_on_switch_left(Map& map, Switch& sw) {

  push_map(l, map);
  on_switch_left(sw);
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_hero_victory_sequence_finished() method of a Lua map.
 * @param map A map.
 */
void LuaContext::map_on_hero_victory_sequence_finished(Map& map) {

  push_map(l, map);
  on_hero_victory_sequence_finished();
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_hero_on_sensor() method of a Lua map.
 * @param map A map.
 * @param sensor A sensor.
 */
void LuaContext::map_on_hero_on_sensor(Map& map, Sensor& sensor) {

  push_map(l, map);
  on_hero_on_sensor(sensor);
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_hero_still_on_sensor() method of a Lua map.
 * @param map A map.
 * @param sensor A sensor.
 */
void LuaContext::map_on_hero_still_on_sensor(Map& map, Sensor& sensor) {

  push_map(l, map);
  on_hero_still_on_sensor(sensor);
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_npc_movement_finished() method of a Lua map.
 * @param map A map.
 * @param npc An NPC.
 */
void LuaContext::map_on_npc_movement_finished(Map& map, NPC& npc) {

  push_map(l, map);
  on_npc_movement_finished(npc);
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_npc_interaction() method of a Lua map.
 * @param map A map.
 * @param npc An NPC.
 */
void LuaContext::map_on_npc_interaction(Map& map, NPC& npc) {

  push_map(l, map);
  on_npc_interaction(npc);
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_npc_interaction_finished() method of a Lua map.
 * @param map A map.
 * @param npc An NPC.
 */
void LuaContext::map_on_npc_interaction_finished(Map& map, NPC& npc) {

  push_map(l, map);
  on_npc_interaction_finished(npc);
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_npc_interaction_item() method of a Lua map.
 * @param map A map.
 * @param npc An NPC.
 * @param item_name Name of an equipment item.
 * @param variant Variant of this equipment item.
 * @return true if an interaction occurred.
 */
bool LuaContext::map_on_npc_interaction_item(Map& map, NPC& npc,
    const std::string& item_name, int variant) {

  push_map(l, map);
  bool result = on_npc_interaction_item(npc, item_name, variant);
  lua_pop(l, 1);
  return result;
}

/**
 * @brief Calls the on_npc_interaction_item_finished() method of a Lua map.
 * @param map A map.
 * @param npc An NPC.
 * @param item_name Name of an equipment item.
 * @param variant Variant of this equipment item.
 */
void LuaContext::map_on_npc_interaction_item_finished(Map& map, NPC& npc,
    const std::string& item_name, int variant) {

  push_map(l, map);
  on_npc_interaction_item_finished(npc, item_name, variant);
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_npc_collision_fire() method of a Lua map.
 * @param map A map.
 * @param npc An NPC.
 */
void LuaContext::map_on_npc_collision_fire(Map& map, NPC& npc) {

  push_map(l, map);
  on_npc_collision_fire(npc);
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_sensor_collision_explosion() method of a Lua map.
 * @param map A map.
 * @param sensor A sensor.
 */
void LuaContext::map_on_sensor_collision_explosion(Map& map, Sensor& sensor) {

  push_map(l, map);
  on_sensor_collision_explosion(sensor);
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_chest_empty() method of a Lua map.
 * @param map A map.
 * @param chest A chest.
 */
bool LuaContext::map_on_chest_empty(Map& map, Chest& chest) {

  push_map(l, map);
  bool result = on_chest_empty(chest);
  lua_pop(l, 1);
  return result;
}

/**
 * @brief Calls the on_shop_item_buying() method of a Lua map.
 * @param map A map.
 * @param shop_item A shop item.
 */
bool LuaContext::map_on_shop_item_buying(Map& map, ShopItem& shop_item) {

  push_map(l, map);
  bool result = on_shop_item_buying(shop_item);
  lua_pop(l, 1);
  return result;
}

/**
 * @brief Calls the on_shop_item_bought() method of a Lua map.
 * @param map A map.
 * @param shop_item A shop item.
 */
void LuaContext::map_on_shop_item_bought(Map& map, ShopItem& shop_item) {

  push_map(l, map);
  on_shop_item_bought(shop_item);
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_door_open() method of a Lua map.
 * @param map A map.
 * @param door A door.
 */
void LuaContext::map_on_door_open(Map& map, Door& door) {

  push_map(l, map);
  on_door_open(door);
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_door_closed() method of a Lua map.
 * @param map A map.
 * @param door A door.
 */
void LuaContext::map_on_door_closed(Map& map, Door& door) {

  push_map(l, map);
  on_door_closed(door);
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_block_moved() method of a Lua map.
 * @param map A map.
 * @param block A block.
 */
void LuaContext::map_on_block_moved(Map& map, Block& block) {

  push_map(l, map);
  on_block_moved(block);
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_enemy_dying() method of a Lua map.
 * @param map A map.
 * @param enemy An enemy.
 */
void LuaContext::map_on_enemy_dying(Map& map, Enemy& enemy) {

  push_map(l, map);
  on_enemy_dying(enemy);
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_enemy_dead() method of a Lua map.
 * @param map A map.
 * @param enemy An enemy.
 */
void LuaContext::map_on_enemy_dead(Map& map, Enemy& enemy) {

  push_map(l, map);
  on_enemy_dead(enemy);
  lua_pop(l, 1);
}

