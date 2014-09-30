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
#include "solarus/MapLoader.h"
#include "solarus/Map.h"
#include "solarus/Game.h"
#include "solarus/Camera.h"
#include "solarus/lowlevel/FileTools.h"
#include "solarus/lowlevel/Surface.h"
#include "solarus/lowlevel/Debug.h"
#include "solarus/lowlevel/Music.h"
#include "solarus/entities/Layer.h"
#include "solarus/entities/TilePattern.h"
#include "solarus/entities/Tileset.h"
#include "solarus/entities/NonAnimatedRegions.h"
#include "solarus/entities/MapEntities.h"
#include "solarus/entities/EntityType.h"
#include "solarus/entities/MapEntity.h"
#include "solarus/lua/LuaTools.h"
#include "solarus/lua/LuaContext.h"

namespace Solarus {

/**
 * \brief Creates a map loader.
 */
MapLoader::MapLoader() {

}

/**
 * \brief Loads a map into the game.
 * \param game The game.
 * \param map The map to load.
 */
void MapLoader::load_map(Game& game, Map& map) {

  map.game = &game;

  // Open the map data file in an independent Lua world.
  const std::string& file_name = std::string("maps/") + map.get_id() + ".dat";
  lua_State* l = luaL_newstate();
  const std::string& buffer = FileTools::data_file_read(file_name);
  const int load_result = luaL_loadbuffer(l, buffer.data(), buffer.size(), file_name.c_str());

  if (load_result != 0) {
    Debug::die(std::string("Failed to load map data file '")
        + file_name + "': " + lua_tostring(l, -1)
    );
    lua_pop(l, 1);
  }

  // Register the properties() function to Lua.
  lua_register(l, "properties", l_properties);

  // Make the Lua world aware of our map.
  luaL_newmetatable(l, LuaContext::map_module_name.c_str());
  lua_pushcfunction(l, LuaContext::userdata_meta_gc);
  lua_setfield(l, -2, "__gc");
  lua_pop(l, 1);
  LuaContext::set_entity_implicit_creation_map(l, &map);

  // Execute the Lua code.
  if (lua_pcall(l, 0, 0, 0) != 0) {
    Debug::die(std::string("Failed to load map data file '")
        + file_name + "': " + lua_tostring(l, -1)
    );
    lua_pop(l, 1);
  }

  lua_close(l);

  // TODO if necessary, store the Lua compiled chunk to speed up next loadings of this map.
}

/**
 * \brief Implementation of the properties() function of the Lua map data file.
 *
 * Sets the properties of the map: position, dimensions, tileset, music, etc.
 * This function must be called before any entity creation function.
 *
 * \param l The Lua state that is calling this function.
 * \return Number of values to return to Lua.
 */
int MapLoader::l_properties(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    // Retrieve the map to build.
    Map* map = LuaContext::get_entity_implicit_creation_map(l);
    Debug::check_assertion(map != nullptr, "No map has not been set in this Lua state");

    // Retrieve the map properties from the table parameter.
    LuaTools::check_type(l, 1, LUA_TTABLE);

    const int x = LuaTools::opt_int_field(l, 1, "x", 0);
    const int y = LuaTools::opt_int_field(l, 1, "y", 0);
    const int width = LuaTools::check_int_field(l, 1, "width");
    const int height = LuaTools::check_int_field(l, 1, "height");
    const std::string& world_name = LuaTools::opt_string_field(l, 1 , "world", "");
    const int floor = LuaTools::opt_int_field(l, 1, "floor", Map::NO_FLOOR);
    const std::string& tileset_id = LuaTools::check_string_field(l, 1, "tileset");
    const std::string& music_id = LuaTools::opt_string_field(l, 1, "music", Music::none);

    // Initialize the map data.
    // TODO implement methods in Map instead to check the values instead of changing directly the fields.
    map->location.set_size(width, height);
    map->width8 = width / 8;
    map->height8 = height / 8;
    map->location.set_xy(x, y);
    map->music_id = music_id;
    map->set_world(world_name);
    map->set_floor(floor);

    map->tileset_id = tileset_id;
    map->tileset = std::unique_ptr<Tileset>(new Tileset(tileset_id));
    map->tileset->load();

    MapEntities& entities = map->get_entities();
    entities.map_width8 = map->width8;
    entities.map_height8 = map->height8;
    entities.tiles_grid_size = map->width8 * map->height8;
    for (int layer = 0; layer < LAYER_NB; ++layer) {

      Ground initial_ground = (layer == LAYER_LOW) ? GROUND_TRAVERSABLE : GROUND_EMPTY;
      for (int i = 0; i < entities.tiles_grid_size; ++i) {
        entities.tiles_ground[layer].push_back(initial_ground);
      }

      entities.non_animated_regions[layer] = std::unique_ptr<NonAnimatedRegions>(
          new NonAnimatedRegions(*map, Layer(layer))
      );
    }
    entities.boomerang = nullptr;
    map->camera = std::unique_ptr<Camera>(new Camera(*map));

    // Properties are set: we now allow the data file to declare entities.
    static const luaL_Reg functions[] = {
        { "tile",             LuaContext::map_api_create_tile },
        { "destination",      LuaContext::map_api_create_destination },
        { "teletransporter",  LuaContext::map_api_create_teletransporter },
        { "pickable",         LuaContext::map_api_create_pickable },
        { "destructible",     LuaContext::map_api_create_destructible },
        { "chest",            LuaContext::map_api_create_chest },
        { "jumper",           LuaContext::map_api_create_jumper },
        { "enemy",            LuaContext::map_api_create_enemy },
        { "npc",              LuaContext::map_api_create_npc },
        { "block",            LuaContext::map_api_create_block },
        { "dynamic_tile",     LuaContext::map_api_create_dynamic_tile },
        { "switch",           LuaContext::map_api_create_switch },
        { "wall",             LuaContext::map_api_create_wall },
        { "sensor",           LuaContext::map_api_create_sensor },
        { "crystal",          LuaContext::map_api_create_crystal },
        { "crystal_block",    LuaContext::map_api_create_crystal_block },
        { "shop_treasure",    LuaContext::map_api_create_shop_treasure },
        { "stream",           LuaContext::map_api_create_stream },
        { "door",             LuaContext::map_api_create_door },
        { "stairs",           LuaContext::map_api_create_stairs },
        { "separator",        LuaContext::map_api_create_separator },
        { "custom_entity",    LuaContext::map_api_create_custom_entity },
        { nullptr, nullptr }
    };
    const luaL_Reg* function = functions;
    while (function->name != nullptr) {
      lua_register(l, function->name, function->func);
      ++function;
    }

    return 0;
  });
}

}

