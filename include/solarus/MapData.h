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
 * \brief Type of a field of an entity in the map data file.
 */
enum class EntityFieldType {
    NIL,      /**< A field that does not exist. */
    STRING,
    INTEGER,
    BOOLEAN
};

/**
 * \brief Stores the value of one field of the entity.
 */
struct FieldValue {

    FieldValue();
    explicit FieldValue(const std::string& value);
    explicit FieldValue(int value);
    explicit FieldValue(bool value);

    const EntityFieldType value_type;
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

    EntityData(EntityType type);

    EntityType get_type() const;

    bool has_name() const;
    std::string get_name() const;
    void set_name(const std::string& name);
    Layer get_layer() const;
    void set_layer(Layer layer);
    Point get_xy() const;
    void set_xy(const Point& xy);

    const std::map<std::string, FieldValue>& get_fields() const;
    FieldValue get_field(const std::string& key) const;
    bool is_string(const std::string& key) const;
    const std::string& get_string(const std::string& key) const;
    void set_string(const std::string& key, const std::string& value);
    bool is_integer(const std::string& key) const;
    int get_integer(const std::string& key) const;
    void set_integer(const std::string& key, int value);
    bool is_boolean(const std::string& key) const;
    bool get_boolean(const std::string& key) const;
    void set_boolean(const std::string& key, bool value);

  private:

    EntityType type;    /**< Type of entity. */
    std::string name;   /**< Unique name of the entity on the map. */
    Layer layer;        /**< Layer of the entity on the map. */
    Point xy;           /**< Entity position on the map. */

    std::map<std::string, FieldValue>
        fields;         /**< Fields specific to the entity type. */

};

/**
 * \brief A list of entities on a layer of the map.
 *
 * The entity at index 0 is the most to the back.
 */
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
    void set_location(const Point& location);
    bool has_world() const;
    const std::string& get_world() const;
    void set_world(const std::string& world);
    bool has_floor() const;
    int get_floor() const;
    void set_floor(int floor);
    const std::string& get_tileset_id() const;
    void set_tileset_id(const std::string& tileset_id);
    const std::string& get_music_id() const;
    void set_music_id(const std::string& music_id);

    int get_num_entities() const;
    int get_num_entities(Layer layer) const;

    void set_entity_layer(Layer src_layer, int src_index, Layer dst_layer);
    void bring_entity_to_front(Layer layer, int index);
    void bring_entity_to_back(Layer layer, int index);

    const EntityData& get_entity(Layer layer, int index) const;
    EntityData& get_entity(Layer layer, int index);
    const EntityData* get_entity_by_name(const std::string& name) const;
    EntityData* get_entity_by_name(const std::string& name);
    bool entity_exists(const std::string& name) const;
    bool set_entity_name(Layer layer, int index, const std::string& name);
    int get_entity_index(const EntityData& entity);

    bool add_entity(const EntityData& entity);
    void remove_entity(Layer layer, int index);
    void remove_entity(const EntityData& entity);

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
    std::map<std::string, std::pair<Layer, int>>
        named_entities;           /**< Entities indexed by their name. */

};

}

#endif


