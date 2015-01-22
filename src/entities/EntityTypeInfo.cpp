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
    { EntityType::TILE, "tile" },
    { EntityType::DESTINATION, "destination" },
    { EntityType::TELETRANSPORTER, "teletransporter" },
    { EntityType::PICKABLE, "pickable" },
    { EntityType::DESTRUCTIBLE, "destructible" },
    { EntityType::CHEST, "chest" },
    { EntityType::JUMPER, "jumper" },
    { EntityType::ENEMY, "enemy" },
    { EntityType::NPC, "npc" },
    { EntityType::BLOCK, "block" },
    { EntityType::DYNAMIC_TILE, "dynamic_tile" },
    { EntityType::SWITCH, "switch" },
    { EntityType::WALL, "wall" },
    { EntityType::SENSOR, "sensor" },
    { EntityType::CRYSTAL, "crystal" },
    { EntityType::CRYSTAL_BLOCK, "crystal_block" },
    { EntityType::SHOP_TREASURE, "shop_treasure" },
    { EntityType::STREAM, "stream" },
    { EntityType::DOOR, "door" },
    { EntityType::STAIRS, "stairs" },
    { EntityType::SEPARATOR, "separator" },
    { EntityType::CUSTOM, "custom_entity" },
    { EntityType::HERO, "hero" },
    { EntityType::CARRIED_ITEM, "carried_object" },
    { EntityType::BOOMERANG, "boomerang" },
    { EntityType::EXPLOSION, "explosion" },
    { EntityType::ARROW, "arrow" },
    { EntityType::BOMB, "bomb" },
    { EntityType::FIRE, "fire" },
    { EntityType::HOOKSHOT, "hookshot" }
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

}

}
