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
#ifndef ZSDX_PARALLAX_TILE_PATTERN_H
#define ZSDX_PARALLAX_TILE_PATTERN_H

#include "Common.h"
#include "entities/SimpleTilePattern.h"

/**
 * Tile pattern with a parallax scrolling effect.
 */
class ParallaxTilePattern: public SimpleTilePattern {

  public:

    ParallaxTilePattern(Obstacle obstacle, int x, int y, int width, int height);
    ~ParallaxTilePattern(void);

    void display(Surface *destination, const Rectangle &destination_position, Surface *tileset_image);
};

#endif

