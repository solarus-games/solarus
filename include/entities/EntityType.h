/*
 * Copyright (C) 2009 Christopho, Solarus - http://www.solarus-engine.org
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

/**
 * @brief Constants to identify each kind of map entity.
 *
 * Each constant correspond to a subclass of MapEntity.
 * Entities with an entity type lower than 1000 are
 * stored in the map file (tiles, teletransporters, chests,
 * stairs, doors, etc.).
 * Entities with an entity type greater than or equal to 1000
 * are only generated dynamically during the game
 * (hero, explosion, boomerang, etc.).
 */
enum EntityType {

  // entities specified in the map file

  TILE                      = 0,    /**< a tile on the map, obstacle or not */
  DESTINATION_POINT         = 1,    /**< a destination point on the map */
  TELETRANSPORTER           = 2,    /**< a teletransporter: the hero is transported
				     * to a destination point when walking on it */
  PICKABLE_ITEM             = 3,    /**< an item that the hero can pick: a rupee, a heart, a fairy... */
  DESTRUCTIBLE_ITEM         = 4,    /**< an item that the hero can cut or lift: a pot, a bush, a stone... */
  CHEST                     = 5,    /**< a chest (small or big) with a treasure */
  JUMPER               = 6,    /**< a sensor that makes the hero jump in a direction */
  ENEMY                     = 7,    /**< an enemy */
  INTERACTIVE_ENTITY        = 8,    /**< an entity the hero can interact with by pressing the action key
				     * in front of it (typically an NPC) */
  BLOCK                     = 9,    /**< a block or a statue that the hero can push */
  DYNAMIC_TILE              = 10,   /**< a dynamic tile (that can be enabled or disabled) */
  SWITCH                    = 11,   /**< a switch */
  CUSTOM_OBSTACLE           = 12,   /**< an obstacle for some other type of entities */
  SENSOR                    = 13,   /**< a sensor that triggers something when the hero overlaps it */
  CRYSTAL            = 14,   /**< a switch that lowers or raises the blue and orange blocks */
  CRYSTAL_BLOCK      = 15,   /**< the blue or orange blocks that are changed by the crystals */
  SHOP_ITEM                 = 16,   /**< an item the hero can buy in a shop */
  CONVEYOR_BELT             = 17,   /**< a special terrain that moves the hero */
  DOOR                      = 18,   /**< a door to open with a key or another action */
  STAIRS                    = 19,   /**< stairs between two maps or inside a single map */

  // entities created only during the game (not stored in maps)

  HERO                      = 1000, /**< the hero */
  CARRIED_ITEM              = 1001, /**< item carried and thrown by the hero (comes from a destructible item) */
  BOOMERANG                 = 1002, /**< a boomerang thrown */
  EXPLOSION                 = 1003, /**< an explosion, that may be caused by a bomb, a bomb flower, an enemy, etc. */
  ARROW                     = 1004, /**< an arrow thrown by the bow */
  BOMB                      = 1005, /**< a bomb that will explode soon */
  FIRE                      = 1006, /**< some fire */
  HOOKSHOT                  = 1007  /**< the hookshot thrown by the hero */
};

#endif

