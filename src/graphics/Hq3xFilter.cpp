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
#include "solarus/graphics/Hq3xFilter.h"
#include "solarus/graphics/Hq4xFilter.h"
#include "solarus/third_party/hqx/hqx.h"

namespace Solarus {

/**
 * \brief Constructor.
 */
Hq3xFilter::Hq3xFilter():
  SoftwarePixelFilter() {
}

/**
 * \copydoc PixelFilter::get_scaling_factor
 */
int Hq3xFilter::get_scaling_factor() const {
  return 3;
}

/**
 * \copydoc PixelFilter::filter
 */
void Hq3xFilter::filter(
    const uint32_t* src,
    int src_width,
    int src_height,
    uint32_t* dst) const {

  // Make sure hqx is initialized.
  Hq4xFilter::initialize_hqx();

  hq3x_32(const_cast<uint32_t*>(src), dst, src_width, src_height);
}

}

