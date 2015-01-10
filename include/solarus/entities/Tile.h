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
#ifndef SOLARUS_TILE_H
#define SOLARUS_TILE_H

#include "solarus/Common.h"
#include "solarus/entities/MapEntity.h"
#include "solarus/lowlevel/SurfacePtr.h"

namespace Solarus {

class TilePattern;
class Tileset;

/**
 * \brief A small fixed piece of the map, optimized for collisions and drawing.
 *
 * A tile is composed of a tile pattern that can be repeated.
 * Its pattern may be animated. It can be an obstacle or not.
 *
 * Tiles are optimized to allow fast detection of obstacles and
 * fast rendering of the non-animated ones.
 * The cost of these optimizations is that their presence on the map, their
 * position, their size and their obstacle property are fixed.
 * Tiles are added when the map is loaded and they are removed when the map
 * is destroyed.
 *
 * If you need to dynamically enable or disable a tile, see DynamicTile.
 */
class Tile: public MapEntity {

  public:

    Tile(
        Layer layer,
        const Point& xy,
        const Size& size,
        Tileset& tileset,
        const std::string& tile_pattern_id
    );

    virtual EntityType get_type() const override;
    virtual void draw_on_map() override;
    void draw(const SurfacePtr& dst_surface, const Point& viewport);
    TilePattern& get_tile_pattern();
    const std::string& get_tile_pattern_id() const;
    bool is_animated() const;
    virtual bool is_drawn_at_its_position() const override;

  private:

    const std::string tile_pattern_id;       /**< Id of the tile pattern. */
    TilePattern& tile_pattern;         /**< Pattern of the tile. */

};

}

#endif

