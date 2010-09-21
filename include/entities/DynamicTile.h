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
#ifndef SOLARUS_DYNAMIC_TILE_H
#define SOLARUS_DYNAMIC_TILE_H

#include "Common.h"
#include "entities/MapEntity.h"

/**
 * @brief A special tile that can be enabled or disabled.
 *
 * A dynamic tile is a tile placed on the map 
 * that can be enabled or disabled by the script,
 * contrary to the Tile instances that are totally static for performance reasons.
 * An enabled dynamic tile behaves like a normal tile and may be an obstacle.
 * A disabled dynamic tile is invisible and can be traversed.
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
    ~DynamicTile();
    static CreationFunction parse;

    EntityType get_type();
    void set_map(Map *map);
    bool is_obstacle_for(MapEntity *other);
    void display_on_map();
    void update();

    bool is_enabled();
    void set_enabled(bool enabled);
};

#endif

