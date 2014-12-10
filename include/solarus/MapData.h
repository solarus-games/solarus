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
#include "solarus/entities/EntityType.h"
#include "solarus/entities/Layer.h"
#include "solarus/lowlevel/Point.h"
#include "solarus/lowlevel/Size.h"
#include "solarus/lua/LuaData.h"
#include <array>
#include <map>
#include <deque>
#include <map>
#include <string>

namespace Solarus {

/**
 * \brief Type of a property value of an entity in the map data file.
 */
enum class EntityPropertyType {
    STRING,
    INTEGER,
    BOOLEAN
};

/**
 * \brief Stores one property of the entity.
 */
struct PropertyValue {

    explicit PropertyValue(const std::string& value);
    explicit PropertyValue(int value);
    explicit PropertyValue(bool value);

    const EntityPropertyType value_type;
    std::string string_value;
    int int_value;  // Also used for boolean.
};

/**
 * \brief Stores the properties of a map entity.
 *
 * Properties includes its name, layer, coordinates and properties specific
 * to each entity type.
 */
class SOLARUS_API EntityData {

  public:

    EntityData();

    std::string get_name() const;
    Layer get_layer() const;

    Point get_xy() const;
    void set_xy(const Point& xy);

    const std::string& get_string(const std::string& key) const;
    void set_string(const std::string& key, const std::string& value);
    int get_integer(const std::string& key) const;
    void set_integer(const std::string& key, int value);
    bool get_boolean(const std::string& key) const;
    void set_boolean(const std::string& key, bool value);

  private:

    friend class MapData;  // Only MapData is allowed to change the name and layer.

    void set_name(const std::string& name);
    void set_layer(Layer layer);

    std::string name;   /**< Unique name of the entity on the map. */
    Layer layer;        /**< Layer of the entity on the map. */
    Point xy;           /**< Entity position on the map. */

    EntityType type;    /**< Type of entity. */
    std::map<std::string, PropertyValue>
        properties;     /**< Properties specific to the entity type. */

};

using EntityList = std::deque<EntityData>;

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

    int get_num_entities() const;
    int get_num_entities(Layer layer) const;

    bool entity_has_name(Layer layer, int index) const;
    std::string get_entity_name(Layer layer, int index) const;
    void set_entity_name(Layer layer, int index, const std::string& name);

    void set_entity_layer(Layer layer, int index, Layer new_layer);
    void bring_entity_to_front(Layer layer, int index);
    void bring_entity_to_back(Layer layer, int index);

    const EntityData& get_entity(Layer layer, int index) const;
    EntityData& get_entity(Layer layer, int index);
    const EntityData& get_entity_by_name(const std::string& name) const;
    EntityData& get_entity_by_name(const std::string& name);

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

    std::array<EntityList, LAYER_NB>
        entities;                 /**< The entities on each layer. */
    std::map<std::string, EntityData*>
        named_entities;           /**< Entities indexed by their name. */
};

}

#endif

