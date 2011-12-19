/*
 * Copyright (C) 2009-2011 Christopho, Solarus - http://www.solarus-engine.org
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
#include "entities/MapEntity.h"

/**
 * @brief A small fixed piece of the map.
 *
 * It is composed of a tile pattern that can be repeated.
 * It can be an obstacle or not.
 * A tile cannot be removed from the map. Its pattern may be animated.
 * Tiles are optimized to allow fast detection of obstacles and
 * fast rendering of the non-animated ones.
 */
class Tile: public MapEntity {

  private:

    int tile_pattern_id;            /**< id of the tile pattern */
    TilePattern* tile_pattern;      /**< pattern of the tile */

  public:

    Tile(Layer layer, int x, int y, int width, int height, int tile_pattern_id);
    ~Tile();
    static CreationFunction parse;

    EntityType get_type();
    void set_map(Map& map);
    void display_on_map();
    void display(Surface* dst_surface, const Rectangle& viewport);
    TilePattern& get_tile_pattern();
    bool is_animated();
};

#endif

