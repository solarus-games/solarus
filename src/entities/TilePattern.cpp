/*
 * Copyright (C) 2009 Christopho, Zelda Solarus - http://www.zelda-solarus.com
 * 
 * Zelda: Mystery of Solarus DX is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Zelda: Mystery of Solarus DX is distributed in the hope that it will be useful,
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

/**
 * Constructor.
 * It is called by the subclasses. 
 * @param obstacle type of obstacle
 * @param width width of the tile in pixels (must be a multiple of 8)
 * @param height height of the tile in pixels (must be a multiple of 8)
 */
TilePattern::TilePattern(Obstacle obstacle, int width, int height):
  obstacle(obstacle), width(width), height(height) {

  // check the width and the height
  if (width <= 0 || height <= 0 ||
      width % 8 != 0 || height % 8 != 0) {
    DIE("Invalid tile pattern: the size is (" << width << "x" << height <<
	") but should be positive and multiple of 8 pixels");
  }

  // diagonal obstacle: check that the tile is square
  if (obstacle >= OBSTACLE_TOP_RIGHT && obstacle <= OBSTACLE_BOTTOM_RIGHT
      && width != height) {
    DIE("Invalid tile pattern: a tile pattern with a diagonal obstacle must be square");
  }
}

/**
 * Destructor.
 */
TilePattern::~TilePattern(void) {

}

/**
 * Returns the width of the tile.
 * @return the width of the tile 
 */
int TilePattern::get_width(void) const {
  return width;
}

/**
 * Returns the height of the tile.
 * @return the height of the tile 
 */
int TilePattern::get_height(void) const {
  return height;
}

/**
 * Returns the obstacle property of this tile pattern.
 * @return the obstacle property of this tile pattern
 */
Obstacle TilePattern::get_obstacle(void) const {
  return obstacle;
}

/**
 * Updates the current frame of all tile patterns.
 * This function is called repeatedly by the map.
 */
void TilePattern::update(void) {
  AnimatedTilePattern::update();
  ScrollingTilePattern::update();
}

/**
 * Displays the tile pattern on the map, repeating it to fit the specified size.
 * @param map the map
 * @param position_in_map location and size of the tile on the map
 */
void TilePattern::display_on_map(Map *map, SDL_Rect &position_in_map) {

  SDL_Surface *map_surface = map->get_visible_surface();
  SDL_Surface *tileset_image = map->get_tileset()->get_tiles_image();

  SDL_Rect dst;
  const SDL_Rect &camera_position = map->get_camera_position();

  dst.w = get_width();
  dst.h = get_height();

  int limit_x = position_in_map.x - camera_position.x + position_in_map.w;
  int limit_y = position_in_map.y - camera_position.y + position_in_map.h;

  for (dst.y = position_in_map.y - camera_position.y; dst.y < limit_y; dst.y += dst.h) {

    if (dst.y <= 240 && dst.y + dst.h > 0) {

      for (dst.x = position_in_map.x - camera_position.x; dst.x < limit_x; dst.x += dst.w) {

	if (dst.x <= 320 && dst.x + dst.w > 0) {
	  display(map_surface, dst, tileset_image);
	}
      }
    }
  }
}

