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
#include "MainLoop.h"
#include "Game.h"
#include "Map.h"
#include "DialogBox.h"
#include "Treasure.h"
#include "entities/MapEntities.h"
#include "entities/Tile.h"
#include "entities/Destination.h"
#include "entities/Teletransporter.h"
#include "entities/Pickable.h"
#include "entities/Destructible.h"
#include "entities/Chest.h"
#include "entities/Jumper.h"
#include "entities/Enemy.h"
#include "entities/NPC.h"
#include "entities/Block.h"
#include "entities/DynamicTile.h"
#include "entities/Switch.h"
#include "entities/Wall.h"
#include "entities/Sensor.h"
#include "entities/Crystal.h"
#include "entities/CrystalBlock.h"
#include "entities/ShopItem.h"
#include "entities/ConveyorBelt.h"
#include "entities/Door.h"
#include "entities/Stairs.h"
#include "entities/Bomb.h"
#include "entities/Explosion.h"
#include "entities/Fire.h"
#include "entities/Hero.h"
#include "movements/Movement.h"
#include "lowlevel/Sound.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"
#include <lua.hpp>
#include <sstream>

const std::string LuaContext::map_module_name = "sol.map";

/**
 * @brief Initializes the map features provided to Lua.
 */
void LuaContext::register_map_module() {

  static const luaL_Reg methods[] = {
      { "get_id", map_api_get_id },
      { "get_game", map_api_get_game },
      { "get_world", map_api_get_world },
      { "get_size", map_api_get_size },
      { "get_location", map_api_get_location },
      { "get_floor", map_api_get_floor },
      { "get_tileset", map_api_get_tileset },
      { "set_tileset", map_api_set_tileset },
      { "is_dialog_enabled", map_api_is_dialog_enabled },
      { "start_dialog", map_api_start_dialog },
      { "set_dialog_variable", map_api_set_dialog_variable },
      { "set_dialog_style", map_api_set_dialog_style },
      { "set_dialog_position", map_api_set_dialog_position },
      { "draw_dialog_box", map_api_draw_dialog_box },
      { "set_pause_enabled", map_api_set_pause_enabled },
      { "get_light", map_api_get_light },
      { "set_light", map_api_set_light },
      { "move_camera", map_api_move_camera },
      { "draw_sprite", map_api_draw_sprite },
      { "get_crystal_state", map_api_get_crystal_state },
      { "set_crystal_state", map_api_set_crystal_state },
      { "change_crystal_state", map_api_change_crystal_state },
      { "open_doors", map_api_open_doors },
      { "close_doors", map_api_close_doors },
      { "set_doors_open", map_api_set_doors_open },
      { "get_entity", map_api_get_entity },
      { "has_entity", map_api_has_entity },
      { "get_entities", map_api_get_entities },
      { "get_entities_count", map_api_get_entities_count },
      { "has_entities", map_api_has_entities },
      { "set_entities_enabled", map_api_set_entities_enabled },
      { "remove_entities", map_api_remove_entities },
      { "create_destination", map_api_create_destination },
      { "create_teletransporter", map_api_create_teletransporter },
      { "create_pickable", map_api_create_pickable },
      { "create_destructible", map_api_create_destructible },
      { "create_chest", map_api_create_chest },
      { "create_jumper", map_api_create_jumper },
      { "create_enemy", map_api_create_enemy },
      { "create_npc", map_api_create_npc },
      { "create_block", map_api_create_block },
      { "create_dynamic_tile", map_api_create_dynamic_tile },
      { "create_switch", map_api_create_switch },
      { "create_wall", map_api_create_wall },
      { "create_sensor", map_api_create_sensor },
      { "create_crystal", map_api_create_crystal },
      { "create_crystal_block", map_api_create_crystal_block },
      { "create_shop_item", map_api_create_shop_item },
      { "create_conveyor_belt", map_api_create_conveyor_belt },
      { "create_door", map_api_create_door },
      { "create_stairs", map_api_create_stairs },
      { "create_bomb", map_api_create_bomb },
      { "create_explosion", map_api_create_explosion },
      { "create_fire", map_api_create_fire },
      { NULL, NULL }
  };
  static const luaL_Reg metamethods[] = {
      { "__gc", userdata_meta_gc },
      { "__newindex", userdata_meta_newindex_as_table },
      { "__index", userdata_meta_index_as_table },
      { NULL, NULL }
  };
  register_type(map_module_name, methods, metamethods);
}

/**
 * @brief Returns whether a value is a userdata of type map.
 * @param l A Lua context.
 * @param index An index in the stack.
 * @return true if the value at this index is a map.
 */
bool LuaContext::is_map(lua_State* l, int index) {
  return is_userdata(l, index, map_module_name);
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
 * @brief Returns the map to be used when creating an entity.
 *
 * This function is meant to be called from entity creation functions.
 * If the first Lua parameter is a map, returns that map and removes it
 * from the Lua stack.
 * Otherwise, an implicit map must have been previously set (by calling
 * set_entity_implicit_creation_map()) and that map is returned.
 * Therefore, after calling this function; you will have the same Lua stack
 * in both cases, without worrying about how the map was passed.
 *
 * @param l A Lua context.
 * @return The map where an entity will be created.
 */
Map& LuaContext::get_entity_creation_map(lua_State* l) {

  Map* map = NULL;

  if (is_map(l, 1)) {
    // The map is passed as a parameter (typically, by the map script).
    map = &check_map(l, 1);
    lua_remove(l, 1);
  }
  else {
    // The map was is implicit (typically, we are loading its data file).
    map = get_entity_implicit_creation_map(l);
    Debug::check_assertion(map != NULL,
        "No implicit creation was been set in this Lua state");
  }

  return *map;
}

/**
 * @brief Returns the map previously stored by
 * set_entity_implicit_creation_map().
 * @param l A Lua context.
 * @return The map stored in this Lua context or NULL.
 */
Map* LuaContext::get_entity_implicit_creation_map(lua_State* l) {

  lua_getfield(l, LUA_REGISTRYINDEX, "map");
  if (lua_isnil(l, -1)) {
    return NULL;
  }

  Map* map = static_cast<Map*>(lua_touserdata(l, -1));
  lua_pop(l, 1);

  return map;
}

/**
 * @brief Stores into the Lua state the map that will be used to create entities.
 *
 * There are two ways to create a entity on the map:
 * - Declare the entity in the map data file.
 * - Create the entity dynamically from a Lua script.
 *
 * As the map data file is in Lua, both approaches actually call the same
 * function: map_api_create_enemy or map_api_create_npc, map_api_create_block,
 * etc. depending on your type of entity.
 *
 * In the second case, the map is always passed explicitly as the first
 * parameter of the entity creation function. For instance:
 * \verbatim
 * map:create_enemy{ x = 160, y = 117, name = "my_enemy", breed = "soldier" }
 * \endverbatim
 *
 * In the first case, the map is never passed:
 * \verbatim
 * enemy{ x = 160, y = 117, name = "my_enemy", breed = "soldier" }
 * \endverbatim
 * To make Lua know the map anyway, call this function before your create
 * your first entity.
 * Then, all entity creation functions will accept to be called without the
 * map as first parameter.
 *
 * @param l A Lua context.
 * @param map The map where future entities should be created.
 * NULL forces the map to be explicitly passed (this is the default
 * behavior).
 */
void LuaContext::set_entity_implicit_creation_map(lua_State* l, Map* map) {

  if (map == NULL) {
    lua_pushnil(l);
  }
  else {
    lua_pushlightuserdata(l, map);
  }
  lua_setfield(l, LUA_REGISTRYINDEX, "map");
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
  if (entity != NULL && !entity->is_being_removed()) {
    push_entity(l, *entity);
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
  Map& map = get_lua_context(l).get_main_loop().get_game()->get_current_map();
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

  lua_context.get_main_loop().get_game()->get_current_map().restore_camera();

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
 * @brief Implementation of \ref lua_api_map_get_id.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_get_id(lua_State* l) {

  Map& map = check_map(l, 1);

  push_string(l, map.get_id());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_map_get_world.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_get_world(lua_State* l) {

  Map& map = check_map(l, 1);

  push_string(l, map.get_world());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_map_get_floor.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_get_floor(lua_State* l) {

  Map& map = check_map(l, 1);

  if (!map.has_floor()) {
    lua_pushnil(l);
  }
  else {
    lua_pushinteger(l, map.get_floor());
  }
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_map_get_size.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::map_api_get_size(lua_State* l) {

  Map& map = check_map(l, 1);

  lua_pushinteger(l, map.get_width());
  lua_pushinteger(l, map.get_height());

  return 2;
}

/**
 * @brief Implementation of \ref lua_api_map_get_location.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::map_api_get_location(lua_State* l) {

  Map& map = check_map(l, 1);

  lua_pushinteger(l, map.get_location().get_x());
  lua_pushinteger(l, map.get_location().get_y());

  return 2;
}

/**
 * @brief Implementation of \ref lua_api_map_get_tileset.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_get_tileset(lua_State* l) {

  Map& map = check_map(l, 1);

  push_string(l, map.get_tileset_id());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_map_set_tileset.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_set_tileset(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& tileset_id = luaL_checkstring(l, 2);

  map.set_tileset(tileset_id);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_is_dialog_enabled.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_is_dialog_enabled(lua_State* l) {

  Map& map = check_map(l, 1);

  lua_pushboolean(l, map.get_game().is_dialog_enabled());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_map_start_dialog.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_start_dialog(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& dialog_id = luaL_checkstring(l, 2);
  int callback_ref = LUA_REFNIL;
  if (lua_gettop(l) >= 3) {
    luaL_checktype(l, 3, LUA_TFUNCTION);
    lua_settop(l, 3);
    callback_ref = luaL_ref(l, LUA_REGISTRYINDEX);
  }

  map.get_game().get_dialog_box().start_dialog(dialog_id, callback_ref);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_set_dialog_variable.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_set_dialog_variable(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& dialog_id = luaL_checkstring(l, 2);
  const std::string& value = luaL_checkstring(l, 3);

  map.get_game().get_dialog_box().set_variable(dialog_id, value);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_set_dialog_style.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_set_dialog_style(lua_State* l) {

  Map& map = check_map(l, 1);
  int style = luaL_checkint(l, 2);

  map.get_game().get_dialog_box().set_style(DialogBox::Style(style));

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_set_dialog_position.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_set_dialog_position(lua_State* l) {

  Map& map = check_map(l, 1);
  int position = luaL_checkint(l, 2);

  map.get_game().get_dialog_box().set_vertical_position(DialogBox::VerticalPosition(position));

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_draw_dialog_box.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_draw_dialog_box(lua_State* l) {

  // TODO remove the built-in dialog box, draw the dialog box from the script
  Map& map = check_map(l, 1);
  Surface& dst_surface = check_surface(l, 2);

  map.get_game().get_dialog_box().draw(dst_surface);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_set_pause_enabled.
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
 * @brief Implementation of \ref lua_api_map_get_light.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_get_light(lua_State* l) {

  Map& map = check_map(l, 1);

  int light = map.get_light();

  lua_pushinteger(l, light);
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_map_set_light.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_set_light(lua_State* l) {

  Map& map = check_map(l, 1);
  int light = luaL_checkint(l, 2);

  map.set_light(light);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_move_camera.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_move_camera(lua_State* l) {

  Map& map = check_map(l, 1);
  int x = luaL_checkint(l, 2);
  int y = luaL_checkint(l, 3);
  int speed = luaL_checkint(l, 4);
  luaL_checktype(l, 5, LUA_TFUNCTION);

  uint32_t delay_before = 1000;
  uint32_t delay_after = 1000;
  if (lua_gettop(l) >= 6) {
    delay_before = luaL_checkint(l, 6);
    if (lua_gettop(l) >= 7) {
      delay_after = luaL_checkint(l, 7);
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
 * @brief Implementation of \ref lua_api_map_draw_sprite.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_draw_sprite(lua_State* l) {

  Map& map = check_map(l, 1);
  Sprite& sprite = check_sprite(l, 2);
  int x = luaL_checkint(l, 3);
  int y = luaL_checkint(l, 4);

  map.draw_sprite(sprite, x, y);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_get_crystal_state.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_get_crystal_state(lua_State* l) {

  Map& map = check_map(l, 1);

  lua_pushboolean(l, map.get_game().get_crystal_state());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_map_set_crystal_state.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_set_crystal_state(lua_State* l) {

  Map& map = check_map(l, 1);
  bool state = lua_toboolean(l, 2);

  Game& game = map.get_game();
  if (game.get_crystal_state() != state) {
    game.change_crystal_state();
  }

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_change_crystal_state.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_change_crystal_state(lua_State* l) {

  Map& map = check_map(l, 1);

  map.get_game().change_crystal_state();

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_open_doors.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_open_doors(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& prefix = luaL_checkstring(l, 2);

  bool done = false;
  MapEntities& entities = map.get_entities();
  std::list<MapEntity*> doors = entities.get_entities_with_prefix(DOOR, prefix);
  std::list<MapEntity*>::iterator it;
  for (it = doors.begin(); it != doors.end(); it++) {
    Door* door = static_cast<Door*>(*it);
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
 * @brief Implementation of \ref lua_api_map_close_doors.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_close_doors(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& prefix = luaL_checkstring(l, 2);

  bool done = false;
  MapEntities& entities = map.get_entities();
  std::list<MapEntity*> doors = entities.get_entities_with_prefix(DOOR, prefix);
  std::list<MapEntity*>::iterator it;
  for (it = doors.begin(); it != doors.end(); it++) {
    Door* door = static_cast<Door*>(*it);
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
 * @brief Implementation of \ref lua_api_map_set_doors_open.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_set_doors_open(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& prefix = luaL_checkstring(l, 2);
  bool open = true;
  if (lua_isboolean(l, 3)) {
    open = lua_toboolean(l, 3);
  }

  MapEntities& entities = map.get_entities();
  std::list<MapEntity*> doors = entities.get_entities_with_prefix(DOOR, prefix);
  std::list<MapEntity*>::iterator it;
  for (it = doors.begin(); it != doors.end(); it++) {
    Door* door = static_cast<Door*>(*it);
    door->set_open(open);
  }

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_get_entity.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_get_entity(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& name = luaL_checkstring(l, 2);

  MapEntity* entity = map.get_entities().find_entity(name);

  if (entity != NULL && !entity->is_being_removed()) {
    push_entity(l, *entity);
  }
  else {
    lua_pushnil(l);
  }
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_map_has_entity.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_has_entity(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& name = luaL_checkstring(l, 2);

  MapEntity* entity = map.get_entities().find_entity(name);

  lua_pushboolean(l, entity != NULL);
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_map_get_entities.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_get_entities(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& prefix = luaL_checkstring(l, 2);

  const std::list<MapEntity*> entities =
    map.get_entities().get_entities_with_prefix(prefix);

  lua_newtable(l);
  int i = 0;
  std::list<MapEntity*>::const_iterator it;
  for (it = entities.begin(); it != entities.end(); it++) {
    MapEntity* entity = *it;
    push_entity(l, *entity);
    lua_rawseti(l, -2, ++i);
  }
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_map_get_entities_count.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_get_entities_count(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& prefix = luaL_checkstring(l, 2);

  const std::list<MapEntity*> entities =
    map.get_entities().get_entities_with_prefix(prefix);

  lua_pushinteger(l, entities.size());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_map_has_entities.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_has_entities(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& prefix = luaL_checkstring(l, 2);

  lua_pushboolean(l, map.get_entities().has_entity_with_prefix(prefix));
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_map_set_entities_enabled.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_set_entities_enabled(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& prefix = luaL_checkstring(l, 2);
  bool enabled = true;
  if (lua_isboolean(l, 3)) {
    enabled = lua_toboolean(l, 3);
  }

  std::list<MapEntity*> entities =
      map.get_entities().get_entities_with_prefix(prefix);
  std::list<MapEntity*>::iterator it;
  for (it = entities.begin(); it != entities.end(); it++) {
    (*it)->set_enabled(enabled);
  }

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_remove_entities.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_remove_entities(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& prefix = luaL_checkstring(l, 2);

  map.get_entities().remove_entities_with_prefix(prefix);
  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_create_tile.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_create_tile(lua_State* l) {

  Map& map = get_entity_creation_map(l);
  luaL_checktype(l, 1, LUA_TTABLE);
  Layer layer = Layer(check_int_field(l, 1, "layer"));
  int x = check_int_field(l, 1, "x");
  int y = check_int_field(l, 1, "y");
  int width = check_int_field(l, 1, "width");
  int height = check_int_field(l, 1, "height");
  int tile_pattern_id = check_int_field(l, 1, "pattern");

  MapEntity* entity = new Tile(layer, x, y, width, height, tile_pattern_id);
  map.get_entities().add_entity(entity);

  if (map.is_started()) {
    push_entity(l, *entity);
    return 1;
  }
  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_create_destination.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_create_destination(lua_State* l) {

  Map& map = get_entity_creation_map(l);
  luaL_checktype(l, 1, LUA_TTABLE);
  Layer layer = Layer(check_int_field(l, 1, "layer"));
  int x = check_int_field(l, 1, "x");
  int y = check_int_field(l, 1, "y");
  const std::string& name = check_string_field(l, 1, "name");
  int direction = check_int_field(l, 1, "direction");
  const std::string& sprite_name = opt_string_field(l, 1, "sprite", "");

  MapEntity* entity = new Destination(name, layer, x, y, direction, sprite_name);
  map.get_entities().add_entity(entity);

  if (map.is_started()) {
    push_entity(l, *entity);
    return 1;
  }
  return 0;
 }

/**
 * @brief Implementation of \ref lua_api_map_create_teletransporter.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_create_teletransporter(lua_State* l) {

  Map& map = get_entity_creation_map(l);
  luaL_checktype(l, 1, LUA_TTABLE);
  Layer layer = Layer(check_int_field(l, 1, "layer"));
  int x = check_int_field(l, 1, "x");
  int y = check_int_field(l, 1, "y");
  int width = check_int_field(l, 1, "width");
  int height = check_int_field(l, 1, "height");
  const std::string& name = check_string_field(l, 1, "name");
  const std::string& sprite_name = opt_string_field(l, 1, "sprite", "");
  const std::string& sound_id = opt_string_field(l, 1, "sound", "");
  Transition::Style transition_style = Transition::Style(check_int_field(l, 1, "transition"));
  const std::string& destination_map_id = check_string_field(l, 1, "destination_map");
  const std::string& destination_name = check_string_field(l, 1, "destination");

  MapEntity* entity = new Teletransporter(
      name,
      layer, x, y, width, height,
      sprite_name,
      sound_id,
      transition_style,
      destination_map_id,
      destination_name);
  map.get_entities().add_entity(entity);

  if (map.is_started()) {
    push_entity(l, *entity);
    return 1;
  }
  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_create_pickable.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_create_pickable(lua_State* l) {

  Map& map = get_entity_creation_map(l);
  luaL_checktype(l, 1, LUA_TTABLE);
  Layer layer = Layer(check_int_field(l, 1, "layer"));
  int x = check_int_field(l, 1, "x");
  int y = check_int_field(l, 1, "y");
  const std::string& treasure_name = opt_string_field(l, 1, "treasure_name", "");
  int treasure_variant = opt_int_field(l, 1, "treasure_variant", 1);
  const std::string& treasure_savegame_variable = opt_string_field(l, 1, "treasure_savegame_variable", "");

  if (!treasure_savegame_variable.empty() && !is_valid_lua_identifier(treasure_savegame_variable)) {
    luaL_argerror(l, 1, (StringConcat() <<
        "Bad field 'treasure_savegame_variable' (invalid savegame variable identifier '" <<
        treasure_savegame_variable << "'").c_str());
  }

  Game& game = map.get_game();
  MapEntity* entity = Pickable::create(
      game, layer, x, y,
      Treasure(game, treasure_name, treasure_variant, treasure_savegame_variable),
      FALLING_MEDIUM, false
  );

  if (entity == NULL) {
    lua_pushnil(l);
    return 1;
  }

  map.get_entities().add_entity(entity);
  if (map.is_started()) {
    push_entity(l, *entity);
    return 1;
  }
  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_create_destructible.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_create_destructible(lua_State* l) {

  Map& map = get_entity_creation_map(l);
  luaL_checktype(l, 1, LUA_TTABLE);
  Layer layer = Layer(check_int_field(l, 1, "layer"));
  int x = check_int_field(l, 1, "x");
  int y = check_int_field(l, 1, "y");
  const std::string& subtype_name = check_string_field(l, 1, "subtype");
  const std::string& treasure_name = opt_string_field(l, 1, "treasure_name", "");
  int treasure_variant = opt_int_field(l, 1, "treasure_variant", 1);
  const std::string& treasure_savegame_variable = opt_string_field(l, 1, "treasure_savegame_variable", "");

  if (!treasure_savegame_variable.empty() && !is_valid_lua_identifier(treasure_savegame_variable)) {
    luaL_argerror(l, 1, (StringConcat() <<
        "Bad field 'treasure_savegame_variable' (invalid savegame variable identifier '" <<
        treasure_savegame_variable << "'").c_str());
  }

  // TODO Like enemies, implement this callback at runtime with function destructible:on_destroyed()
  int destruction_callback_ref = LUA_REFNIL;
  if (map.is_loaded()) {
    // We are at map runtime, i.e. running a Lua script.
    // In this case, we can do more than from the map data file.
    destruction_callback_ref = opt_function_field(l, 1, "on_destroyed");
  }
  else {
    // We are at map loading time, i.e. parsing a map data file
    // (that happens to be written in Lua but it could be XML or anything...).
    // This current Lua state l is only used to parse the map data file.
    // It knows nothing from the Solarus API (it just reuses these
    // map_api_create_XXX functions for to avoid to reimplement all of them).
    // But we don't allow to pass the on_destroyed callback as a function
    // since it's not the same Lua state.
    // We also want the map data file, including the on_destroyed field, to be
    // editable in clear text by a quest editor, so it has to be a string.

    // TODO Allow to pass the on_destroyed callback as a string in this case.
  }

  Destructible* destructible = new Destructible(
      layer, x, y,
      Destructible::get_subtype_by_name(subtype_name),
      Treasure(map.get_game(), treasure_name, treasure_variant, treasure_savegame_variable));
  destructible->set_destruction_callback(destruction_callback_ref);
  map.get_entities().add_entity(destructible);

  if (map.is_started()) {
    push_entity(l, *destructible);
    return 1;
  }
  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_create_chest.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_create_chest(lua_State* l) {

  Map& map = get_entity_creation_map(l);
  luaL_checktype(l, 1, LUA_TTABLE);
  Layer layer = Layer(check_int_field(l, 1, "layer"));
  int x = check_int_field(l, 1, "x");
  int y = check_int_field(l, 1, "y");
  bool big_chest = check_boolean_field(l, 1, "is_big_chest");
  const std::string& name = check_string_field(l, 1, "name");
  const std::string& treasure_name = opt_string_field(l, 1, "treasure_name", "");
  int treasure_variant = opt_int_field(l, 1, "treasure_variant", 1);
  const std::string& treasure_savegame_variable = opt_string_field(l, 1, "treasure_savegame_variable", "");

  if (!treasure_savegame_variable.empty() && !is_valid_lua_identifier(treasure_savegame_variable)) {
    luaL_argerror(l, 1, (StringConcat() <<
        "Bad field 'treasure_savegame_variable' (invalid savegame variable identifier '" <<
        treasure_savegame_variable << "'").c_str());
  }

  Game& game = map.get_game();
  MapEntity* entity = new Chest(name, layer, x, y, big_chest,
      Treasure(game, treasure_name, treasure_variant, treasure_savegame_variable));
  map.get_entities().add_entity(entity);

  if (map.is_started()) {
    push_entity(l, *entity);
    return 1;
  }
  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_create_jumper.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_create_jumper(lua_State* l) {

  Map& map = get_entity_creation_map(l);
  luaL_checktype(l, 1, LUA_TTABLE);
  Layer layer = Layer(check_int_field(l, 1, "layer"));
  int x = check_int_field(l, 1, "x");
  int y = check_int_field(l, 1, "y");
  int width = check_int_field(l, 1, "width");
  int height = check_int_field(l, 1, "height");
  const std::string& name = check_string_field(l, 1, "name");
  int direction = check_int_field(l, 1, "direction");
  int jump_length = check_int_field(l, 1, "jump_length");

  MapEntity* entity = new Jumper(name, layer, x, y, width, height,
      direction, jump_length);
  map.get_entities().add_entity(entity);

  if (map.is_started()) {
    push_entity(l, *entity);
    return 1;
  }
  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_create_enemy.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_create_enemy(lua_State* l) {

  Map& map = get_entity_creation_map(l);
  luaL_checktype(l, 1, LUA_TTABLE);
  Layer layer = Layer(check_int_field(l, 1, "layer"));
  int x = check_int_field(l, 1, "x");
  int y = check_int_field(l, 1, "y");
  const std::string& name = check_string_field(l, 1, "name");
  int direction = check_int_field(l, 1, "direction");
  const std::string& breed = check_string_field(l, 1, "breed");
  Enemy::Rank rank = Enemy::Rank(check_int_field(l, 1, "rank"));
  const std::string& savegame_variable = opt_string_field(l, 1, "savegame_variable", "");
  const std::string& treasure_name = opt_string_field(l, 1, "treasure_name", "");
  int treasure_variant = opt_int_field(l, 1, "treasure_variant", 1);
  const std::string& treasure_savegame_variable = opt_string_field(l, 1, "treasure_savegame_variable", "");

  if (!savegame_variable.empty() && !is_valid_lua_identifier(savegame_variable)) {
    luaL_argerror(l, 1, (StringConcat() <<
        "Bad field 'savegame_variable' (invalid savegame variable identifier '" <<
        savegame_variable << "'").c_str());
  }

  if (!treasure_savegame_variable.empty() && !is_valid_lua_identifier(treasure_savegame_variable)) {
    luaL_argerror(l, 1, (StringConcat() <<
        "Bad field 'treasure_savegame_variable' (invalid savegame variable identifier '" <<
        treasure_savegame_variable << "'").c_str());
  }

  Game& game = map.get_game();
  MapEntity* entity = Enemy::create(
      game, breed, rank, savegame_variable, name,
      layer, x, y, direction,
      Treasure(game, treasure_name, treasure_variant, treasure_savegame_variable));

  if (entity == NULL) {
    lua_pushnil(l);
    return 1;
  }

  map.get_entities().add_entity(entity);
  if (map.is_started()) {
    push_entity(l, *entity);
    return 1;
  }
  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_create_npc.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_create_npc(lua_State* l) {

  Map& map = get_entity_creation_map(l);
  luaL_checktype(l, 1, LUA_TTABLE);
  Layer layer = Layer(check_int_field(l, 1, "layer"));
  int x = check_int_field(l, 1, "x");
  int y = check_int_field(l, 1, "y");
  const std::string& name = check_string_field(l, 1, "name");
  int direction = check_int_field(l, 1, "direction");
  const std::string& subtype_name = check_string_field(l, 1, "subtype");
  const std::string& sprite_name = opt_string_field(l, 1, "sprite", "");
  const std::string& behavior = opt_string_field(l, 1, "behavior", "");

  int subtype;
  std::istringstream iss(subtype_name);
  iss >> subtype;

  Game& game = map.get_game();
  MapEntity* entity = new NPC(game, name, layer, x, y, NPC::Subtype(subtype),
      sprite_name, direction, behavior);
  map.get_entities().add_entity(entity);

  if (map.is_started()) {
    push_entity(l, *entity);
    return 1;
  }
  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_create_block.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_create_block(lua_State* l) {

  Map& map = get_entity_creation_map(l);
  luaL_checktype(l, 1, LUA_TTABLE);
  Layer layer = Layer(check_int_field(l, 1, "layer"));
  int x = check_int_field(l, 1, "x");
  int y = check_int_field(l, 1, "y");
  const std::string& name = check_string_field(l, 1, "name");
  int direction = check_int_field(l, 1, "direction");
  const std::string& sprite_name = check_string_field(l, 1, "sprite");
  bool pushable = check_boolean_field(l, 1, "pushable");
  bool pullable = check_boolean_field(l, 1, "pullable");
  int maximum_moves = check_int_field(l, 1, "maximum_moves");

  Block* entity = new Block(name, layer, x, y, direction,
      sprite_name, pushable, pullable, maximum_moves);
  map.get_entities().add_entity(entity);

  if (map.is_started()) {
    push_entity(l, *entity);
    return 1;
  }
  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_create_dynamic_tile.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_create_dynamic_tile(lua_State* l) {

  Map& map = get_entity_creation_map(l);
  luaL_checktype(l, 1, LUA_TTABLE);
  Layer layer = Layer(check_int_field(l, 1, "layer"));
  int x = check_int_field(l, 1, "x");
  int y = check_int_field(l, 1, "y");
  int width = check_int_field(l, 1, "width");
  int height = check_int_field(l, 1, "height");
  const std::string& name = check_string_field(l, 1, "name");
  int tile_pattern_id = check_int_field(l, 1, "pattern");
  bool enabled_at_start = check_boolean_field(l, 1, "enabled_at_start");

  MapEntity* entity = new DynamicTile(name, layer, x, y, width, height,
      tile_pattern_id, enabled_at_start);
  map.get_entities().add_entity(entity);

  if (map.is_started()) {
    push_entity(l, *entity);
    return 1;
  }
  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_create_switch.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_create_switch(lua_State* l) {

  Map& map = get_entity_creation_map(l);
  luaL_checktype(l, 1, LUA_TTABLE);
  Layer layer = Layer(check_int_field(l, 1, "layer"));
  int x = check_int_field(l, 1, "x");
  int y = check_int_field(l, 1, "y");
  const std::string& name = check_string_field(l, 1, "name");
  const std::string& subtype_name = check_string_field(l, 1, "subtype");
  bool needs_block = check_boolean_field(l, 1, "needs_block");
  bool inactivate_when_leaving = check_boolean_field(l, 1, "inactivate_when_leaving");

  int subtype;
  std::istringstream iss(subtype_name);
  iss >> subtype;

  MapEntity* entity = new Switch(name, layer, x, y,
      Switch::Subtype(subtype), needs_block, inactivate_when_leaving);
  map.get_entities().add_entity(entity);

  if (map.is_started()) {
    push_entity(l, *entity);
    return 1;
  }
  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_create_wall.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_create_wall(lua_State* l) {

  Map& map = get_entity_creation_map(l);
  luaL_checktype(l, 1, LUA_TTABLE);
  Layer layer = Layer(check_int_field(l, 1, "layer"));
  int x = check_int_field(l, 1, "x");
  int y = check_int_field(l, 1, "y");
  int width = check_int_field(l, 1, "width");
  int height = check_int_field(l, 1, "height");
  const std::string& name = check_string_field(l, 1, "name");
  bool stops_hero = check_boolean_field(l, 1, "stops_hero");
  bool stops_npcs = check_boolean_field(l, 1, "stops_npcs");
  bool stops_enemies = check_boolean_field(l, 1, "stops_enemies");
  bool stops_blocks = check_boolean_field(l, 1, "stops_blocks");

  MapEntity* entity = new Wall(name, layer, x, y, width, height,
      stops_hero, stops_npcs, stops_enemies, stops_blocks);
  map.get_entities().add_entity(entity);

  if (map.is_started()) {
    push_entity(l, *entity);
    return 1;
  }
  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_create_sensor.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_create_sensor(lua_State* l) {

  Map& map = get_entity_creation_map(l);
  luaL_checktype(l, 1, LUA_TTABLE);
  Layer layer = Layer(check_int_field(l, 1, "layer"));
  int x = check_int_field(l, 1, "x");
  int y = check_int_field(l, 1, "y");
  int width = check_int_field(l, 1, "width");
  int height = check_int_field(l, 1, "height");
  const std::string& name = check_string_field(l, 1, "name");

  MapEntity* entity = new Sensor(name, layer, x, y, width, height);
  map.get_entities().add_entity(entity);

  if (map.is_started()) {
    push_entity(l, *entity);
    return 1;
  }
  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_create_crystal.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_create_crystal(lua_State* l) {

  Map& map = get_entity_creation_map(l);
  luaL_checktype(l, 1, LUA_TTABLE);
  Layer layer = Layer(check_int_field(l, 1, "layer"));
  int x = check_int_field(l, 1, "x");
  int y = check_int_field(l, 1, "y");
  const std::string& name = check_string_field(l, 1, "name");

  MapEntity* entity = new Crystal(name, layer, x, y);
  map.get_entities().add_entity(entity);

  if (map.is_started()) {
    push_entity(l, *entity);
    return 1;
  }
  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_create_crystal_block.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_create_crystal_block(lua_State* l) {

  Map& map = get_entity_creation_map(l);
  luaL_checktype(l, 1, LUA_TTABLE);
  Layer layer = Layer(check_int_field(l, 1, "layer"));
  int x = check_int_field(l, 1, "x");
  int y = check_int_field(l, 1, "y");
  int width = check_int_field(l, 1, "width");
  int height = check_int_field(l, 1, "height");
  const std::string& subtype_name = check_string_field(l, 1, "subtype");

  int subtype;
  std::istringstream iss(subtype_name);
  iss >> subtype;

  Game& game = map.get_game();
  MapEntity* entity = new CrystalBlock(game, layer, x, y, width, height,
      CrystalBlock::Subtype(subtype));
  map.get_entities().add_entity(entity);

  if (map.is_started()) {
    push_entity(l, *entity);
    return 1;
  }
  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_create_shop_item.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_create_shop_item(lua_State* l) {

  Map& map = get_entity_creation_map(l);
  luaL_checktype(l, 1, LUA_TTABLE);
  Layer layer = Layer(check_int_field(l, 1, "layer"));
  int x = check_int_field(l, 1, "x");
  int y = check_int_field(l, 1, "y");
  const std::string& name = check_string_field(l, 1, "name");
  const std::string& treasure_name = check_string_field(l, 1, "treasure_name");
  int treasure_variant = opt_int_field(l, 1, "treasure_variant", 1);
  const std::string& treasure_savegame_variable = opt_string_field(l, 1, "treasure_savegame_variable", "");
  int price = check_int_field(l, 1, "price");
  const std::string& dialog_id = check_string_field(l, 1, "dialog");

  if (!treasure_savegame_variable.empty() && !is_valid_lua_identifier(treasure_savegame_variable)) {
    luaL_argerror(l, 1, (StringConcat() <<
        "Bad field 'treasure_savegame_variable' (invalid savegame variable identifier '" <<
        treasure_savegame_variable << "'").c_str());
  }

  Game& game = map.get_game();
  MapEntity* entity = ShopItem::create(game, name, layer, x, y,
      Treasure(game, treasure_name, treasure_variant, treasure_savegame_variable),
      price, dialog_id);

  if (entity == NULL) {
    lua_pushnil(l);
    return 1;
  }

  map.get_entities().add_entity(entity);
  if (map.is_started()) {
    push_entity(l, *entity);
    return 1;
  }
  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_create_conveyor_belt.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_create_conveyor_belt(lua_State* l) {

  Map& map = get_entity_creation_map(l);
  luaL_checktype(l, 1, LUA_TTABLE);
  Layer layer = Layer(check_int_field(l, 1, "layer"));
  int x = check_int_field(l, 1, "x");
  int y = check_int_field(l, 1, "y");
  int direction = check_int_field(l, 1, "direction");

  MapEntity* entity = new ConveyorBelt(layer, x, y, direction);
  map.get_entities().add_entity(entity);

  if (map.is_started()) {
    push_entity(l, *entity);
    return 1;
  }
  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_create_door.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_create_door(lua_State* l) {

  Map& map = get_entity_creation_map(l);
  luaL_checktype(l, 1, LUA_TTABLE);
  Layer layer = Layer(check_int_field(l, 1, "layer"));
  int x = check_int_field(l, 1, "x");
  int y = check_int_field(l, 1, "y");
  const std::string& name = check_string_field(l, 1, "name");
  int direction = check_int_field(l, 1, "direction");
  const std::string& subtype_name = check_string_field(l, 1, "subtype");
  const std::string& savegame_variable = opt_string_field(l, 1, "savegame_variable", "");

  if (!savegame_variable.empty() && !is_valid_lua_identifier(savegame_variable)) {
    luaL_argerror(l, 1, (StringConcat() <<
        "Bad field 'savegame_variable' (invalid savegame variable identifier '" <<
        savegame_variable << "'").c_str());
  }

  int subtype;
  std::istringstream iss(subtype_name);
  iss >> subtype;

  Game& game = map.get_game();
  MapEntity* entity = new Door(game, name, layer, x, y, direction,
      Door::Subtype(subtype), savegame_variable);
  map.get_entities().add_entity(entity);

  if (map.is_started()) {
    push_entity(l, *entity);
    return 1;
  }
  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_create_stairs.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_create_stairs(lua_State* l) {

  Map& map = get_entity_creation_map(l);
  luaL_checktype(l, 1, LUA_TTABLE);
  Layer layer = Layer(check_int_field(l, 1, "layer"));
  int x = check_int_field(l, 1, "x");
  int y = check_int_field(l, 1, "y");
  const std::string& name = check_string_field(l, 1, "name");
  int direction = check_int_field(l, 1, "direction");
  const std::string& subtype_name = check_string_field(l, 1, "subtype");

  int subtype;
  std::istringstream iss(subtype_name);
  iss >> subtype;

  MapEntity* entity = new Stairs(name, layer, x, y, direction,
      Stairs::Subtype(subtype));
  map.get_entities().add_entity(entity);

  if (map.is_started()) {
    push_entity(l, *entity);
    return 1;
  }
  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_create_bomb.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_create_bomb(lua_State* l) {

  Map& map = get_entity_creation_map(l);
  luaL_checktype(l, 1, LUA_TTABLE);
  Layer layer = Layer(check_int_field(l, 1, "layer"));
  int x = check_int_field(l, 1, "x");
  int y = check_int_field(l, 1, "y");

  MapEntity* entity = new Bomb(layer, x, y);
  map.get_entities().add_entity(entity);

  if (map.is_started()) {
    push_entity(l, *entity);
    return 1;
  }
  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_create_explosion.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_create_explosion(lua_State* l) {

  Map& map = get_entity_creation_map(l);
  luaL_checktype(l, 1, LUA_TTABLE);
  Layer layer = Layer(check_int_field(l, 1, "layer"));
  int x = check_int_field(l, 1, "x");
  int y = check_int_field(l, 1, "y");

  MapEntity* entity = new Explosion(layer, Rectangle(x, y), true);
  map.get_entities().add_entity(entity);

  if (map.is_started()) {
    push_entity(l, *entity);
    return 1;
  }
  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_create_fire(lua_State* l) {

  Map& map = get_entity_creation_map(l);
  luaL_checktype(l, 1, LUA_TTABLE);
  Layer layer = Layer(check_int_field(l, 1, "layer"));
  int x = check_int_field(l, 1, "x");
  int y = check_int_field(l, 1, "y");

  MapEntity* entity = new Fire(layer, Rectangle(x, y));
  map.get_entities().add_entity(entity);

  if (map.is_started()) {
    push_entity(l, *entity);
    return 1;
  }
  return 0;
}

/**
 * @brief Calls the on_update() method of a Lua map.
 * @param map A map.
 */
void LuaContext::map_on_update(Map& map) {

  push_map(l, map);
  on_update();
  menus_on_update(-1);
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_draw() method of a Lua map.
 * @param map A map.
 * @param dst_surface The destination surface.
 */
void LuaContext::map_on_draw(Map& map, Surface& dst_surface) {

  push_map(l, map);
  menus_on_draw(-1, dst_surface);
  on_draw(dst_surface);
  lua_pop(l, 1);
}

/**
 * @brief Notifies a Lua map that an input event has just occurred.
 *
 * The appropriate callback in the map is triggered if it exists.
 *
 * @param event The input event to handle.
 * @param map A map.
 * @return \c true if the event was handled and should stop being propagated.
 */
bool LuaContext::map_on_input(Map& map, InputEvent& event) {

  bool handled = false;
  push_map(l, map);
  handled = on_input(event);
  if (!handled) {
    handled = menus_on_input(-1, event);
  }
  lua_pop(l, 1);
  return handled;
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
  remove_timers(-1);  // Stop timers and menus associated to this map.
  remove_menus(-1);
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
 * @brief Calls the on_camera_back() method of a Lua map.
 * @param map A map.
 */
void LuaContext::map_on_camera_back(Map& map) {

  push_map(l, map);
  on_camera_back();
  lua_pop(l, 1);
}

