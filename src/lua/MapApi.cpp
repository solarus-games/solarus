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
#include "solarus/entities/GroundInfo.h"
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
      { nullptr, nullptr }
  };

  static const luaL_Reg metamethods[] = {
      { "__gc", userdata_meta_gc },
      { "__newindex", userdata_meta_newindex_as_table },
      { "__index", userdata_meta_index_as_table },
      { nullptr, nullptr }
  };

  register_type(map_module_name, nullptr, methods, metamethods);

  // Add map:create_* functions as closures because we pass the entity type as upvalue.
  luaL_getmetatable(l, map_module_name.c_str());
  const std::vector<EntityType> entity_types_in_api = {
      EntityType::DESTINATION,
      EntityType::TELETRANSPORTER,
      EntityType::PICKABLE,
      EntityType::DESTRUCTIBLE,
      EntityType::CHEST,
      EntityType::JUMPER,
      EntityType::ENEMY,
      EntityType::NPC,
      EntityType::BLOCK,
      EntityType::DYNAMIC_TILE,
      EntityType::SWITCH,
      EntityType::WALL,
      EntityType::SENSOR,
      EntityType::CRYSTAL,
      EntityType::CRYSTAL_BLOCK,
      EntityType::SHOP_TREASURE,
      EntityType::STREAM,
      EntityType::DOOR,
      EntityType::STAIRS,
      EntityType::SEPARATOR,
      EntityType::CUSTOM,
      EntityType::BOMB,
      EntityType::EXPLOSION,
      EntityType::FIRE
  };
  for (EntityType type : entity_types_in_api) {
    const std::string& type_name = MapEntity::get_entity_type_name(type);
    std::string function_name = "create_" + type_name;
    push_string(l, type_name);
    lua_pushcclosure(l, map_api_create_entity, 1);
    lua_setfield(l, -2, function_name.c_str());
  }
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
 * \brief Creates a tile on the map.
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::l_create_tile(lua_State* l) {

  Map& map = *check_map(l, 1);
  EntityData& data = *(static_cast<EntityData*>(lua_touserdata(l, 2)));
  const int x = data.get_xy().x;
  const int y = data.get_xy().y;
  const int width = data.get_integer("width");
  const int height = data.get_integer("height");
  const std::string& tile_pattern_id = data.get_string("pattern");

  entity_creation_check_size(l, 1, width, height);

  const TilePattern& pattern = map.get_tileset().get_tile_pattern(tile_pattern_id);

  for (int current_y = y; current_y < y + height; current_y += pattern.get_height()) {
    for (int current_x = x; current_x < x + width; current_x += pattern.get_width()) {
      MapEntityPtr entity = std::make_shared<Tile>(
          data.get_layer(),
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
}

/**
 * \brief Creates a destination on the map.
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::l_create_destination(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Map& map = *check_map(l, 1);
    EntityData& data = *(static_cast<EntityData*>(lua_touserdata(l, 2)));

    MapEntityPtr entity = std::make_shared<Destination>(
        data.get_name(),
        data.get_layer(),
        data.get_xy().x,
        data.get_xy().y,
        data.get_integer("direction"),
        data.get_string("sprite"),
        data.get_boolean("default")
    );
    map.get_entities().add_entity(entity);

    if (map.is_started()) {
      push_entity(l, *entity);
      return 1;
    }
    return 0;
  });
}

const std::map<EntityType, lua_CFunction> LuaContext::entity_creation_functions = {
    { EntityType::TILE, LuaContext::l_create_tile },
    { EntityType::DESTINATION, LuaContext::l_create_destination }
    // TODO
};

/**
 * \brief Creates on the current map an entity from the specified data.
 * \param map The map where to create an entity.
 * \param Description of the entity to create.
 */
void LuaContext::create_map_entity_from_data(Map& map, const EntityData& entity_data) {

  const std::string& type_name = MapEntity::get_entity_type_name(entity_data.get_type());
  std::string function_name = "create_" + type_name;
  const auto& it = entity_creation_functions.find(entity_data.get_type());
  if (it == entity_creation_functions.end()) {
    LuaTools::error(l, "Missing entity creation function for type '" + type_name + "'");
  }
  lua_CFunction function = it->second;

  lua_pushcfunction(l, function);
  push_map(l, map);
  lua_pushlightuserdata(l, const_cast<EntityData*>(&entity_data));
  call_function(2, 0, function_name.c_str());

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

    push_string(l, GroundInfo::get_ground_name(ground));
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
    std::list<MapEntity*> doors = entities.get_entities_with_prefix(EntityType::DOOR, prefix);
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
    std::list<MapEntity*> doors = entities.get_entities_with_prefix(EntityType::DOOR, prefix);
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
    std::list<MapEntity*> doors = entities.get_entities_with_prefix(EntityType::DOOR, prefix);
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

/**
 * \brief Implementation of all entity creation functions: map_api_create_*.
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::map_api_create_entity(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {

    EntityType type = LuaTools::check_enum<EntityType>(
        l, lua_upvalueindex(1), MapEntity::entity_type_names
    );
    Map& map = *check_map(l, 1);
    const EntityData& data = EntityData::check_entity_data(l, 1, type);

    get_lua_context(l).create_map_entity_from_data(map, data);

    return 1;
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
bool LuaContext::map_on_command_pressed(Map& map, GameCommand command) {

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
bool LuaContext::map_on_command_released(Map& map, GameCommand command) {

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

