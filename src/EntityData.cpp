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
#include "solarus/EntityData.h"
#include "solarus/entities/MapEntity.h"
#include "solarus/lowlevel/Debug.h"
#include "solarus/lua/LuaTools.h"

namespace Solarus {

namespace {

using EntityTypeDescription = EntityData::EntityTypeDescription;
using OptionalFlag = EntityData::OptionalFlag;
using FieldValue = EntityData::FieldValue;

/**
 * \brief Definition of the format of all entity types in map data files.
 */
const std::map<EntityType, const EntityTypeDescription> entity_type_descriptions = {

    {
        EntityType::TILE, {
            { "pattern", OptionalFlag::MANDATORY, FieldValue("") },
            { "width", OptionalFlag::MANDATORY, FieldValue(0) },
            { "height", OptionalFlag::MANDATORY, FieldValue(0) }
        }
    },

    {
        EntityType::DYNAMIC_TILE, {
            { "pattern", OptionalFlag::MANDATORY, FieldValue("") },
            { "width", OptionalFlag::MANDATORY, FieldValue(0) },
            { "height", OptionalFlag::MANDATORY, FieldValue(0) },
            { "enabled_at_start", OptionalFlag::MANDATORY, FieldValue(true) }
        }
    },

    {
        EntityType::TELETRANSPORTER, {
            { "width", OptionalFlag::MANDATORY, FieldValue(0) },
            { "height", OptionalFlag::MANDATORY, FieldValue(0) },
            { "transition", OptionalFlag::OPTIONAL, FieldValue("fade") },
            { "destination_map", OptionalFlag::MANDATORY, FieldValue("") },
            { "destination", OptionalFlag::OPTIONAL, FieldValue("") },
            { "sprite", OptionalFlag::OPTIONAL, FieldValue("") },
            { "sound", OptionalFlag::OPTIONAL, FieldValue("") }
        }
    },

    {
        EntityType::DESTINATION, {
            { "direction", OptionalFlag::MANDATORY, FieldValue(0) },
            { "sprite", OptionalFlag::OPTIONAL, FieldValue("") },
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
            { "sprite", OptionalFlag::MANDATORY, FieldValue("") },
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
            { "direction", OptionalFlag::MANDATORY, FieldValue(6) },
            { "width", OptionalFlag::MANDATORY, FieldValue(0) },
            { "height", OptionalFlag::MANDATORY, FieldValue(0) },
            { "jump_length", OptionalFlag::MANDATORY, FieldValue(40) }
        }
    },

    {
        EntityType::SWITCH, {
            { "subtype", OptionalFlag::MANDATORY, FieldValue("walkable") },
            { "sprite", OptionalFlag::MANDATORY, FieldValue("entities/switch") },
            { "sound", OptionalFlag::OPTIONAL, FieldValue("") },
            { "needs_block", OptionalFlag::MANDATORY, FieldValue(false) },
            { "inactivate_when_leaving", OptionalFlag::MANDATORY, FieldValue(false) },
        }
    },

    {
        EntityType::SENSOR, {
            { "width", OptionalFlag::MANDATORY, FieldValue(0) },
            { "height", OptionalFlag::MANDATORY, FieldValue(0) },
        }
    },

    {
        EntityType::WALL, {
            { "width", OptionalFlag::MANDATORY, FieldValue(0) },
            { "height", OptionalFlag::MANDATORY, FieldValue(0) },
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
            { "width", OptionalFlag::MANDATORY, FieldValue(0) },
            { "height", OptionalFlag::MANDATORY, FieldValue(0) },
            { "subtype", OptionalFlag::MANDATORY, FieldValue(0) },
        }
    },

    {
        EntityType::STREAM, {
            { "direction", OptionalFlag::MANDATORY, FieldValue(6) },
            { "sprite", OptionalFlag::OPTIONAL, FieldValue("entities/block") },
            { "speed", OptionalFlag::OPTIONAL, FieldValue(64) },
            { "allow_movement", OptionalFlag::OPTIONAL, FieldValue(true) },
            { "allow_attack", OptionalFlag::OPTIONAL, FieldValue(true) },
            { "allow_item", OptionalFlag::OPTIONAL, FieldValue(true) },
        }
    },

    {
        EntityType::DOOR, {
            { "direction", OptionalFlag::MANDATORY, FieldValue(6) },
            { "sprite", OptionalFlag::MANDATORY, FieldValue("") },
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
            { "width", OptionalFlag::MANDATORY, FieldValue(0) },
            { "height", OptionalFlag::MANDATORY, FieldValue(0) },
        }
    },

    {
        EntityType::CUSTOM, {
            { "direction", OptionalFlag::MANDATORY, FieldValue(0) },
            { "width", OptionalFlag::MANDATORY, FieldValue(0) },
            { "height", OptionalFlag::MANDATORY, FieldValue(0) },
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

    // TODO put direction, width, height with common properties?
};

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
 * \brief Creates data for an entity of the given type.
 * \param type A type of entity.
 */
EntityData::EntityData(EntityType type) :
    type(type),
    name(),
    layer(LAYER_LOW),
    xy(),
    fields() {

  // Initialize fields with their default values.
  const EntityTypeDescription& type_description = entity_type_descriptions.at(type);
  for (const EntityFieldDescription& field_description : type_description) {
    fields.insert(std::make_pair(field_description.key, field_description.default_value));
  }
}

/**
 * \brief Returns the type of entity.
 * \return The type of entity.
 */
EntityType EntityData::get_type() const {
  return type;
}

/**
 * \brief Returns the name of the type of entity.
 * \return The type name of entity.
 */
const std::string& EntityData::get_type_name() const {
  return MapEntity::get_entity_type_name(get_type());
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
Layer EntityData::get_layer() const {
  return layer;
}

/**
 * \brief Sets the layer of this entity on the map.
 * \param layer The layer.
 */
void EntityData::set_layer(Layer layer) {
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
 * \brief Returns all fields of this entity.
 * \return The fields.
 */
const std::map<std::string, FieldValue>& EntityData::get_fields() const {
  return fields;
}

/**
 * \brief Returns a field of this entity if it exists.
 * \param key Key of the field to get.
 * \return The corresponding field or a nil field.
 */
FieldValue EntityData::get_field(const std::string& key) const {

  const auto& it = fields.find(key);
  if (it == fields.end()) {
    return FieldValue();
  }

  return it->second;
}

/**
 * \brief Returns whether a field of this entity is a string.
 * \param key Key of the field to get.
 * \return \c true if the field exists and is a string.
 */
bool EntityData::is_string(const std::string& key) const {

  const auto& it = fields.find(key);
  if (it == fields.end()) {
    return false;
  }
  return it->second.value_type == EntityFieldType::STRING;
}

/**
 * \brief Returns the value of a string field of this entity.
 * \param key Key of the field to get. It must exist and be a string.
 * \return The string value.
 */
const std::string& EntityData::get_string(const std::string& key) const {

  const auto& it = fields.find(key);
  Debug::check_assertion(it != fields.end(),
      "No such entity field in " + get_type_name() + ": '" + key + "'");

  Debug::check_assertion(it->second.value_type == EntityFieldType::STRING,
      "Field '" + key + "' is not a string");

  return it->second.string_value;
}

/**
 * \brief Sets the value of a string field of this entity.
 * \param key Key of the field to set. It must exist and be a string.
 * \param value The string value.
 */
void EntityData::set_string(const std::string& key, const std::string& value) {

  const auto& it = fields.find(key);
  Debug::check_assertion(it != fields.end(),
      "No such entity field in " + get_type_name() + ": '" + key + "'");

  Debug::check_assertion(it->second.value_type == EntityFieldType::STRING,
      "Field '" + key + "' is not a string");

  it->second.string_value = value;
}

/**
 * \brief Returns whether a field of this entity is an integer.
 * \param key Key of the field to get.
 * \return \c true if the field exists and is an integer.
 */
bool EntityData::is_integer(const std::string& key) const {

  const auto& it = fields.find(key);
  if (it == fields.end()) {
    return false;
  }
  return it->second.value_type == EntityFieldType::INTEGER;
}

/**
 * \brief Returns the value of an integer field of this entity.
 * \param key Key of the field to get. It must exist and be an integer.
 * \return The integer value.
 */
int EntityData::get_integer(const std::string& key) const {

  const auto& it = fields.find(key);
  Debug::check_assertion(it != fields.end(),
      "No such entity field in " + get_type_name() + ": '" + key + "'");

  Debug::check_assertion(it->second.value_type == EntityFieldType::INTEGER,
      "Field '" + key + "' is not a string");

  return it->second.int_value;
}

/**
 * \brief Sets the value of an integer field of this entity.
 * \param key Key of the field to set. It must exist and be an integer.
 * \param value The integer value.
 */
void EntityData::set_integer(const std::string& key, int value) {

  const auto& it = fields.find(key);
  Debug::check_assertion(it != fields.end(),
      "No such entity field in " + get_type_name() + ": '" + key + "'");

  Debug::check_assertion(it->second.value_type == EntityFieldType::INTEGER,
      "Field '" + key + "' is not an integer");

  it->second.int_value = value;
}

/**
 * \brief Returns whether a field of this entity is a boolean.
 * \param key Key of the field to get.
 * \return \c true if the field exists and is a boolean.
 */
bool EntityData::is_boolean(const std::string& key) const {

  const auto& it = fields.find(key);
  if (it == fields.end()) {
    return false;
  }
  return it->second.value_type == EntityFieldType::BOOLEAN;
}

/**
 * \brief Returns the value of a boolean field of this entity.
 * \param key Key of the field to get. It must exist and be a boolean.
 * \return The boolean value.
 */
bool EntityData::get_boolean(const std::string& key) const {

  const auto& it = fields.find(key);
  Debug::check_assertion(it != fields.end(),
      "No such entity field in " + get_type_name() + ": '" + key + "'");

  Debug::check_assertion(it->second.value_type == EntityFieldType::BOOLEAN,
      "Field '" + key + "' is not a boolean");

  return it->second.int_value != 0;
}

/**
 * \brief Sets the value of a boolean field of this entity.
 * \param key Key of the field to set. It must exist and be a boolean.
 * \param value The boolean value.
 */
void EntityData::set_boolean(const std::string& key, bool value) {

  const auto& it = fields.find(key);
  Debug::check_assertion(it != fields.end(),
      "No such entity field in " + get_type_name() + ": '" + key + "'");

  Debug::check_assertion(it->second.value_type == EntityFieldType::BOOLEAN,
      "Field '" + key + "' is not an boolean");

  it->second.int_value = value ? 1 : 0;
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
  const std::string& name = LuaTools::opt_string_field(l, 1, "name", "");
  Layer layer = LuaTools::check_layer_field(l, 1, "layer");
  int x = LuaTools::check_int_field(l, 1, "x");
  int y = LuaTools::check_int_field(l, 1, "y");

  EntityData entity(type);
  entity.set_name(name);
  entity.set_layer(layer);
  entity.set_xy({ x, y });

  // Additional fields for this type.
  const EntityTypeDescription& type_description = entity_type_descriptions.at(type);
  for (const EntityFieldDescription& field_description : type_description) {

    const std::string& key = field_description.key;
    const FieldValue& default_value = field_description.default_value;
    switch (default_value.value_type) {

      case EntityFieldType::STRING:
      {
        std::string value;
        if (field_description.optional == OptionalFlag::OPTIONAL) {
          value = LuaTools::opt_string_field(l, 1, key, default_value.string_value);
        }
        else {
          value = LuaTools::check_string_field(l, 1, key);
        }
        entity.set_string(key, value);
        break;
      }

      case EntityFieldType::INTEGER:
      {
        int value;
        if (field_description.optional == OptionalFlag::OPTIONAL) {
          value = LuaTools::opt_int_field(l, 1, key, default_value.int_value);
        }
        else {
          value = LuaTools::check_int_field(l, 1, key);
        }
        entity.set_integer(key, value);
        break;
      }

      case EntityFieldType::BOOLEAN:
      {
        bool value;
        if (field_description.optional == OptionalFlag::OPTIONAL) {
          value = LuaTools::opt_boolean_field(l, 1, key, default_value.int_value != 0);
        }
        else {
          value = LuaTools::check_boolean_field(l, 1, key);
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
 * This is the case of HERO, CARRIED_ITEM, BOOMERANG, ARROW and HOOKHOT.
 * The ones in the map can be created either by declaring them in a map
 * data file or by the Lua API, or both.
 */
const std::map<EntityType, const EntityTypeDescription> EntityData::get_entity_type_descriptions() {
  return entity_type_descriptions;
}

}  // namespace Solarus
