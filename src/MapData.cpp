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
#include "solarus/lowlevel/Debug.h"
#include "solarus/MapData.h"
#include <vector>

namespace Solarus {

namespace {

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

/**
 * \brief Definition of the format of all entity types in map data files.
 *
 * Note: entities whose type is not in this map can only be created
 * by the engine.
 * This is the case of HERO, CARRIED_ITEM, BOOMERANG, ARROW and HOOKHOT.
 * The ones in the map can be created either by declaring them in a map
 * data file or by the Lua API, or both.
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
            { "stops_hero", OptionalFlag::MANDATORY, FieldValue(false) },
            { "stops_npcs", OptionalFlag::MANDATORY, FieldValue(false) },
            { "stops_enemies", OptionalFlag::MANDATORY, FieldValue(false) },
            { "stops_blocks", OptionalFlag::MANDATORY, FieldValue(false) },
            { "stops_projectiles", OptionalFlag::MANDATORY, FieldValue(false) },
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
 * \brief Creates a value of type string.
 * \param value The value.
 */
FieldValue::FieldValue(const std::string& value):
    value_type(EntityFieldType::STRING),
    string_value(value),
    int_value(0) {

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

bool EntityData::is_string(const std::string& key) const {

  const auto& it = fields.find(key);
  if (it == fields.end()) {
    return false;
  }
  return it->second.value_type == EntityFieldType::STRING;
}

const std::string& EntityData::get_string(const std::string& key) const {

  const auto& it = fields.find(key);
  Debug::check_assertion(it != fields.end(),
      "No such entity field: '" + key + "'");

  Debug::check_assertion(it->second.value_type == EntityFieldType::STRING,
      "Field '" + key + "' is not a string");

  return it->second.string_value;
}

void EntityData::set_string(const std::string& key, const std::string& value) {

  const auto& it = fields.find(key);
  Debug::check_assertion(it != fields.end(),
      "No such entity field: '" + key + "'");

  Debug::check_assertion(it->second.value_type == EntityFieldType::STRING,
      "Field '" + key + "' is not a string");

  it->second.string_value = value;
}

bool EntityData::is_integer(const std::string& key) const {

  const auto& it = fields.find(key);
  if (it == fields.end()) {
    return false;
  }
  return it->second.value_type == EntityFieldType::INTEGER;
}

int EntityData::get_integer(const std::string& key) const {

  const auto& it = fields.find(key);
  Debug::check_assertion(it != fields.end(),
      "No such entity field: '" + key + "'");

  Debug::check_assertion(it->second.value_type == EntityFieldType::INTEGER,
      "Field '" + key + "' is not a string");

  return it->second.int_value;
}

void EntityData::set_integer(const std::string& key, int value) {

  const auto& it = fields.find(key);
  Debug::check_assertion(it != fields.end(),
      "No such entity field: '" + key + "'");

  Debug::check_assertion(it->second.value_type == EntityFieldType::INTEGER,
      "Field '" + key + "' is not an integer");

  it->second.int_value = value;
}

bool EntityData::is_boolean(const std::string& key) const {

  const auto& it = fields.find(key);
  if (it == fields.end()) {
    return false;
  }
  return it->second.value_type == EntityFieldType::BOOLEAN;
}

bool EntityData::get_boolean(const std::string& key) const {

  const auto& it = fields.find(key);
  Debug::check_assertion(it != fields.end(),
      "No such entity field: '" + key + "'");

  Debug::check_assertion(it->second.value_type == EntityFieldType::BOOLEAN,
      "Field '" + key + "' is not a boolean");

  return it->second.int_value != 0;
}

void EntityData::set_boolean(const std::string& key, bool value) {

  const auto& it = fields.find(key);
  Debug::check_assertion(it != fields.end(),
      "No such entity field: '" + key + "'");

  Debug::check_assertion(it->second.value_type == EntityFieldType::BOOLEAN,
      "Field '" + key + "' is not an boolean");

  it->second.int_value = value ? 1 : 0;
}

/**
 * \brief Creates an empty map data object.
 */
MapData::MapData():
    size(0, 0),
    world(),
    location(0, 0),
    floor(NO_FLOOR),
    tileset_id(),
    music_id("none"),
    entities(),
    named_entities() {

}

/**
 * \brief Returns the size of the map.
 * \return The map size in pixels.
 */
Size MapData::get_size() const {
  return size;
}

/**
 * \brief Sets the size of the map.
 * \param size The map size in pixels.
 */
void MapData::set_size(const Size& size) {
  this->size = size;
}

/**
 * \brief Returns the location of this map in its world.
 * \return The location of the upper-left corner of the map in pixels.
 */
Point MapData::get_location() const {
  return location;
}

/**
 * \brief Sets the location of this map in its world.
 * \param location The location of the upper-left corner of the map in pixels.
 */
void MapData::set_location(const Point& location) {
  this->location = location;
}

/**
 * \brief Returns whether this map is in a world.
 * \return \c true if the map is in a world.
 */
bool MapData::has_world() const {
  return !world.empty();
}

/**
 * \brief Returns the world this maps belongs to.
 * \return The world or an empty string.
 */
const std::string& MapData::get_world() const {
  return world;
}

/**
 * \brief Sets the world this maps belongs to.
 * \param world The world or an empty string.
 */
void MapData::set_world(const std::string& world) {
  this->world = world;
}

/**
 * \brief Returns whether this map has a floor number.
 * \return \c true if the map has a floor number.
 */
bool MapData::has_floor() const {
  return floor != NO_FLOOR;
}

/**
 * \brief Returns the floor number of this map.
 * \return The floor number or Map::NO_FLOOR.
 */
int MapData::get_floor() const {
  return floor;
}

/**
 * \brief Sets the floor number of this map.
 * \param floor The floor number or Map::NO_FLOOR.
 */
void MapData::set_floor(int floor) {
  this->floor = floor;
}

/**
 * \brief Returns the tileset used to draw this map.
 * \return The tileset id.
 * An empty string means that the tileset is not set yet.
 */
const std::string& MapData::get_tileset_id() const {
  return tileset_id;
}

/**
 * \brief Sets the tileset used to draw this map.
 * \param tileset_id The tileset id.
 * An empty string means that the tileset is not set yet.
 */
void MapData::set_tileset_id(const std::string& tileset_id) {
  this->tileset_id = tileset_id;
}

/**
 * \brief Returns the music played on this map.
 * \return The music id or "none" or "same".
 */
const std::string& MapData::get_music_id() const {
  return music_id;
}

/**
 * \brief Sets the music played on this map.
 * \param music_id The music id or "none" or "same".
 */
void MapData::set_music_id(const std::string& music_id) {
  this->music_id = music_id;
}

/**
 * \brief Returns the total number of entities on this map.
 * \return The number of entities.
 */
int MapData::get_num_entities() const {

  int num_entities = 0;
  for (const EntityList& layer_entities : entities) {
    num_entities += layer_entities.size();
  }

  return num_entities;
}

/**
 * \brief Returns the number of entities on a layer of this map.
 * \param layer A layer.
 * \return The number of entities on that layer.
 */
int MapData::get_num_entities(Layer layer) const {
  return entities[layer].size();
}

/**
 * \brief Changes the layer of an entity.
 * \param layer The current layer.
 * \param index Index of the entity to change in that layer.
 * \param new_layer The new layer to set.
 */
void MapData::set_entity_layer(Layer /* layer */, int /* index */, Layer /* new_layer */) {
  // TODO
}

/**
 * \brief Brings an entity to front on its layer.
 * \param layer The layer of the entity to change.
 * \param index Index of the entity in that layer.
 */
void MapData::bring_entity_to_front(Layer /* layer */, int /* index */) {
  // TODO
}

/**
 * \brief Brings an entity to back on its layer.
 * \param layer The layer of the entity to change.
 * \param index Index of the entity in that layer.
 */
void MapData::bring_entity_to_back(Layer /* layer */, int /* index */) {
  // TODO
}

/**
 * \brief Returns the entity at the given layer and index.
 * \param layer The layer of the entity to get.
 * \param index Index of the entity in that layer.
 * \return The entity data.
 */
const EntityData& MapData::get_entity(Layer layer, int index) const {
  return entities[layer][index];
}

/**
 * \brief Returns the entity at the given layer and index.
 *
 * Non-const version.
 *
 * \param layer The layer of the entity to get.
 * \param index Index of the entity in that layer.
 * \return The entity data.
 */
EntityData& MapData::get_entity(Layer layer, int index) {
  return entities[layer][index];
}

/**
 * \brief Returns an entity given its name.
 * \param name Name of the entity to get.
 * \return The entity or nullptr if there is no such entity.
 */
const EntityData* MapData::get_entity_by_name(const std::string& /* name */) const {
  // TODO
  return nullptr;
}

/**
 * \brief Returns an entity given its name.
 *
 * Non-const version. Ownership of the pointer remains to MapData.
 *
 * \param name Name of the entity to get.
 * \return The entity or nullptr if there is no such entity.
 */
EntityData* MapData::get_entity_by_name(const std::string& /* name */) {
  // TODO
  return nullptr;
}

/**
 * \brief Returns whether there exists an entity with the specified name.
 * \param name The name to check.
 * \return \c true if there exists an entity with this name on the map.
 */
bool MapData::entity_exists(const std::string& /* name */) const {
  // TODO
  return false;
}

/**
 * \brief Changes the name of an entity.
 * \param layer Layer of the entity to change.
 * \param index Index of the entity in that layer.
 * \param name The new name. An empty string means no name.
 * \return \c true in case of success, \c false if the name was already used.
 */
bool MapData::set_entity_name(Layer /* layer */, int /* index */, const std::string& /* name */) {
  // TODO
  return false;
}

/**
 * \brief Returns the index of an entity in its layer.
 * \param entity The information of an entity.
 * \return The index of this entity in its layer.
 * Returns -1 if the entity is not on this map.
 */
int MapData::get_entity_index(const EntityData& /* entity */) {
  // TODO
  return 0;
}

/**
 * \brief Adds an entity to the map.
 *
 * If the new entity has a name, it should be unique on the map.
 *
 * \param entity The information of an entity.
 * \return The index of this entity in its layer.
 */
bool MapData::add_entity(const EntityData& /* entity */) {
  // TODO
  return false;
}

/**
 * \brief Removes an entity from the map.
 * \param layer Layer of the entity to remove.
 * \param index Index of the entity in its layer.
 */
void MapData::remove_entity(Layer /* layer */, int /* index */) {
  // TODO
}

/**
 * \brief Removes an entity from the map.
 * \param entity The entity information.
 */
void MapData::remove_entity(const EntityData& /* entity */) {
  // TODO
}

/**
 * \copydoc LuaData::import_from_lua
 */
bool MapData::import_from_lua(lua_State* /* l */) {
  // TODO
  return false;
}

/**
 * \copydoc LuaData::export_to_lua
 */
bool MapData::export_to_lua(std::ostream& /* out */) const {
  // TODO
  return false;
}


}  // namespace Solarus
