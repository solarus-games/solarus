/*
 * Copyright (C) 2006-2015 Christopho, Solarus - http://www.solarus-games.org
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
#ifndef SOLARUS_TILESET_H
#define SOLARUS_TILESET_H

#include "solarus/Common.h"
#include "solarus/lowlevel/Color.h"
#include "solarus/lowlevel/SurfacePtr.h"
#include <map>
#include <memory>
#include <string>

struct lua_State;

namespace Solarus {

class TilePattern;
class TilePatternData;

/**
 * \brief A set of tile patterns that are used to compose a map.
 *
 * A tileset represents the skin of a map.
 */
class Tileset {

  public:

    Tileset(const std::string& id);

    void load();
    void unload();

    const std::string& get_id();
    Color& get_background_color();
    bool is_loaded();
    const SurfacePtr& get_tiles_image();
    const SurfacePtr& get_entities_image();
    TilePattern& get_tile_pattern(const std::string& id);
    void set_images(const std::string& other_id);

  private:

    void add_tile_pattern(
        const std::string& id,
        const TilePatternData& pattern_data
    );

    const std::string id;                             /**< id of the tileset */
    std::map<std::string, std::unique_ptr<TilePattern>>
        tile_patterns;                                /**< tile patterns in this tileset */
    Color background_color;                           /**< background color of the tileset */
    SurfacePtr tiles_image;                           /**< image from which the tile patterns are extracted */
    SurfacePtr entities_image;                        /**< image from which the skin-dependent entities are extracted */

};

}

#endif

