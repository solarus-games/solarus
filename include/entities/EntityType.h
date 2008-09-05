#ifndef ENTITY_TYPE_H
#define ENTITY_TYPE_H

/**
 * Constants to identify each kind of map entity.
 */
enum EntityType {
  ENTITY_TILE                      = 0, /**< a tile on the map, obstacle or not */
  ENTITY_ENTRANCE                  = 1, /**< an entrance of the map */
  ENTITY_EXIT                      = 2, /**< an exit: Link is transported to another map when walking on an exit */
  ENTITY_PICKABLE_ITEM             = 3, /**< an item that Link can pick: a rupee, a heart, a fairy... */
  ENTITY_TRANSPORTABLE_ITEM        = 4, /**< an item that Link can lift, carry and throw: a pot, a bush, a stone... */
  ENTITY_CHEST                     = 5, /**< a chest (small or big) with a treasure */
};

#endif
