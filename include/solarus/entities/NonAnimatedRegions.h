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
#ifndef SOLARUS_NON_ANIMATED_REGIONS_H
#define SOLARUS_NON_ANIMATED_REGIONS_H

#include "solarus/core/Common.h"
#include "solarus/containers/Grid.h"
#include "solarus/entities/TileInfo.h"
#include "solarus/graphics/SurfacePtr.h"
#include <vector>

namespace Solarus {

class Map;

/**
 * \brief Manages the tiles that are in non-animated regions.
 *
 * A non-animated region is a rectangle of the map that contains no animated
 * tile. The tiles in such rectangles of the map can be pre-drawn once for all
 * on an intermediate surface for performance. Furthermore, this intermediate
 * surface is drawn lazily when the camera moves.
 */
class NonAnimatedRegions {

  public:

    NonAnimatedRegions(Map& map, int layer);

    void add_tile(const TileInfo& tile);
    void build(std::vector<TileInfo>& rejected_tiles);
    void notify_tileset_changed();
    void draw_on_map();

  private:

    bool overlaps_animated_tile(const TileInfo& tile) const;
    void build_cell(int cell_index);

    Map& map;                               /**< The map. */
    int layer;                              /**< Layer of the map managed by this object. */
    std::vector<TileInfo>
        tiles;                              /**< All tiles contained in this layer and candidates to
                                             * be optimized. This list is cleared after build() is called. */
    std::vector<bool> are_squares_animated; /**< Whether each 8x8 square of the map has animated tiles. */

    // Handle the lazy drawing.
    Grid<TileInfo>
        non_animated_tiles;                 /**< All non-animated tiles. Stored in a grid so that
                                             * we can quickly find the ones to draw lazily later when the
                                             * camera moves. */
    std::vector<SurfacePtr>
        optimized_tiles_surfaces;           /**< All non-animated tiles are drawn here once for all
                                             * for performance. Each cell of the grid has a surface
                                             * or nullptr before it is drawn. */

};

}

#endif

