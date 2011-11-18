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
#include "entities/TilePattern.h"
#include "entities/AnimatedTilePattern.h"
#include "entities/TimeScrollingTilePattern.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"

/**
 * @brief Constructor.
 *
 * It is called by the subclasses. 
 * 
 * @param obstacle type of obstacle
 * @param width width of the tile in pixels (must be a multiple of 8)
 * @param height height of the tile in pixels (must be a multiple of 8)
 */
TilePattern::TilePattern(Obstacle obstacle, int width, int height):
  obstacle(obstacle), width(width), height(height) {

  // check the width and the height
  Debug::check_assertion(width > 0
      && height > 0
      && width % 8 == 0
      && height % 8 == 0,
      StringConcat() << "Invalid tile pattern: the size is (" << width << "x" << height <<
      ") but should be positive and multiple of 8 pixels");

  // diagonal obstacle: check that the tile is square
  Debug::check_assertion(obstacle < OBSTACLE_TOP_RIGHT
      || obstacle > OBSTACLE_BOTTOM_RIGHT
      || width == height,
      "Invalid tile pattern: a tile pattern with a diagonal obstacle must be square");
}

/**
 * @brief Destructor.
 */
TilePattern::~TilePattern() {

}

/**
 * @brief Returns the width of the tile pattern.
 * @return the width of the tile 
 */
int TilePattern::get_width() const {
  return width;
}

/**
 * @brief Returns the height of the tile pattern.
 * @return the height of the tile 
 */
int TilePattern::get_height() const {
  return height;
}

/**
 * @brief Returns the obstacle property of this tile pattern.
 * @return the obstacle property of this tile pattern
 */
Obstacle TilePattern::get_obstacle() const {
  return obstacle;
}

/**
 * @brief Updates the current frame of all tile patterns.
 *
 * This function is called repeatedly by the map.
 */
void TilePattern::update() {
  AnimatedTilePattern::update();
  TimeScrollingTilePattern::update();
}

/**
 * @brief Returns whether this tile pattern is animated, i.e. not always displayed
 * the same way.
 *
 * Non-animated tiles may be rendered faster by using intermediate surfaces
 * that are drawn only once.
 * This function should return false if the tile pattern is always displayed the same way.
 * Returns true by default.
 *
 * @return true if this tile pattern is animated
 */
bool TilePattern::is_animated() {
  return true;
}

/**
 * @brief Returns whether tiles having this tile pattern are displayed at their
 * position.
 *
 * Usually, this function returns true, and when it is the case, display() is
 * called only for tiles that are located in the current viewport.
 *
 * However, some tile patterns may want to be displayed even when they are not
 * in the viewport, typically to make an illusion of movement like parallax
 * scrolling.
 *
 * @return true if tiles having this pattern are displayed where they are placed
 */
bool TilePattern::is_displayed_at_its_position() {
  return true;
}

