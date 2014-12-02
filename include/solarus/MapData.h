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
#ifndef SOLARUS_MAP_DATA_H
#define SOLARUS_MAP_DATA_H

#include "solarus/Common.h"
#include "solarus/entities/Layer.h"
#include "solarus/lowlevel/Point.h"
#include "solarus/lowlevel/Size.h"
#include "solarus/lua/LuaData.h"
#include <array>
#include <deque>
#include <string>

namespace Solarus {

/**
 * \brief Stores the properties of a map entity.
 */
class SOLARUS_API EntityData {

    // TODO
};

/**
 * \brief Stores the contents of a map data file.
 */
class SOLARUS_API MapData : public LuaData {

  public:

    MapData();

    Size get_size() const;
    void set_size(const Size& size);
    Point get_location() const;
    void set_location(const Point& point);
    const std::string& get_world() const;
    void set_world(const std::string& world);
    const std::string& get_tileset_id() const;
    void set_tileset_id(const std::string& tileset_id);
    const std::string& get_music_id() const;
    void set_music_id(const std::string& music_id);

    virtual bool import_from_lua(lua_State* l) override;
    virtual bool export_to_lua(std::ostream& out) const override;

    static constexpr int NO_FLOOR = -9999;  /**< Represents a non-existent floor (nil in Lua data files). */

  private:

    Size size;                    /**< Size of the map in pixels. */
    std::string world;            /**< World of the map or an empty string. */
    Point location;               /**< Coordinates of the upper-left corner of the map in its world. */
    int floor;                    /**< Floor of the map or NO_FLOOR. */
    std::string tileset_id;       /**< Tileset to use as skin for the map. */
    std::string music_id;         /**< Background music id or "none" or "same". */

    std::array<std::deque<EntityData>, LAYER_NB>
        entities;                 /**< The entities on each layer. */
};

}

#endif

