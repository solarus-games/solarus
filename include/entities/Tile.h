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
#ifndef SOLARUS_TILE_H
#define SOLARUS_TILE_H

#include "Common.h"
#include "MapEntity.h"

/**
 * A tile is a small fixed piece of the map background.
 * It is composed of a tile pattern that can be repeated.
 */
class Tile: public MapEntity {

  private:

    int tile_pattern_id;       /**< id of the tile pattern */
    TilePattern *tile_pattern; /**< pattern of the tile: instance of SimpleTile or AnimatedTile */

  public:

    Tile(Layer layer, int x, int y, int width, int height, int tile_pattern_id);
    ~Tile(void);
    static CreationFunction parse;

    EntityType get_type(void);
    void set_map(Map *map);
    void display_on_map(void);
    TilePattern *get_tile_pattern(void);
};

#endif

