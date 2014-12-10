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

/**
 * \brief Whether a property is optional for an entity of some type.
 */
enum class OptionalFlag {
    MANDATORY,
    OPTIONAL
};

/**
 * \brief Whether an entity of some type can be created in the map data file
 * or only dynamically.
 */
enum class InDataFileFlag {
    YES,
    NO
};

/**
 * \brief Describes a property of an entity type.
 */
struct EntityPropertyDescription {

    std::string key;              /**< Name of the property. */
    OptionalFlag optional;        /**< Whether the property is optional. */
    PropertyValue default_value;  /**< Default value (also determines the value type). */

};

using EntityTypeDescription = std::vector<EntityPropertyDescription>;

/**
 * \brief Definition of the format of all entity types in map data files.
 */
const std::map<EntityType, EntityTypeDescription> entity_type_descriptions = {

    {
        EntityType::TILE, {
            { "pattern", OptionalFlag::MANDATORY, PropertyValue("") },
            { "width", OptionalFlag::MANDATORY, PropertyValue(0) },
            { "height", OptionalFlag::MANDATORY, PropertyValue(0) }
        }
    },

    {
        EntityType::DYNAMIC_TILE, {
            { "pattern", OptionalFlag::MANDATORY, PropertyValue("") },
            { "width", OptionalFlag::MANDATORY, PropertyValue(0) },
            { "height", OptionalFlag::MANDATORY, PropertyValue(0) },
            { "enabled_at_start", OptionalFlag::MANDATORY, PropertyValue(true) }
        }
    },

    {
        EntityType::TELETRANSPORTER, {
            { "width", OptionalFlag::MANDATORY, PropertyValue(0) },
            { "height", OptionalFlag::MANDATORY, PropertyValue(0) },
            { "transition", OptionalFlag::OPTIONAL, PropertyValue("fade") },
            { "destination_map", OptionalFlag::MANDATORY, PropertyValue("") },
            { "destination", OptionalFlag::OPTIONAL, PropertyValue("") },
            { "sprite", OptionalFlag::OPTIONAL, PropertyValue("") },
            { "sound", OptionalFlag::OPTIONAL, PropertyValue("") }
        }
    },

    {
        EntityType::DESTINATION, {
            { "direction", OptionalFlag::MANDATORY, PropertyValue(0) },
            { "sprite", OptionalFlag::OPTIONAL, PropertyValue("") },
            { "default", OptionalFlag::OPTIONAL, PropertyValue(false) }
        }
    },

    {
        EntityType::PICKABLE, {
            { "treasure_name", OptionalFlag::OPTIONAL, PropertyValue("") },
            { "treasure_variant", OptionalFlag::OPTIONAL, PropertyValue(1) },
            { "treasure_savegame_variable", OptionalFlag::OPTIONAL, PropertyValue("") }
        }
    },

    {
        EntityType::DESTRUCTIBLE, {
            { "treasure_name", OptionalFlag::OPTIONAL, PropertyValue("") },
            { "treasure_variant", OptionalFlag::OPTIONAL, PropertyValue(1) },
            { "treasure_savegame_variable", OptionalFlag::OPTIONAL, PropertyValue("") },
            { "sprite", OptionalFlag::MANDATORY, PropertyValue("") },
            { "destruction_sound", OptionalFlag::OPTIONAL, PropertyValue("") },
            { "weight", OptionalFlag::OPTIONAL, PropertyValue(0) },
            { "can_be_cut", OptionalFlag::OPTIONAL, PropertyValue(false) },
            { "can_explode", OptionalFlag::OPTIONAL, PropertyValue(false) },
            { "can_regenerate", OptionalFlag::OPTIONAL, PropertyValue(false) },
            { "damage_on_enemies", OptionalFlag::OPTIONAL, PropertyValue(1) },
            { "ground", OptionalFlag::OPTIONAL, PropertyValue("wall") }
        }
    },

    {
        EntityType::CHEST, {
            { "treasure_name", OptionalFlag::OPTIONAL, PropertyValue("") },
            { "treasure_variant", OptionalFlag::OPTIONAL, PropertyValue(1) },
            { "treasure_savegame_variable", OptionalFlag::OPTIONAL, PropertyValue("") },
            { "sprite", OptionalFlag::MANDATORY, PropertyValue("") },
            { "opening_method", OptionalFlag::OPTIONAL, PropertyValue("interaction") },
            { "opening_condition", OptionalFlag::OPTIONAL, PropertyValue("") },
            { "opening_condition_consumed", OptionalFlag::OPTIONAL, PropertyValue(false) },
            { "cannot_open_dialog", OptionalFlag::OPTIONAL, PropertyValue("") },
        }
    },

    {
        EntityType::SHOP_TREASURE, {
            { "treasure_name", OptionalFlag::MANDATORY, PropertyValue("") },
            { "treasure_variant", OptionalFlag::OPTIONAL, PropertyValue(1) },
            { "treasure_savegame_variable", OptionalFlag::OPTIONAL, PropertyValue("") },
            { "price", OptionalFlag::MANDATORY, PropertyValue(0) },
            { "dialog", OptionalFlag::MANDATORY, PropertyValue("") }
        }
    },

    {
        EntityType::ENEMY, {
            { "direction", OptionalFlag::MANDATORY, PropertyValue(0) },
            { "breed", OptionalFlag::MANDATORY, PropertyValue("") },
            { "rank", OptionalFlag::OPTIONAL, PropertyValue(0) },
            { "savegame_variable", OptionalFlag::OPTIONAL, PropertyValue("") },
            { "treasure_name", OptionalFlag::OPTIONAL, PropertyValue("") },
            { "treasure_variant", OptionalFlag::OPTIONAL, PropertyValue(1) },
            { "treasure_savegame_variable", OptionalFlag::OPTIONAL, PropertyValue("") },
        }
    },

    {
        EntityType::NPC, {
            { "direction", OptionalFlag::MANDATORY, PropertyValue(3) },
            { "subtype", OptionalFlag::MANDATORY, PropertyValue(1) },
            { "sprite", OptionalFlag::OPTIONAL, PropertyValue("") },
            { "behavior", OptionalFlag::OPTIONAL, PropertyValue("map") },
        }
    },

    {
        EntityType::BLOCK, {
            { "direction", OptionalFlag::OPTIONAL, PropertyValue(-1) },
            { "sprite", OptionalFlag::MANDATORY, PropertyValue("entities/block") },
            { "pushable", OptionalFlag::MANDATORY, PropertyValue(true) },
            { "pullable", OptionalFlag::MANDATORY, PropertyValue(false) },
            { "maximum_moves", OptionalFlag::MANDATORY, PropertyValue(1) },
        }
    },

    {
        EntityType::JUMPER, {
            { "direction", OptionalFlag::MANDATORY, PropertyValue(6) },
            { "width", OptionalFlag::MANDATORY, PropertyValue(0) },
            { "height", OptionalFlag::MANDATORY, PropertyValue(0) },
            { "jump_length", OptionalFlag::MANDATORY, PropertyValue(40) }
        }
    },

    {
        EntityType::SWITCH, {
            { "subtype", OptionalFlag::MANDATORY, PropertyValue("walkable") },
            { "sprite", OptionalFlag::MANDATORY, PropertyValue("entities/switch") },
            { "sound", OptionalFlag::OPTIONAL, PropertyValue("") },
            { "needs_block", OptionalFlag::MANDATORY, PropertyValue(false) },
            { "inactivate_when_leaving", OptionalFlag::MANDATORY, PropertyValue(false) },
        }
    }

    // TODO direction, width, height should be with common properties?
    // TODO other types of entities
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
