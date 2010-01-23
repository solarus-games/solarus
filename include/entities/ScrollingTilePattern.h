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
#ifndef SOLARUS_SCROLLING_TILE_PATTERN_H
#define SOLARUS_SCROLLING_TILE_PATTERN_H

#include "Common.h"
#include "entities/SimpleTilePattern.h"

/**
 * Tile pattern with a scrolling effect.
 */
class ScrollingTilePattern: public SimpleTilePattern {

  private:

    static int shift;                /**< number of pixels to shift, increased with the time */
    static uint32_t next_shift_date; /**< when the shift variable is incremented */

  public:

    ScrollingTilePattern(Obstacle obstacle, int x, int y, int width, int height);
    ~ScrollingTilePattern(void);

    static void update(void);
    void display(Surface *destination, const Rectangle &destination_position, Surface *tileset_image);
};

#endif

