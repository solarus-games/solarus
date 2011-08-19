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
#ifndef SOLARUS_PARALLAX_TILE_PATTERN_H
#define SOLARUS_PARALLAX_TILE_PATTERN_H

#include "Common.h"
#include "entities/SimpleTilePattern.h"

/**
 * @brief Tile pattern with a parallax scrolling effect.
 *
 * The pattern shifts when the camera moves, giving
 * an illusion of depth.
 */
class ParallaxTilePattern: public SimpleTilePattern {

  public:

    ParallaxTilePattern(Obstacle obstacle, int x, int y, int width, int height);
    ~ParallaxTilePattern();

    void display(Surface *destination, const Rectangle &destination_position, Tileset &tileset);

    virtual bool is_animated();
};

#endif

