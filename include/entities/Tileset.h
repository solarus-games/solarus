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
#ifndef SOLARUS_TILESET_H
#define SOLARUS_TILESET_H

#include "Common.h"
#include "lowlevel/Color.h"
#include <map>
#include <string>

struct lua_State;

namespace solarus {

/**
 * \brief A compare functor for std:string.
 *
 * This is needed for putting std:string as the key in a STL map.
 */
class strless {
  public:
    bool operator() (const std::string& first, const std::string& second ) const  {
        return first < second;
    }
};
 

/**
 * \brief An image containing all tile patterns.
 *
 * A tileset is an image containing a set of elements (tile patterns)
 * that one can use to compose a map.
 * See the directory images/tilesets of the data package.
 */
class Tileset {

  public:

    Tileset(const std::string& id);
    ~Tileset();

    void load();
    void unload();

    const std::string& get_id();
    Color& get_background_color();
    bool is_loaded();
    Surface& get_tiles_image();
    Surface& get_entities_image();
    TilePattern& get_tile_pattern(const std::string& id);
    void set_images(const std::string& other_id);

    static const std::string ground_names[];  /**< Lua name of each ground type. */

  private:

    void add_tile_pattern(const std::string& id, TilePattern* tile_pattern);

    static int l_background_color(lua_State* l);
    static int l_tile_pattern(lua_State* l);

    const std::string id;                             /**< id of the tileset */
    std::map<std::string, TilePattern*> tile_patterns;        /**< tile patterns in this tileset */
    Color background_color;                           /**< background color of the tileset */
    Surface* tiles_image;                             /**< image from which the tile patterns are extracted */
    Surface* entities_image;                          /**< image from which the skin-dependent entities are extracted */

};

}

#endif

