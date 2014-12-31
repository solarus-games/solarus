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
#include "solarus/lowlevel/QuestFiles.h"
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

  // Read the map data file.
  MapData data;
  const std::string& file_name = std::string("maps/") + map.get_id() + ".dat";
  bool success = data.import_from_quest_file(file_name);

  if (!success) {
    Debug::die("Failed to load map data file '" + file_name + "'");
  }

  // Initialize the map from the data just read.
  // TODO make a method in Map instead of changing directly the fields.
  map.game = &game;
  map.location.set_size(data.get_size());
  map.width8 = data.get_size().width / 8;
  map.height8 = data.get_size().height / 8;
  map.location.set_xy(data.get_location());
  map.music_id = data.get_music_id();
  map.set_world(data.get_world());
  map.set_floor(data.get_floor());
  map.tileset_id = data.get_tileset_id();
  map.tileset = std::unique_ptr<Tileset>(new Tileset(data.get_tileset_id()));
  map.tileset->load();

  MapEntities& entities = map.get_entities();
  entities.map_width8 = map.width8;
  entities.map_height8 = map.height8;
  entities.tiles_grid_size = map.width8 * map.height8;
  for (int layer = 0; layer < LAYER_NB; ++layer) {

    Ground initial_ground = (layer == LAYER_LOW) ? Ground::TRAVERSABLE : Ground::EMPTY;
    for (int i = 0; i < entities.tiles_grid_size; ++i) {
      entities.tiles_ground[layer].push_back(initial_ground);
    }

    entities.non_animated_regions[layer] = std::unique_ptr<NonAnimatedRegions>(
        new NonAnimatedRegions(map, Layer(layer))
    );
  }
  entities.boomerang = nullptr;
  map.camera = std::unique_ptr<Camera>(new Camera(map));

  // Create entities by calling the Lua API functions.
  LuaContext& lua_context = map.get_lua_context();
  for (int k = LAYER_LOW; k < LAYER_NB; ++k) {
    Layer layer = (Layer) k;
    for (int i = 0; i < (int) data.get_num_entities(); ++i) {
      const EntityData& entity_data = data.get_entity({ layer, i });
      lua_context.create_map_entity_from_data(map, entity_data);
    }
  }
}

}
