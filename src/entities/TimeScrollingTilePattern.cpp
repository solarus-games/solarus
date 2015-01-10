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
#include "solarus/entities/TimeScrollingTilePattern.h"
#include "solarus/entities/Tileset.h"
#include "solarus/lowlevel/System.h"
#include "solarus/lowlevel/Surface.h"

namespace Solarus {

int TimeScrollingTilePattern::shift = 0;
uint32_t TimeScrollingTilePattern::next_shift_date = 0;

/**
 * \brief Creates a tile pattern with scrolling.
 * \param ground Kind of ground of the tile pattern.
 * \param xy Coordinates of the tile pattern in the tileset.
 * \param size Size of the tile pattern in the tileset.
 */
TimeScrollingTilePattern::TimeScrollingTilePattern(Ground ground,
    const Point& xy, const Size& size):
  SimpleTilePattern(ground, xy, size) {

}

/**
 * \brief Updates all scrolling tiles patterns.
 *
 * This function is called repeatedly by the map.
 */
void TimeScrollingTilePattern::update() {

  const uint32_t now = System::now();

  while (now >= next_shift_date) {
    shift++;
    next_shift_date += 50;
  }
}

/**
 * \brief Draws the tile image on a surface.
 * \param dst_surface the surface to draw
 * \param dst_position position where tile pattern should be drawn on dst_surface
 * \param tileset the tileset of this tile
 * \param viewport coordinates of the top-left corner of dst_surface relative
 * to the map (may be used for scrolling tiles)
 */
void TimeScrollingTilePattern::draw(
    const SurfacePtr& dst_surface,
    const Point& dst_position,
    Tileset& tileset,
    const Point& /* viewport */
) {
  Rectangle src = position_in_tileset;
  Point dst = dst_position;

  int offset_x, offset_y; // draw the tile with an offset that depends on the time

  offset_x = src.get_width() - (shift % src.get_width());
  offset_y = shift % src.get_height();

  src.add_x(offset_x);
  src.add_width(-offset_x);
  src.add_y(offset_y);
  src.add_height(-offset_y);
  tileset.get_tiles_image()->draw_region(src, dst_surface, dst);

  src = position_in_tileset;
  dst = dst_position;
  src.add_y(offset_y);
  src.add_height(-offset_y);
  dst.x += src.get_width() - offset_x;
  src.set_width(offset_x);
  tileset.get_tiles_image()->draw_region(src, dst_surface, dst);

  src = position_in_tileset;
  dst = dst_position;
  src.add_x(offset_x);
  src.add_width(-offset_x);
  dst.y += src.get_height() - offset_y;
  src.set_height(offset_y);
  tileset.get_tiles_image()->draw_region(src, dst_surface, dst);

  src = position_in_tileset;
  dst = dst_position;
  dst.x += src.get_width() - offset_x;
  src.set_width(offset_x);
  dst.y += src.get_height() - offset_y;
  src.set_height(offset_y);
  tileset.get_tiles_image()->draw_region(src, dst_surface, dst);
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
bool TimeScrollingTilePattern::is_animated() const {
  return true;
}

}

