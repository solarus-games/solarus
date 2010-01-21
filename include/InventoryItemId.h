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
#ifndef SOLARUS_INVENTORY_ITEM_ID_H
#define SOLARUS_INVENTORY_ITEM_ID_H

/**
 * Constants identifying each item of the inventory.
 * These constants allow the engine to determine
 * the savegame variable of each item and
 * the place for each item in the inventory screen.
 */
enum InventoryItemId {
  INVENTORY_NONE                        = -1,

  INVENTORY_FEATHER                     = 0,
  INVENTORY_BOMBS                       = 1,
  INVENTORY_BOW                         = 2, /**< 1: bow without arrows, 2: bow with arrows */
  INVENTORY_BOOMERANG                   = 3,
  INVENTORY_LAMP                        = 4,
  INVENTORY_HOOK_SHOT                   = 5,
  INVENTORY_BOTTLE_1                    = 6, /**< 1: empty, 2: water, 3: red potion, 4: green potion, 5: blue potion, 6: fairy */

  INVENTORY_PEGASUS_SHOES               = 7,
  INVENTORY_MYSTIC_MIRROR               = 8,
  INVENTORY_CANE_OF_SOMARIA             = 9,
  INVENTORY_APPLES                      = 10,
  INVENTORY_PAINS_AU_CHOCOLAT           = 11,
  INVENTORY_CROISSANTS                  = 12,
  INVENTORY_BOTTLE_2                    = 13,

  INVENTORY_ROCK_KEY                    = 14,
  INVENTORY_RED_KEY                     = 15,
  INVENTORY_CLAY_KEY                    = 16,
  INVENTORY_L4_WAY_BONE_KEY             = 17, /**< 1: apple pie, 2: gold bars, 3: edelweiss, 4: bone key */
  INVENTORY_FLIPPERS                    = 18,
  INVENTORY_MAGIC_CAPE                  = 19,
  INVENTORY_BOTTLE_3                    = 20,

  INVENTORY_IRON_KEY                    = 21,
  INVENTORY_STONE_KEY                   = 22,
  INVENTORY_WOODEN_KEY                  = 23,
  INVENTORY_ICE_KEY                     = 24,
  INVENTORY_GLOVE                       = 25, /**< 1: iron glove, 2: golden glove */
  INVENTORY_FIRE_STONES                 = 26,
  INVENTORY_BOTTLE_4                    = 27,
};

#endif
