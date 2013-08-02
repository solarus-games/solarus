/*
 * Copyright (C) 2006-2013 Christopho, Solarus - http://www.solarus-games.org
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
 * \brief Creates a new tile.
 * \param layer layer of the tile
 * \param x x position of the tile on the map
 * \param y y position of the tile on the map
 * \param width width of the tile (the pattern can be repeated)
 * \param height height of the tile (the pattern can be repeated)
 * \param tile_pattern_id id of the tile pattern
 */
Tile::Tile(Layer layer, int x, int y, int width, int height, int tile_pattern_id):
  MapEntity(layer, x, y, width, height),
  tile_pattern_id(tile_pattern_id),
  tile_pattern(NULL) {

}

/**
 * \brief Destructor.
 */
Tile::~Tile() {

}

/**
 * \brief Returns the type of entity.
 * \return the type of entity
 */
EntityType Tile::get_type() const {
  return TILE;
}

/**
 * \brief Sets the map of this entity.
 *
 * Warning: as this function is called when initializing the map,
 * the current map of the game is still the old one.
 *
 * \param map the map
 */
void Tile::set_map(Map& map) {
  MapEntity::set_map(map);
  this->tile_pattern = &map.get_tileset().get_tile_pattern(tile_pattern_id);
}

/**
 * \brief Returns whether this entity is drawn at its position on the map.
 * \return true if this entity is drawn where it is located.
 */
bool Tile::is_drawn_at_its_position() const {
  return tile_pattern->is_drawn_at_its_position();
}

/**
 * \brief Draws the tile on the map.
 */
void Tile::draw_on_map() {

  if (!is_drawn()) {
    return;
  }

  // Note that the tiles are also optimized for drawing.
  // This function is called at each frame only if the tile is in an
  // animated region. Otherwise, tiles are drawn once when loading the map.
  draw(get_map().get_visible_surface(), get_map().get_camera_position());
}

/**
 * \brief Draws the tile on the specified surface.
 * \param dst_surface the destination surface
 * \param viewport coordinates of the top-left corner of dst_surface
 * relative to the map
 */
void Tile::draw(Surface& dst_surface, const Rectangle& viewport) {

  Rectangle dst_position(get_top_left_x() - viewport.get_x(),
      get_top_left_y() - viewport.get_y(),
      get_width(), get_height());

  tile_pattern->fill_surface(dst_surface, dst_position,
      get_map().get_tileset(), viewport);
}

/**
 * \brief Returns the pattern of this tile.
 * \return the tile pattern
 */
TilePattern& Tile::get_tile_pattern() {
  return *tile_pattern;
}

/**
 * \brief Returns whether the pattern is animated.
 *
 * Non-animated tiles may be rendered faster by using intermediate surfaces
 * that are drawn only once.
 * This function should return false if the tile pattern is always drawn the same way.
 *
 * \return true if the pattern of this tile is animated
 */
bool Tile::is_animated() {
  return tile_pattern->is_animated();
}

