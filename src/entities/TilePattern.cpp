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
#include "entities/ScrollingTilePattern.h"
#include "entities/Tileset.h"
#include "Map.h"
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
  Debug::assert(width > 0
      && height > 0
      && width % 8 == 0
      && height % 8 == 0,
      StringConcat() << "Invalid tile pattern: the size is (" << width << "x" << height <<
      ") but should be positive and multiple of 8 pixels");

  // diagonal obstacle: check that the tile is square
  Debug::assert(obstacle < OBSTACLE_TOP_RIGHT
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
  ScrollingTilePattern::update();
}

/**
 * @brief Displays the tile pattern on the map, repeating it to fit the specified size.
 * @param map the map
 * @param position_in_map location and size of the tile on the map
 */
void TilePattern::display_on_map(Map *map, const Rectangle &position_in_map) {

  Surface *map_surface = map->get_visible_surface();
  Surface *tileset_image = map->get_tileset().get_tiles_image();

  Rectangle dst;
  const Rectangle &camera_position = map->get_camera_position();

  dst.set_size(get_width(), get_height());

  int limit_x = position_in_map.get_x() - camera_position.get_x() + position_in_map.get_width();
  int limit_y = position_in_map.get_y() - camera_position.get_y() + position_in_map.get_height();

  for (int y = position_in_map.get_y() - camera_position.get_y(); y < limit_y; y += dst.get_height()) {

    if (y <= 240 && y + dst.get_height() > 0) {
      dst.set_y(y);

      for (int x = position_in_map.get_x() - camera_position.get_x(); x < limit_x; x += dst.get_width()) {

	if (x <= 320 && x + dst.get_width() > 0) {
	  dst.set_x(x);
	  display(map_surface, dst, tileset_image);
	}
      }
    }
  }
}

