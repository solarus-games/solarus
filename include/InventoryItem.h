#ifndef ZSDX_INVENTORY_ITEM_H
#define ZSDX_INVENTORY_ITEM_H

#include "Common.h"
#include "Savegame.h"

/**
 * This class provides the description of each item of the inventory.
 * This does not include the dungeon items (map, compass, etc.)
 * nor the items of the quest status screen.
 */
class InventoryItem {

 public:

  /**
   * Constant identifying each item.
   * These constants permit to determine:
   * - the savegame slot of each item
   * - the place for each item in the inventory screen
   */
  enum ItemId {
    ITEM_FEATHER                     = 0,
    ITEM_BOMBS                       = 1,
    ITEM_BOW                         = 2, /**< 1: bow without arrows, 2: bow with arrows */
    ITEM_BOOMERANG                   = 3,
    ITEM_LAMP                        = 4,
    ITEM_HOOK_SHOT                   = 5,
    ITEM_BOTTLE_1                    = 6, /**< 1: empty, 2: red potion, 3: green potion, 4: blue potion, 5: fairy */

    ITEM_PEGASUS_SHOES               = 7,
    ITEM_MYSTIC_MIRROR               = 8,
    ITEM_CANE_OF_SOMARIA             = 9,
    ITEM_MAGIC_CAPE                  = 10,
    ITEM_GLOVE                       = 11, /**< 1: iron glove, 2: golden glove */
    ITEM_FIRE_STONES                 = 12,
    ITEM_BOTTLE_2                    = 13,

    ITEM_APPLES                      = 14,
    ITEM_PAINS_AU_CHOCOLAT           = 15,
    ITEM_CROISSANTS                  = 16,
    ITEM_L4_WAY_BONE_KEY             = 17, /**< 1: apple pie, 2: gold bars, 3: eidelweiss, 4: bone key */
    ITEM_FLIPPERS                    = 18,
    ITEM_RED_KEY                     = 19,
    ITEM_20_TODO                     = 20, /**< free slot for a new item (or bottle 3 if no need) */

    ITEM_CLAY_KEY                    = 21,
    ITEM_ROCK_KEY                    = 22,
    ITEM_IRON_KEY                    = 23,
    ITEM_STONE_KEY                   = 24,
    ITEM_WOODEN_KEY                  = 25,
    ITEM_ICE_KEY                     = 26,
    ITEM_27_TODO                     = 27,
  };
 
  bool can_be_equiped; /**< true if this item can be equiped to icon X or V */
  int index;           /**< for an item with a counter (bombs, arrows, etc.): index of the
			* savegame variable indicating the counter's value */

 private:

  static const InventoryItem items[28];

 public:

  static InventoryItem *get_item(ItemId id);
};

#endif
