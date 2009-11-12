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
#include "entities/ScrollingTilePattern.h"

int ScrollingTilePattern::shift = 0;
Uint32 ScrollingTilePattern::next_shift_date = 0;

/**
 * Creates a tile pattern with scrolling.
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
 * Destructor.
 */
ScrollingTilePattern::~ScrollingTilePattern(void) {

}

/**
 * Updates all scrolling tiles patterns.
 * This function is called repeatedly by the map.
 */
void ScrollingTilePattern::update(void) {

  Uint32 now = SDL_GetTicks();

  while (now >= next_shift_date) {
    shift++;
    next_shift_date += 50;
  }
}


/**
 * Displays the tile image on a surface.
 * @param destination the destination surface
 * @param dst_position position of the tile pattern on the destination surface
 * @param tileset_image the tileset image of this tile
 */
void ScrollingTilePattern::display(SDL_Surface *destination, const SDL_Rect &dst_position, SDL_Surface *tileset_image) {

  SDL_Rect src = position_in_tileset;
  SDL_Rect dst = dst_position;

  int offset_x, offset_y; // display the tile with an offset that depends on the time

  offset_x = src.w - (shift % src.w);
  offset_y = shift % src.h;

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

