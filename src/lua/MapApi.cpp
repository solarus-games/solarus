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
#include "solarus/entities/Block.h"
#include "solarus/entities/Bomb.h"
#include "solarus/entities/Chest.h"
#include "solarus/entities/Crystal.h"
#include "solarus/entities/CrystalBlock.h"
#include "solarus/entities/CustomEntity.h"
#include "solarus/entities/Destination.h"
#include "solarus/entities/Destructible.h"
#include "solarus/entities/Door.h"
#include "solarus/entities/DynamicTile.h"
#include "solarus/entities/Enemy.h"
#include "solarus/entities/Explosion.h"
#include "solarus/entities/Fire.h"
#include "solarus/entities/Hero.h"
#include "solarus/entities/Jumper.h"
#include "solarus/entities/MapEntities.h"
#include "solarus/entities/Npc.h"
#include "solarus/entities/Pickable.h"
#include "solarus/entities/Sensor.h"
#include "solarus/entities/Separator.h"
#include "solarus/entities/ShopTreasure.h"
#include "solarus/entities/Stairs.h"
#include "solarus/entities/Stream.h"
#include "solarus/entities/Switch.h"
#include "solarus/entities/Teletransporter.h"
#include "solarus/entities/Tile.h"
#include "solarus/entities/TilePattern.h"
#include "solarus/entities/Tileset.h"
#include "solarus/entities/Wall.h"
#include "solarus/lowlevel/Debug.h"
#include "solarus/lowlevel/Music.h"
#include "solarus/lowlevel/Sound.h"
#include "solarus/lua/LuaContext.h"
#include "solarus/lua/LuaTools.h"
#include "solarus/movements/Movement.h"
#include "solarus/Equipment.h"
#include "solarus/EquipmentItem.h"
#include "solarus/Game.h"
#include "solarus/MainLoop.h"
#include "solarus/Map.h"
#include "solarus/Timer.h"
#include "solarus/Treasure.h"
#include <lua.hpp>
#include <sstream>

namespace Solarus {

/**
 * Name of the Lua table representing the map module.
 */
const std::string LuaContext::map_module_name = "sol.map";

/**
 * \brief Initializes the map features provided to Lua.
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
      { "get_music", map_api_get_music },
      { "get_camera_position", map_api_get_camera_position },
      { "move_camera", map_api_move_camera },
      { "get_ground", map_api_get_ground },
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
      { "get_hero", map_api_get_hero },
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
      { "create_shop_treasure", map_api_create_shop_treasure },
      { "create_stream", map_api_create_stream },
      { "create_door", map_api_create_door },
      { "create_stairs", map_api_create_stairs },
      { "create_separator", map_api_create_separator },
      { "create_custom_entity", map_api_create_custom_entity },
      { "create_bomb", map_api_create_bomb },
      { "create_explosion", map_api_create_explosion },
      { "create_fire", map_api_create_fire },
      { nullptr, nullptr }
  };

  static const luaL_Reg metamethods[] = {
      { "__gc", userdata_meta_gc },
      { "__newindex", userdata_meta_newindex_as_table },
      { "__index", userdata_meta_index_as_table },
      { nullptr, nullptr }
  };

  register_type(map_module_name, nullptr, methods, metamethods);
}

/**
 * \brief Returns whether a value is a userdata of type map.
 * \param l A Lua context.
 * \param index An index in the stack.
 * \return true if the value at this index is a map.
 */
bool LuaContext::is_map(lua_State* l, int index) {
  return is_userdata(l, index, map_module_name);
}

/**
 * \brief Checks that the userdata at the specified index of the stack is a
 * map that is currently loaded and returns it.
 * \param l A Lua context.
 * \param index An index in the stack.
 * \return The map.
 */
std::shared_ptr<Map> LuaContext::check_map(lua_State* l, int index) {

  return std::static_pointer_cast<Map>(check_userdata(
      l, index, map_module_name
  ));
}

/**
 * \brief Pushes a map userdata onto the stack.
 * \param l A Lua context.
 * \param game A game.
 */
void LuaContext::push_map(lua_State* l, Map& map) {

  push_userdata(l, map);
}

/**
 * \brief Returns the map to be used when creating an entity.
 *
 * This function is meant to be called from entity creation functions.
 * If the first Lua parameter is a map, returns that map and removes it
 * from the Lua stack.
 * Otherwise, an implicit map must have been previously set (by calling
 * set_entity_implicit_creation_map()) and that map is returned.
 * Therefore, after calling this function; you will have the same Lua stack
 * in both cases, without worrying about how the map was passed.
 *
 * \param l A Lua context.
 * \return The map where an entity will be created.
 */
Map& LuaContext::get_entity_creation_map(lua_State* l) {

  Map* map = nullptr;

  if (is_map(l, 1)) {
    // The map is passed as a parameter (typically, by the map script).
    map = check_map(l, 1).get();
    lua_remove(l, 1);
  }
  else {
    // The map was is implicit (typically, we are loading its data file).
    map = get_entity_implicit_creation_map(l);
    Debug::check_assertion(map != nullptr,
        "No implicit creation was been set in this Lua state");
  }

  return *map;
}

/**
 * \brief Returns the map previously stored by
 * set_entity_implicit_creation_map().
 * \param l A Lua context.
 * \return The map stored in this Lua context or nullptr.
 */
Map* LuaContext::get_entity_implicit_creation_map(lua_State* l) {

  lua_getfield(l, LUA_REGISTRYINDEX, "map");
  if (lua_isnil(l, -1)) {
    return nullptr;
  }

  Map* map = static_cast<Map*>(lua_touserdata(l, -1));
  lua_pop(l, 1);

  return map;
}

/**
 * \brief Stores into the Lua state the map that will be used to create entities.
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
 * \param l A Lua context.
 * \param map The map where future entities should be created.
 * nullptr forces the map to be explicitly passed (this is the default
 * behavior).
 */
void LuaContext::set_entity_implicit_creation_map(lua_State* l, Map* map) {

  if (map == nullptr) {
    lua_pushnil(l);
  }
  else {
    lua_pushlightuserdata(l, map);
  }
  lua_setfield(l, LUA_REGISTRYINDEX, "map");
}

/**
 * \brief __index function of the environment of the map's code.
 *
 * This special __index function allows the map's Lua code to get a map
 * entity like a global value.
 * If an entity exists with the specified name, this entity is returned.
 * Otherwise, we fall back to the usual behavior of global values:
 * a global value with this name (or \c nil) is returned.
 *
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::l_get_map_entity_or_global(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    lua_pushvalue(l, lua_upvalueindex(1));  // Because check_map does not like pseudo-indexes.
    Map& map = *check_map(l, -1);
    const std::string& name = LuaTools::check_string(l, 2);

    MapEntity* entity = nullptr;
    if (map.is_started()) {
      entity = map.get_entities().find_entity(name);
    }

    if (entity != nullptr && !entity->is_being_removed()) {
      push_entity(l, *entity);
    }
    else {
      lua_getglobal(l, name.c_str());
    }
    return 1;
  });
}

/**
 * \brief Executes the callback function of a camera movement.
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::l_camera_do_callback(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    // Execute the function.
    lua_settop(l, 0);
    lua_getfield(l, LUA_REGISTRYINDEX, "sol.camera_function");
    LuaTools::call_function(l, 0, 0, "camera callback");

    // Set a second timer to restore the camera.
    Map& map = get_lua_context(l).get_main_loop().get_game()->get_current_map();
    push_map(l, map);
    lua_getfield(l, LUA_REGISTRYINDEX, "sol.camera_delay_after");
    lua_pushcfunction(l, l_camera_restore);
    timer_api_start(l);
    const TimerPtr& timer = check_timer(l, -1);
    timer->set_suspended_with_map(false);

    return 0;
  });
}

/**
 * \brief Moves the camera back to the hero.
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::l_camera_restore(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    LuaContext& lua_context = get_lua_context(l);

    lua_context.get_main_loop().get_game()->get_current_map().restore_camera();

    return 0;
  });
}

/**
 * \brief Implementation of map:get_game().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::map_api_get_game(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Map& map = *check_map(l, 1);

    push_game(l, map.get_game().get_savegame());
    return 1;
  });
}

/**
 * \brief Implementation of map:get_id().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::map_api_get_id(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const Map& map = *check_map(l, 1);

    push_string(l, map.get_id());
    return 1;
  });
}

/**
 * \brief Implementation of map:get_world().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::map_api_get_world(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const Map& map = *check_map(l, 1);

    const std::string& world = map.get_world();

    if (world.empty()) {
      lua_pushnil(l);
    }
    else {
      push_string(l, world);
    }
    return 1;
  });
}

/**
 * \brief Implementation of map:get_floor().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::map_api_get_floor(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const Map& map = *check_map(l, 1);

    if (!map.has_floor()) {
      lua_pushnil(l);
    }
    else {
      lua_pushinteger(l, map.get_floor());
    }
    return 1;
  });
}

/**
 * \brief Implementation of map:get_size().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::map_api_get_size(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const Map& map = *check_map(l, 1);

    lua_pushinteger(l, map.get_width());
    lua_pushinteger(l, map.get_height());

    return 2;
  });
}

/**
 * \brief Implementation of map:get_location().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::map_api_get_location(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const Map& map = *check_map(l, 1);

    lua_pushinteger(l, map.get_location().get_x());
    lua_pushinteger(l, map.get_location().get_y());

    return 2;
  });
}

/**
 * \brief Implementation of map:get_tileset().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::map_api_get_tileset(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const Map& map = *check_map(l, 1);

    push_string(l, map.get_tileset_id());
    return 1;
  });
}

/**
 * \brief Implementation of map:get_music().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::map_api_get_music(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const Map& map = *check_map(l, 1);

    const std::string& music_id = map.get_music_id();
    if (music_id == Music::none) {
      // Special id to stop any music.
      lua_pushnil(l);
    }
    else if (music_id == Music::unchanged) {
      // Special id to keep the music unchanged.
      push_string(l, "same");
    }
    else {
      push_string(l, music_id);
    }
    return 1;
  });
}

/**
 * \brief Implementation of map:set_tileset().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::map_api_set_tileset(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Map& map = *check_map(l, 1);
    const std::string& tileset_id = LuaTools::check_string(l, 2);

    map.set_tileset(tileset_id);

    return 0;
  });
}

/**
 * \brief Implementation of map:get_camera_position().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::map_api_get_camera_position(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const Map& map = *check_map(l, 1);

    const Rectangle& camera_position = map.get_camera_position();

    lua_pushinteger(l, camera_position.get_x());
    lua_pushinteger(l, camera_position.get_y());
    lua_pushinteger(l, camera_position.get_width());
    lua_pushinteger(l, camera_position.get_height());
    return 4;
  });
}

/**
 * \brief Implementation of map:move_camera().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::map_api_move_camera(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Map& map = *check_map(l, 1);
    int x = LuaTools::check_int(l, 2);
    int y = LuaTools::check_int(l, 3);
    int speed = LuaTools::check_int(l, 4);
    LuaTools::check_type(l, 5, LUA_TFUNCTION);

    uint32_t delay_before = 1000;
    uint32_t delay_after = 1000;
    if (lua_gettop(l) >= 6) {
      delay_before = LuaTools::check_int(l, 6);
      if (lua_gettop(l) >= 7) {
        delay_after = LuaTools::check_int(l, 7);
      }
    }
    lua_settop(l, 5); // let the function on top of the stack

    // store the function and the delays
    // TODO store this as Lua refs instead of globally
    lua_setfield(l, LUA_REGISTRYINDEX, "sol.camera_function");
    lua_pushinteger(l, delay_before);
    lua_setfield(l, LUA_REGISTRYINDEX, "sol.camera_delay_before");
    lua_pushinteger(l, delay_after);
    lua_setfield(l, LUA_REGISTRYINDEX, "sol.camera_delay_after");

    // start the camera
    map.move_camera(x, y, speed);

    return 0;
  });
}

/**
 * \brief Implementation of map:get_ground().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::map_api_get_ground(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const Map& map = *check_map(l, 1);
    int x = LuaTools::check_int(l, 2);
    int y = LuaTools::check_int(l, 3);
    Layer layer = LuaTools::check_layer(l, 4);

    Ground ground = map.get_ground(layer, x, y);

    push_string(l, Tileset::ground_names[ground]);
    return 1;
  });
}

/**
 * \brief Implementation of map:draw_sprite().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::map_api_draw_sprite(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Map& map = *check_map(l, 1);
    Sprite& sprite = *check_sprite(l, 2);
    int x = LuaTools::check_int(l, 3);
    int y = LuaTools::check_int(l, 4);

    map.draw_sprite(sprite, x, y);

    return 0;
  });
}

/**
 * \brief Implementation of map:get_crystal_state().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::map_api_get_crystal_state(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Map& map = *check_map(l, 1);

    lua_pushboolean(l, map.get_game().get_crystal_state());
    return 1;
  });
}

/**
 * \brief Implementation of map:set_crystal_state().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::map_api_set_crystal_state(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Map& map = *check_map(l, 1);
    bool state = LuaTools::check_boolean(l, 2);

    Game& game = map.get_game();
    if (game.get_crystal_state() != state) {
      game.change_crystal_state();
    }

    return 0;
  });
}

/**
 * \brief Implementation of map:change_crystal_state().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::map_api_change_crystal_state(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Map& map = *check_map(l, 1);

    map.get_game().change_crystal_state();

    return 0;
  });
}

/**
 * \brief Implementation of map:open_doors().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::map_api_open_doors(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Map& map = *check_map(l, 1);
    const std::string& prefix = LuaTools::check_string(l, 2);

    bool done = false;
    MapEntities& entities = map.get_entities();
    std::list<MapEntity*> doors = entities.get_entities_with_prefix(ENTITY_DOOR, prefix);
    for (auto it = doors.begin(); it != doors.end(); ++it) {
      Door* door = static_cast<Door*>(*it);
      if (!door->is_open() || door->is_closing()) {
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
  });
}

/**
 * \brief Implementation of map:close_doors().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::map_api_close_doors(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Map& map = *check_map(l, 1);
    const std::string& prefix = LuaTools::check_string(l, 2);

    bool done = false;
    MapEntities& entities = map.get_entities();
    std::list<MapEntity*> doors = entities.get_entities_with_prefix(ENTITY_DOOR, prefix);
    for (auto it = doors.begin(); it != doors.end(); ++it) {
      Door* door = static_cast<Door*>(*it);
      if (door->is_open() || door->is_opening()) {
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
  });
}

/**
 * \brief Implementation of map:set_doors_open().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::map_api_set_doors_open(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Map& map = *check_map(l, 1);
    const std::string& prefix = LuaTools::check_string(l, 2);
    bool open = LuaTools::opt_boolean(l, 3, true);

    MapEntities& entities = map.get_entities();
    std::list<MapEntity*> doors = entities.get_entities_with_prefix(ENTITY_DOOR, prefix);
    for (auto it = doors.begin(); it != doors.end(); ++it) {
      Door* door = static_cast<Door*>(*it);
      door->set_open(open);
    }

    return 0;
  });
}

/**
 * \brief Implementation of map:get_entity().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::map_api_get_entity(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Map& map = *check_map(l, 1);
    const std::string& name = LuaTools::check_string(l, 2);

    MapEntity* entity = map.get_entities().find_entity(name);

    if (entity != nullptr && !entity->is_being_removed()) {
      push_entity(l, *entity);
    }
    else {
      lua_pushnil(l);
    }
    return 1;
  });
}

/**
 * \brief Implementation of map:has_entity().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::map_api_has_entity(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Map& map = *check_map(l, 1);
    const std::string& name = LuaTools::check_string(l, 2);

    MapEntity* entity = map.get_entities().find_entity(name);

    lua_pushboolean(l, entity != nullptr);
    return 1;
  });
}

/**
 * \brief Implementation of map:get_entities().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::map_api_get_entities(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Map& map = *check_map(l, 1);
    const std::string& prefix = LuaTools::check_string(l, 2);

    const std::list<MapEntity*> entities =
        map.get_entities().get_entities_with_prefix(prefix);

    lua_newtable(l);
    for (auto it = entities.begin(); it != entities.end(); ++it) {
      MapEntity* entity = *it;
      push_entity(l, *entity);
      lua_pushboolean(l, true);
      lua_rawset(l, -3);
    }
    lua_getglobal(l, "pairs");
    lua_pushvalue(l, -2);
    lua_call(l, 1, 3);  // TODO don't call the pairs global value, implement our
    // own iterator instead.
    // Or at least store pairs in the registry (like we do
    // with io.open) to be sure it is the original one.

    return 3;
  });
}

/**
 * \brief Implementation of map:get_entities_count().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::map_api_get_entities_count(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Map& map = *check_map(l, 1);
    const std::string& prefix = LuaTools::check_string(l, 2);

    const std::list<MapEntity*> entities =
        map.get_entities().get_entities_with_prefix(prefix);

    lua_pushinteger(l, entities.size());
    return 1;
  });
}

/**
 * \brief Implementation of map:has_entities().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::map_api_has_entities(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const Map& map = *check_map(l, 1);
    const std::string& prefix = LuaTools::check_string(l, 2);

    lua_pushboolean(l, map.get_entities().has_entity_with_prefix(prefix));
    return 1;
  });
}

/**
 * \brief Implementation of map:get_hero().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::map_api_get_hero(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Map& map = *check_map(l, 1);

    // Return the hero even if he is no longer on this map.
    push_hero(l, *map.get_game().get_hero());
    return 1;
  });
}

/**
 * \brief Implementation of map:set_entities_enabled().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::map_api_set_entities_enabled(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Map& map = *check_map(l, 1);
    const std::string& prefix = LuaTools::check_string(l, 2);
    bool enabled = LuaTools::opt_boolean(l, 3, true);

    std::list<MapEntity*> entities =
        map.get_entities().get_entities_with_prefix(prefix);
    for (auto it = entities.begin(); it != entities.end(); ++it) {
      (*it)->set_enabled(enabled);
    }

    return 0;
  });
}

/**
 * \brief Implementation of map:remove_entities().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::map_api_remove_entities(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Map& map = *check_map(l, 1);
    const std::string& prefix = LuaTools::check_string(l, 2);

    map.get_entities().remove_entities_with_prefix(prefix);
    return 0;
  });
}

namespace {

/**
 * \brief Utility function to check the size of an entity to be created.
 * \param l A Lua state.
 * \param index Index of the argument in the Lua stack.
 * \param width Width to check.
 * \param height Height to check.
 */
void entity_creation_check_size(
    lua_State* l,
    int index,
    int width,
    int height) {

  if (width < 0 || width % 8 != 0) {
    std::ostringstream oss;
    oss << "Invalid width " << width << ": should be a positive multiple of 8";
    LuaTools::arg_error(l, index, oss.str());
  }
  if (height < 0 || height % 8 != 0) {
    std::ostringstream oss;
    oss << "Invalid height " << height << ": should be a positive multiple of 8";
    LuaTools::arg_error(l, index, oss.str());
  }
}

}

/**
 * \brief Implementation of the tile creation function.
 *
 * Tiles cannot be created dynamically:
 * they can only be declared in the map data file.
 *
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::map_api_create_tile(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Map& map = get_entity_creation_map(l);

    // Should not happen: create_tile is not in the map metatable.
    Debug::check_assertion(!map.is_started(),
        "Cannot create a tile when the map is already started");

    LuaTools::check_type(l, 1, LUA_TTABLE);
    const Layer layer = LuaTools::check_layer_field(l, 1, "layer");
    const int x = LuaTools::check_int_field(l, 1, "x");
    const int y = LuaTools::check_int_field(l, 1, "y");
    const int width = LuaTools::check_int_field(l, 1, "width");
    const int height = LuaTools::check_int_field(l, 1, "height");
    const std::string& tile_pattern_id = LuaTools::check_string_field(l, 1, "pattern");

    entity_creation_check_size(l, 1, width, height);

    const TilePattern& pattern = map.get_tileset().get_tile_pattern(tile_pattern_id);

    for (int current_y = y; current_y < y + height; current_y += pattern.get_height()) {
      for (int current_x = x; current_x < x + width; current_x += pattern.get_width()) {
        MapEntityPtr entity = std::make_shared<Tile>(
            layer,
            current_x,
            current_y,
            pattern.get_width(),
            pattern.get_height(),
            map.get_tileset(),
            tile_pattern_id
        );
        map.get_entities().add_entity(entity);
      }
    }

    return 0;
  });
}

/**
 * \brief Implementation of map:create_destination().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::map_api_create_destination(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Map& map = get_entity_creation_map(l);
    LuaTools::check_type(l, 1, LUA_TTABLE);
    const std::string& name = LuaTools::opt_string_field(l, 1, "name", "");
    Layer layer = LuaTools::check_layer_field(l, 1, "layer");
    int x = LuaTools::check_int_field(l, 1, "x");
    int y = LuaTools::check_int_field(l, 1, "y");
    int direction = LuaTools::check_int_field(l, 1, "direction");
    const std::string& sprite_name = LuaTools::opt_string_field(l, 1, "sprite", "");
    bool is_default = LuaTools::opt_boolean_field(l, 1, "default", false);

    MapEntityPtr entity = std::make_shared<Destination>(
        name,
        layer,
        x,
        y,
        direction,
        sprite_name,
        is_default
    );
    map.get_entities().add_entity(entity);

    if (map.is_started()) {
      push_entity(l, *entity);
      return 1;
    }
    return 0;
  });
}

/**
 * \brief Implementation of map:create_teletransporter().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::map_api_create_teletransporter(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Map& map = get_entity_creation_map(l);
    LuaTools::check_type(l, 1, LUA_TTABLE);
    const std::string& name = LuaTools::opt_string_field(l, 1, "name", "");
    Layer layer = LuaTools::check_layer_field(l, 1, "layer");
    int x = LuaTools::check_int_field(l, 1, "x");
    int y = LuaTools::check_int_field(l, 1, "y");
    int width = LuaTools::check_int_field(l, 1, "width");
    int height = LuaTools::check_int_field(l, 1, "height");
    const std::string& sprite_name = LuaTools::opt_string_field(l, 1, "sprite", "");
    const std::string& sound_id = LuaTools::opt_string_field(l, 1, "sound", "");
    Transition::Style transition_style = LuaTools::opt_enum_field<Transition::Style>(
        l, 1, "transition", Transition::style_names, Transition::FADE);
    const std::string& destination_map_id = LuaTools::check_string_field(l, 1, "destination_map");
    const std::string& destination_name = LuaTools::opt_string_field(l, 1, "destination", "");

    entity_creation_check_size(l, 1, width, height);

    MapEntityPtr entity = std::make_shared<Teletransporter>(
        name,
        layer,
        x,
        y,
        width,
        height,
        sprite_name,
        sound_id,
        transition_style,
        destination_map_id,
        destination_name
    );
    map.get_entities().add_entity(entity);

    if (map.is_started()) {
      push_entity(l, *entity);
      return 1;
    }
    return 0;
  });
}

/**
 * \brief Implementation of map:create_pickable().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::map_api_create_pickable(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Map& map = get_entity_creation_map(l);
    LuaTools::check_type(l, 1, LUA_TTABLE);
    const std::string& name = LuaTools::opt_string_field(l, 1, "name", "");
    Layer layer = LuaTools::check_layer_field(l, 1, "layer");
    int x = LuaTools::check_int_field(l, 1, "x");
    int y = LuaTools::check_int_field(l, 1, "y");
    const std::string& treasure_name = LuaTools::opt_string_field(l, 1, "treasure_name", "");
    int treasure_variant = LuaTools::opt_int_field(l, 1, "treasure_variant", 1);
    const std::string& treasure_savegame_variable = LuaTools::opt_string_field(l, 1, "treasure_savegame_variable", "");

    if (!treasure_savegame_variable.empty()
        && !LuaTools::is_valid_lua_identifier(treasure_savegame_variable)) {
      LuaTools::arg_error(l, 1, std::string(
          "Bad field 'treasure_savegame_variable' (invalid savegame variable identifier: '")
      + treasure_savegame_variable + "')"
      );
    }

    bool force_persistent = false;
    FallingHeight falling_height = FALLING_MEDIUM;
    if (!map.is_loaded()) {
      // Different behavior when the pickable is already placed on the map.
      falling_height = FALLING_NONE;
      force_persistent = true;
    }

    Game& game = map.get_game();
    const std::shared_ptr<Pickable>& entity = Pickable::create(
        game,
        name,
        layer,
        x,
        y,
        Treasure(game, treasure_name, treasure_variant, treasure_savegame_variable),
        falling_height, force_persistent
    );

    if (entity == nullptr) {
      lua_pushnil(l);
      return 1;
    }

    map.get_entities().add_entity(entity);
    if (map.is_started()) {
      push_entity(l, *entity);
      return 1;
    }
    return 0;
  });
}

/**
 * \brief Implementation of map:create_destructible().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::map_api_create_destructible(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Map& map = get_entity_creation_map(l);
    LuaTools::check_type(l, 1, LUA_TTABLE);
    const std::string& name = LuaTools::opt_string_field(l, 1, "name", "");
    Layer layer = LuaTools::check_layer_field(l, 1, "layer");
    int x = LuaTools::check_int_field(l, 1, "x");
    int y = LuaTools::check_int_field(l, 1, "y");
    const std::string& treasure_name = LuaTools::opt_string_field(l, 1, "treasure_name", "");
    int treasure_variant = LuaTools::opt_int_field(l, 1, "treasure_variant", 1);
    const std::string& treasure_savegame_variable =
        LuaTools::opt_string_field(l, 1, "treasure_savegame_variable", "");
    const std::string& animation_set_id = LuaTools::check_string_field(l, 1, "sprite");
    Ground modified_ground = LuaTools::opt_enum_field<Ground>(
        l, 1, "ground", Tileset::ground_names, GROUND_WALL
    );
    const std::string& destruction_sound_id = LuaTools::opt_string_field(
        l, 1, "destruction_sound", ""
    );
    int weight = LuaTools::opt_int_field(l, 1, "weight", 0);
    int damage_on_enemies = LuaTools::opt_int_field(l, 1, "damage_on_enemies", 1);
    bool can_be_cut = LuaTools::opt_boolean_field(l, 1, "can_be_cut", false);
    bool can_explode = LuaTools::opt_boolean_field(l, 1, "can_explode", false);
    bool can_regenerate = LuaTools::opt_boolean_field(l, 1, "can_regenerate", false);

    if (!treasure_savegame_variable.empty()
        && !LuaTools::is_valid_lua_identifier(treasure_savegame_variable)) {
      LuaTools::arg_error(l, 1, std::string(
          "Bad field 'treasure_savegame_variable' (invalid savegame variable identifier: '")
      + treasure_savegame_variable + "'");
    }

    std::shared_ptr<Destructible> destructible = std::make_shared<Destructible>(
        name,
        layer,
        x,
        y,
        animation_set_id,
        Treasure(map.get_game(), treasure_name, treasure_variant, treasure_savegame_variable),
        modified_ground
    );
    destructible->set_destruction_sound(destruction_sound_id);
    destructible->set_weight(weight);
    destructible->set_can_be_cut(can_be_cut);
    destructible->set_can_explode(can_explode);
    destructible->set_can_regenerate(can_regenerate);
    destructible->set_damage_on_enemies(damage_on_enemies);

    map.get_entities().add_entity(destructible);

    if (map.is_started()) {
      push_entity(l, *destructible);
      return 1;
    }
    return 0;
  });
}

/**
 * \brief Implementation of map:create_chest().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::map_api_create_chest(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Map& map = get_entity_creation_map(l);
    LuaTools::check_type(l, 1, LUA_TTABLE);
    const std::string& name = LuaTools::opt_string_field(l, 1, "name", "");
    Layer layer = LuaTools::check_layer_field(l, 1, "layer");
    int x = LuaTools::check_int_field(l, 1, "x");
    int y = LuaTools::check_int_field(l, 1, "y");
    const std::string& treasure_name = LuaTools::opt_string_field(l, 1, "treasure_name", "");
    int treasure_variant = LuaTools::opt_int_field(l, 1, "treasure_variant", 1);
    const std::string& treasure_savegame_variable = LuaTools::opt_string_field(l, 1, "treasure_savegame_variable", "");
    const std::string& sprite_name = LuaTools::check_string_field(l, 1, "sprite");
    Chest::OpeningMethod opening_method = LuaTools::opt_enum_field<Chest::OpeningMethod>(l, 1, "opening_method",
        Chest::opening_method_names, Chest::OPENING_BY_INTERACTION);
    const std::string& opening_condition = LuaTools::opt_string_field(l, 1, "opening_condition", "");
    bool opening_condition_consumed = LuaTools::opt_boolean_field(l, 1, "opening_condition_consumed", false);
    const std::string& cannot_open_dialog_id = LuaTools::opt_string_field(l, 1, "cannot_open_dialog", "");

    if (!treasure_savegame_variable.empty()
        && !LuaTools::is_valid_lua_identifier(treasure_savegame_variable)) {
      LuaTools::arg_error(l, 1, std::string(
          "Bad field 'treasure_savegame_variable' (invalid savegame variable identifier '")
      + treasure_savegame_variable + "')"
      );
    }

    Game& game = map.get_game();

    if (opening_method == Chest::OPENING_BY_INTERACTION_IF_SAVEGAME_VARIABLE) {
      if (!LuaTools::is_valid_lua_identifier(opening_condition)) {
        LuaTools::arg_error(l, 1, std::string(
            "Bad field 'opening_condition' (invalid valid savegame variable identifier: '")
        + opening_condition + "')"
        );
      }
    }

    else if (opening_method == Chest::OPENING_BY_INTERACTION_IF_ITEM) {
      if (!opening_condition.empty() || !game.get_equipment().item_exists(opening_condition)) {
        LuaTools::arg_error(l, 1, std::string(
            "Bad field 'opening_condition' (no such equipment item: '")
        + opening_condition + "')"
        );
      }
      EquipmentItem& item = game.get_equipment().get_item(opening_condition);
      if (!item.is_saved()) {
        LuaTools::arg_error(l, 1, std::string(
            "Bad field 'opening_condition' (equipment item '")
        + opening_condition + "' is not saved)"
        );
      }
    }

    std::shared_ptr<Chest> chest = std::make_shared<Chest>(
        name,
        layer,
        x,
        y,
        sprite_name,
        Treasure(game, treasure_name, treasure_variant, treasure_savegame_variable)
    );
    chest->set_opening_method(opening_method);
    chest->set_opening_condition(opening_condition);
    chest->set_opening_condition_consumed(opening_condition_consumed);
    chest->set_cannot_open_dialog_id(cannot_open_dialog_id);
    map.get_entities().add_entity(chest);

    if (map.is_started()) {
      push_entity(l, *chest);
      return 1;
    }
    return 0;
  });
}

/**
 * \brief Implementation of map:create_jumper().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::map_api_create_jumper(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Map& map = get_entity_creation_map(l);
    LuaTools::check_type(l, 1, LUA_TTABLE);
    const std::string& name = LuaTools::opt_string_field(l, 1, "name", "");
    Layer layer = LuaTools::check_layer_field(l, 1, "layer");
    int x = LuaTools::check_int_field(l, 1, "x");
    int y = LuaTools::check_int_field(l, 1, "y");
    int width = LuaTools::check_int_field(l, 1, "width");
    int height = LuaTools::check_int_field(l, 1, "height");
    int direction = LuaTools::check_int_field(l, 1, "direction");
    int jump_length = LuaTools::check_int_field(l, 1, "jump_length");

    entity_creation_check_size(l, 1, width, height);

    MapEntityPtr entity = std::make_shared<Jumper>(
        name,
        layer,
        x,
        y,
        width,
        height,
        direction,
        jump_length
    );
    map.get_entities().add_entity(entity);

    if (map.is_started()) {
      push_entity(l, *entity);
      return 1;
    }
    return 0;
  });
}

/**
 * \brief Implementation of map:create_enemy().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::map_api_create_enemy(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Map& map = get_entity_creation_map(l);
    LuaTools::check_type(l, 1, LUA_TTABLE);
    const std::string& name = LuaTools::opt_string_field(l, 1, "name", "");
    Layer layer = LuaTools::check_layer_field(l, 1, "layer");
    int x = LuaTools::check_int_field(l, 1, "x");
    int y = LuaTools::check_int_field(l, 1, "y");
    int direction = LuaTools::check_int_field(l, 1, "direction");
    const std::string& breed = LuaTools::check_string_field(l, 1, "breed");
    Enemy::Rank rank = Enemy::Rank(LuaTools::opt_int_field(l, 1, "rank", 0));
    const std::string& savegame_variable = LuaTools::opt_string_field(l, 1, "savegame_variable", "");
    const std::string& treasure_name = LuaTools::opt_string_field(l, 1, "treasure_name", "");
    int treasure_variant = LuaTools::opt_int_field(l, 1, "treasure_variant", 1);
    const std::string& treasure_savegame_variable = LuaTools::opt_string_field(l, 1, "treasure_savegame_variable", "");

    if (!savegame_variable.empty()
        && !LuaTools::is_valid_lua_identifier(savegame_variable)) {
      LuaTools::arg_error(l, 1, std::string(
          "Bad field 'savegame_variable' (invalid savegame variable identifier: '")
      + savegame_variable + "')"
      );
    }

    if (!treasure_savegame_variable.empty()
        && !LuaTools::is_valid_lua_identifier(treasure_savegame_variable)) {
      LuaTools::arg_error(l, 1, std::string(
          "Bad field 'treasure_savegame_variable' (invalid savegame variable identifier '")
      + treasure_savegame_variable + "')"
      );
    }

    Game& game = map.get_game();
    MapEntityPtr entity = Enemy::create(
        game,
        breed,
        rank,
        savegame_variable,
        name,
        layer,
        x,
        y,
        direction,
        Treasure(game, treasure_name, treasure_variant, treasure_savegame_variable)
    );

    if (entity == nullptr) {
      lua_pushnil(l);
      return 1;
    }

    map.get_entities().add_entity(entity);
    if (map.is_started()) {
      push_entity(l, *entity);
      return 1;
    }
    return 0;
  });
}

/**
 * \brief Implementation of map:create_npc().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::map_api_create_npc(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Map& map = get_entity_creation_map(l);
    LuaTools::check_type(l, 1, LUA_TTABLE);
    const std::string& name = LuaTools::opt_string_field(l, 1, "name", "");
    Layer layer = LuaTools::check_layer_field(l, 1, "layer");
    int x = LuaTools::check_int_field(l, 1, "x");
    int y = LuaTools::check_int_field(l, 1, "y");
    int direction = LuaTools::check_int_field(l, 1, "direction");
    const std::string& subtype_name = LuaTools::check_string_field(l, 1, "subtype");
    const std::string& sprite_name = LuaTools::opt_string_field(l, 1, "sprite", "");
    const std::string& behavior = LuaTools::opt_string_field(l, 1, "behavior", "map");

    int subtype;
    std::istringstream iss(subtype_name);
    iss >> subtype;

    Game& game = map.get_game();
    MapEntityPtr entity = std::make_shared<Npc>(
        game,
        name,
        layer,
        x,
        y,
        Npc::Subtype(subtype),
        sprite_name,
        direction,
        behavior
    );
    map.get_entities().add_entity(entity);

    if (map.is_started()) {
      push_entity(l, *entity);
      return 1;
    }
    return 0;
  });
}

/**
 * \brief Implementation of map:create_block().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::map_api_create_block(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Map& map = get_entity_creation_map(l);
    LuaTools::check_type(l, 1, LUA_TTABLE);
    const std::string& name = LuaTools::opt_string_field(l, 1, "name", "");
    Layer layer = LuaTools::check_layer_field(l, 1, "layer");
    int x = LuaTools::check_int_field(l, 1, "x");
    int y = LuaTools::check_int_field(l, 1, "y");
    int direction = LuaTools::opt_int_field(l, 1, "direction", -1);
    const std::string& sprite_name = LuaTools::check_string_field(l, 1, "sprite");
    bool pushable = LuaTools::check_boolean_field(l, 1, "pushable");
    bool pullable = LuaTools::check_boolean_field(l, 1, "pullable");
    int maximum_moves = LuaTools::check_int_field(l, 1, "maximum_moves");

    if (maximum_moves < 0 || maximum_moves > 2) {
      std::ostringstream oss;
      oss << "Invalid maximum_moves: " << maximum_moves;
      LuaTools::arg_error(l, 1, oss.str());
    }

    std::shared_ptr<Block> entity = std::make_shared<Block>(
        name,
        layer,
        x,
        y,
        direction,
        sprite_name,
        pushable,
        pullable,
        maximum_moves
    );
    map.get_entities().add_entity(entity);

    if (map.is_started()) {
      push_entity(l, *entity);
      return 1;
    }
    return 0;
  });
}

/**
 * \brief Implementation of map:create_dynamic_tile().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::map_api_create_dynamic_tile(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Map& map = get_entity_creation_map(l);
    LuaTools::check_type(l, 1, LUA_TTABLE);
    const std::string& name = LuaTools::opt_string_field(l, 1, "name", "");
    Layer layer = LuaTools::check_layer_field(l, 1, "layer");
    int x = LuaTools::check_int_field(l, 1, "x");
    int y = LuaTools::check_int_field(l, 1, "y");
    int width = LuaTools::check_int_field(l, 1, "width");
    int height = LuaTools::check_int_field(l, 1, "height");
    const std::string& tile_pattern_id = LuaTools::check_string_field(l, 1, "pattern");
    bool enabled_at_start = LuaTools::check_boolean_field(l, 1, "enabled_at_start");

    entity_creation_check_size(l, 1, width, height);

    MapEntityPtr entity = std::make_shared<DynamicTile>(
        name,
        layer,
        x,
        y,
        width,
        height,
        map.get_tileset(),
        tile_pattern_id,
        enabled_at_start
    );
    map.get_entities().add_entity(entity);

    if (map.is_started()) {
      push_entity(l, *entity);
      return 1;
    }
    return 0;
  });
}

/**
 * \brief Implementation of map:create_switch().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::map_api_create_switch(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Map& map = get_entity_creation_map(l);
    LuaTools::check_type(l, 1, LUA_TTABLE);
    const std::string& name = LuaTools::opt_string_field(l, 1, "name", "");
    Layer layer = LuaTools::check_layer_field(l, 1, "layer");
    int x = LuaTools::check_int_field(l, 1, "x");
    int y = LuaTools::check_int_field(l, 1, "y");
    Switch::Subtype subtype = LuaTools::check_enum_field<Switch::Subtype>(
        l, 1, "subtype", Switch::subtype_names
    );
    const std::string& sprite_name = LuaTools::opt_string_field(l, 1, "sprite", "");
    const std::string& sound_id = LuaTools::opt_string_field(l, 1, "sound", "");
    bool needs_block = LuaTools::check_boolean_field(l, 1, "needs_block");
    bool inactivate_when_leaving = LuaTools::check_boolean_field(l, 1, "inactivate_when_leaving");

    MapEntityPtr entity = std::make_shared<Switch>(
        name,
        layer,
        x,
        y,
        subtype,
        sprite_name,
        sound_id,
        needs_block,
        inactivate_when_leaving
    );
    map.get_entities().add_entity(entity);

    if (map.is_started()) {
      push_entity(l, *entity);
      return 1;
    }
    return 0;
  });
}

/**
 * \brief Implementation of map:create_wall().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::map_api_create_wall(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Map& map = get_entity_creation_map(l);
    LuaTools::check_type(l, 1, LUA_TTABLE);
    const std::string& name = LuaTools::opt_string_field(l, 1, "name", "");
    Layer layer = LuaTools::check_layer_field(l, 1, "layer");
    int x = LuaTools::check_int_field(l, 1, "x");
    int y = LuaTools::check_int_field(l, 1, "y");
    int width = LuaTools::check_int_field(l, 1, "width");
    int height = LuaTools::check_int_field(l, 1, "height");
    bool stops_hero = LuaTools::opt_boolean_field(l, 1, "stops_hero", false);
    bool stops_enemies = LuaTools::opt_boolean_field(l, 1, "stops_enemies", false);
    bool stops_npcs = LuaTools::opt_boolean_field(l, 1, "stops_npcs", false);
    bool stops_blocks = LuaTools::opt_boolean_field(l, 1, "stops_blocks", false);
    bool stops_projectiles = LuaTools::opt_boolean_field(l, 1, "stops_projectiles", false);

    entity_creation_check_size(l, 1, width, height);

    MapEntityPtr entity = std::make_shared<Wall>(
        name,
        layer,
        x,
        y,
        width,
        height,
        stops_hero,
        stops_enemies,
        stops_npcs,
        stops_blocks,
        stops_projectiles
    );
    map.get_entities().add_entity(entity);

    if (map.is_started()) {
      push_entity(l, *entity);
      return 1;
    }
    return 0;
  });
}

/**
 * \brief Implementation of map:create_sensor().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::map_api_create_sensor(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Map& map = get_entity_creation_map(l);
    LuaTools::check_type(l, 1, LUA_TTABLE);
    const std::string& name = LuaTools::opt_string_field(l, 1, "name", "");
    Layer layer = LuaTools::check_layer_field(l, 1, "layer");
    int x = LuaTools::check_int_field(l, 1, "x");
    int y = LuaTools::check_int_field(l, 1, "y");
    int width = LuaTools::check_int_field(l, 1, "width");
    int height = LuaTools::check_int_field(l, 1, "height");

    entity_creation_check_size(l, 1, width, height);

    MapEntityPtr entity = std::make_shared<Sensor>(
        name,
        layer,
        x,
        y,
        width,
        height
    );
    map.get_entities().add_entity(entity);

    if (map.is_started()) {
      push_entity(l, *entity);
      return 1;
    }
    return 0;
  });
}

/**
 * \brief Implementation of map:create_crystal().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::map_api_create_crystal(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Map& map = get_entity_creation_map(l);
    LuaTools::check_type(l, 1, LUA_TTABLE);
    const std::string& name = LuaTools::opt_string_field(l, 1, "name", "");
    Layer layer = LuaTools::check_layer_field(l, 1, "layer");
    int x = LuaTools::check_int_field(l, 1, "x");
    int y = LuaTools::check_int_field(l, 1, "y");

    MapEntityPtr entity = std::make_shared<Crystal>(
        name,
        layer,
        x,
        y
    );
    map.get_entities().add_entity(entity);

    if (map.is_started()) {
      push_entity(l, *entity);
      return 1;
    }
    return 0;
  });
}

/**
 * \brief Implementation of map:create_crystal_block().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::map_api_create_crystal_block(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Map& map = get_entity_creation_map(l);
    LuaTools::check_type(l, 1, LUA_TTABLE);
    const std::string& name = LuaTools::opt_string_field(l, 1, "name", "");
    Layer layer = LuaTools::check_layer_field(l, 1, "layer");
    int x = LuaTools::check_int_field(l, 1, "x");
    int y = LuaTools::check_int_field(l, 1, "y");
    int width = LuaTools::check_int_field(l, 1, "width");
    int height = LuaTools::check_int_field(l, 1, "height");
    const std::string& subtype_name = LuaTools::check_string_field(l, 1, "subtype");

    entity_creation_check_size(l, 1, width, height);

    int subtype;
    std::istringstream iss(subtype_name);
    iss >> subtype;

    Game& game = map.get_game();
    MapEntityPtr entity = std::make_shared<CrystalBlock>(
        game,
        name,
        layer,
        x,
        y,
        width,
        height,
        CrystalBlock::Subtype(subtype)
    );
    map.get_entities().add_entity(entity);

    if (map.is_started()) {
      push_entity(l, *entity);
      return 1;
    }
    return 0;
  });
}

/**
 * \brief Implementation of map:create_shop_treasure().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::map_api_create_shop_treasure(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Map& map = get_entity_creation_map(l);
    LuaTools::check_type(l, 1, LUA_TTABLE);
    const std::string& name = LuaTools::opt_string_field(l, 1, "name", "");
    Layer layer = LuaTools::check_layer_field(l, 1, "layer");
    int x = LuaTools::check_int_field(l, 1, "x");
    int y = LuaTools::check_int_field(l, 1, "y");
    const std::string& treasure_name = LuaTools::check_string_field(l, 1, "treasure_name");
    int treasure_variant = LuaTools::opt_int_field(l, 1, "treasure_variant", 1);
    const std::string& treasure_savegame_variable = LuaTools::opt_string_field(l, 1, "treasure_savegame_variable", "");
    int price = LuaTools::check_int_field(l, 1, "price");
    const std::string& dialog_id = LuaTools::check_string_field(l, 1, "dialog");

    if (!treasure_savegame_variable.empty()
        && !LuaTools::is_valid_lua_identifier(treasure_savegame_variable)) {
      LuaTools::arg_error(l, 1, std::string(
          "Bad field 'treasure_savegame_variable' (invalid savegame variable identifier: '")
      + treasure_savegame_variable + "')"
      );
    }

    Game& game = map.get_game();
    MapEntityPtr entity = ShopTreasure::create(
        game,
        name,
        layer,
        x,
        y,
        Treasure(game, treasure_name, treasure_variant, treasure_savegame_variable),
        price,
        dialog_id
    );

    if (entity == nullptr) {
      lua_pushnil(l);
      return 1;
    }

    map.get_entities().add_entity(entity);
    if (map.is_started()) {
      push_entity(l, *entity);
      return 1;
    }
    return 0;
  });
}

/**
 * \brief Implementation of map:create_stream().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::map_api_create_stream(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Map& map = get_entity_creation_map(l);
    LuaTools::check_type(l, 1, LUA_TTABLE);
    const std::string& name = LuaTools::opt_string_field(l, 1, "name", "");
    Layer layer = LuaTools::check_layer_field(l, 1, "layer");
    int x = LuaTools::check_int_field(l, 1, "x");
    int y = LuaTools::check_int_field(l, 1, "y");
    int direction = LuaTools::check_int_field(l, 1, "direction");
    const std::string& sprite_name = LuaTools::opt_string_field(l, 1, "sprite", "");
    int speed = LuaTools::opt_int_field(l, 1, "speed", 64);
    bool allow_movement = LuaTools::opt_boolean_field(l, 1, "allow_movement", true);
    bool allow_attack = LuaTools::opt_boolean_field(l, 1, "allow_attack", true);
    bool allow_item = LuaTools::opt_boolean_field(l, 1, "allow_item", true);

    std::shared_ptr<Stream> stream = std::make_shared<Stream>(
        name,
        layer,
        x,
        y,
        direction,
        sprite_name
    );
    stream->set_speed(speed);
    stream->set_allow_movement(allow_movement);
    stream->set_allow_attack(allow_attack);
    stream->set_allow_item(allow_item);
    map.get_entities().add_entity(stream);

    if (map.is_started()) {
      push_stream(l, *stream);
      return 1;
    }
    return 0;
  });
}

/**
 * \brief Implementation of map:create_door().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::map_api_create_door(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Map& map = get_entity_creation_map(l);
    LuaTools::check_type(l, 1, LUA_TTABLE);
    const std::string& name = LuaTools::opt_string_field(l, 1, "name", "");
    Layer layer = LuaTools::check_layer_field(l, 1, "layer");
    int x = LuaTools::check_int_field(l, 1, "x");
    int y = LuaTools::check_int_field(l, 1, "y");
    int direction = LuaTools::check_int_field(l, 1, "direction");
    const std::string& sprite_name = LuaTools::check_string_field(l, 1, "sprite");
    const std::string& savegame_variable = LuaTools::opt_string_field(l, 1, "savegame_variable", "");
    Door::OpeningMethod opening_method = LuaTools::opt_enum_field<Door::OpeningMethod>(l, 1, "opening_method",
        Door::opening_method_names, Door::OPENING_NONE);
    const std::string& opening_condition = LuaTools::opt_string_field(l, 1, "opening_condition", "");
    bool opening_condition_consumed = LuaTools::opt_boolean_field(l, 1, "opening_condition_consumed", false);
    const std::string& cannot_open_dialog_id = LuaTools::opt_string_field(l, 1, "cannot_open_dialog", "");

    Game& game = map.get_game();

    if (opening_method == Door::OPENING_BY_INTERACTION_IF_SAVEGAME_VARIABLE) {
      if (!LuaTools::is_valid_lua_identifier(opening_condition)) {
        LuaTools::arg_error(l, 1, std::string(
            "Bad field 'opening_condition' (invalid savegame variable identifier: '")
        + opening_condition + "')"
        );
      }
    }

    else if (opening_method == Door::OPENING_BY_INTERACTION_IF_ITEM) {
      if (opening_condition.empty() || !game.get_equipment().item_exists(opening_condition)) {
        LuaTools::arg_error(l, 1, std::string(
            "Bad field 'opening_condition' (no such equipment item: '")
        + opening_condition + "')"
        );
      }
      EquipmentItem& item = game.get_equipment().get_item(opening_condition);
      if (!item.is_saved()) {
        LuaTools::arg_error(l, 1, std::string(
            "Bad field 'opening_condition' (equipment item '")
        + opening_condition + "' is not saved)"
        );
      }
    }

    std::shared_ptr<Door> door = std::make_shared<Door>(
        game,
        name,
        layer,
        x,
        y,
        direction,
        sprite_name,
        savegame_variable
    );
    door->set_opening_method(opening_method);
    door->set_opening_condition(opening_condition);
    door->set_opening_condition_consumed(opening_condition_consumed);
    door->set_cannot_open_dialog_id(cannot_open_dialog_id);
    map.get_entities().add_entity(door);

    if (map.is_started()) {
      push_entity(l, *door);
      return 1;
    }
    return 0;
  });
}

/**
 * \brief Implementation of map:create_stairs().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::map_api_create_stairs(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Map& map = get_entity_creation_map(l);
    LuaTools::check_type(l, 1, LUA_TTABLE);
    const std::string& name = LuaTools::opt_string_field(l, 1, "name", "");
    Layer layer = LuaTools::check_layer_field(l, 1, "layer");
    int x = LuaTools::check_int_field(l, 1, "x");
    int y = LuaTools::check_int_field(l, 1, "y");
    int direction = LuaTools::check_int_field(l, 1, "direction");
    const std::string& subtype_name = LuaTools::check_string_field(l, 1, "subtype");

    int subtype;
    std::istringstream iss(subtype_name);
    iss >> subtype;

    MapEntityPtr entity = std::make_shared<Stairs>(
        name,
        layer,
        x,
        y,
        direction,
        Stairs::Subtype(subtype)
    );
    map.get_entities().add_entity(entity);

    if (map.is_started()) {
      push_entity(l, *entity);
      return 1;
    }
    return 0;
  });
}

/**
 * \brief Implementation of map:create_separator().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::map_api_create_separator(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Map& map = get_entity_creation_map(l);
    LuaTools::check_type(l, 1, LUA_TTABLE);
    const std::string& name = LuaTools::opt_string_field(l, 1, "name", "");
    Layer layer = LuaTools::check_layer_field(l, 1, "layer");
    int x = LuaTools::check_int_field(l, 1, "x");
    int y = LuaTools::check_int_field(l, 1, "y");
    int width = LuaTools::check_int_field(l, 1, "width");
    int height = LuaTools::check_int_field(l, 1, "height");

    entity_creation_check_size(l, 1, width, height);

    MapEntityPtr entity = std::make_shared<Separator>(
        name,
        layer,
        x,
        y,
        width,
        height
    );
    map.get_entities().add_entity(entity);

    if (map.is_started()) {
      push_entity(l, *entity);
      return 1;
    }
    return 0;
  });
}

/**
 * \brief Implementation of map:create_custom_entity().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::map_api_create_custom_entity(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Map& map = get_entity_creation_map(l);
    LuaTools::check_type(l, 1, LUA_TTABLE);
    const std::string& name = LuaTools::opt_string_field(l, 1, "name", "");
    int direction = LuaTools::check_int_field(l, 1, "direction");
    Layer layer = LuaTools::check_layer_field(l, 1, "layer");
    int x = LuaTools::check_int_field(l, 1, "x");
    int y = LuaTools::check_int_field(l, 1, "y");
    int width = LuaTools::opt_int_field(l, 1, "width", 16);
    int height = LuaTools::opt_int_field(l, 1, "height", 16);
    const std::string& sprite_name = LuaTools::opt_string_field(l, 1, "sprite", "");
    const std::string& model = LuaTools::opt_string_field(l, 1, "model", "");

    entity_creation_check_size(l, 1, width, height);

    Game& game = map.get_game();
    MapEntityPtr entity = std::make_shared<CustomEntity>(
        game,
        name,
        direction,
        layer,
        x,
        y,
        width,
        height,
        sprite_name,
        model
    );

    map.get_entities().add_entity(entity);
    if (map.is_started()) {
      push_entity(l, *entity);
      return 1;
    }
    return 0;
  });
}

/**
 * \brief Implementation of map:create_bomb().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::map_api_create_bomb(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Map& map = get_entity_creation_map(l);
    LuaTools::check_type(l, 1, LUA_TTABLE);
    const std::string& name = LuaTools::opt_string_field(l, 1, "name", "");
    Layer layer = LuaTools::check_layer_field(l, 1, "layer");
    int x = LuaTools::check_int_field(l, 1, "x");
    int y = LuaTools::check_int_field(l, 1, "y");

    MapEntityPtr entity = std::make_shared<Bomb>(
        name,
        layer,
        x,
        y
    );
    map.get_entities().add_entity(entity);

    if (map.is_started()) {
      push_entity(l, *entity);
      return 1;
    }
    return 0;
  });
}

/**
 * \brief Implementation of map:create_explosion().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::map_api_create_explosion(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Map& map = get_entity_creation_map(l);
    LuaTools::check_type(l, 1, LUA_TTABLE);
    const std::string& name = LuaTools::opt_string_field(l, 1, "name", "");
    Layer layer = LuaTools::check_layer_field(l, 1, "layer");
    int x = LuaTools::check_int_field(l, 1, "x");
    int y = LuaTools::check_int_field(l, 1, "y");

    MapEntityPtr entity = std::make_shared<Explosion>(
        name,
        layer,
        Point(x, y),
        true
    );
    map.get_entities().add_entity(entity);

    if (map.is_started()) {
      push_entity(l, *entity);
      return 1;
    }
    return 0;
  });
}

/**
 * \brief Implementation of map:create_fire().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::map_api_create_fire(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Map& map = get_entity_creation_map(l);
    LuaTools::check_type(l, 1, LUA_TTABLE);
    const std::string& name = LuaTools::opt_string_field(l, 1, "name", "");
    Layer layer = LuaTools::check_layer_field(l, 1, "layer");
    int x = LuaTools::check_int_field(l, 1, "x");
    int y = LuaTools::check_int_field(l, 1, "y");

    MapEntityPtr entity = std::make_shared<Fire>(
        name,
        layer,
        Point(x, y)
    );
    map.get_entities().add_entity(entity);

    if (map.is_started()) {
      push_entity(l, *entity);
      return 1;
    }
    return 0;
  });
}

/**
 * \brief Calls the on_started() method of a Lua map.
 *
 * Does nothing if the method is not defined.
 *
 * \param map A map.
 * \param destination The destination point used (nullptr if it's a special one).
 */
void LuaContext::map_on_started(Map& map, Destination* destination) {

  if (!userdata_has_field(map, "on_started")) {
    return;
  }

  push_map(l, map);
  on_started(destination);
  lua_pop(l, 1);
}

/**
 * \brief Calls the on_finished() method of a Lua map if it is defined.
 *
 * Also stops timers and menus associated to the map.
 *
 * \param map A map.
 */
void LuaContext::map_on_finished(Map& map) {

  push_map(l, map);
  if (userdata_has_field(map, "on_finished")) {
    on_finished();
  }
  remove_timers(-1);  // Stop timers and menus associated to this map.
  remove_menus(-1);
  lua_pop(l, 1);
}

/**
 * \brief Calls the on_update() method of a Lua map.
 *
 * Also calls the method on its menus.
 *
 * \param map A map.
 */
void LuaContext::map_on_update(Map& map) {

  push_map(l, map);
  // This particular method is tried so often that we want to save optimize
  // the std::string construction.
  static const std::string method_name = "on_update";
  if (userdata_has_field(map, method_name)) {
    on_update();
  }
  menus_on_update(-1);
  lua_pop(l, 1);
}

/**
 * \brief Calls the on_draw() method of a Lua map if it is defined.
 *
 * Also calls the method on its menus.
 *
 * \param map A map.
 * \param dst_surface The destination surface.
 */
void LuaContext::map_on_draw(Map& map, const SurfacePtr& dst_surface) {

  push_map(l, map);
  if (userdata_has_field(map, "on_draw")) {
    on_draw(dst_surface);
  }
  menus_on_draw(-1, dst_surface);
  lua_pop(l, 1);
}

/**
 * \brief Notifies a Lua map that an input event has just occurred.
 *
 * The appropriate callback in the map is triggered if it exists.
 * Also notifies the menus of the game if the game itself does not handle the
 * event.
 *
 * \param event The input event to handle.
 * \param map A map.
 * \return \c true if the event was handled and should stop being propagated.
 */
bool LuaContext::map_on_input(Map& map, const InputEvent& event) {

  push_map(l, map);
  bool handled = on_input(event);
  if (!handled) {
    handled = menus_on_input(-1, event);
  }
  lua_pop(l, 1);
  return handled;
}

/**
 * \brief Calls the on_command_pressed() method of a Lua map.
 *
 * Also notifies the menus of the game if the game itself does not handle the
 * event.
 *
 * \param map A map.
 * \param command The command pressed.
 * \return \c true if the event was handled and should stop being propagated.
 */
bool LuaContext::map_on_command_pressed(Map& map, GameCommands::Command command) {

  bool handled = false;
  push_map(l, map);
  if (userdata_has_field(map, "on_command_pressed")) {
    handled = on_command_pressed(command);
  }
  if (!handled) {
    handled = menus_on_command_pressed(-1, command);
  }
  lua_pop(l, 1);
  return handled;
}

/**
 * \brief Calls the on_command_released() method of a Lua map.
 *
 * Also notifies the menus of the game if the game itself does not handle the
 * event.
 *
 * \param map A map.
 * \param command The command released.
 * \return \c true if the event was handled and should stop being propagated.
 */
bool LuaContext::map_on_command_released(Map& map, GameCommands::Command command) {

  bool handled = false;
  push_map(l, map);
  if (userdata_has_field(map, "on_command_released")) {
    handled = on_command_released(command);
  }
  if (!handled) {
    handled = menus_on_command_released(-1, command);
  }
  lua_pop(l, 1);
  return handled;
}

/**
 * \brief Calls the on_suspended() method of a Lua map.
 *
 * Does nothing if the method is not defined.
 *
 * \param map A map.
 * \param suspended true if the map is suspended.
 */
void LuaContext::map_on_suspended(Map& map, bool suspended) {

  if (!userdata_has_field(map, "on_suspended")) {
    return;
  }

  push_map(l, map);
  on_suspended(suspended);
  lua_pop(l, 1);
}

/**
 * \brief Calls the on_opening_transition_finished() method of a Lua map.
 *
 * Does nothing if the method is not defined.
 *
 * \param map A map.
 * \param destination The destination point used (nullptr if it's a special one).
 */
void LuaContext::map_on_opening_transition_finished(Map& map,
    Destination* destination) {

  if (!userdata_has_field(map, "on_opening_transition_finished")) {
    //return;
  }

  push_map(l, map);
  on_opening_transition_finished(destination);
  lua_pop(l, 1);
}

/**
 * \brief Calls the on_camera_back() method of a Lua map.
 *
 * Does nothing if the method is not defined.
 *
 * \param map A map.
 */
void LuaContext::map_on_camera_back(Map& map) {

  if (!userdata_has_field(map, "on_camera_back")) {
    return;
  }

  push_map(l, map);
  on_camera_back();
  lua_pop(l, 1);
}

/**
 * \brief Calls the on_obtaining_treasure() method of a Lua map.
 *
 * Does nothing if the method is not defined.
 *
 * \param map A map.
 * \param treasure A treasure the hero is about to obtain on that map.
 */
void LuaContext::map_on_obtaining_treasure(Map& map, const Treasure& treasure) {

  if (!userdata_has_field(map, "on_obtaining_treasure")) {
    return;
  }

  push_map(l, map);
  on_obtaining_treasure(treasure);
  lua_pop(l, 1);
}

/**
 * \brief Calls the on_obtained_treasure() method of a Lua map.
 *
 * Does nothing if the method is not defined.
 *
 * \param map A map.
 * \param treasure The treasure just obtained.
 */
void LuaContext::map_on_obtained_treasure(Map& map, const Treasure& treasure) {

  if (!userdata_has_field(map, "on_obtained_treasure")) {
    return;
  }

  push_map(l, map);
  on_obtained_treasure(treasure);
  lua_pop(l, 1);
}

}

