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
#include "solarus/entities/Tile.h"
#include "solarus/entities/Tileset.h"
#include "solarus/entities/TilePattern.h"
#include "solarus/lowlevel/Surface.h"
#include "solarus/lowlevel/QuestFiles.h"
#include "solarus/Map.h"

namespace Solarus {

/**
 * \brief Creates a new tile.
 * \param layer layer of the tile
 * \param xy Coordinates of the tile on the map
 * \param size Size of the tile (the pattern can be repeated).
 * \param tileset The tileset to use.
 * \param tile_pattern_id Id of the tile pattern in the tileset.
 */
Tile::Tile(
    Layer layer,
    const Point& xy,
    const Size& size,
    Tileset& tileset,
    const std::string& tile_pattern_id
):
  MapEntity("", 0, layer, xy, size),
  tile_pattern_id(tile_pattern_id),
  tile_pattern(tileset.get_tile_pattern(tile_pattern_id)) {

}

/**
 * \brief Returns the type of entity.
 * \return the type of entity
 */
EntityType Tile::get_type() const {
  return EntityType::TILE;
}

/**
 * \brief Returns whether this entity is drawn at its position on the map.
 * \return true if this entity is drawn where it is located.
 */
bool Tile::is_drawn_at_its_position() const {
  return tile_pattern.is_drawn_at_its_position();
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
  draw(get_map().get_visible_surface(), get_map().get_camera_position().get_xy());
}

/**
 * \brief Draws the tile on the specified surface.
 * \param dst_surface the destination surface
 * \param viewport coordinates of the top-left corner of dst_surface
 * relative to the map
 */
void Tile::draw(const SurfacePtr& dst_surface, const Point& viewport) {

  Rectangle dst_position(
      get_top_left_x() - viewport.x,
      get_top_left_y() - viewport.y,
      get_width(),
      get_height()
  );

  tile_pattern.fill_surface(
      dst_surface,
      dst_position,
      get_map().get_tileset(),
      viewport
  );
}

/**
 * \brief Returns the pattern of this tile.
 * \return the tile pattern
 */
TilePattern& Tile::get_tile_pattern() {
  return tile_pattern;
}

/**
 * \brief Returns the id of the pattern of this tile.
 * \return The tile pattern id.
 */
const std::string& Tile::get_tile_pattern_id() const {
  return tile_pattern_id;
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
bool Tile::is_animated() const {
  return tile_pattern.is_animated();
}

}

