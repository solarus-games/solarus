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
#ifndef ZSDX_INVENTORY_ITEM_ID_H
#define ZSDX_INVENTORY_ITEM_ID_H

/**
 * Constant identifying each item of the inventory.
 * These constants allow the class InventoryItem to determine
 * the savegame slot of each item and
 * the place for each item in the inventory screen.
 */
enum InventoryItemId {
  ITEM_NONE                        = -1,

  ITEM_FEATHER                     = 0,
  ITEM_BOMBS                       = 1,
  ITEM_BOW                         = 2, /**< 1: bow without arrows, 2: bow with arrows */
  ITEM_BOOMERANG                   = 3,
  ITEM_LAMP                        = 4,
  ITEM_HOOK_SHOT                   = 5,
  ITEM_BOTTLE_1                    = 6, /**< 1: empty, 2: water, 3: red potion, 4: green potion, 5: blue potion, 6: fairy */

  ITEM_PEGASUS_SHOES               = 7,
  ITEM_MYSTIC_MIRROR               = 8,
  ITEM_CANE_OF_SOMARIA             = 9,
  ITEM_APPLES                      = 10,
  ITEM_PAINS_AU_CHOCOLAT           = 11,
  ITEM_CROISSANTS                  = 12,
  ITEM_BOTTLE_2                    = 13,

  ITEM_ROCK_KEY                    = 14,
  ITEM_RED_KEY                     = 15,
  ITEM_CLAY_KEY                    = 16,
  ITEM_L4_WAY_BONE_KEY             = 17, /**< 1: apple pie, 2: gold bars, 3: edelweiss, 4: bone key */
  ITEM_FLIPPERS                    = 18,
  ITEM_MAGIC_CAPE                  = 19,
  ITEM_BOTTLE_3                    = 20,

  ITEM_IRON_KEY                    = 21,
  ITEM_STONE_KEY                   = 22,
  ITEM_WOODEN_KEY                  = 23,
  ITEM_ICE_KEY                     = 24,
  ITEM_GLOVE                       = 25, /**< 1: iron glove, 2: golden glove */
  ITEM_FIRE_STONES                 = 26,
  ITEM_BOTTLE_4                    = 27,
};

#endif
