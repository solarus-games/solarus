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
#ifndef SOLARUS_TYPE_H
#define SOLARUS_TYPE_H

namespace Solarus {

/**
 * \brief Constants to identify each kind of map entity.
 *
 * Each constant correspond to a subclass of Entity.
 * They can be stored in the map file
 * (tiles, teletransporters, chests, stairs, doors, etc.).
 * or generated dynamically during the game
 * (hero, explosion, boomerang, etc.).
 */
enum class EntityType {

  // entities specified in the map file

  TILE,               /**< a tile on the map, obstacle or not */
  DESTINATION,        /**< a destination point on the map */
  TELETRANSPORTER,    /**< a teletransporter: the hero is transported
                              * to a destination point when walking on it */
  PICKABLE,           /**< a treasure that the hero can pick up: a rupee, a heart, a fairy... */
  DESTRUCTIBLE,       /**< something that the hero can cut or lift: a pot, a bush, a stone... */
  CHEST,              /**< a chest (small or big) with a treasure */
  JUMPER,             /**< a sensor that makes the hero jump in a direction */
  ENEMY,              /**< an enemy */
  NPC,                /**< an entity the hero can interact with by pressing the action key
                              * in front of it (not necessarily a person) */
  BLOCK,              /**< a block or a statue that the hero can push */
  DYNAMIC_TILE,       /**< a dynamic tile (that can be enabled or disabled) */
  SWITCH,             /**< a switch */
  WALL,               /**< an invisible obstacle for some type of entities */
  SENSOR,             /**< a sensor that triggers something when the hero overlaps it */
  CRYSTAL,            /**< a switch that lowers or raises the blue and orange blocks */
  CRYSTAL_BLOCK,      /**< the blue or orange blocks that are changed by the crystals */
  SHOP_TREASURE,      /**< a treasure the hero can buy in a shop */
  STREAM,             /**< a special terrain that moves the hero */
  DOOR,               /**< a door to open with a key or another action */
  STAIRS,             /**< stairs between two maps or inside a single map */
  SEPARATOR,          /**< an horizontal or vertical separation for the camera */
  CUSTOM,             /**< a entity fully controlled by the scripts */

  // entities created only during the game (not stored in maps)

  CAMERA,             /**< the camera */
  HERO,               /**< the hero */
  CARRIED_OBJECT,       /**< item carried and thrown by the hero (comes from a destructible item) */
  BOOMERANG,          /**< a boomerang thrown */
  EXPLOSION,          /**< an explosion, that may be caused by a bomb, a bomb flower, an enemy, etc. */
  ARROW,              /**< an arrow thrown by the bow */
  BOMB,               /**< a bomb that will explode soon */
  FIRE,               /**< some fire */
  HOOKSHOT,           /**< the hookshot thrown by the hero */
};

}

#endif

