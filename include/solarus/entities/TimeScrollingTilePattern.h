/*
 * Copyright (C) 2006-2014 Christopho, Solarus - http://www.solarus-games.org
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
#ifndef SOLARUS_TIME_SCROLLING_TILE_PATTERN_H
#define SOLARUS_TIME_SCROLLING_TILE_PATTERN_H

#include "solarus/Common.h"
#include "solarus/entities/SimpleTilePattern.h"

namespace Solarus {

/**
 * \brief Tile pattern with a scrolling effect.
 *
 * The pattern scrolls on itself with time,
 * giving the illusion of a movement.
 */
class TimeScrollingTilePattern: public SimpleTilePattern {

  public:

    TimeScrollingTilePattern(Ground ground, const Point& xy, const Size& size);

    static void update();
    virtual void draw(
        const SurfacePtr& dst_surface,
        const Point& dst_position,
        Tileset& tileset,
        const Point& viewport
    ) override;

    virtual bool is_animated() const override;

  private:

    static int shift;                   /**< number of pixels to shift, increased with the time */
    static uint32_t next_shift_date;    /**< when the shift variable is incremented */

};

}

#endif

