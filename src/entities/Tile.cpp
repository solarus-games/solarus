/*
 * Copyright (C) 2006-2018 Christopho, Solarus - http://www.solarus-games.org
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
#include "solarus/core/QuestFiles.h"
#include "solarus/core/Map.h"
#include "solarus/entities/Tile.h"
#include "solarus/entities/TileInfo.h"
#include "solarus/entities/Tileset.h"
#include "solarus/entities/TilePattern.h"
#include "solarus/graphics/Surface.h"

namespace Solarus {

/**
 * \brief Creates a new tile.
 * \param tile_info Construction parameters.
 */
Tile::Tile(
    const TileInfo& tile_info
):
  Entity("", 0, tile_info.layer, tile_info.box.get_xy(), tile_info.box.get_size()),
  tile_pattern_id(tile_info.pattern_id),
  tile_pattern(*tile_info.pattern) {

}

/**
 * \brief Returns the type of entity.
 * \return the type of entity
 */
EntityType Tile::get_type() const {
  return ThisType;
}

/**
 * \copydoc Entity::is_drawn_at_its_position()
 */
bool Tile::is_drawn_at_its_position() const {
  return tile_pattern.is_drawn_at_its_position();
}

/**
 * \brief Draws the tile on the map.
 */
void Tile::draw_on_map() {

  const CameraPtr& camera = get_map().get_camera();
  if (camera == nullptr) {
    return;
  }

  // Note that the tiles are also optimized for drawing.
  // This function is called at each frame only if the tile is in an
  // animated region. Otherwise, tiles are drawn once when loading the map.
  draw(get_map().get_camera_surface(), camera->get_top_left_xy());
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
 * \return The tile pattern.
 */
const TilePattern& Tile::get_tile_pattern() const {
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

