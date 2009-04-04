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
#ifndef ZSDX_PIXEL_BITS_H
#define ZSDX_PIXEL_BITS_H

#include "Common.h"

/**
 * Represents the non-transparent pixels of a 8-bit image.
 * For each pixel of the image, a bit indicates whether
 * this pixel is transparent.
 * This class allows to perform fast pixel-perfect collision checks.
 */
class PixelBits {

 private:

  int width;               /**< width of the image in pixels */
  int height;              /**< height of the image in pixels */
  int nb_integers_per_row; /**< number of Uint32 necessary to store
			    * the bits of a row of the image */

  Uint32 **bits;           /**< two-dimensional array representing the
			    * transparency bit of each pixel in the image */

  bool check_rectangle_collision(SDL_Rect &bounding_box1, SDL_Rect &bounding_box2);

 public:

  PixelBits(SDL_Surface *surface, SDL_Rect &image_position);
  ~PixelBits(void);

  bool check_collision(PixelBits *other, SDL_Rect &location1, SDL_Rect &location2);
};

#endif
