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
    NONE                        = -1,

    FEATHER                     = 0,
    BOMBS                       = 1,
    BOW                         = 2, /**< 1: bow without arrows, 2: bow with arrows */
    BOOMERANG                   = 3,
    LAMP                        = 4,
    HOOK_SHOT                   = 5,
    BOTTLE_1                    = 6, /**< 1: empty, 2: red potion, 3: green potion, 4: blue potion, 5: fairy */

    PEGASUS_SHOES               = 7,
    MYSTIC_MIRROR               = 8,
    CANE_OF_SOMARIA             = 9,
    MAGIC_CAPE                  = 10,
    GLOVE                       = 11, /**< 1: iron glove, 2: golden glove */
    FIRE_STONES                 = 12,
    BOTTLE_2                    = 13,

    APPLES                      = 14,
    PAINS_AU_CHOCOLAT           = 15,
    CROISSANTS                  = 16,
    L4_WAY_BONE_KEY             = 17, /**< 1: apple pie, 2: gold bars, 3: edelweiss, 4: bone key */
    FLIPPERS                    = 18,
    RED_KEY                     = 19,
    TODO_20                     = 20, /**< free slot for a new item (or bottle 3 if no need) */

    CLAY_KEY                    = 21,
    ROCK_KEY                    = 22,
    IRON_KEY                    = 23,
    STONE_KEY                   = 24,
    WOODEN_KEY                  = 25,
    ICE_KEY                     = 26,
    TODO_27                     = 27,
  };

 private:

  bool attributable;   /**< true if this item can be assigned to icon X or V */
  int counter_index;   /**< for an item with a counter (bombs, arrows, etc.),
						   * index of the savegame variable indicating the
						   * counter's value (0 if there is no counter) */
  
  static InventoryItem items[28];

  InventoryItem(bool attributable, int counter_index);
  ~InventoryItem(void);

 public:

  static InventoryItem *get_item(ItemId id);

  bool is_attributable(void);
  int get_counter_index(void);
};

#endif
