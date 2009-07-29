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
#ifndef ZSDX_DYNAMIC_TILE_H
#define ZSDX_DYNAMIC_TILE_H

#include "Common.h"
#include "entities/MapEntity.h"
#include <istream>

/**
 * A dynamic tile is a tile placed on the map 
 * that can be enabled or disabled by the script,
 * contrary to the TileOnMap instances that are static for performance reasons.
 */
class DynamicTile: public MapEntity {

  private:

    int tile_pattern_id;       /**< id of the tile pattern */
    TilePattern *tile_pattern; /**< pattern of the tile: instance of SimpleTile or AnimatedTile */
    bool enabled;              /**< indicates that the tile is enabled (visible) */
    bool waiting_enabled;      /**< indicates that the tile is waiting to be enabled */

  public:

    DynamicTile(const std::string &name, Layer layer, int x, int y,
	int width, int height, int tile_pattern_id, bool visible);
    ~DynamicTile(void);
    static DynamicTile * create_from_stream(std::istream &is, Layer layer, int x, int y);

    EntityType get_type(void);
    void set_map(Map *map);
    bool is_obstacle_for(MapEntity *other);
    void display_on_map(void);
    void update(void);

    bool is_enabled(void);
    void set_enabled(bool enabled);
};

#endif

