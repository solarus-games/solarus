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
#include "entities/ScrollingTilePattern.h"
#include "entities/Tileset.h"
#include "lowlevel/System.h"
#include "lowlevel/Surface.h"

int ScrollingTilePattern::shift = 0;
uint32_t ScrollingTilePattern::next_shift_date = 0;

/**
 * @brief Creates a tile pattern with scrolling.
 * @param obstacle is the tile pattern an obstacle?
 * @param x x position of the tile pattern in the tileset
 * @param y y position of the tile pattern in the tileset
 * @param width width of the tile pattern in the tileset
 * @param height height of the tile pattern in the tileset
 */
ScrollingTilePattern::ScrollingTilePattern(Obstacle obstacle, int x, int y, int width, int height):
  SimpleTilePattern(obstacle, x, y, width, height) {

}

/**
 * @brief Destructor.
 */
ScrollingTilePattern::~ScrollingTilePattern() {

}

/**
 * @brief Updates all scrolling tiles patterns.
 *
 * This function is called repeatedly by the map.
 */
void ScrollingTilePattern::update() {

  uint32_t now = System::now();

  while (now >= next_shift_date) {
    shift++;
    next_shift_date += 50;
  }
}


/**
 * @brief Displays the tile image on a surface.
 * @param destination the destination surface
 * @param dst_position position of the tile pattern on the destination surface
 * @param tileset the tileset of this tile
 */
void ScrollingTilePattern::display(Surface *destination, const Rectangle &dst_position, Tileset &tileset) {

  Rectangle src = position_in_tileset;
  Rectangle dst = dst_position;

  int offset_x, offset_y; // display the tile with an offset that depends on the time

  offset_x = src.get_width() - (shift % src.get_width());
  offset_y = shift % src.get_height();

  src.add_x(offset_x);
  src.add_width(-offset_x);
  src.add_y(offset_y);
  src.add_height(-offset_y);
  tileset.get_tiles_image()->blit(src, destination, dst);

  src = position_in_tileset;
  dst = dst_position;
  src.add_y(offset_y);
  src.add_height(-offset_y);
  dst.add_x(src.get_width() - offset_x);
  src.set_width(offset_x);
  tileset.get_tiles_image()->blit(src, destination, dst);

  src = position_in_tileset;
  dst = dst_position;
  src.add_x(offset_x);
  src.add_width(-offset_x);
  dst.add_y(src.get_height() - offset_y);
  src.set_height(offset_y);
  tileset.get_tiles_image()->blit(src, destination, dst);

  src = position_in_tileset;
  dst = dst_position;
  dst.add_x(src.get_width() - offset_x);
  src.set_width(offset_x);
  dst.add_y(src.get_height() - offset_y);
  src.set_height(offset_y);
  tileset.get_tiles_image()->blit(src, destination, dst);
}

/**
 * @brief Returns whether this tile pattern is animated, i.e. not always displayed
 * the same way.
 *
 * Non-animated tiles may be rendered faster by using intermediate surfaces
 * that are drawn only once.
 *
 * @return true if this tile pattern is animated
 */
bool ScrollingTilePattern::is_animated() {
  return true;
}

