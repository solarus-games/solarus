#ifndef ZSDX_PICKABLE_ITEM_TYPE_H
#define ZSDX_PICKABLE_ITEM_TYPE_H

/**
 * Types of pickable items.
 */
enum PickableItemType {

  PICKABLE_ITEM_RANDOM      = -1,   /**< special value to indicate to choose another value randomly (including PICKABLE_ITEM_NONE) */
  PICKABLE_ITEM_NONE        = 0,    /**< special value to indicate that there is no pickable item */
  PICKABLE_ITEM_RUPEE_1     = 1,
  PICKABLE_ITEM_RUPEE_5     = 2,
  PICKABLE_ITEM_RUPEE_20    = 3,
  PICKABLE_ITEM_HEART       = 4,
  PICKABLE_ITEM_SMALL_MAGIC = 5,
  PICKABLE_ITEM_BIG_MAGIC   = 6,
  PICKABLE_ITEM_FAIRY       = 7,
  PICKABLE_ITEM_BOMB_1      = 8,
  PICKABLE_ITEM_BOMB_5      = 9,
  PICKABLE_ITEM_BOMB_10     = 10,
  PICKABLE_ITEM_ARROW_1     = 11,
  PICKABLE_ITEM_ARROW_5     = 12,
  PICKABLE_ITEM_ARROW_10    = 13,

  /* TODO add an additional parameter in the entity syntax in the map file,
     for items that one can take only once (keys, pieces of heart, heart container)
     and ignored for the other items
  PICKABLE_ITEM_SMALL_KEY   = 14,
  PICKABLE_ITEM_BIG_KEY,
  PICKABLE_ITEM_BOSS_KEY,
  */
};

#endif
