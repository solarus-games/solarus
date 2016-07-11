/*
 * Copyright (C) 2006-2016 Christopho, Solarus - http://www.solarus-games.org
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
#ifndef SOLARUS_BLEND_MODE_H
#define SOLARUS_BLEND_MODE_H

namespace Solarus {

/**
 * \brief Possible blend modes for drawable objects.
 */
enum class BlendMode {

  NO_BLENDING,          /**< No blending: the destination is replaced by the source. */
  ALPHA_BLENDING,       /**< Alpha blending (default). */
  ADDITIVE_BLENDING,    /**< Additive blending. */
  COLOR_MODULATE        /**< Color modulate. */
};

}

#endif

