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
#include "solarus/entities/SelfScrollingTilePattern.h"
#include "solarus/entities/Tileset.h"
#include "solarus/lowlevel/Surface.h"

namespace Solarus {

/**
 * \brief Creates a tile pattern with self scrolling.
 * \param ground Kind of ground of the tile pattern.
 * \param xy Coordinates of the tile pattern in the tileset.
 * \param size Size of the tile pattern in the tileset.
 */
SelfScrollingTilePattern::SelfScrollingTilePattern(Ground ground, const Point& xy, const Size& size):
  SimpleTilePattern(ground, xy, size) {

}

/**
 * \brief Draws the tile image on a surface.
 * \param dst_surface the surface to draw
 * \param dst_position position where tile pattern should be drawn on dst_surface
 * \param tileset the tileset of this tile
 * \param viewport coordinates of the top-left corner of dst_surface relative
 * to the map (may be used for scrolling tiles)
 */
void SelfScrollingTilePattern::draw(
    const SurfacePtr& dst_surface,
    const Point& dst_position, Tileset& tileset,
    const Point& /* viewport */) {

  Rectangle src = position_in_tileset;
  Point dst = dst_position;

  // draw the tile with an offset that depends on its position modulo its size
  int offset_x, offset_y;

  if (dst.x >= 0) {
    offset_x = dst.x % src.get_width();
  }
  else { // the modulo operation does not like negative numbers
    offset_x = src.get_width() - (-dst.x % src.get_width());
  }

  if (dst.y >= 0) {
    offset_y = dst.y % src.get_height();
  }
  else {
    offset_y = src.get_height() - (-dst.y % src.get_height());
  }

  // apply a scrolling ratio
  offset_x /= 2;
  offset_y /= 2;

  // draw the pattern in four steps
  const SurfacePtr& tileset_image = tileset.get_tiles_image();

  src.add_x(offset_x);
  src.add_width(-offset_x);
  src.add_y(offset_y);
  src.add_height(-offset_y);
  tileset_image->draw_region(src, dst_surface, dst);

  src = position_in_tileset;
  dst = dst_position;
  src.add_y(offset_y);
  src.add_height(-offset_y);
  dst.x += src.get_width() - offset_x;
  src.set_width(offset_x);
  tileset_image->draw_region(src, dst_surface, dst);

  src = position_in_tileset;
  dst = dst_position;
  src.add_x(offset_x);
  src.add_width(-offset_x);
  dst.y += src.get_height() - offset_y;
  src.set_height(offset_y);
  tileset_image->draw_region(src, dst_surface, dst);

  src = position_in_tileset;
  dst = dst_position;
  dst.x += src.get_width() - offset_x;
  src.set_width(offset_x);
  dst.y += src.get_height() - offset_y;
  src.set_height(offset_y);
  tileset_image->draw_region(src, dst_surface, dst);
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
bool SelfScrollingTilePattern::is_animated() const {
  return true;
}

}

