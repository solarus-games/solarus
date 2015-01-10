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
#include "solarus/entities/SimpleTilePattern.h"
#include "solarus/entities/Tileset.h"
#include "solarus/lowlevel/Surface.h"
#include "solarus/Map.h"

namespace Solarus {

/**
 * \brief Creates a simple tile pattern.
 * \param ground Kind of the of the tile pattern.
 * \param xy Coordinates of the tile pattern in the tileset.
 * \param size Size of the tile pattern in the tileset.
 */
SimpleTilePattern::SimpleTilePattern(Ground ground, const Point& xy, const Size& size):
  TilePattern(ground, size),
  position_in_tileset(xy, size) {

}

/**
 * \brief Draws the tile image on a surface.
 * \param dst_surface the surface to draw
 * \param dst_position position where tile pattern should be drawn on dst_surface
 * \param tileset the tileset of this tile
 * \param viewport coordinates of the top-left corner of dst_surface relative
 * to the map (may be used for scrolling tiles)
 */
void SimpleTilePattern::draw(
    const SurfacePtr& dst_surface,
    const Point& dst_position,
    Tileset& tileset,
    const Point& /* viewport */
) {
  const SurfacePtr& tileset_image = tileset.get_tiles_image();
  tileset_image->draw_region(position_in_tileset, dst_surface, dst_position);
}

/**
 * \brief Returns whether this tile pattern is animated, i.e. not always displayed
 * the same way.
 *
 * Non-animated tiles may be rendered faster by using intermediate surfaces
 * that are drawn only once.
 *
 * \return true if this tile pattern is animated
 */
bool SimpleTilePattern::is_animated() const {
  return false;
}

}

