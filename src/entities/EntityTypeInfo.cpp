/*
 * Copyright (C) 2006-2015 Christopho, Solarus - http://www.solarus-games.org
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
#include "solarus/entities/EntityTypeInfo.h"
#include "solarus/lowlevel/Debug.h"

namespace Solarus {

namespace {

/**
 * \brief Lua name of each map entity type.
 */
const std::map<EntityType, std::string> entity_type_names = {
    { EntityType::ARROW, "arrow" },
    { EntityType::BLOCK, "block" },
    { EntityType::BOMB, "bomb" },
    { EntityType::BOOMERANG, "boomerang" },
    { EntityType::CARRIED_ITEM, "carried_object" },
    { EntityType::CHEST, "chest" },
    { EntityType::CRYSTAL, "crystal" },
    { EntityType::CRYSTAL_BLOCK, "crystal_block" },
    { EntityType::CUSTOM, "custom_entity" },
    { EntityType::DESTINATION, "destination" },
    { EntityType::DESTRUCTIBLE, "destructible" },
    { EntityType::DOOR, "door" },
    { EntityType::DYNAMIC_TILE, "dynamic_tile" },
    { EntityType::ENEMY, "enemy" },
    { EntityType::EXPLOSION, "explosion" },
    { EntityType::HERO, "hero" },
    { EntityType::HOOKSHOT, "hookshot" },
    { EntityType::FIRE, "fire" },
    { EntityType::JUMPER, "jumper" },
    { EntityType::NPC, "npc" },
    { EntityType::PICKABLE, "pickable" },
    { EntityType::SENSOR, "sensor" },
    { EntityType::SEPARATOR, "separator" },
    { EntityType::SHOP_TREASURE, "shop_treasure" },
    { EntityType::STAIRS, "stairs" },
    { EntityType::STREAM, "stream" },
    { EntityType::SWITCH, "switch" },
    { EntityType::TELETRANSPORTER, "teletransporter" },
    { EntityType::TILE, "tile" },
    { EntityType::WALL, "wall" }
};

}  // Anonymous namespace

namespace EntityTypeInfo {

/**
 * \brief Returns the entity types and their Lua name.
 *
 * This is the name used in data files and return by the Lua function
 * entity:get_type().
 *
 * \return The name of each entity type.
 */
const std::map<EntityType, std::string>& get_entity_type_names() {
  return entity_type_names;
}

/**
 * \brief Returns the name of an entity type.
 *
 * This is the name used in data files and return by the Lua function
 * entity:get_type().
 *
 * \param type A type of entity.
 * \return The corresponding name.
 */
const std::string& get_entity_type_name(EntityType entity_type) {

  return entity_type_names.at(entity_type);
}

/**
 * \brief Returns the entity type with the given name.
 *
 * This is the name used in data files and return by the Lua function
 * entity:get_type().
 *
 * \param entity_type_name The name of an entity type. It must exist.
 * \return The corresponding type.
 */
EntityType get_entity_type_by_name(const std::string& entity_type_name) {

  for (const auto& kvp: entity_type_names) {
    if (kvp.second == entity_type_name) {
      return kvp.first;
    }
  }

  Debug::die(std::string("Unknown entity type: ") + entity_type_name);
  return EntityType();
}


/**
 * \brief Returns whether entities of the specified type can be stored in map files.
 * \param type A type of entity.
 * \return \c true if this type can be stored.
 */
bool can_be_stored_in_map_file(EntityType type) {

  switch (type) {

  case EntityType::BLOCK:
  case EntityType::CHEST:
  case EntityType::CRYSTAL:
  case EntityType::CRYSTAL_BLOCK:
  case EntityType::CUSTOM:
  case EntityType::DESTINATION:
  case EntityType::DESTRUCTIBLE:
  case EntityType::DOOR:
  case EntityType::DYNAMIC_TILE:
  case EntityType::ENEMY:
  case EntityType::JUMPER:
  case EntityType::NPC:
  case EntityType::PICKABLE:
  case EntityType::SENSOR:
  case EntityType::SEPARATOR:
  case EntityType::SHOP_TREASURE:
  case EntityType::STAIRS:
  case EntityType::STREAM:
  case EntityType::SWITCH:
  case EntityType::TELETRANSPORTER:
  case EntityType::TILE:
  case EntityType::WALL:
    return true;

  case EntityType::ARROW:
  case EntityType::BOMB:
  case EntityType::BOOMERANG:
  case EntityType::CARRIED_ITEM:
  case EntityType::EXPLOSION:
  case EntityType::HERO:
  case EntityType::HOOKSHOT:
  case EntityType::FIRE:
    return false;
  }

  return false;
}

/**
 * \brief Returns whether entities of the specified type can be created
 * dynamically by using the Lua scripting API.
 * \param type A type of entity.
 * \return \c true if this type can be created through the scripting API.
 */
bool can_be_created_from_lua_api(EntityType type) {

  switch (type) {

  case EntityType::BLOCK:
  case EntityType::BOMB:
  case EntityType::CHEST:
  case EntityType::CRYSTAL:
  case EntityType::CRYSTAL_BLOCK:
  case EntityType::CUSTOM:
  case EntityType::DESTINATION:
  case EntityType::DESTRUCTIBLE:
  case EntityType::DOOR:
  case EntityType::DYNAMIC_TILE:
  case EntityType::ENEMY:
  case EntityType::EXPLOSION:
  case EntityType::FIRE:
  case EntityType::JUMPER:
  case EntityType::NPC:
  case EntityType::PICKABLE:
  case EntityType::SENSOR:
  case EntityType::SEPARATOR:
  case EntityType::SHOP_TREASURE:
  case EntityType::STAIRS:
  case EntityType::STREAM:
  case EntityType::SWITCH:
  case EntityType::TELETRANSPORTER:
  case EntityType::WALL:
    // These ones can be created by scripts.
    return true;

  case EntityType::ARROW:
  case EntityType::BOOMERANG:
  case EntityType::CARRIED_ITEM:
  case EntityType::HERO:
  case EntityType::HOOKSHOT:
  case EntityType::TILE:
    // These ones are only created by the engine.
    // Tiles can only stored in the map data file.
    return false;

  }

  return false;
}

}  // namespace EntityTypeInfo

}  // namespace Solarus

