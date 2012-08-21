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
      { "start_dialog", map_api_start_dialog },
      { "set_dialog_variable", map_api_set_dialog_variable },
      { "set_dialog_style", map_api_set_dialog_style },
      { "set_pause_enabled", map_api_set_pause_enabled },
      { "get_light", map_api_get_light },
      { "set_light", map_api_get_light },
      { "move_camera", map_api_move_camera },
      { "draw_sprite", map_api_draw_sprite },
      { "get_tileset", map_api_get_tileset },
      { "set_tileset", map_api_set_tileset },
      { "get_hero", map_api_get_hero },
      { "crystal_get_state", map_api_crystal_get_state },
      { "crystal_set_state", map_api_crystal_set_state },
      { "crystal_change_state", map_api_crystal_change_state },
      { "arrow_remove", map_api_arrow_remove },
      { "open_doors", map_api_open_doors },
      { "close_doors", map_api_close_doors },
      { "set_doors_open", map_api_set_doors_open },
      { "create_pickable", map_api_create_pickable },
      { "create_destructible", map_api_create_destructible },
      { "create_block", map_api_create_block },
      { "create_bomb", map_api_create_bomb },
      { "create_explosion", map_api_create_explosion },
      { "create_fire", map_api_create_fire },
      { "create_enemy", map_api_create_enemy },
      { "get_entity", map_api_get_entity },
      { "has_entity", map_api_has_entity },
      { "get_entities", map_api_get_entities },
      { "get_entities_count", map_api_get_entities_count },
      { "has_entities", map_api_has_entities },
      { "set_entities_enabled", map_api_set_entities_enabled },
      { "remove_entities", map_api_remove_entities },
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
 * @brief Implementation of \ref lua_api_map_start_dialog.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_start_dialog(lua_State* l) {

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
  int style = luaL_checkinteger(l, 2);

  map.get_game().get_dialog_box().set_style(DialogBox::Style(style));

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
  int light = luaL_checkinteger(l, 2);

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
 * @brief Implementation of \ref lua_api_map_draw_sprite.
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
 * @brief Implementation of \ref lua_api_map_get_tileset.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_get_tileset(lua_State* l) {

  Map& map = check_map(l, 1);

  TilesetId id = map.get_tileset_id();

  lua_pushinteger(l, id);
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_map_set_tileset.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_set_tileset(lua_State* l) {

  Map& map = check_map(l, 1);
  TilesetId id = luaL_checkinteger(l, 2);

  map.set_tileset(id);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_get_hero.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_get_hero(lua_State* l) {

  Map& map = check_map(l, 1);

  push_hero(l, map.get_entities().get_hero());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_map_crystal_get_state.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_crystal_get_state(lua_State* l) {

  Map& map = check_map(l, 1);

  lua_pushboolean(l, map.get_game().get_crystal_state());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_map_crystal_set_state.
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
 * @brief Implementation of \ref lua_api_map_crystal_change_state.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_crystal_change_state(lua_State* l) {

  Map& map = check_map(l, 1);

  map.get_game().change_crystal_state();

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_map_arrow_remove.
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

  if (entity != NULL) {
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
 * @brief Implementation of \ref lua_api_map_create_pickable.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_create_pickable(lua_State* l) {

  Map& map = check_map(l, 1);
  const std::string& item_name = luaL_checkstring(l, 2);
  int variant = luaL_checkinteger(l, 3);
  int savegame_variable = luaL_checkinteger(l, 4);
  int x = luaL_checkinteger(l, 5);
  int y = luaL_checkinteger(l, 6);
  Layer layer = Layer(luaL_checkinteger(l, 7));

  Game& game = map.get_game();
  Pickable* pickable = Pickable::create(
      game, layer, x, y,
      Treasure(game, item_name, variant, savegame_variable),
      FALLING_MEDIUM, false
      );
  map.get_entities().add_entity(pickable);

  push_entity(l, *pickable);
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_map_create_destructible.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_create_destructible(lua_State* l) {

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
  Destructible* destructible =
      new Destructible(layer, x, y, subtype, Treasure(map.get_game(),
          treasure_item, treasure_variant, treasure_savegame_variable));
  destructible->set_destruction_callback(destruction_callback_ref);
  map.get_entities().add_entity(destructible);

  push_entity(l, *destructible);
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_map_create_block.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_create_block(lua_State* l) {

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

  Block* block = new Block(name, layer, x, y, direction, sprite_name,
      can_be_pushed, can_be_pulled, maximum_moves);
  map.get_entities().add_entity(block);
  block->check_collision_with_detectors(false);

  push_entity(l, *block);
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_map_create_bomb.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_create_bomb(lua_State* l) {

  Map& map = check_map(l, 1);
  int x = luaL_checkinteger(l, 2);
  int y = luaL_checkinteger(l, 3);
  Layer layer = Layer(luaL_checkinteger(l, 4));

  Bomb* bomb = new Bomb(layer, x, y);
  map.get_entities().add_entity(bomb);

  push_entity(l, *bomb);
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_map_create_explosion.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_create_explosion(lua_State* l) {

  Map& map = check_map(l, 1);
  int x = luaL_checkinteger(l, 2);
  int y = luaL_checkinteger(l, 3);
  Layer layer = Layer(luaL_checkinteger(l, 4));

  Explosion* explosion = new Explosion(layer, Rectangle(x, y), true);
  map.get_entities().add_entity(explosion);

  push_entity(l, *explosion);
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_map_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_create_fire(lua_State* l) {

  Map& map = check_map(l, 1);
  int x = luaL_checkinteger(l, 2);
  int y = luaL_checkinteger(l, 3);
  Layer layer = Layer(luaL_checkinteger(l, 4));

  Fire* fire = new Fire(layer, Rectangle(x, y));
  map.get_entities().add_entity(fire);

  push_entity(l, *fire);
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_map_create_enemy.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::map_api_create_enemy(lua_State* l) {

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

  push_enemy(l, *enemy);
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

