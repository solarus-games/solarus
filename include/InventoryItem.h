/*
 * Copyright (C) 2009 Christopho, Zelda Solarus - http://www.zelda-solarus.com
 * 
 * Zelda: Mystery of Solarus DX is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Zelda: Mystery of Solarus DX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
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
    BOTTLE_1                    = 6, /**< 1: empty, 2: water, 3: red potion, 4: green potion, 5: blue potion, 6: fairy */

    PEGASUS_SHOES               = 7,
    MYSTIC_MIRROR               = 8,
    CANE_OF_SOMARIA             = 9,
    APPLES                      = 10,
    PAINS_AU_CHOCOLAT           = 11,
    CROISSANTS                  = 12,
    BOTTLE_2                    = 13,

    ROCK_KEY                    = 14,
    RED_KEY                     = 15,
    CLAY_KEY                    = 16,
    L4_WAY_BONE_KEY             = 17, /**< 1: apple pie, 2: gold bars, 3: edelweiss, 4: bone key */
    FLIPPERS                    = 18,
    MAGIC_CAPE                  = 19,
    BOTTLE_3                    = 20,

    IRON_KEY                    = 21,
    STONE_KEY                   = 22,
    WOODEN_KEY                  = 23,
    ICE_KEY                     = 24,
    GLOVE                       = 25, /**< 1: iron glove, 2: golden glove */
    FIRE_STONES                 = 26,
    BOTTLE_4                    = 27,
  };

 private:

  ItemId id;           /**< id of  this item */
  bool attributable;   /**< true if this item can be assigned to icon X or V */
  int counter_index;   /**< for an item with a counter (bombs, arrows, etc.),
			* index of the savegame variable indicating the
			* counter's value (0 if there is no counter) */
  
  static InventoryItem items[28];

  InventoryItem(ItemId id, bool attributable, int counter_index);
  ~InventoryItem(void);

 public:

  static InventoryItem *get_item(ItemId id);

  bool is_attributable(void);
  bool has_counter(void);
  int get_counter_index(void);
  void use(void);
};

#endif
