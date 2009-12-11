/*
 * Copyright (C) 2009 Christopho, Zelda Solarus - http://www.zelda-solarus.com
 * 
 * Zelda: Mystery of Solarus DX is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Zelda: Mystery of Solarus DX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "entities/Tile.h"
#include "entities/Tileset.h"
#include "entities/TilePattern.h"
#include "lowlevel/FileTools.h"
#include "Map.h"

/**
 * Creates a new tile on the map.
 * @param layer layer of the tile
 * @param x x position of the tile on the map
 * @param y y position of the tile on the map
 * @param width width of the tile (the pattern can be repeated)
 * @param height height of the tile (the pattern can be repeated)
 * @param tile_pattern_id id of the tile pattern
 */
Tile::Tile(Layer layer, int x, int y, int width, int height, int tile_pattern_id):
  MapEntity(layer, x, y, width, height), tile_pattern_id(tile_pattern_id), tile_pattern(NULL) {

}

/**
 * Creates an instance from an input stream.
 * The input stream must respect the syntax of this entity type.
 * @param is an input stream
 * @param layer the layer
 * @param x x coordinate of the entity
 * @param y y coordinate of the entity
 * @return the instance created
 */
MapEntity * Tile::parse(std::istream &is, Layer layer, int x, int y) {

  int width, height, tile_pattern_id;

  FileTools::read(is, width);
  FileTools::read(is, height);
  FileTools::read(is, tile_pattern_id);

  return new Tile(layer, x, y, width, height, tile_pattern_id);
}

/**
 * Destructor.
 */
Tile::~Tile(void) {

}

/**
 * Returns the type of entity.
 * @return the type of entity
 */
EntityType Tile::get_type() {
  return TILE;
}

/**
 * Sets the map of this entity.
 * @param map the map
 */
void Tile::set_map(Map *map) {
  MapEntity::set_map(map);
  this->tile_pattern = map->get_tileset()->get_tile_pattern(tile_pattern_id);
}

/**
 * Displays the tile on the map.
 */
void Tile::display_on_map(void) {
  tile_pattern->display_on_map(map, bounding_box);
}

/**
 * Returns the pattern of this tile.
 * @return the tile pattern
 */
TilePattern * Tile::get_tile_pattern(void) {
  return tile_pattern;
}

