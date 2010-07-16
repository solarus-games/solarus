/*
 * Copyright (C) 2009 Christopho, Solarus - http://www.solarus-engine.org
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
#include "entities/DynamicTile.h"
#include "entities/Tileset.h"
#include "entities/TilePattern.h"
#include "entities/Hero.h"
#include "Game.h"
#include "lowlevel/FileTools.h"
#include "Map.h"

/**
 * @brief Creates a new dynamic tile on the map.
 * @param name a name to identify this tile
 * @param layer layer of the tile
 * @param x x position of the tile on the map
 * @param y y position of the tile on the map
 * @param width width of the tile (the pattern can be repeated)
 * @param height height of the tile (the pattern can be repeated)
 * @param tile_pattern_id id of the tile pattern in the tileset 
 * @param enabled true to make the tile active on the map
 */
DynamicTile::DynamicTile(const std::string &name, Layer layer, int x, int y,
			 int width, int height, int tile_pattern_id, bool enabled):
  MapEntity(name, 0, layer, x, y, width, height),
  tile_pattern_id(tile_pattern_id), tile_pattern(NULL),
  enabled(enabled), waiting_enabled(false) {

}

/**
 * @brief Destructor.
 */
DynamicTile::~DynamicTile(void) {

}

/**
 * @brief Creates an instance from an input stream.
 *
 * The input stream must respect the syntax of this entity type.
 *
 * @param game the game that will contain the entity created
 * @param is an input stream
 * @param layer the layer
 * @param x x coordinate of the entity
 * @param y y coordinate of the entity
 * @return the instance created
 */
MapEntity * DynamicTile::parse(Game *game, std::istream &is, Layer layer, int x, int y) {

  int width, height, tile_pattern_id, enabled;
  std::string name;

  FileTools::read(is, width);
  FileTools::read(is, height);
  FileTools::read(is, name);
  FileTools::read(is, tile_pattern_id);
  FileTools::read(is, enabled);

  return new DynamicTile(name, Layer(layer), x, y, width, height, tile_pattern_id, enabled != 0);
}

/**
 * @brief Returns the type of entity.
 * @return the type of entity
 */
EntityType DynamicTile::get_type(void) {
  return DYNAMIC_TILE;
}

/**
 * @brief Sets the map of this entity.
 * @param map the map
 */
void DynamicTile::set_map(Map *map) {
  MapEntity::set_map(map);
  this->tile_pattern = map->get_tileset()->get_tile_pattern(tile_pattern_id);
}

/**
 * @brief Returns whether this tile is an obstacle for the specified entity.
 * @param other an entity
 * @return true if this tile is an obstacle for the entity
 */
bool DynamicTile::is_obstacle_for(MapEntity *other) {
  return enabled && tile_pattern->get_obstacle() >= OBSTACLE;
}

/**
 * @brief Displays the tile on the map.
 */
void DynamicTile::display_on_map(void) {
  if (enabled) {
    tile_pattern->display_on_map(map, bounding_box);
  }
}

/**
 * @brief Updates the entity.
 */
void DynamicTile::update(void) {

  MapEntity::update();

  if (waiting_enabled) {
    Hero *hero = game->get_hero();
    if (tile_pattern->get_obstacle() < OBSTACLE || !overlaps(hero)) {
      this->enabled = true;
      this->waiting_enabled = false;
    }
  }
}

/**
 * @brief Returns whether this dynamic tile is enabled.
 * @return true if this tile is enabled
 */
bool DynamicTile::is_enabled(void) {
  return enabled;
}

/**
 * @brief Enables or disables this dynamic tile.
 * @param enabled true to enable the tile, false to disable it
 */
void DynamicTile::set_enabled(bool enabled) {

  if (enabled) {
    // enable the tile as soon as possible
    this->waiting_enabled = true;
  }
  else {
    this->enabled = false;
    this->waiting_enabled = false;
  }
}

