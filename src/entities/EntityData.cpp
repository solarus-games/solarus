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
#include "solarus/core/Debug.h"
#include "solarus/entities/EntityData.h"
#include "solarus/entities/EntityTypeInfo.h"
#include "solarus/lua/LuaTools.h"
#include <ostream>

namespace Solarus {

namespace {

using EntityTypeDescription = EntityData::EntityTypeDescription;
using OptionalFlag = EntityData::OptionalFlag;
using FieldValue = EntityData::FieldValue;

/**
 * \brief Definition of the format of entity types in map data files and Lua API.
 */
const std::map<EntityType, const EntityTypeDescription> entity_type_descriptions = {

    {
        EntityType::TILE, {
            { "width", OptionalFlag::MANDATORY, FieldValue(16) },
            { "height", OptionalFlag::MANDATORY, FieldValue(16) },
            { "pattern", OptionalFlag::MANDATORY, FieldValue("") },
            { "tileset", OptionalFlag::OPTIONAL, FieldValue("") }
        }
    },

    {
        EntityType::DYNAMIC_TILE, {
            { "pattern", OptionalFlag::MANDATORY, FieldValue("") },
            { "width", OptionalFlag::MANDATORY, FieldValue(16) },
            { "height", OptionalFlag::MANDATORY, FieldValue(16) },
            { "enabled_at_start", OptionalFlag::MANDATORY, FieldValue(true) },
            { "tileset", OptionalFlag::OPTIONAL, FieldValue("") }
        }
    },

    {
        EntityType::TELETRANSPORTER, {
            { "width", OptionalFlag::MANDATORY, FieldValue(16) },
            { "height", OptionalFlag::MANDATORY, FieldValue(16) },
            { "transition", OptionalFlag::OPTIONAL, FieldValue("fade") },
            { "destination_map", OptionalFlag::MANDATORY, FieldValue("") },
            { "destination", OptionalFlag::OPTIONAL, FieldValue("") },
            { "sprite", OptionalFlag::OPTIONAL, FieldValue("") },
            { "sound", OptionalFlag::OPTIONAL, FieldValue("") }
        }
    },

    {
        EntityType::DESTINATION, {
            { "direction", OptionalFlag::MANDATORY, FieldValue(1) },
            { "sprite", OptionalFlag::OPTIONAL, FieldValue("") },
            { "starting_location_mode", OptionalFlag::OPTIONAL, FieldValue("when_world_changes") },
            { "default", OptionalFlag::OPTIONAL, FieldValue(false) }
        }
    },

    {
        EntityType::PICKABLE, {
            { "treasure_name", OptionalFlag::OPTIONAL, FieldValue("") },
            { "treasure_variant", OptionalFlag::OPTIONAL, FieldValue(1) },
            { "treasure_savegame_variable", OptionalFlag::OPTIONAL, FieldValue("") }
        }
    },

    {
        EntityType::DESTRUCTIBLE, {
            { "treasure_name", OptionalFlag::OPTIONAL, FieldValue("") },
            { "treasure_variant", OptionalFlag::OPTIONAL, FieldValue(1) },
            { "treasure_savegame_variable", OptionalFlag::OPTIONAL, FieldValue("") },
            { "sprite", OptionalFlag::MANDATORY, FieldValue("") },
            { "destruction_sound", OptionalFlag::OPTIONAL, FieldValue("") },
            { "weight", OptionalFlag::OPTIONAL, FieldValue(0) },
            { "can_be_cut", OptionalFlag::OPTIONAL, FieldValue(false) },
            { "can_explode", OptionalFlag::OPTIONAL, FieldValue(false) },
            { "can_regenerate", OptionalFlag::OPTIONAL, FieldValue(false) },
            { "damage_on_enemies", OptionalFlag::OPTIONAL, FieldValue(1) },
            { "ground", OptionalFlag::OPTIONAL, FieldValue("wall") }
        }
    },

    {
        EntityType::CHEST, {
            { "treasure_name", OptionalFlag::OPTIONAL, FieldValue("") },
            { "treasure_variant", OptionalFlag::OPTIONAL, FieldValue(1) },
            { "treasure_savegame_variable", OptionalFlag::OPTIONAL, FieldValue("") },
            { "sprite", OptionalFlag::MANDATORY, FieldValue("entities/chest") },
            { "opening_method", OptionalFlag::OPTIONAL, FieldValue("interaction") },
            { "opening_condition", OptionalFlag::OPTIONAL, FieldValue("") },
            { "opening_condition_consumed", OptionalFlag::OPTIONAL, FieldValue(false) },
            { "cannot_open_dialog", OptionalFlag::OPTIONAL, FieldValue("") },
        }
    },

    {
        EntityType::SHOP_TREASURE, {
            { "treasure_name", OptionalFlag::MANDATORY, FieldValue("") },
            { "treasure_variant", OptionalFlag::OPTIONAL, FieldValue(1) },
            { "treasure_savegame_variable", OptionalFlag::OPTIONAL, FieldValue("") },
            { "price", OptionalFlag::MANDATORY, FieldValue(0) },
            { "font", OptionalFlag::OPTIONAL, FieldValue("") },
            { "dialog", OptionalFlag::MANDATORY, FieldValue("") }
        }
    },

    {
        EntityType::ENEMY, {
            { "direction", OptionalFlag::MANDATORY, FieldValue(0) },
            { "breed", OptionalFlag::MANDATORY, FieldValue("") },
            { "rank", OptionalFlag::OPTIONAL, FieldValue(0) },
            { "savegame_variable", OptionalFlag::OPTIONAL, FieldValue("") },
            { "treasure_name", OptionalFlag::OPTIONAL, FieldValue("") },
            { "treasure_variant", OptionalFlag::OPTIONAL, FieldValue(1) },
            { "treasure_savegame_variable", OptionalFlag::OPTIONAL, FieldValue("") },
        }
    },

    {
        EntityType::NPC, {
            { "direction", OptionalFlag::MANDATORY, FieldValue(3) },
            { "subtype", OptionalFlag::MANDATORY, FieldValue(1) },
            { "sprite", OptionalFlag::OPTIONAL, FieldValue("") },
            { "behavior", OptionalFlag::OPTIONAL, FieldValue("map") },
        }
    },

    {
        EntityType::BLOCK, {
            { "direction", OptionalFlag::OPTIONAL, FieldValue(-1) },
            { "sprite", OptionalFlag::MANDATORY, FieldValue("entities/block") },
            { "pushable", OptionalFlag::MANDATORY, FieldValue(true) },
            { "pullable", OptionalFlag::MANDATORY, FieldValue(false) },
            { "maximum_moves", OptionalFlag::MANDATORY, FieldValue(1) },
        }
    },

    {
        EntityType::JUMPER, {
            { "width", OptionalFlag::MANDATORY, FieldValue(32) },
            { "height", OptionalFlag::MANDATORY, FieldValue(8) },
            { "direction", OptionalFlag::MANDATORY, FieldValue(6) },
            { "jump_length", OptionalFlag::MANDATORY, FieldValue(40) }
        }
    },

    {
        EntityType::SWITCH, {
            { "subtype", OptionalFlag::MANDATORY, FieldValue("walkable") },
            { "sprite", OptionalFlag::OPTIONAL, FieldValue("") },
            { "sound", OptionalFlag::OPTIONAL, FieldValue("") },
            { "needs_block", OptionalFlag::MANDATORY, FieldValue(false) },
            { "inactivate_when_leaving", OptionalFlag::MANDATORY, FieldValue(false) },
        }
    },

    {
        EntityType::SENSOR, {
            { "width", OptionalFlag::MANDATORY, FieldValue(16) },
            { "height", OptionalFlag::MANDATORY, FieldValue(16) },
        }
    },

    {
        EntityType::WALL, {
            { "width", OptionalFlag::MANDATORY, FieldValue(16) },
            { "height", OptionalFlag::MANDATORY, FieldValue(16) },
            { "stops_hero", OptionalFlag::OPTIONAL, FieldValue(false) },
            { "stops_npcs", OptionalFlag::OPTIONAL, FieldValue(false) },
            { "stops_enemies", OptionalFlag::OPTIONAL, FieldValue(false) },
            { "stops_blocks", OptionalFlag::OPTIONAL, FieldValue(false) },
            { "stops_projectiles", OptionalFlag::OPTIONAL, FieldValue(false) },
        }
    },

    {
        EntityType::CRYSTAL, {
            // No additional fields.
        }
    },

    {
        EntityType::CRYSTAL_BLOCK, {
            { "width", OptionalFlag::MANDATORY, FieldValue(16) },
            { "height", OptionalFlag::MANDATORY, FieldValue(16) },
            { "subtype", OptionalFlag::MANDATORY, FieldValue(0) },
        }
    },

    {
        EntityType::STREAM, {
            { "direction", OptionalFlag::MANDATORY, FieldValue(6) },
            { "sprite", OptionalFlag::OPTIONAL, FieldValue("") },
            { "speed", OptionalFlag::OPTIONAL, FieldValue(64) },
            { "allow_movement", OptionalFlag::OPTIONAL, FieldValue(true) },
            { "allow_attack", OptionalFlag::OPTIONAL, FieldValue(true) },
            { "allow_item", OptionalFlag::OPTIONAL, FieldValue(true) },
        }
    },

    {
        EntityType::DOOR, {
            { "direction", OptionalFlag::MANDATORY, FieldValue(3) },
            { "sprite", OptionalFlag::MANDATORY, FieldValue("") },
            { "savegame_variable", OptionalFlag::OPTIONAL, FieldValue("") },
            { "opening_method", OptionalFlag::OPTIONAL, FieldValue("none") },
            { "opening_condition", OptionalFlag::OPTIONAL, FieldValue("") },
            { "opening_condition_consumed", OptionalFlag::OPTIONAL, FieldValue(false) },
            { "cannot_open_dialog", OptionalFlag::OPTIONAL, FieldValue("") },
        }
    },

    {
        EntityType::STAIRS, {
            { "direction", OptionalFlag::MANDATORY, FieldValue(1) },
            { "subtype", OptionalFlag::MANDATORY, FieldValue(0) },
        }
    },

    {
        EntityType::SEPARATOR, {
            { "width", OptionalFlag::MANDATORY, FieldValue(16) },
            { "height", OptionalFlag::MANDATORY, FieldValue(16) },
        }
    },

    {
        EntityType::CUSTOM, {
            { "width", OptionalFlag::MANDATORY, FieldValue(16) },
            { "height", OptionalFlag::MANDATORY, FieldValue(16) },
            { "direction", OptionalFlag::MANDATORY, FieldValue(0) },
            { "sprite", OptionalFlag::OPTIONAL, FieldValue("") },
            { "model", OptionalFlag::OPTIONAL, FieldValue("") },
        }
    },

    {
        EntityType::EXPLOSION, {
            // No additional fields.
        }
    },

    {
        EntityType::BOMB, {
            // No additional fields.
        }
    },

    {
        EntityType::FIRE, {
            // No additional fields.
        }
    }
};


/**
 * \brief Implementation of all entity data creation functions.
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int l_create_entity_data(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {

    // Retrieve the entity data instance to build.
    lua_getfield(l, LUA_REGISTRYINDEX, "entity");
    EntityData& entity = *static_cast<EntityData*>(lua_touserdata(l, -1));
    lua_pop(l, 1);

    // Get the type of entity, it should have been passed as an upvalue.
    EntityType type = LuaTools::check_enum<EntityType>(
        l, lua_upvalueindex(1)
    );

    entity = EntityData::check_entity_data(l, 1, type);

    return 0;
  });
}


}  // Anonymous namespace.

/**
 * \brief Creates a nil value.
 */
FieldValue::FieldValue():
    value_type(EntityFieldType::NIL),
    string_value(),
    int_value(0) {

}

/**
 * \brief Creates a value of type string.
 * \param value The value.
 */
FieldValue::FieldValue(const std::string& value):
    value_type(EntityFieldType::STRING),
    string_value(value),
    int_value(0) {

}

/**
 * \brief Creates a value of type string.
 *
 * This overload makes sure that string literal parameters
 * won't get converted to bool.
 *
 * \param value The value.
 */
FieldValue::FieldValue(const char* value):
    FieldValue(std::string(value)) {

}

/**
 * \brief Creates a value of type integer.
 * \param value The value.
 */
FieldValue::FieldValue(int value):
    value_type(EntityFieldType::INTEGER),
    string_value(),
    int_value(value) {

}

/**
 * \brief Creates a value of type boolean.
 * \param value The value.
 */
FieldValue::FieldValue(bool value):
    value_type(EntityFieldType::BOOLEAN),
    string_value(),
    int_value(value ? 1 : 0) {

}

/**
 * \brief Returns whether two values are equal.
 */
bool FieldValue::operator==(const FieldValue& other) const {

  return value_type == other.value_type &&
      string_value == other.string_value &&
      int_value == other.int_value;
}

/**
 * \brief Returns whether two values are different.
 */
bool FieldValue::operator!=(const FieldValue& other) const {

  return !(*this == other);
}

/**
 * \brief Creates data for an entity of a default-constructed type.
 */
EntityData::EntityData() :
    type(),
    name(),
    layer(0),
    xy(),
    user_properties() ,
    specific_properties() {

  initialize_specific_properties();
}

/**
 * \brief Creates data for an entity of the given type.
 * \param type A type of entity.
 */
EntityData::EntityData(EntityType type) :
    type(type),
    name(),
    layer(0),
    xy(),
    user_properties(),
    specific_properties() {

  initialize_specific_properties();
}

/**
 * \brief Returns the type of entity.
 * \return The type of entity.
 */
EntityType EntityData::get_type() const {
  return type;
}

/**
 * \brief Sets the type of this entity.
 *
 * When the type changes, all fields are reset to their default value.
 *
 * \param type The new type.
 */
void EntityData::set_type(EntityType type) {

  if (type == this->type) {
    return;
  }

  this->type = type;
  initialize_specific_properties();
}

/**
 * \brief Returns whether this entity is a dynamic one.
 * \return \c true if this is a dynamic entity, \c false if this is a tile.
 */
bool EntityData::is_dynamic() const {
  return type != EntityType::TILE;
}

/**
 * \brief Returns the name of the type of entity.
 * \return The type name of entity.
 */
const std::string& EntityData::get_type_name() const {
  return enum_to_name(get_type());
}

/**
 * \brief Returns whether the entity has a name.
 */
bool EntityData::has_name() const {
  return !name.empty();
}

/**
 * \brief Returns the name of this entity.
 * \return The entity name or an empty string.
 */
std::string EntityData::get_name() const {
  return name;
}

/**
 * \brief Changes the name of this entity.
 * \param name The entity name or an empty string.
 */
void EntityData::set_name(const std::string& name) {
  this->name = name;
}

/**
 * \brief Returns the layer of this entity on the map.
 * \return The layer.
 */
int EntityData::get_layer() const {
  return layer;
}

/**
 * \brief Sets the layer of this entity on the map.
 * \param layer The layer.
 */
void EntityData::set_layer(int layer) {
  this->layer = layer;
}

/**
 * \brief Returns the coordinates of this entity on the map.
 * \return The coordinates of the entity.
 */
Point EntityData::get_xy() const {
  return xy;
}

/**
 * \brief Sets the coordinates of this entity on the map.
 * \param xy The coordinates of the entity.
 */
void EntityData::set_xy(const Point& xy) {
  this->xy = xy;
}

/**
 * \brief Returns whether a string is a valid key for a user property.
 *
 * It should only contain alphanumeric characters or '_' and
 * not start with a digit.
 *
 * \param key The string to check.
 * \return \c true if this is a valid user property key.
 */
bool EntityData::is_user_property_key_valid(const std::string& key) {

  // Same rules as Lua identifiers.
  return LuaTools::is_valid_lua_identifier(key);
}

/**
 * \brief Returns the user-defined properties of this entity.
 * \return The user-defined properties.
 */
const std::vector<EntityData::UserProperty>& EntityData::get_user_properties() const {
  return user_properties;
}

/**
 * \brief Returns the number of user-defined properties of this entity.
 * \return The number of user-defined properties.
 */
int EntityData::get_user_property_count() const {
  return static_cast<int>(user_properties.size());
}

/**
 * \brief Returns the user property at the given index.
 * \param index An index between \c 0 and <tt>get_user_property_count() - 1</tt>.
 * \return The user property at this index.
 */
const EntityData::UserProperty& EntityData::get_user_property(int index) const {

  Debug::check_assertion(index >= 0 && index < get_user_property_count(),
      "Invalid user property index");
  return user_properties.at(index);
}

/**
 * \brief Sets the user property at the given index.
 * \param index An index between \c 0 and <tt>get_user_property_count() - 1</tt>.
 * \param user_property The new key and value to set.
 */
void EntityData::set_user_property(int index, const UserProperty& user_property) {

  Debug::check_assertion(index >= 0 && index < get_user_property_count(),
      "Invalid user property index");

  Debug::check_assertion(is_user_property_key_valid(user_property.first),
      "Invalid user property key");

  int existing_index = get_user_property_index(user_property.first);
  Debug::check_assertion(existing_index == -1 || existing_index == index,
      "This user property already exists");

  user_properties[index] = user_property;
}

/**
 * \brief Sets all user-defined properties.
 * \param user_properties The properties to set. Keys must be unique.
 */
void EntityData::set_user_properties(const std::vector<UserProperty>& user_properties) {

  for (const UserProperty& user_property : user_properties) {
    Debug::check_assertion(is_user_property_key_valid(user_property.first),
        "Invalid user property key");
  }

  this->user_properties = user_properties;
}

/**
 * \brief Adds a user property.
 *
 * The key of the property should not already exist.
 *
 * \param user_property The new key and value to add.
 */
void EntityData::add_user_property(const UserProperty& user_property) {

  Debug::check_assertion(is_user_property_key_valid(user_property.first),
      "Invalid user property key");

  Debug::check_assertion(!has_user_property(user_property.first),
      "This user property already exists");

  user_properties.emplace_back(user_property);
}

/**
 * \brief Returns a user property.
 * \param index An index between \c 0 and <tt>get_user_property_count() - 1</tt>.
 */
void EntityData::remove_user_property(int index) {

  Debug::check_assertion(index >= 0 && index < get_user_property_count(),
      "Invalid user property index");

  user_properties.erase(user_properties.begin() + index);
}

/**
 * \brief Returns the index of a user property in the user property list.
 * \param key Key of the property to get.
 * \return The corresponding index, or -1 if there is no such property.
 */
int EntityData::get_user_property_index(const std::string& key) const {

  int index = 0;
  for (const UserProperty& user_property : user_properties) {
    if (user_property.first == key) {
      return index;
    }
    ++index;
  }
  return -1;
}

/**
 * \brief Returns whether a user property exists with the given key.
 * \param key Key of the property to get.
 * \return \c true if the entity has such a property.
 */
bool EntityData::has_user_property(const std::string& key) const {
  return get_user_property_index(key) != -1;
}

/**
 * \brief Initializes specific properties with their default values for the type.
 */
void EntityData::initialize_specific_properties() {

  specific_properties.clear();
  const EntityTypeDescription& type_description = entity_type_descriptions.at(type);
  for (const EntityFieldDescription& field_description : type_description) {
    specific_properties.emplace(field_description.key, field_description.default_value);
  }
}

/**
 * \brief Returns all specific properties of this entity.
 * \return The entity properties that are specific to its type.
 */
const std::map<std::string, FieldValue>& EntityData::get_specific_properties() const {
  return specific_properties;
}

/**
 * \brief Returns a specific property of this entity if it exists.
 * \param key Key of the specific property to get.
 * \return The corresponding specific property or a nil field.
 */
FieldValue EntityData::get_specific_property(const std::string& key) const {

  const auto& it = specific_properties.find(key);
  if (it == specific_properties.end()) {
    return FieldValue();
  }

  return it->second;
}

/**
 * \brief Returns whether a specific property of this entity is a string.
 * \param key Key of the specific property to get.
 * \return \c true if the specific property exists and is a string.
 */
bool EntityData::is_string(const std::string& key) const {

  const auto& it = specific_properties.find(key);
  if (it == specific_properties.end()) {
    return false;
  }
  return it->second.value_type == EntityFieldType::STRING;
}

/**
 * \brief Returns the value of a string specific property of this entity.
 * \param key Key of the specific property to get.
 * It must exist and be a string.
 * \return The string value.
 */
const std::string& EntityData::get_string(const std::string& key) const {

  const auto& it = specific_properties.find(key);
  Debug::check_assertion(it != specific_properties.end(),
      "No such entity field in " + get_type_name() + ": '" + key + "'");

  Debug::check_assertion(it->second.value_type == EntityFieldType::STRING,
      "Field '" + key + "' is not a string");

  return it->second.string_value;
}

/**
 * \brief Sets the value of a string specific property of this entity.
 * \param key Key of the specific property to set.
 * It must exist and be a string.
 * \param value The string value.
 */
void EntityData::set_string(const std::string& key, const std::string& value) {

  const auto& it = specific_properties.find(key);
  Debug::check_assertion(it != specific_properties.end(),
      "No such entity field in " + get_type_name() + ": '" + key + "'");

  Debug::check_assertion(it->second.value_type == EntityFieldType::STRING,
      "Field '" + key + "' is not a string");

  it->second.string_value = value;
}

/**
 * \brief Returns whether a specific property of this entity is an integer.
 * \param key Key of the specific property to get.
 * \return \c true if the specific property exists and is an integer.
 */
bool EntityData::is_integer(const std::string& key) const {

  const auto& it = specific_properties.find(key);
  if (it == specific_properties.end()) {
    return false;
  }
  return it->second.value_type == EntityFieldType::INTEGER;
}

/**
 * \brief Returns the value of an integer specific property of this entity.
 * \param key Key of the specific property to get.
 * It must exist and be an integer.
 * \return The integer value.
 */
int EntityData::get_integer(const std::string& key) const {

  const auto& it = specific_properties.find(key);
  Debug::check_assertion(it != specific_properties.end(),
      "No such entity field in " + get_type_name() + ": '" + key + "'");

  Debug::check_assertion(it->second.value_type == EntityFieldType::INTEGER,
      "Field '" + key + "' is not a string");

  return it->second.int_value;
}

/**
 * \brief Sets the value of an integer specific property of this entity.
 * \param key Key of the specific property to set.
 * It must exist and be an integer.
 * \param value The integer value.
 */
void EntityData::set_integer(const std::string& key, int value) {

  const auto& it = specific_properties.find(key);
  Debug::check_assertion(it != specific_properties.end(),
      "No such entity field in " + get_type_name() + ": '" + key + "'");

  Debug::check_assertion(it->second.value_type == EntityFieldType::INTEGER,
      "Field '" + key + "' is not an integer");

  it->second.int_value = value;
}

/**
 * \brief Returns whether a specific property of this entity is a boolean.
 * \param key Key of the specific property to get.
 * \return \c true if the specific property exists and is a boolean.
 */
bool EntityData::is_boolean(const std::string& key) const {

  const auto& it = specific_properties.find(key);
  if (it == specific_properties.end()) {
    return false;
  }
  return it->second.value_type == EntityFieldType::BOOLEAN;
}

/**
 * \brief Returns the value of a boolean specific property of this entity.
 * \param key Key of the specific property to get.
 * It must exist and be a boolean.
 * \return The boolean value.
 */
bool EntityData::get_boolean(const std::string& key) const {

  const auto& it = specific_properties.find(key);
  Debug::check_assertion(it != specific_properties.end(),
      "No such entity field in " + get_type_name() + ": '" + key + "'");

  Debug::check_assertion(it->second.value_type == EntityFieldType::BOOLEAN,
      "Field '" + key + "' is not a boolean");

  return it->second.int_value != 0;
}

/**
 * \brief Sets the value of a boolean specific property of this entity.
 * \param key Key of the specific property to set.
 * It must exist and be a boolean.
 * \param value The boolean value.
 */
void EntityData::set_boolean(const std::string& key, bool value) {

  const auto& it = specific_properties.find(key);
  Debug::check_assertion(it != specific_properties.end(),
      "No such entity field in " + get_type_name() + ": '" + key + "'");

  Debug::check_assertion(it->second.value_type == EntityFieldType::BOOLEAN,
      "Field '" + key + "' is not an boolean");

  it->second.int_value = value ? 1 : 0;
}

/**
 * \brief Returns whether a specific property exists for this type of entity.
 * \param key Key of the specific property to check.
 * \return \c true if such a specific property exists.
 */
bool EntityData::has_specific_property(const std::string& key) const {

  return get_specific_property(key).value_type != EntityData::EntityFieldType::NIL;
}

/**
 * \brief Returns whether a specific property is optional.
 * \param key Key of the specific property to check.
 * \return \c true if the specific property exists and is optional.
 */
bool EntityData::is_specific_property_optional(const std::string& key) const {

  const EntityTypeDescription& type_description = entity_type_descriptions.at(type);
  for (const EntityFieldDescription& field_description : type_description) {
    if (field_description.key == key) {
      return field_description.optional == OptionalFlag::OPTIONAL;
    }
  }

  return false;
}

/**
 * \brief Returns whether a specific property is unset.
 *
 * A specific property can only be unset if it is optional.
 *
 * \param key Key of the specific property to check.
 * \return \c true if the specific property exists, is optional and is equal to its
 * default value.
 */
bool EntityData::is_specific_property_unset(const std::string& key) const {

  const EntityTypeDescription& type_description = entity_type_descriptions.at(type);
  for (const EntityFieldDescription& field_description : type_description) {
    if (field_description.key != key) {
      continue;
    }

    if (field_description.optional != OptionalFlag::OPTIONAL) {
      // Mandatory specific property: always set.
      return false;
    }

    const FieldValue& default_value = field_description.default_value;
    switch (default_value.value_type) {

    case EntityFieldType::STRING:
      return get_string(key) == default_value.string_value;

    case EntityFieldType::INTEGER:
      return get_integer(key) == default_value.int_value;

    case EntityFieldType::BOOLEAN:
      return get_boolean(key) == (default_value.int_value != 0);

    case EntityFieldType::NIL:
      Debug::die("Nil entity field");
      break;
    }
  }

  // The specific property does not exist.
  return false;
}

/**
 * \brief Reads an EntityData from the Lua table at the specified index.
 * \param l A Lua state.
 * \param index Index of the table in the Lua stack.
 * \param type Type of entity to read.
 * \return The EntityData created.
 */
EntityData EntityData::check_entity_data(lua_State* l, int index, EntityType type) {

  LuaTools::check_type(l, index, LUA_TTABLE);
  const std::string& name = LuaTools::opt_string_field(l, index, "name", "");
  int layer = LuaTools::check_int_field(l, index, "layer");
  int x = LuaTools::check_int_field(l, index, "x");
  int y = LuaTools::check_int_field(l, index, "y");

  EntityData entity(type);
  entity.set_name(name);
  entity.set_layer(layer);
  entity.set_xy({ x, y });

  // User-defined properties.
  lua_getfield(l, index, "properties");
  if (!lua_isnil(l, -1)) {
    LuaTools::check_type(l, -1, LUA_TTABLE);
    lua_pushnil(l);
    while (lua_next(l, -2) != 0) {
      LuaTools::check_type(l, -1, LUA_TTABLE);
      const std::string& key = LuaTools::check_string_field(l, -1, "key");
      const std::string& value = LuaTools::check_string_field(l, -1, "value");
      if (entity.has_user_property(key)) {
        LuaTools::error(l, "Duplicate property '" + key + "'");
      }
      if (!EntityData::is_user_property_key_valid(key)) {
        LuaTools::error(l, "Invalid property key: '" + key + "'");
      }
      entity.add_user_property(std::make_pair(key, value));
      lua_pop(l, 1);
    }
  }
  lua_pop(l, 1);

  // Additional properties specific to the entity type.
  const EntityTypeDescription& type_description = entity_type_descriptions.at(type);
  for (const EntityFieldDescription& field_description : type_description) {

    const std::string& key = field_description.key;
    const FieldValue& default_value = field_description.default_value;
    switch (default_value.value_type) {

      case EntityFieldType::STRING:
      {
        std::string value;
        if (field_description.optional == OptionalFlag::OPTIONAL) {
          value = LuaTools::opt_string_field(l, index, key, default_value.string_value);
        }
        else {
          value = LuaTools::check_string_field(l, index, key);
        }
        entity.set_string(key, value);
        break;
      }

      case EntityFieldType::INTEGER:
      {
        int value;
        if (field_description.optional == OptionalFlag::OPTIONAL) {
          value = LuaTools::opt_int_field(l, index, key, default_value.int_value);
        }
        else {
          value = LuaTools::check_int_field(l, index, key);
        }
        entity.set_integer(key, value);
        break;
      }

      case EntityFieldType::BOOLEAN:
      {
        bool value;
        if (field_description.optional == OptionalFlag::OPTIONAL) {
          value = LuaTools::opt_boolean_field(l, index, key, default_value.int_value != 0);
        }
        else {
          value = LuaTools::check_boolean_field(l, index, key);
        }
        entity.set_boolean(key, value);
        break;
      }

      case EntityFieldType::NIL:
        Debug::die("Nil entity field");
        break;
    }
  }

  return entity;
}

/**
 * \brief Returns the definition of the format of all entity types in map data files.
 *
 * Note: entities whose type is not in this map can only be created
 * by the engine.
 * This is the case of HERO, CARRIED_OBJECT, BOOMERANG, ARROW and HOOKHOT.
 * The ones in the map can be created either by declaring them in a map
 * data file or by the Lua API, or both.
 *
 * \return The format description of entity types.
 */
const std::map<EntityType, const EntityTypeDescription> EntityData::get_entity_type_descriptions() {
  return entity_type_descriptions;
}

/**
 * \copydoc LuaData::import_from_lua
 */
bool EntityData::import_from_lua(lua_State* l) {

  lua_pushlightuserdata(l, this);
  lua_setfield(l, LUA_REGISTRYINDEX, "entity");
  for (const auto& kvp : EnumInfoTraits<EntityType>::names) {
    EntityType type = kvp.first;
    const std::string& type_name = kvp.second;
    if (!EntityTypeInfo::can_be_stored_in_map_file(type)) {
      continue;
    }
    std::string function_name = type_name;
    lua_pushstring(l, function_name.c_str());
    lua_pushcclosure(l, l_create_entity_data, 1);
    lua_setglobal(l, function_name.c_str());
  }

  if (lua_pcall(l, 0, 0, 0) != 0) {
    Debug::error(std::string("Failed to load entity: ") + lua_tostring(l, -1));
    lua_pop(l, 1);
    return false;
  }

  return true;
}

/**
 * \copydoc LuaData::export_to_lua
 */
bool EntityData::export_to_lua(std::ostream& out) const {

  // Entity type.
  out << get_type_name() << "{\n";

  // Name.
  if (has_name()) {
    out << "  name = \"" << escape_string(get_name()) << "\",\n";
  }

  // Position on the map.
  out << "  layer = " << get_layer() << ",\n"
      << "  x = " << get_xy().x << ",\n"
      << "  y = " << get_xy().y << ",\n";

  // User-defined properties.
  export_user_properties(out);

  // Properties specific to the type of entity.
  export_specific_properties(out);

  out << "}\n\n";

  return true;
}

/**
 * \brief Exports the user-defined properties to a stream.
 * \param out Output stream to write.
 */
void EntityData::export_user_properties(std::ostream& out) const {

  const std::vector<UserProperty>& user_properties = get_user_properties();
  if (user_properties.empty()) {
    return;
  }

  out << "  properties = {\n";
  for (const UserProperty& user_property : user_properties) {
    out << "    {\n";
    out << "      key = \"" << escape_string(user_property.first) << "\",\n";
    out << "      value = \"" << escape_string(user_property.second) << "\",\n";
    out << "    },\n";
  }
  out << "  },\n";
}

/**
 * \brief Exports the properties to the entity type to a stream.
 * \param out Output stream to write.
 */
void EntityData::export_specific_properties(std::ostream& out) const {

  // Properties specific to the type of entity.
  const EntityTypeDescription& type_description = entity_type_descriptions.at(get_type());

  for (const EntityFieldDescription& field_description : type_description) {

    const std::string& key = field_description.key;
    const bool optional = field_description.optional == OptionalFlag::OPTIONAL;
    switch (field_description.default_value.value_type) {

      case EntityFieldType::STRING:
      {
        const std::string& value = get_string(key);
        const std::string& default_value = field_description.default_value.string_value;
        if (optional && value == default_value) {
          // No need to write the value.
          continue;
        }
        out << "  " << key << " = \"" << escape_string(value) << "\",\n";
        break;
      }

      case EntityFieldType::INTEGER:
      {
        int value = get_integer(key);
        int default_value = field_description.default_value.int_value;
        if (optional && value == default_value) {
          // No need to write the value.
          continue;
        }
        out << "  " << key << " = " << value << ",\n";
        break;
      }

      case EntityFieldType::BOOLEAN:
      {
        bool value = get_boolean(key);
        bool default_value = field_description.default_value.int_value != 0;
        if (optional && value == default_value) {
          // No need to write the value.
          continue;
        }
        out << "  " << key << " = " << (value ? "true" : "false") << ",\n";

        break;
      }

      case EntityFieldType::NIL:
        Debug::die("Nil entity field");
        break;
    }
  }
}

}  // namespace Solarus

