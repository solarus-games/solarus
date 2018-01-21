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
#include "solarus/graphics/Scale2xFilter.h"

namespace Solarus {

/**
 * \brief Constructor.
 */
Scale2xFilter::Scale2xFilter():
  SoftwarePixelFilter() {
}

/**
 * \copydoc PixelFilter::get_scaling_factor
 */
int Scale2xFilter::get_scaling_factor() const {
  return 2;
}

/**
 * \copydoc PixelFilter::filter
 */
void Scale2xFilter::filter(
    const uint32_t* src,
    int src_width,
    int src_height,
    uint32_t* dst) const {

  const int dst_width = src_width * 2;

  int e1 = 0;
  int e2, e3, e4;
  int b, d, e = 0, f,  h;
  for (int row = 0; row < src_height; row++) {
    for (int col = 0; col < src_width; col++) {

      // compute a to i

      b = e - src_width;
      d = e - 1;
      f = e + 1;
      h = e + src_width;

      if (row == 0) {
        b = e;
      }
      if (row == src_height - 1) {
        h = e;
      }
      if (col == 0) {
        d = e;
      }
      if (col == src_width - 1) {
        f = e;
      }

      // compute e1 to e4
      e2 = e1 + 1;
      e3 = e1 + dst_width;
      e4 = e3 + 1;

      // compute the color

      if (src[b] != src[h] && src[d] != src[f]) {
        dst[e1] = src[(src[d] == src[b]) ? d : e];
        dst[e2] = src[(src[b] == src[f]) ? f : e];
        dst[e3] = src[(src[d] == src[h]) ? d : e];
        dst[e4] = src[(src[h] == src[f]) ? f : e];
      }
      else {
        dst[e1] = dst[e2] = dst[e3] = dst[e4] = src[e];
      }
      e1 += 2;
      e++;
    }
    e1 += dst_width;
  }

}

}

