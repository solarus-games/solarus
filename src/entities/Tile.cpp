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
#include "entities/Tile.h"
#include "entities/Tileset.h"
#include "entities/TilePattern.h"
#include "lowlevel/Surface.h"
#include "lowlevel/FileTools.h"
#include "Map.h"

/**
 * @brief Creates a new tile.
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
MapEntity* Tile::parse(Game& game, std::istream& is, Layer layer, int x, int y) {

  int width, height, tile_pattern_id;

  FileTools::read(is, width);
  FileTools::read(is, height);
  FileTools::read(is, tile_pattern_id);

  return new Tile(layer, x, y, width, height, tile_pattern_id);
}

/**
 * @brief Destructor.
 */
Tile::~Tile() {

}

/**
 * @brief Returns the type of entity.
 * @return the type of entity
 */
EntityType Tile::get_type() {
  return TILE;
}

/**
 * @brief Sets the map of this entity.
 *
 * Warning: as this function is called when initializing the map,
 * the current map of the game is still the old one.
 *
 * @param map the map
 */
void Tile::set_map(Map& map) {
  MapEntity::set_map(map);
  this->tile_pattern = &map.get_tileset().get_tile_pattern(tile_pattern_id);
}

/**
 * @brief Displays the tile on the map.
 */
void Tile::display_on_map() {

  display(get_map().get_visible_surface(), get_map().get_camera_position());
}

/**
 * @brief Displays the tile on the specified surface.
 * @param dst_surface the destination surface
 * @param viewport coordinates of the top-left corner of dst_surface
 * relative to the map
 */
void Tile::display(Surface* dst_surface, const Rectangle& viewport) {

  Rectangle dst(0, 0);

  int limit_x = get_top_left_x() - viewport.get_x() + get_width();
  int limit_y = get_top_left_y() - viewport.get_y() + get_height();

  for (int y = get_top_left_y() - viewport.get_y();
      y < limit_y;
      y += tile_pattern->get_height()) {

    if (y <= dst_surface->get_height() && y + tile_pattern->get_height() > 0) {
      dst.set_y(y);

      for (int x = get_top_left_x() - viewport.get_x();
          x < limit_x;
          x += tile_pattern->get_width()) {

        if (x <= dst_surface->get_width() && x + tile_pattern->get_width() > 0) {
          dst.set_x(x);
          tile_pattern->display(dst_surface, dst, get_map().get_tileset());
        }
      }
    }
  }
}

/**
 * @brief Returns the pattern of this tile.
 * @return the tile pattern
 */
TilePattern& Tile::get_tile_pattern() {
  return *tile_pattern;
}

/**
 * @brief Returns whether the pattern is animated.
 *
 * Non-animated tiles may be rendered faster by using intermediate surfaces
 * that are drawn only once.
 * This function should return false if the tile pattern is always displayed the same way.
 *
 * @return true if the pattern of this tile is animated
 */
bool Tile::is_animated() {
  return tile_pattern->is_animated();
}

