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
#ifndef SOLARUS_ENTITY_TYPE_H
#define SOLARUS_ENTITY_TYPE_H

namespace Solarus {

/**
 * \brief Constants to identify each kind of map entity.
 *
 * Each constant correspond to a subclass of MapEntity.
 * They can be stored in the map file
 * (tiles, teletransporters, chests, stairs, doors, etc.).
 * or generated dynamically during the game
 * (hero, explosion, boomerang, etc.).
 */
enum EntityType {

  // entities specified in the map file

  ENTITY_TILE,               /**< a tile on the map, obstacle or not */
  ENTITY_DESTINATION,        /**< a destination point on the map */
  ENTITY_TELETRANSPORTER,    /**< a teletransporter: the hero is transported
                              * to a destination point when walking on it */
  ENTITY_PICKABLE,           /**< a treasure that the hero can pick up: a rupee, a heart, a fairy... */
  ENTITY_DESTRUCTIBLE,       /**< something that the hero can cut or lift: a pot, a bush, a stone... */
  ENTITY_CHEST,              /**< a chest (small or big) with a treasure */
  ENTITY_JUMPER,             /**< a sensor that makes the hero jump in a direction */
  ENTITY_ENEMY,              /**< an enemy */
  ENTITY_NPC,                /**< an entity the hero can interact with by pressing the action key
                              * in front of it (not necessarily a person) */
  ENTITY_BLOCK,              /**< a block or a statue that the hero can push */
  ENTITY_DYNAMIC_TILE,       /**< a dynamic tile (that can be enabled or disabled) */
  ENTITY_SWITCH,             /**< a switch */
  ENTITY_WALL,               /**< an invisible obstacle for some type of entities */
  ENTITY_SENSOR,             /**< a sensor that triggers something when the hero overlaps it */
  ENTITY_CRYSTAL,            /**< a switch that lowers or raises the blue and orange blocks */
  ENTITY_CRYSTAL_BLOCK,      /**< the blue or orange blocks that are changed by the crystals */
  ENTITY_SHOP_TREASURE,      /**< a treasure the hero can buy in a shop */
  ENTITY_STREAM,             /**< a special terrain that moves the hero */
  ENTITY_DOOR,               /**< a door to open with a key or another action */
  ENTITY_STAIRS,             /**< stairs between two maps or inside a single map */
  ENTITY_SEPARATOR,          /**< an horizontal or vertical separation for the camera */
  ENTITY_CUSTOM,             /**< a entity fully controlled by the scripts */

  // entities created only during the game (not stored in maps)

  ENTITY_HERO,               /**< the hero */
  ENTITY_CARRIED_ITEM,       /**< item carried and thrown by the hero (comes from a destructible item) */
  ENTITY_BOOMERANG,          /**< a boomerang thrown */
  ENTITY_EXPLOSION,          /**< an explosion, that may be caused by a bomb, a bomb flower, an enemy, etc. */
  ENTITY_ARROW,              /**< an arrow thrown by the bow */
  ENTITY_BOMB,               /**< a bomb that will explode soon */
  ENTITY_FIRE,               /**< some fire */
  ENTITY_HOOKSHOT,           /**< the hookshot thrown by the hero */

  ENTITY_NUMBER,             /**< Number of entity types. */
};

}

#endif

