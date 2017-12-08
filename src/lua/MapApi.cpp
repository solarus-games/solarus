/*
 * Copyright (C) 2006-2016 Christopho, Solarus - http://www.solarus-games.org
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
#include "solarus/entities/Entities.h"
#include "solarus/entities/EntityTypeInfo.h"
#include "solarus/entities/Explosion.h"
#include "solarus/entities/Fire.h"
#include "solarus/entities/GroundInfo.h"
#include "solarus/entities/Hero.h"
#include "solarus/entities/Jumper.h"
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

namespace {

/**
 * \brief Lua equivalent of the deprecated map:move_camera() function.
 */
const char* move_camera_code =
"local map, x, y, speed, callback, delay_before, delay_after = ...\n"
"local camera = map:get_camera()\n"
"local game = map:get_game()\n"
"local hero = map:get_hero()\n"
"delay_before = delay_before or 1000\n"
"delay_after = delay_after or 1000\n"
"local back_x, back_y = camera:get_position_to_track(hero)\n"
"game:set_suspended(true)\n"
"camera:start_manual()\n"
"local movement = sol.movement.create(\"target\")\n"
"movement:set_target(camera:get_position_to_track(x, y))\n"
"movement:set_ignore_obstacles(true)\n"
"movement:set_speed(speed)\n"
"movement:start(camera, function()\n"
"  local timer_1 = sol.timer.start(map, delay_before, function()\n"
"    callback()\n"
"    local timer_2 = sol.timer.start(map, delay_after, function()\n"
"      local movement = sol.movement.create(\"target\")\n"
"      movement:set_target(back_x, back_y)\n"
"      movement:set_ignore_obstacles(true)\n"
"      movement:set_speed(speed)\n"
"      movement:start(camera, function()\n"
"        game:set_suspended(false)\n"
"        camera:start_tracking(hero)\n"
"        if map.on_camera_back ~= nil then\n"
"          map:on_camera_back()\n"
"        end\n"
"      end)\n"
"    end)\n"
"    timer_2:set_suspended_with_map(false)\n"
"  end)\n"
"  timer_1:set_suspended_with_map(false)\n"
"end)\n";

}  // Anonymous namespace.

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
      { "set_world", map_api_set_world },
      { "get_min_layer", map_api_get_min_layer },
      { "get_max_layer", map_api_get_max_layer },
      { "get_size", map_api_get_size },
      { "get_location", map_api_get_location },
      { "get_floor", map_api_get_floor },
      { "set_floor", map_api_set_floor },
      { "get_tileset", map_api_get_tileset },
      { "set_tileset", map_api_set_tileset },
      { "get_music", map_api_get_music },
      { "get_camera", map_api_get_camera },
      { "get_camera_position", map_api_get_camera_position },
      { "move_camera", map_api_move_camera },
      { "get_ground", map_api_get_ground },
      { "draw_visual", map_api_draw_visual },
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
      { "get_entities_by_type", map_api_get_entities_by_type },
      { "get_entities_in_rectangle", map_api_get_entities_in_rectangle },
      { "get_entities_in_region", map_api_get_entities_in_region },
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
  for (const auto& kvp : EnumInfoTraits<EntityType>::names) {
    EntityType type = kvp.first;
    const std::string& type_name = kvp.second;
    if (!EntityTypeInfo::can_be_created_from_lua_api(type)) {
      continue;
    }
    std::string function_name = "create_" + type_name;
    push_string(l, type_name);
    lua_pushcclosure(l, map_api_create_entity, 1);
    lua_setfield(l, -2, function_name.c_str());
  }

  // Add a Lua implementation of the deprecated map:move_camera() function.
  int result = luaL_loadstring(l, move_camera_code);
  if (result != 0) {
    Debug::error(std::string("Failed to initialize map:move_camera(): ") + lua_tostring(l, -1));
    lua_pop(l, 1);
  }
  else {
    Debug::check_assertion(lua_isfunction(l, -1), "map:move_camera() is not a function");
    lua_setfield(l, LUA_REGISTRYINDEX, "map.move_camera");
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
 * \brief Checks and returns the layer of an entity to be created.
 *
 * The layer is assumed to be specified in a field "layer".
 * Throws a LuaException if the layer is invalid for the map.
 *
 * \param l A Lua state.
 * \param index Index of the argument in the Lua stack.
 * \param entity_data Description of the entity to create.
 * \param map The map where to create the entity.
 * \return The layer.
 */
int entity_creation_check_layer(
    lua_State* l,
    int index,
    const EntityData& entity_data,
    const Map& map) {

  const int layer = entity_data.get_layer();

  if (!map.is_valid_layer(layer)) {
    std::ostringstream oss;
    oss << "Invalid layer: " << layer;
    LuaTools::arg_error(l, index, oss.str());
  }

  return layer;
}

/**
 * \brief Checks and returns the size of an entity to be created.
 *
 * The size is assumed to be specified in fields "width" and "height".
 * Throws a LuaException if the size is invalid.
 *
 * \param l A Lua state.
 * \param index Index of the argument in the Lua stack.
 * \param entity_data Description of the entity to create.
 * \return The size.
 */
Size entity_creation_check_size(
    lua_State* l,
    int index,
    const EntityData& entity_data) {

  const Size size = {
      entity_data.get_integer("width"),
      entity_data.get_integer("height")
  };

  if (size.width < 0 || size.width % 8 != 0) {
    std::ostringstream oss;
    oss << "Invalid width " << size.width << ": should be a positive multiple of 8";
    LuaTools::arg_error(l, index, oss.str());
  }
  if (size.height < 0 || size.height % 8 != 0) {
    std::ostringstream oss;
    oss << "Invalid height " << size.height << ": should be a positive multiple of 8";
    LuaTools::arg_error(l, index, oss.str());
  }

  return size;
}

/**
 * \brief Checks and returns a mandatory savegame variable field for an
 * entity to be created.
 *
 * Throws a LuaException if the savegame variable name is invalid or empty.
 *
 * \param l A Lua state.
 * \param index Index of the argument in the Lua stack.
 * \param entity_data Description of the entity to create.
 * \param field_name Name of the savegame variable field to check.
 * \return The savegame variable name.
 */
std::string entity_creation_check_savegame_variable_mandatory(
    lua_State* l,
    int index,
    const EntityData& entity_data,
    const std::string& field_name
) {

  const std::string& savegame_variable = entity_data.get_string(field_name);

  if (!LuaTools::is_valid_lua_identifier(savegame_variable)) {
    LuaTools::arg_error(l, index,
        "Bad field '" + field_name + "' (invalid savegame variable identifier: '"
        + savegame_variable + "')"
    );
  }

  return savegame_variable;
}

/**
 * \brief Checks and returns an optional savegame variable field for an
 * entity to be created.
 *
 * Throws a LuaException if the savegame variable name is invalid.
 *
 * \param l A Lua state.
 * \param index Index of the argument in the Lua stack.
 * \param entity_data Description of the entity to create.
 * \param field_name Name of the savegame variable field to check.
 * \return The savegame variable name.
 * An empty string means no savegame variable.
 */
std::string entity_creation_check_savegame_variable_optional(
    lua_State* l,
    int index,
    const EntityData& entity_data,
    const std::string& field_name
) {
  const std::string& savegame_variable = entity_data.get_string(field_name);

  if (savegame_variable.empty()) {
    return savegame_variable;
  }

  return entity_creation_check_savegame_variable_mandatory(l, index, entity_data, field_name);
}

/**
 * \brief Checks an enum field for an entity to be created.
 *
 * Throws a LuaException if the value is not a string or if the string is not
 * a valid name for the enum.
 * This is a useful function for mapping strings to C or C++ enums.
 *
 * \param l A Lua state.
 * \param index Index of the argument in the Lua stack.
 * \param entity_data Description of the entity to
 * \param field_name Name of the field to check.
 * \param names A mapping of enum values to strings to search in.
 * \return The enumerated value corresponding to this string.
 */
template<typename E>
E entity_creation_check_enum(
    lua_State* l,
    int index,
    const EntityData& entity_data,
    const std::string& field_name,
    const std::map<E, std::string>& names
) {
  const std::string& name = entity_data.get_string(field_name);
  for (const auto& kvp: names) {
    if (kvp.second == name) {
      return kvp.first;
    }
  }

  // The value was not found. Build an error message with possible values.
  std::string allowed_names;
  for (const auto& kvp: names) {
    allowed_names += "\"" + kvp.second + "\", ";
  }
  allowed_names = allowed_names.substr(0, allowed_names.size() - 2);

  LuaTools::arg_error(l, index,
      std::string("Invalid name '") + name + "'. Allowed names are: "
      + allowed_names
  );
  return E();  // Make sure the compiler is happy.
}

template<typename E>
E entity_creation_check_enum(
    lua_State* l,
    int index,
    const EntityData& entity_data,
    const std::string& field_name
) {
  return entity_creation_check_enum<E>(l, index, entity_data, field_name, EnumInfoTraits<E>::names);
}

}

/**
 * \brief Creates a tile on the map.
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::l_create_tile(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Map& map = *check_map(l, 1);
    EntityData& data = *(static_cast<EntityData*>(lua_touserdata(l, 2)));
    const int layer = entity_creation_check_layer(l, 1, data, map);
    const int x = data.get_xy().x;
    const int y = data.get_xy().y;
    const Size size =  entity_creation_check_size(l, 1, data);
    const std::string& tile_pattern_id = data.get_string("pattern");

    const Tileset& tileset = map.get_tileset();
    const TilePattern& pattern = tileset.get_tile_pattern(tile_pattern_id);
    const Size& pattern_size = pattern.get_size();
    Entities& entities = map.get_entities();

    // If the tile is big, divide it in several smaller tiles so that
    // most of them can still be optimized away.
    // Otherwise, tiles expanded in big rectangles like a lake or a dungeon
    // floor would be entirely redrawn at each frame if just one small
    // animated tile overlapped them.

    TileInfo tile_info;
    tile_info.layer = layer;
    tile_info.box = { Point(), pattern_size };
    tile_info.pattern_id = tile_pattern_id;
    tile_info.pattern = &pattern;

    for (int current_y = y; current_y < y + size.height; current_y += pattern.get_height()) {
      for (int current_x = x; current_x < x + size.width; current_x += pattern.get_width()) {
        tile_info.box.set_xy(current_x, current_y);
        // The tile will actually be created only if it cannot be optimized away.
        entities.add_tile_info(
            tile_info
        );
      }
    }

    return 0;
  });
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

    std::shared_ptr<Destination> entity = std::make_shared<Destination>(
        data.get_name(),
        entity_creation_check_layer(l, 1, data, map),
        data.get_xy(),
        data.get_integer("direction"),
        data.get_string("sprite"),
        data.get_boolean("default")
    );
    StartingLocationMode starting_location_mode =
        entity_creation_check_enum<StartingLocationMode>(l, 1, data, "starting_location_mode");
    entity->set_starting_location_mode(starting_location_mode);
    map.get_entities().add_entity(entity);

    if (map.is_started()) {
      push_entity(l, *entity);
      return 1;
    }
    return 0;
  });
}

/**
 * \brief Creates a teletransporter on the map.
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::l_create_teletransporter(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Map& map = *check_map(l, 1);
    EntityData& data = *(static_cast<EntityData*>(lua_touserdata(l, 2)));

    EntityPtr entity = std::make_shared<Teletransporter>(
        data.get_name(),
        entity_creation_check_layer(l, 1, data, map),
        data.get_xy(),
        entity_creation_check_size(l, 1, data),
        data.get_string("sprite"),
        data.get_string("sound"),
        entity_creation_check_enum<Transition::Style>(l, 1, data, "transition"),
        data.get_string("destination_map"),
        data.get_string("destination")
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
 * \brief Creates a pickable treasure on the map.
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::l_create_pickable(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Map& map = *check_map(l, 1);
    EntityData& data = *(static_cast<EntityData*>(lua_touserdata(l, 2)));

    Game& game = map.get_game();
    bool force_persistent = false;
    FallingHeight falling_height = FALLING_MEDIUM;
    if (!map.is_loaded()) {
      // Different behavior when the pickable is already placed on the map.
      falling_height = FALLING_NONE;
      force_persistent = true;
    }

    const std::shared_ptr<Pickable>& entity = Pickable::create(
        game,
        data.get_name(),
        entity_creation_check_layer(l, 1, data, map),
        data.get_xy(),
        Treasure(
            game,
            data.get_string("treasure_name"),
            data.get_integer("treasure_variant"),
            entity_creation_check_savegame_variable_optional(l, 1, data, "treasure_savegame_variable")
        ),
        falling_height,
        force_persistent
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
 * \brief Creates a destructible object on the map.
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::l_create_destructible(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Map& map = *check_map(l, 1);
    EntityData& data = *(static_cast<EntityData*>(lua_touserdata(l, 2)));

    std::shared_ptr<Destructible> destructible = std::make_shared<Destructible>(
        data.get_name(),
        entity_creation_check_layer(l, 1, data, map),
        data.get_xy(),
        data.get_string("sprite"),
        Treasure(
            map.get_game(),
            data.get_string("treasure_name"),
            data.get_integer("treasure_variant"),
            entity_creation_check_savegame_variable_optional(l, 1, data, "treasure_savegame_variable")
        ),
        entity_creation_check_enum<Ground>(l, 1, data, "ground")
    );
    destructible->set_destruction_sound(data.get_string("destruction_sound"));
    destructible->set_weight(data.get_integer("weight"));
    destructible->set_can_be_cut(data.get_boolean("can_be_cut"));
    destructible->set_can_explode(data.get_boolean("can_explode"));
    destructible->set_can_regenerate(data.get_boolean("can_regenerate"));
    destructible->set_damage_on_enemies(data.get_integer("damage_on_enemies"));
    map.get_entities().add_entity(destructible);
    if (map.is_started()) {
      push_entity(l, *destructible);
      return 1;
    }
    return 0;
  });
}

/**
 * \brief Creates a chest on the map.
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::l_create_chest(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Map& map = *check_map(l, 1);
    EntityData& data = *(static_cast<EntityData*>(lua_touserdata(l, 2)));

    Chest::OpeningMethod opening_method = entity_creation_check_enum<Chest::OpeningMethod>(
        l,
        1,
        data,
        "opening_method",
        Chest::opening_method_names
    );

    // Check the value of opening_condition depending on the opening method.
    Game& game = map.get_game();
    const std::string& opening_condition = data.get_string("opening_condition");
    if (opening_method == Chest::OpeningMethod::BY_INTERACTION_IF_SAVEGAME_VARIABLE) {
      entity_creation_check_savegame_variable_mandatory(l, 1, data, "opening_condition");
    }
    else if (opening_method == Chest::OpeningMethod::BY_INTERACTION_IF_ITEM) {
      if (!game.get_equipment().item_exists(opening_condition)) {
        LuaTools::arg_error(l, 1, "Bad field 'opening_condition' (no such equipment item: '"
            + opening_condition + "')"
        );
      }
      EquipmentItem& item = game.get_equipment().get_item(opening_condition);
      if (!item.is_saved()) {
        LuaTools::arg_error(l, 1, "Bad field 'opening_condition' (equipment item '"
            + opening_condition + "' is not saved)"
        );
      }
    }

    std::shared_ptr<Chest> chest = std::make_shared<Chest>(
        data.get_name(),
        entity_creation_check_layer(l, 1, data, map),
        data.get_xy(),
        data.get_string("sprite"),
        Treasure(
            game,
            data.get_string("treasure_name"),
            data.get_integer("treasure_variant"),
            entity_creation_check_savegame_variable_optional(l, 1, data, "treasure_savegame_variable")
        )
    );
    chest->set_opening_method(opening_method);
    chest->set_opening_condition(opening_condition);
    chest->set_opening_condition_consumed(data.get_boolean("opening_condition_consumed"));
    chest->set_cannot_open_dialog_id(data.get_string("cannot_open_dialog"));
    map.get_entities().add_entity(chest);
    if (map.is_started()) {
      push_entity(l, *chest);
      return 1;
    }
    return 0;
  });
}

/**
 * \brief Creates a jumper on the map.
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::l_create_jumper(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Map& map = *check_map(l, 1);
    EntityData& data = *(static_cast<EntityData*>(lua_touserdata(l, 2)));

    EntityPtr entity = std::make_shared<Jumper>(
        data.get_name(),
        entity_creation_check_layer(l, 1, data, map),
        data.get_xy(),
        entity_creation_check_size(l, 1, data),
        data.get_integer("direction"),
        data.get_integer("jump_length")
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
 * \brief Creates an enemy on the map.
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::l_create_enemy(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Map& map = *check_map(l, 1);
    EntityData& data = *(static_cast<EntityData*>(lua_touserdata(l, 2)));
    Game& game = map.get_game();
    EntityPtr entity = Enemy::create(
        game,
        data.get_string("breed"),
        entity_creation_check_savegame_variable_optional(l, 1, data, "savegame_variable"),
        data.get_name(),
        entity_creation_check_layer(l, 1, data, map),
        data.get_xy(),
        data.get_integer("direction"),
        Treasure(
            game,
            data.get_string("treasure_name"),
            data.get_integer("treasure_variant"),
            entity_creation_check_savegame_variable_optional(l, 1, data, "treasure_savegame_variable")
        )
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
 * \brief Creates an NPC on the map.
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::l_create_npc(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Map& map = *check_map(l, 1);
    EntityData& data = *(static_cast<EntityData*>(lua_touserdata(l, 2)));

    Game& game = map.get_game();
    EntityPtr entity = std::make_shared<Npc>(
        game,
        data.get_name(),
        entity_creation_check_layer(l, 1, data, map),
        data.get_xy(),
        Npc::Subtype(data.get_integer("subtype")),
        data.get_string("sprite"),
        data.get_integer("direction"),
        data.get_string("behavior")
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
 * \brief Creates a block on the map.
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::l_create_block(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Map& map = *check_map(l, 1);
    EntityData& data = *(static_cast<EntityData*>(lua_touserdata(l, 2)));

    int maximum_moves = data.get_integer("maximum_moves");
    if (maximum_moves < 0 || maximum_moves > 2) {
      std::ostringstream oss;
      oss << "Invalid maximum_moves: " << maximum_moves;
      LuaTools::arg_error(l, 1, oss.str());
    }
    std::shared_ptr<Block> entity = std::make_shared<Block>(
        data.get_name(),
        entity_creation_check_layer(l, 1, data, map),
        data.get_xy(),
        data.get_integer("direction"),
        data.get_string("sprite"),
        data.get_boolean("pushable"),
        data.get_boolean("pullable"),
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
 * \brief Creates a dynamic tile on the map.
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::l_create_dynamic_tile(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Map& map = *check_map(l, 1);
    EntityData& data = *(static_cast<EntityData*>(lua_touserdata(l, 2)));

    EntityPtr entity = std::make_shared<DynamicTile>(
        data.get_name(),
        entity_creation_check_layer(l, 1, data, map),
        data.get_xy(),
        entity_creation_check_size(l, 1, data),
        map.get_tileset(),
        data.get_string("pattern"),
        data.get_boolean("enabled_at_start")
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
 * \brief Creates a switch on the map.
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::l_create_switch(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Map& map = *check_map(l, 1);
    EntityData& data = *(static_cast<EntityData*>(lua_touserdata(l, 2)));

    EntityPtr entity = std::make_shared<Switch>(
        data.get_name(),
        entity_creation_check_layer(l, 1, data, map),
        data.get_xy(),
        entity_creation_check_enum<Switch::Subtype>(l, 1, data, "subtype", Switch::subtype_names),
        data.get_string("sprite"),
        data.get_string("sound"),
        data.get_boolean("needs_block"),
        data.get_boolean("inactivate_when_leaving")
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
 * \brief Creates a wall on the map.
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::l_create_wall(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Map& map = *check_map(l, 1);
    EntityData& data = *(static_cast<EntityData*>(lua_touserdata(l, 2)));

    EntityPtr entity = std::make_shared<Wall>(
        data.get_name(),
        entity_creation_check_layer(l, 1, data, map),
        data.get_xy(),
        entity_creation_check_size(l, 1, data),
        data.get_boolean("stops_hero"),
        data.get_boolean("stops_enemies"),
        data.get_boolean("stops_npcs"),
        data.get_boolean("stops_blocks"),
        data.get_boolean("stops_projectiles")
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
 * \brief Creates a sensor on the map.
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::l_create_sensor(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Map& map = *check_map(l, 1);
    EntityData& data = *(static_cast<EntityData*>(lua_touserdata(l, 2)));

    EntityPtr entity = std::make_shared<Sensor>(
        data.get_name(),
        entity_creation_check_layer(l, 1, data, map),
        data.get_xy(),
        entity_creation_check_size(l, 1, data)
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
 * \brief Creates a crystal on the map.
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::l_create_crystal(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Map& map = *check_map(l, 1);
    EntityData& data = *(static_cast<EntityData*>(lua_touserdata(l, 2)));

    EntityPtr entity = std::make_shared<Crystal>(
        data.get_name(),
        entity_creation_check_layer(l, 1, data, map),
        data.get_xy()
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
 * \brief Creates a crystal block on the map.
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::l_create_crystal_block(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Map& map = *check_map(l, 1);
    EntityData& data = *(static_cast<EntityData*>(lua_touserdata(l, 2)));

    Game& game = map.get_game();
    EntityPtr entity = std::make_shared<CrystalBlock>(
        game,
        data.get_name(),
        entity_creation_check_layer(l, 1, data, map),
        data.get_xy(),
        entity_creation_check_size(l, 1, data),
        CrystalBlock::Subtype(data.get_integer("subtype"))
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
 * \brief Creates a shop treasure on the map.
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::l_create_shop_treasure(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Map& map = *check_map(l, 1);
    EntityData& data = *(static_cast<EntityData*>(lua_touserdata(l, 2)));

    Game& game = map.get_game();
    EntityPtr entity = ShopTreasure::create(
        game,
        data.get_name(),
        entity_creation_check_layer(l, 1, data, map),
        data.get_xy(),
        Treasure(
            game,
            data.get_string("treasure_name"),
            data.get_integer("treasure_variant"),
            entity_creation_check_savegame_variable_optional(l, 1, data, "treasure_savegame_variable")
        ),
        data.get_integer("price"),
        data.get_string("font"),
        data.get_string("dialog")
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
 * \brief Creates a stream on the map.
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::l_create_stream(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Map& map = *check_map(l, 1);
    EntityData& data = *(static_cast<EntityData*>(lua_touserdata(l, 2)));

    std::shared_ptr<Stream> stream = std::make_shared<Stream>(
        data.get_name(),
        entity_creation_check_layer(l, 1, data, map),
        data.get_xy(),
        data.get_integer("direction"),
        data.get_string("sprite")
    );
    stream->set_speed(data.get_integer("speed"));
    stream->set_allow_movement(data.get_boolean("allow_movement"));
    stream->set_allow_attack(data.get_boolean("allow_attack"));
    stream->set_allow_item(data.get_boolean("allow_item"));

    map.get_entities().add_entity(stream);
    if (map.is_started()) {
      push_stream(l, *stream);
      return 1;
    }
    return 0;
  });
}

/**
 * \brief Creates a door on the map.
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::l_create_door(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Map& map = *check_map(l, 1);
    EntityData& data = *(static_cast<EntityData*>(lua_touserdata(l, 2)));

    Door::OpeningMethod opening_method = entity_creation_check_enum<Door::OpeningMethod>(
        l,
        1,
        data,
        "opening_method",
        Door::opening_method_names
    );

    // Check the value of opening_condition depending on the opening method.
    Game& game = map.get_game();
    const std::string& opening_condition = data.get_string("opening_condition");
    if (opening_method == Door::OpeningMethod::BY_INTERACTION_IF_SAVEGAME_VARIABLE) {
      entity_creation_check_savegame_variable_mandatory(l, 1, data, "opening_condition");
    }
    else if (opening_method == Door::OpeningMethod::BY_INTERACTION_IF_ITEM) {
      if (!game.get_equipment().item_exists(opening_condition)) {
        LuaTools::arg_error(l, 1, "Bad field 'opening_condition' (no such equipment item: '"
            + opening_condition + "')"
        );
      }
      EquipmentItem& item = game.get_equipment().get_item(opening_condition);
      if (!item.is_saved()) {
        LuaTools::arg_error(l, 1, "Bad field 'opening_condition' (equipment item '"
            + opening_condition + "' is not saved)"
        );
      }
    }
    std::shared_ptr<Door> door = std::make_shared<Door>(
        game,
        data.get_name(),
        entity_creation_check_layer(l, 1, data, map),
        data.get_xy(),
        data.get_integer("direction"),
        data.get_string("sprite"),
        entity_creation_check_savegame_variable_optional(l, 1, data, "savegame_variable")
    );
    door->set_opening_method(opening_method);
    door->set_opening_condition(opening_condition);
    door->set_opening_condition_consumed(data.get_boolean("opening_condition_consumed"));
    door->set_cannot_open_dialog_id(data.get_string("cannot_open_dialog"));
    map.get_entities().add_entity(door);
    if (map.is_started()) {
      push_entity(l, *door);
      return 1;
    }
    return 0;
  });
}

/**
 * \brief Creates stairs on the map.
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::l_create_stairs(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Map& map = *check_map(l, 1);
    EntityData& data = *(static_cast<EntityData*>(lua_touserdata(l, 2)));

    EntityPtr entity = std::make_shared<Stairs>(
        data.get_name(),
        entity_creation_check_layer(l, 1, data, map),
        data.get_xy(),
        data.get_integer("direction"),
        Stairs::Subtype(data.get_integer("subtype"))
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
 * \brief Creates a separator on the map.
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::l_create_separator(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Map& map = *check_map(l, 1);
    EntityData& data = *(static_cast<EntityData*>(lua_touserdata(l, 2)));

    EntityPtr entity = std::make_shared<Separator>(
        data.get_name(),
        entity_creation_check_layer(l, 1, data, map),
        data.get_xy(),
        entity_creation_check_size(l, 1, data)
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
 * \brief Creates a custom entity on the map.
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::l_create_custom_entity(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Map& map = *check_map(l, 1);
    EntityData& data = *(static_cast<EntityData*>(lua_touserdata(l, 2)));

    Game& game = map.get_game();
    EntityPtr entity = std::make_shared<CustomEntity>(
        game,
        data.get_name(),
        data.get_integer("direction"),
        entity_creation_check_layer(l, 1, data, map),
        data.get_xy(),
        entity_creation_check_size(l, 1, data),
        data.get_string("sprite"),
        data.get_string("model")
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
 * \brief Creates a bomb on the map.
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::l_create_bomb(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Map& map = *check_map(l, 1);
    EntityData& data = *(static_cast<EntityData*>(lua_touserdata(l, 2)));

    EntityPtr entity = std::make_shared<Bomb>(
        data.get_name(),
        entity_creation_check_layer(l, 1, data, map),
        data.get_xy()
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
 * \brief Creates an explosion on the map.
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::l_create_explosion(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Map& map = *check_map(l, 1);
    EntityData& data = *(static_cast<EntityData*>(lua_touserdata(l, 2)));

    const bool with_damage = true;
    EntityPtr entity = std::make_shared<Explosion>(
        data.get_name(),
        entity_creation_check_layer(l, 1, data, map),
        data.get_xy(),
        with_damage
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
 * \brief Creates a fire entity on the map.
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::l_create_fire(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Map& map = *check_map(l, 1);
    EntityData& data = *(static_cast<EntityData*>(lua_touserdata(l, 2)));

    EntityPtr entity = std::make_shared<Fire>(
        data.get_name(),
        entity_creation_check_layer(l, 1, data, map),
        data.get_xy()
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
    { EntityType::DESTINATION, LuaContext::l_create_destination },
    { EntityType::TELETRANSPORTER, LuaContext::l_create_teletransporter },
    { EntityType::PICKABLE, LuaContext::l_create_pickable },
    { EntityType::DESTRUCTIBLE, LuaContext::l_create_destructible },
    { EntityType::CHEST, LuaContext::l_create_chest },
    { EntityType::JUMPER, LuaContext::l_create_jumper },
    { EntityType::ENEMY, LuaContext::l_create_enemy },
    { EntityType::NPC, LuaContext::l_create_npc },
    { EntityType::BLOCK, LuaContext::l_create_block },
    { EntityType::DYNAMIC_TILE, LuaContext::l_create_dynamic_tile },
    { EntityType::SWITCH, LuaContext::l_create_switch },
    { EntityType::WALL, LuaContext::l_create_wall },
    { EntityType::SENSOR, LuaContext::l_create_sensor },
    { EntityType::CRYSTAL, LuaContext::l_create_crystal },
    { EntityType::CRYSTAL_BLOCK, LuaContext::l_create_crystal_block },
    { EntityType::SHOP_TREASURE, LuaContext::l_create_shop_treasure },
    { EntityType::STREAM, LuaContext::l_create_stream },
    { EntityType::DOOR, LuaContext::l_create_door },
    { EntityType::STAIRS, LuaContext::l_create_stairs },
    { EntityType::SEPARATOR, LuaContext::l_create_separator },
    { EntityType::CUSTOM, LuaContext::l_create_custom_entity },
    { EntityType::EXPLOSION, LuaContext::l_create_explosion },
    { EntityType::BOMB, LuaContext::l_create_bomb },
    { EntityType::FIRE, LuaContext::l_create_fire },
};

/**
 * \brief Creates on the current map an entity from the specified data.
 *
 * Pushes onto the Lua stack the created entity.
 * In case of error, pushes nothing and returns \c false.
 *
 * \param map The map where to create an entity.
 * \param entity_data Description of the entity to create.
 * \return \c true if the entity was successfully created.
 */
bool LuaContext::create_map_entity_from_data(Map& map, const EntityData& entity_data) {

  const std::string& type_name = enum_to_name(entity_data.get_type());
  std::string function_name = "create_" + type_name;
  const auto& it = entity_creation_functions.find(entity_data.get_type());
  Debug::check_assertion(it != entity_creation_functions.end(),
      "Missing entity creation function for type '" + type_name + "'"
  );
  lua_CFunction function = it->second;

  lua_pushcfunction(l, function);
  push_map(l, map);
  lua_pushlightuserdata(l, const_cast<EntityData*>(&entity_data));
  return call_function(2, 1, function_name.c_str());
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

    EntityPtr entity = nullptr;
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
 * \brief Closure of an iterator over a list of entities.
 *
 * This closure expects 3 upvalues in this order:
 * - The array of entities.
 * - The size of the array (for performance).
 * - The current index in the array.
 *
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::l_entity_iterator_next(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {

    // Get upvalues.
    const int table_index = lua_upvalueindex(1);
    const int size = lua_tointeger(l, lua_upvalueindex(2));
    int index = lua_tointeger(l, lua_upvalueindex(3));

    if (index > size) {
      // Finished.
      return 0;
    }

    // Get the next value.
    lua_rawgeti(l, table_index, index);

    // Increment index.
    ++index;
    lua_pushinteger(l, index);
    lua_replace(l, lua_upvalueindex(3));

    return 1;
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
 * \brief Implementation of map:set_world().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::map_api_set_world(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Map& map = *check_map(l, 1);
    std::string world;
    if (lua_type(l, 2) != LUA_TSTRING && lua_type(l, 2) != LUA_TNIL) {
      LuaTools::type_error(l, 2, "string or nil");
    }
    if (!lua_isnil(l, 2)) {
      world = LuaTools::check_string(l, 2);
    }

    map.set_world(world);

    return 0;
  });
}

/**
 * \brief Implementation of map:get_min_layer().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::map_api_get_min_layer(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const Map& map = *check_map(l, 1);

    lua_pushinteger(l, map.get_min_layer());
    return 1;
  });
}

/**
 * \brief Implementation of map:get_max_layer().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::map_api_get_max_layer(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const Map& map = *check_map(l, 1);

    lua_pushinteger(l, map.get_max_layer());
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
 * \brief Implementation of map:set_floor().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::map_api_set_floor(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Map& map = *check_map(l, 1);
    int floor = MapData::NO_FLOOR;
    if (lua_type(l, 2) != LUA_TNUMBER && lua_type(l, 2) != LUA_TNIL) {
      LuaTools::type_error(l, 2, "number or nil");
    }
    if (!lua_isnil(l, 2)) {
      floor = LuaTools::check_int(l, 2);
    }

    map.set_floor(floor);

    return 0;
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
 * \brief Implementation of map:get_camera().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::map_api_get_camera(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Map& map = *check_map(l, 1);

    const CameraPtr& camera = map.get_camera();
    if (camera == nullptr) {
      lua_pushnil(l);
      return 1;
    }

    push_camera(l, *camera);
    return 1;
  });
}

/**
 * \brief Implementation of map:get_camera_position().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::map_api_get_camera_position(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {

    get_lua_context(l).warning_deprecated(
        { 1, 5 },
        "map:get_camera_position()",
        "Use map:get_camera():get_bounding_box() instead.");

    const Map& map = *check_map(l, 1);

    const CameraPtr& camera = map.get_camera();
    if (camera == nullptr) {
      lua_pushnil(l);
      return 1;
    }

    const Rectangle& camera_position = camera->get_bounding_box();

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

    LuaContext& lua_context = get_lua_context(l);
    lua_context.warning_deprecated(
        { 1, 5 },
        "map:move_camera()",
        "Make a target movement on map:get_camera() instead.");

    check_map(l, 1);
    LuaTools::check_int(l, 2);
    LuaTools::check_int(l, 3);
    LuaTools::check_int(l, 4);
    LuaTools::check_type(l, 5, LUA_TFUNCTION);
    if (lua_gettop(l) >= 6) {
      LuaTools::check_int(l, 6);
    }
    if (lua_gettop(l) >= 7) {
      LuaTools::check_int(l, 7);
    }
    lua_settop(l, 7); // Make sure that we always have 7 arguments.

    lua_getfield(l, LUA_REGISTRYINDEX, "map.move_camera");
    if (!lua_isnil(l, -1)) {
      Debug::check_assertion(lua_isfunction(l, -1), "map:move_camera() is not a function");
      lua_insert(l, 1);
      lua_context.call_function(7, 0, "move_camera");
    }

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
    Map& map = *check_map(l, 1);
    int x = LuaTools::check_int(l, 2);
    int y = LuaTools::check_int(l, 3);
    int layer = LuaTools::check_layer(l, 4, map);

    Ground ground = map.get_ground(layer, x, y, nullptr);

    push_string(l, enum_to_name(ground));
    return 1;
  });
}

/**
 * \brief Implementation of map:draw_visual().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::map_api_draw_visual(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {

    Map& map = *check_map(l, 1);
    Drawable& drawable = *check_drawable(l, 2);
    int x = LuaTools::check_int(l, 3);
    int y = LuaTools::check_int(l, 4);

    map.draw_visual(drawable, x, y);

    return 0;
  });
}

/**
 * \brief Implementation of map:draw_sprite().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::map_api_draw_sprite(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {

    get_lua_context(l).warning_deprecated(
        { 1, 5 },
        "map:draw_sprite()",
        "Use map:draw_visual() instead.");

    Map& map = *check_map(l, 1);
    Sprite& sprite = *check_sprite(l, 2);
    int x = LuaTools::check_int(l, 3);
    int y = LuaTools::check_int(l, 4);

    map.draw_visual(sprite, x, y);

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
    Entities& entities = map.get_entities();
    const std::vector<EntityPtr>& doors = entities.get_entities_with_prefix(EntityType::DOOR, prefix);
    for (const EntityPtr& entity: doors) {
      Door& door = *std::static_pointer_cast<Door>(entity);
      if (!door.is_open() || door.is_closing()) {
        door.open();
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
    Entities& entities = map.get_entities();
    const std::vector<EntityPtr>& doors = entities.get_entities_with_prefix(EntityType::DOOR, prefix);
    for (const EntityPtr& entity: doors) {
      Door& door = *std::static_pointer_cast<Door>(entity);
      if (door.is_open() || door.is_opening()) {
        door.close();
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

    Entities& entities = map.get_entities();
    const std::vector<EntityPtr>& doors = entities.get_entities_with_prefix(EntityType::DOOR, prefix);
    for (const EntityPtr& entity: doors) {
      Door& door = *std::static_pointer_cast<Door>(entity);
      door.set_open(open);
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

    const EntityPtr& entity = map.get_entities().find_entity(name);

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

    const EntityPtr& entity = map.get_entities().find_entity(name);

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
    const std::string& prefix = LuaTools::opt_string(l, 2, "");

    const EntityVector& entities =
        map.get_entities().get_entities_with_prefix_sorted(prefix);

    push_entity_iterator(l, entities);
    return 1;
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

    const EntityVector& entities =
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
 * \brief Implementation of map:get_entities_by_type().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::map_api_get_entities_by_type(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Map& map = *check_map(l, 1);
    EntityType type = LuaTools::check_enum<EntityType>(l, 2);

    const EntityVector& entities =
        map.get_entities().get_entities_by_type_sorted(type);

    push_entity_iterator(l, entities);
    return 1;
  });
}

/**
 * \brief Implementation of map:get_entities_in_rectangle().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::map_api_get_entities_in_rectangle(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Map& map = *check_map(l, 1);
    const int x = LuaTools::check_int(l, 2);
    const int y = LuaTools::check_int(l, 3);
    const int width = LuaTools::check_int(l, 4);
    const int height = LuaTools::check_int(l, 5);

    EntityVector entities;
    map.get_entities().get_entities_in_rectangle_sorted(
        Rectangle(x, y, width, height), entities
    );

    push_entity_iterator(l, entities);
    return 1;
  });
}

/**
 * \brief Implementation of map:get_entities_in_region().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::map_api_get_entities_in_region(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Map& map = *check_map(l, 1);
    Point xy;
    EntityPtr entity;
    if (lua_isnumber(l, 2)) {
      int x = LuaTools::check_int(l, 2);
      int y = LuaTools::check_int(l, 3);
      xy = Point(x, y);
    }
    else if (is_entity(l, 2)) {
      entity = check_entity(l, 2);
      xy = entity->get_xy();
    }
    else {
      LuaTools::type_error(l, 2, "entity or number");
    }

    EntityVector entities;
    map.get_entities().get_entities_in_region_sorted(
        xy, entities
    );

    if (entity != nullptr) {
      // Entity variant: remove the entity itself.
      const auto& it = std::find(entities.begin(), entities.end(), entity);
      if (it != entities.end()) {
        entities.erase(it);
      }
    }

    push_entity_iterator(l, entities);
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

    std::vector<EntityPtr> entities =
        map.get_entities().get_entities_with_prefix(prefix);
    for (const EntityPtr& entity: entities) {
      entity->set_enabled(enabled);
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
        l, lua_upvalueindex(1)
    );
    Map& map = *check_map(l, 1);
    const EntityData& data = EntityData::check_entity_data(l, 2, type);

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
 * \param destination The destination point used (nullptr if it is a special one).
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

