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
#ifndef ZSDX_SCROLLING_TILE_PATTERN_H
#define ZSDX_SCROLLING_TILE_PATTERN_H

#include "Common.h"
#include "entities/SimpleTilePattern.h"

/**
 * Tile pattern with a scrolling effect.
 */
class ScrollingTilePattern: public SimpleTilePattern {

  private:

    static int shift;              /**< number of pixels to shift, increased with the time */
    static Uint32 next_shift_date; /**< when the shift variable is incremented */

  public:

    ScrollingTilePattern(Obstacle obstacle, int x, int y, int width, int height);
    ~ScrollingTilePattern(void);

    static void update(void);
    void display(SDL_Surface *destination, const SDL_Rect &destination_position, SDL_Surface *tileset_image);
};

#endif

