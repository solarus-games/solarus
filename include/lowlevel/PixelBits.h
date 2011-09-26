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
#ifndef SOLARUS_PIXEL_BITS_H
#define SOLARUS_PIXEL_BITS_H

#include "Common.h"

/**
 * @brief Provides pixel-perfect collision checks for a surface.
 *
 * This class stores efficiently the location of the non-transparent pixels of a surface.
 * For each pixel of the image, a bit indicates whether this pixel is transparent.
 * This class perform fast pixel-perfect collision checks.
 * Note that only 8-bit images are supported.
 */
class PixelBits {

  private:

    int width;               /**< width of the image in pixels */
    int height;              /**< height of the image in pixels */
    int nb_integers_per_row; /**< number of uint32_t necessary to store
                              * the bits of a row of the image */

    uint32_t** bits;         /**< a two-dimensional array representing the
                              * transparency bit of each pixel in the image */

    void print() const;
    void print_mask(uint32_t mask) const;

  public:

    PixelBits(Surface& surface, const Rectangle& image_position);
    ~PixelBits();

    bool test_collision(const PixelBits& other, const Rectangle& location1, const Rectangle& location2) const;
};

#endif

