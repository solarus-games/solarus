/*
 * Copyright (C) 2009-2011 Christopho, Solarus - http://www.solarus-engine.org
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
#include "MapLoader.h"
#include "Map.h"
#include "Game.h"
#include "Camera.h"
#include "lowlevel/FileTools.h"
#include "lowlevel/Surface.h"
#include "lua/MapScript.h"
#include "entities/Obstacle.h"
#include "entities/Layer.h"
#include "entities/Tileset.h"
#include "entities/MapEntities.h"
#include "entities/EntityType.h"
#include "entities/MapEntity.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"
#include <iomanip>

/**
 * @brief Creates a map loader.
 */
MapLoader::MapLoader() {

}

/**
 * @brief Destructor.
 */
MapLoader::~MapLoader() {

}

/**
 * @brief Loads a map into the game.
 * @param game the game
 * @param map the map to load
 */
void MapLoader::load_map(Game &game, Map &map) {

  map.game = &game;

  // get the id of the map
  int id = map.get_id();

  // compute the file name, depending on the id
  std::ostringstream oss;
  oss << "maps/map" << std::setfill('0') << std::setw(4) << id << ".dat";
  const std::string &file_name = oss.str();

  // open the map file
  std::istream &map_file = FileTools::data_file_open(file_name);

  // parse the map file
  std::string line;
  TilesetId tileset_id;
  int x, y, width, height;

  // first line: map general info
  // syntax: width height world floor x y small_keys_variable tileset_id music_id
  if (!std::getline(map_file, line)) {
    Debug::die(StringConcat() << "Cannot load map '" << id << "': the file '" << file_name << "' is empty");
  }

  std::istringstream iss0(line);
  FileTools::read(iss0, width);
  FileTools::read(iss0, height);
  FileTools::read(iss0, map.world);
  FileTools::read(iss0, map.floor);
  FileTools::read(iss0, x);
  FileTools::read(iss0, y);
  FileTools::read(iss0, map.small_keys_variable);
  FileTools::read(iss0, tileset_id);
  FileTools::read(iss0, map.music_id);

  map.location.set_size(width, height);
  map.width8 = width / 8;
  map.height8 = height / 8;
  map.location.set_xy(x, y);

  map.tileset = new Tileset(tileset_id);
  map.tileset->load();

  // initialize data
  MapEntities &entities = map.get_entities();
  entities.map_width8 = map.width8;
  entities.map_height8 = map.height8;
  entities.tiles_grid_size = map.width8 * map.height8;
  for (int layer = 0; layer < LAYER_NB; layer++) {

    entities.animated_tiles[layer] = new bool[entities.tiles_grid_size];
    entities.obstacle_tiles[layer] = new Obstacle[entities.tiles_grid_size];
    Obstacle initial_obstacle = (layer == LAYER_LOW) ? OBSTACLE_NONE : OBSTACLE_EMPTY;
    for (int i = 0; i < entities.tiles_grid_size; i++) {
      entities.animated_tiles[layer][i] = false;
      entities.obstacle_tiles[layer][i] = initial_obstacle;
    }
  }
  entities.boomerang = NULL;

  // read the entities
  while (std::getline(map_file, line)) {

    int entity_type, layer;

    std::istringstream iss(line);
    FileTools::read(iss, entity_type);
    FileTools::read(iss, layer);
    FileTools::read(iss, x);
    FileTools::read(iss, y);

    MapEntity *entity = MapEntity::creation_functions[entity_type](game, iss, Layer(layer), x, y);
    entities.add_entity(entity);
  }

  FileTools::data_file_close(map_file);

  // load the script
  map.script = new MapScript(map);
  map.camera = new Camera(map);
}

