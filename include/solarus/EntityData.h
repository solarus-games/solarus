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
#ifndef SOLARUS_ENTITY_DATA_H
#define SOLARUS_ENTITY_DATA_H

#include "solarus/Common.h"
#include "solarus/entities/EntityType.h"
#include "solarus/entities/Layer.h"
#include "solarus/lowlevel/Point.h"
#include "solarus/lua/LuaData.h"
#include <map>
#include <string>
#include <vector>

namespace Solarus {

/**
 * \brief Stores the properties of a map entity.
 *
 * Properties includes its name, layer, coordinates and properties specific
 * to each entity type.
 */
class SOLARUS_API EntityData {

  public:

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
        explicit FieldValue(const char* value);
        explicit FieldValue(int value);
        explicit FieldValue(bool value);

        const EntityFieldType value_type;
        std::string string_value;
        int int_value;  // Also used for boolean.
    };

    /**
     * \brief Whether a field is optional for an entity of some type.
     */
    enum class OptionalFlag {
        MANDATORY,
        OPTIONAL
    };

    /**
     * \brief Describes a field of an entity type.
     */
    struct EntityFieldDescription {

        std::string key;              /**< Name of the field. */
        OptionalFlag optional;        /**< Whether the field is optional. */
        FieldValue default_value;     /**< Default value (also determines the value type). */

    };

    using EntityTypeDescription = std::vector<EntityFieldDescription>;

    explicit EntityData(EntityType type);

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

    static EntityData check_entity_data(lua_State* l, int index, EntityType type);
    static const std::map<EntityType, const EntityTypeDescription> get_entity_type_descriptions();

  private:

    EntityType type;    /**< Type of entity. */
    std::string name;   /**< Unique name of the entity on the map. */
    Layer layer;        /**< Layer of the entity on the map. */
    Point xy;           /**< Entity position on the map. */

    std::map<std::string, FieldValue>
        fields;         /**< Fields specific to the entity type. */

};

}  // namespace Solarus

#endif
