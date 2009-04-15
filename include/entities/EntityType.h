#ifndef ZSDX_ENTITY_TYPE_H
#define ZSDX_ENTITY_TYPE_H

/**
 * Constants to identify each kind of map entity.
 */
enum EntityType {
  TILE                      = 0,    /**< a tile on the map, obstacle or not */
  DESTINATION_POINT         = 1,    /**< a destination point on the map */
  TELETRANSPORTER           = 2,    /**< a teletransporter: the hero is transported
				     * to a destination point when walking on it */
  PICKABLE_ITEM             = 3,    /**< an item that the hero can pick: a rupee, a heart, a fairy... */
  DESTRUCTIBLE_ITEM         = 4,    /**< an item that the hero can cut or lift: a pot, a bush, a stone... */
  CHEST                     = 5,    /**< a chest (small or big) with a treasure */
  JUMP_SENSOR               = 6,    /**< a sensor that makes the hero jump in a direction */
  ENEMY                     = 7,    /**< an enemy */
  INTERACTIVE_ENTITY        = 8,    /**< an entity the hero can interact with by pressing the action key
				     * in front of it (typically an NPC) */
  BLOCK                     = 9,    /**< a block or a statue that the hero can push */
  DYNAMIC_TILE              = 10,   /**< a dynamic tile (that can be enabled or disabled) */
  SWITCH                    = 11,   /**< a switch */
  CUSTOM_OBSTACLE           = 12,   /**< an obstacle for some other type of entities */
  SENSOR                    = 13,   /**< a sensor that triggers something when the hero overlaps it */
  CRYSTAL_SWITCH            = 14,   /**< a switch that lower or raises the blue and orange blocks */
  CRYSTAL_SWITCH_BLOCK      = 15,   /**< the blue or orange blocks that are changed by the crystal switches */

  HERO                      = 1000, /**< the hero */
  CARRIED_ITEM              = 1001, /**< item carried and thrown by the hero (comes from a destructible item) */
};

#endif
