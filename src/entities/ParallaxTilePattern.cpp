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
#include "entities/ParallaxTilePattern.h"
#include "Map.h"

/**
 * Creates a tile pattern with parallax scrolling.
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
 * Destructor.
 */
ParallaxTilePattern::~ParallaxTilePattern(void) {

}

/**
 * Displays the tile image on a surface.
 * @param destination the destination surface
 * @param dst_position position of the tile pattern on the destination surface
 * @param tileset_image the tileset image of this tile
 */
void ParallaxTilePattern::display(SDL_Surface *destination, const SDL_Rect &dst_position, SDL_Surface *tileset_image) {

  SDL_Rect src = position_in_tileset;
  SDL_Rect dst = dst_position;

  int offset_x, offset_y; // display the tile with an offset that depends on its position modulo its size

  if (dst.x >= 0) {
    offset_x = dst.x % src.w;
  }
  else { // the modulo operation does not like negative numbers
    offset_x = src.w - (-dst.x % src.w);
  }

  if (dst.y >= 0) {
    offset_y = dst.y % src.h;
  }
  else {
    offset_y = src.h - (-dst.y % src.h);
  }

  // apply a scrolling ratio
  offset_x /= 2;
  offset_y /= 2;

  src.x += offset_x;
  src.w -= offset_x;
  src.y += offset_y;
  src.h -= offset_y;
  SDL_BlitSurface(tileset_image, &src, destination, &dst);

  src = position_in_tileset;
  dst = dst_position;
  src.y += offset_y;
  src.h -= offset_y;
  dst.x += src.w - offset_x;
  src.w = offset_x;
  SDL_BlitSurface(tileset_image, &src, destination, &dst);

  src = position_in_tileset;
  dst = dst_position;
  src.x += offset_x;
  src.w -= offset_x;
  dst.y += src.h - offset_y;
  src.h = offset_y;
  SDL_BlitSurface(tileset_image, &src, destination, &dst);

  src = position_in_tileset;
  dst = dst_position;
  dst.x += src.w - offset_x;
  src.w = offset_x;
  dst.y += src.h - offset_y;
  src.h = offset_y;
  SDL_BlitSurface(tileset_image, &src, destination, &dst);

}

