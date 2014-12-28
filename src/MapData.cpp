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
 * \brief Creates an empty map data object.
 */
MapData::MapData():
    size(0, 0),
    world(),
    location(0, 0),
    floor(NO_FLOOR),
    tileset_id(),
    music_id(),
    entities(),
    named_entities() {

}

/**
 * \brief Returns the size of the map in pixels.
 * \return The map size.
 */
Size MapData::get_size() const {
  return size;
}

void MapData::set_size(const Size& size) {
  this->size = size;
}

Point MapData::get_location() const {
  return location;
}

void MapData::set_location(const Point& location) {
  this->location = location;
}

bool MapData::has_world() const {
  return !world.empty();
}

const std::string& MapData::get_world() const {
  return world;
}

void MapData::set_world(const std::string& world) {
  this->world = world;
}

bool MapData::has_floor() const {
  return floor != NO_FLOOR;
}

int MapData::get_floor() const {
  return floor;
}

void MapData::set_floor(int floor) {
  this->floor = floor;
}

const std::string& MapData::get_tileset_id() const {
  return tileset_id;
}

void MapData::set_tileset_id(const std::string& tileset_id) {
  this->tileset_id = tileset_id;
}

const std::string& MapData::get_music_id() const {
  return music_id;
}

void MapData::set_music_id(const std::string& music_id) {
  this->music_id = music_id;
}

int MapData::get_num_entities() const {

  int num_entities = 0;
  for (const EntityList& entities : entities) {
    num_entities += entities.size();
  }

  return num_entities;
}

int MapData::get_num_entities(Layer layer) const {
  return entities[layer].size();
}

void MapData::set_entity_layer(Layer /* layer */, int /* index */, Layer /* new_layer */) {
  // TODO
}

void MapData::bring_entity_to_front(Layer /* layer */, int /* index */) {
  // TODO
}

void MapData::bring_entity_to_back(Layer /* layer */, int /* index */) {
  // TODO
}

const EntityData& MapData::get_entity(Layer layer, int index) const {
  return entities[layer][index];
}

EntityData& MapData::get_entity(Layer layer, int index) {
  return entities[layer][index];
}

const EntityData* MapData::get_entity_by_name(const std::string& /* name */) const {
  // TODO
  return nullptr;
}

EntityData* MapData::get_entity_by_name(const std::string& /* name */) {
  // TODO
  return nullptr;
}

bool MapData::entity_exists(const std::string& /* name */) const {
  // TODO
  return false;
}

bool MapData::set_entity_name(Layer /* layer */, int /* index */, const std::string& /* name */) {
  // TODO
  return false;
}

int MapData::get_entity_index(const EntityData& /* entity */) {
  // TODO
  return 0;
}

void MapData::add_entity(const EntityData& /* entity */) {
  // TODO
}

void MapData::remove_entity(Layer /* layer */, int /* index */) {
  // TODO
}

void MapData::remove_entity(const EntityData& /* entity */) {
  // TODO
}

bool MapData::import_from_lua(lua_State* /* l */) {
  // TODO
  return false;
}

bool MapData::export_to_lua(std::ostream& /* out */) const {
  // TODO
  return false;
}


}  // namespace Solarus
