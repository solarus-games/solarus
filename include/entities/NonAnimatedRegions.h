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
#ifndef SOLARUS_NON_ANIMATED_REGIONS_H
#define SOLARUS_NON_ANIMATED_REGIONS_H

#include "Common.h"
#include "entities/Layer.h"
#include <vector>

namespace solarus {

/**
 * \brief Manages the tiles that are in non-animated regions.
 *
 * A non-animated region is a rectangle of the map that contains no animated
 * tile. The tiles in such rectangles of the map can be pre-drawn once for all
 * on an intermediate surface for performance.
 */
class NonAnimatedRegions {

  public:

    NonAnimatedRegions(Map& map, Layer layer);
    ~NonAnimatedRegions();

    void clear();
    void add_tile(Tile* tile);
    void build(std::vector<Tile*>& rejected_tiles);
    void notify_tileset_changed();
    void draw_on_map();

  private:

    bool overlaps_animated_tile(Tile& tile) const;

    Map& map;                               /**< The map. */
    Layer layer;                            /**< Layer of the map managed by this object. */
    std::vector<Tile*> tiles;               /**< The tiles contained in this layer.
                                             * After build() is called, rejected ones are removed. */
    Surface* non_animated_tiles_surface;    /**< All non-animated tiles are drawn here once for all
                                             * for performance. */
    std::vector<bool> are_squares_animated; /**< Whether each 8x8 square of the map has animated tiles. */

};

}

#endif

