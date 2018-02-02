/*
 * Copyright (C) 2006-2018 Christopho, Solarus - http://www.solarus-games.org
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

#include "solarus/core/Common.h"
#include "solarus/core/Point.h"
#include "solarus/entities/EntityType.h"
#include "solarus/lua/LuaData.h"
#include <iosfwd>
#include <map>
#include <string>
#include <vector>

namespace Solarus {

/**
 * \brief Stores the properties of a map entity.
 *
 * Some properties (or fields) are common to all entity types:
 * name (except tiles), layer, x, y and custom properties.
 *
 * Properties that are specific to each entity type
 * and are called entity specific fields.
 */
class SOLARUS_API EntityData : public LuaData {

  public:

    /**
     * \brief Type of an entity specific field in the map data file.
     */
    enum class EntityFieldType {
        NIL,      /**< A field that does not exist. */
        STRING,
        INTEGER,
        BOOLEAN
    };

    /**
     * \brief Stores the value of an entity specific field of the entity.
     */
    struct SOLARUS_API FieldValue {

        FieldValue();
        explicit FieldValue(const std::string& value);
        explicit FieldValue(const char* value);
        explicit FieldValue(int value);
        explicit FieldValue(bool value);

        bool operator==(const FieldValue& other) const;
        bool operator!=(const FieldValue& other) const;

        EntityFieldType value_type;
        std::string string_value;
        int int_value;  // Also used for boolean.
    };

    /**
     * \brief Whether an entity specific field is optional.
     */
    enum class OptionalFlag {
        MANDATORY,
        OPTIONAL
    };

    /**
     * \brief Describes an entity specific field.
     */
    struct EntityFieldDescription {

        std::string key;              /**< Name of the field. */
        OptionalFlag optional;        /**< Whether the field is optional. */
        FieldValue default_value;     /**< Default value (also determines the value type). */
    };

    /**
     * \brief List of entity specific fields for an entity type.
     */
    using EntityTypeDescription = std::vector<EntityFieldDescription>;

    /**
     * \brief Property defined by the user as any string key-value pair.
     */
    using UserProperty = std::pair<std::string, std::string>;

    EntityData();
    explicit EntityData(EntityType type);

    EntityType get_type() const;
    void set_type(EntityType type);
    const std::string& get_type_name() const;
    bool is_dynamic() const;

    // Common properties.
    bool has_name() const;
    std::string get_name() const;
    void set_name(const std::string& name);
    int get_layer() const;
    void set_layer(int layer);
    Point get_xy() const;
    void set_xy(const Point& xy);

    static bool is_user_property_key_valid(const std::string& key);
    const std::vector<UserProperty>& get_user_properties() const;
    void set_user_properties(const std::vector<UserProperty>& user_properties);
    int get_user_property_count() const;
    const UserProperty& get_user_property(int index) const;
    void set_user_property(int index, const UserProperty& user_property);
    void add_user_property(const UserProperty& user_property);
    void remove_user_property(int index);
    int get_user_property_index(const std::string& key) const;
    bool has_user_property(const std::string& key) const;

    // Specific properties.
    void initialize_specific_properties();
    const std::map<std::string, FieldValue>& get_specific_properties() const;
    FieldValue get_specific_property(const std::string& key) const;
    bool is_string(const std::string& key) const;
    const std::string& get_string(const std::string& key) const;
    void set_string(const std::string& key, const std::string& value);
    bool is_integer(const std::string& key) const;
    int get_integer(const std::string& key) const;
    void set_integer(const std::string& key, int value);
    bool is_boolean(const std::string& key) const;
    bool get_boolean(const std::string& key) const;
    void set_boolean(const std::string& key, bool value);
    bool has_specific_property(const std::string& key) const;
    bool is_specific_property_optional(const std::string& key) const;
    bool is_specific_property_unset(const std::string& key) const;

    bool import_from_lua(lua_State* l) override;
    bool export_to_lua(std::ostream& out) const override;

    static EntityData check_entity_data(lua_State* l, int index, EntityType type);
    static const std::map<EntityType, const EntityTypeDescription> get_entity_type_descriptions();

  private:

    void export_user_properties(std::ostream& out) const;
    void export_specific_properties(std::ostream& out) const;

    EntityType type;              /**< Type of entity. */

    // Common properties.
    std::string name;             /**< Unique name of the entity on the map. */
    int layer;                    /**< Layer of the entity on the map. */
    Point xy;                     /**< Entity position on the map. */
    std::vector<UserProperty>
        user_properties;          /**< User-defined properties. */

    // Specific properties.
    std::map<std::string, FieldValue>
        specific_properties;      /**< Additional properties specific to the entity type. */

};

}  // namespace Solarus

#endif
