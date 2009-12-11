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
    int nb_integers_per_row; /**< number of uint32_t necessary to store
			      * the bits of a row of the image */

    uint32_t **bits;         /**< a two-dimensional array representing the
			      * transparency bit of each pixel in the image */

    void print(void);
    void print_mask(uint32_t mask);

  public:

    PixelBits(Surface *surface, const Rectangle &image_position);
    ~PixelBits(void);

    bool test_collision(PixelBits *other, const Rectangle &location1, const Rectangle &location2);
};

#endif

