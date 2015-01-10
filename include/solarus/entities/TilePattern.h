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
#ifndef SOLARUS_TILE_PATTERN_H
#define SOLARUS_TILE_PATTERN_H

#include "solarus/Common.h"
#include "solarus/entities/Ground.h"
#include "solarus/lowlevel/SurfacePtr.h"
#include "solarus/lowlevel/Size.h"

namespace Solarus {

class Point;
class Rectangle;
class Surface;
class Tileset;

/**
 * \brief Abstract class for a tile pattern.
 *
 * A tile pattern defines a rectangle image in a tileset and has a ground property.
 * It may have a special animation.
 * The width and the height of a tile pattern are always multiples or 8.
 */
class TilePattern {

  public:

    virtual ~TilePattern();

    int get_width() const;
    int get_height() const;
    const Size& get_size() const;
    Ground get_ground() const;

    static void update();
    void fill_surface(
        const SurfacePtr& dst_surface,
        const Rectangle& dst_position,
        Tileset& tileset,
        const Point& viewport
    );

    /**
     * \brief Draws the tile image on a surface.
     * \param dst_surface The surface to draw.
     * \param dst_position Position where the tile pattern should be drawn.
     * \param tileset The tileset of this tile.
     * \param viewport Coordinates of the top-left corner of dst_surface
     * relative to the map (may be used for scrolling tiles).
     */
    virtual void draw(
        const SurfacePtr& dst_surface,
        const Point& dst_position,
        Tileset& tileset,
        const Point& viewport
    ) = 0;
    virtual bool is_animated() const;
    virtual bool is_drawn_at_its_position() const;

  protected:

    TilePattern(Ground ground, const Size& size);

    const Ground ground;     /**< Kind of tile. */

    const Size size;         /**< Pattern size (multiple of 8). */

};

}

#endif

