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
#include "entities/ParallaxTilePattern.h"
#include "lowlevel/Surface.h"

/**
 * @brief Creates a tile pattern with parallax scrolling.
 * @param obstacle is the tile pattern an obstacle?
 * @param x x position of the tile pattern in the tileset
 * @param y y position of the tile pattern in the tileset
 * @param width width of the tile pattern in the tileset
 * @param height height of the tile pattern in the tileset
 */
ParallaxTilePattern::ParallaxTilePattern(Obstacle obstacle, int x, int y, int width, int height):
  SimpleTilePattern(obstacle, x, y, width, height) {

}

/**
 * @brief Destructor.
 */
ParallaxTilePattern::~ParallaxTilePattern() {

}

/**
 * @brief Displays the tile image on a surface.
 * @param destination the destination surface
 * @param dst_position position of the tile pattern on the destination surface
 * @param tileset_image the tileset image of this tile
 */
void ParallaxTilePattern::display(Surface *destination, const Rectangle &dst_position, Surface *tileset_image) {

  Rectangle src = position_in_tileset;
  Rectangle dst = dst_position;

  int offset_x, offset_y; // display the tile with an offset that depends on its position modulo its size

  if (dst.get_x() >= 0) {
    offset_x = dst.get_x() % src.get_width();
  }
  else { // the modulo operation does not like negative numbers
    offset_x = src.get_width() - (-dst.get_x() % src.get_width());
  }

  if (dst.get_y() >= 0) {
    offset_y = dst.get_y() % src.get_height();
  }
  else {
    offset_y = src.get_height() - (-dst.get_y() % src.get_height());
  }

  // apply a scrolling ratio
  offset_x /= 2;
  offset_y /= 2;

  src.add_x(offset_x);
  src.add_width(-offset_x);
  src.add_y(offset_y);
  src.add_height(-offset_y);
  tileset_image->blit(src, destination, dst);

  src = position_in_tileset;
  dst = dst_position;
  src.add_y(offset_y);
  src.add_height(-offset_y);
  dst.add_x(src.get_width() - offset_x);
  src.set_width(offset_x);
  tileset_image->blit(src, destination, dst);

  src = position_in_tileset;
  dst = dst_position;
  src.add_x(offset_x);
  src.add_width(-offset_x);
  dst.add_y(src.get_height() - offset_y);
  src.set_height(offset_y);
  tileset_image->blit(src, destination, dst);

  src = position_in_tileset;
  dst = dst_position;
  dst.add_x(src.get_width() - offset_x);
  src.set_width(offset_x);
  dst.add_y(src.get_height() - offset_y);
  src.set_height(offset_y);
  tileset_image->blit(src, destination, dst);

}

