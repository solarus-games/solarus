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
#include "solarus/MapData.h"
#include <vector>

namespace Solarus {

namespace {

enum class OptionalFlag {
    MANDATORY,
    OPTIONAL
};

enum class InDataFileFlag {
    YES,
    NO
};

enum class InLuaApiFlag {
    YES,
    NO
};

/**
 * \brief Describes the format of a property of an entity type.
 */
struct EntityPropertyDescription {

    std::string key;              /**< Name of the property. */
    OptionalFlag optional;        /**< Whether the property is optional. */
    PropertyValue default_value;  /**< Default value (also determines the value type). */

};

/**
 * \brief Describes the format of an entity type.
 */
struct EntityTypeDescription {

    InDataFileFlag enable_in_data_file;
    InLuaApiFlag enabled_in_lua_api;
    std::vector<EntityPropertyDescription> properties;
};

/**
 * \brief Full definition of the format of each entity in the data file.
 */
const std::map<EntityType, EntityTypeDescription> entity_type_descriptions = {

    {
        EntityType::TILE, {
            InDataFileFlag::YES,
            InLuaApiFlag::NO,
            {
                { "pattern", OptionalFlag::MANDATORY, PropertyValue("") },
                { "width", OptionalFlag::MANDATORY, PropertyValue(0) },
                { "height", OptionalFlag::MANDATORY, PropertyValue(0) }
            }
        }
    }

    // TODO other types
};

}  // Anonymous namespace.


PropertyValue::PropertyValue(const std::string& value):
    value_type(EntityPropertyType::STRING),
    string_value(value),
    int_value(0) {

}

PropertyValue::PropertyValue(int value):
    value_type(EntityPropertyType::INTEGER),
    string_value(),
    int_value(value) {


}

PropertyValue::PropertyValue(bool value):
    value_type(EntityPropertyType::BOOLEAN),
    string_value(),
    int_value(value ? 1 : 0) {


}


}  // namespace Solarus.
