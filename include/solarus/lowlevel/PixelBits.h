/*
 * Copyright (C) 2006-2018 Christopho, Solarus - http://www.solarus-games.org
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

#include "solarus/Common.h"
#include <cstdint>
#include <vector>

namespace Solarus {

class Point;
class Rectangle;
class Surface;

/**
 * \brief Provides pixel-perfect collision checks for a surface.
 *
 * This class stores efficiently the location of the non-transparent pixels of a surface.
 * For each pixel of the image, a bit indicates whether this pixel is transparent.
 * This class perform fast pixel-perfect collision checks.
 */
class PixelBits {

  public:

    PixelBits(const Surface& surface, const Rectangle& image_position);

    bool test_collision(const PixelBits& other,
        const Point& location1, const Point& location2) const;

  private:

    void print() const;
    void print_mask(uint32_t mask) const;

    int width;               /**< width of the image in pixels */
    int height;              /**< height of the image in pixels */
    int nb_integers_per_row; /**< number of uint32_t necessary to store
                              * the bits of a row of the image */

    std::vector<std::vector<uint32_t>>
        bits;                /**< A two-dimensional array representing the
                              * transparency bit of each pixel in the image. */

};

}

#endif

