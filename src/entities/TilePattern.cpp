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
#include "solarus/entities/AnimatedTilePattern.h"
#include "solarus/entities/GroundInfo.h"
#include "solarus/entities/TilePattern.h"
#include "solarus/entities/TimeScrollingTilePattern.h"
#include "solarus/lowlevel/Debug.h"
#include "solarus/lowlevel/Surface.h"
#include <sstream>

namespace Solarus {

/**
 * \brief Constructor.
 *
 * It is called by the subclasses.
 *
 * \param ground Kind of ground.
 * \param size Size of the pattern in pixels (must be a multiple of 8).
 */
TilePattern::TilePattern(Ground ground, const Size& size):
  ground(ground),
  size(size) {

  // Check the width and the height.
  if (size.width <= 0 || size.height <= 0
      || size.width % 8 != 0 || size.height % 8 != 0) {
    std::ostringstream oss;
    oss << "Invalid tile pattern: the size is ("
        << size.width << "x" << size.height <<
        ") but should be positive and multiple of 8 pixels";
    Debug::die(oss.str());
  }

  // Diagonal obstacle: check that the tile is square.
  if (GroundInfo::is_ground_diagonal(ground)) {
    Debug::check_assertion(size.is_square(),
        "Invalid tile pattern: a tile pattern with a diagonal wall must be square");
  }
}

/**
 * \brief Destructor.
 */
TilePattern::~TilePattern() {
}

/**
 * \brief Returns the width of the tile pattern.
 * \return The width of the pattern.
 */
int TilePattern::get_width() const {
  return size.width;
}

/**
 * \brief Returns the height of the tile pattern.
 * \return The height of the pattern.
 */
int TilePattern::get_height() const {
  return size.height;
}

/**
 * \brief Returns the size of the tile pattern.
 * \return The size of the pattern.
 */
const Size& TilePattern::get_size() const {
  return size;
}

/**
 * \brief Returns the king of ground of this tile pattern.
 * \return The ground of this tile pattern
 */
Ground TilePattern::get_ground() const {
  return ground;
}

/**
 * \brief Updates the current frame of all tile patterns.
 *
 * This function is called repeatedly by the map.
 */
void TilePattern::update() {
  AnimatedTilePattern::update();
  TimeScrollingTilePattern::update();
}

/**
 * \brief Returns whether this tile pattern is animated, i.e. not always drawn
 * the same way.
 *
 * Non-animated tiles may be rendered faster by using intermediate surfaces
 * that are drawn only once.
 * This function should return false if the tile pattern is always drawn the same way.
 * Returns true by default.
 *
 * \return true if this tile pattern is animated
 */
bool TilePattern::is_animated() const {
  return true;
}

/**
 * \brief Returns whether tiles having this tile pattern are drawn at their
 * position.
 *
 * Usually, this function returns true, and when it is the case, draw() is
 * called only for tiles that are located in the current viewport.
 *
 * However, some tile patterns may want to be drawn even when they are not
 * in the viewport, typically to make an illusion of movement like parallax
 * scrolling.
 *
 * \return true if tiles having this pattern are drawn where they are placed
 */
bool TilePattern::is_drawn_at_its_position() const {
  return true;
}

/**
 * \brief Fills a rectangle by repeating this tile pattern.
 * \param dst_surface The destination surface.
 * \param dst_position Coordinates of the rectangle to fill in \c dst_surface.
 * \param tileset The tileset to use.
 * \param viewport Coordinates of the top-left corner of \c dst_surface
 * relative to the map (may be used for scrolling tiles).
 */
void TilePattern::fill_surface(
    const SurfacePtr& dst_surface,
    const Rectangle& dst_position,
    Tileset& tileset,
    const Point& viewport
) {
  Point dst;

  int limit_x = dst_position.get_x() + dst_position.get_width();
  int limit_y = dst_position.get_y() + dst_position.get_height();

  for (int y = dst_position.get_y();
      y < limit_y;
      y += get_height()) {

    if ((y <= dst_surface->get_height() && y + get_height() > 0)
        || !is_drawn_at_its_position()) {
      dst.y = y;

      for (int x = dst_position.get_x();
          x < limit_x;
          x += get_width()) {

        if ((x <= dst_surface->get_width() && x + get_width() > 0)
            || !is_drawn_at_its_position()) {
          dst.x = x;
          draw(dst_surface, dst, tileset, viewport);
        }
      }
    }
  }
}

}

