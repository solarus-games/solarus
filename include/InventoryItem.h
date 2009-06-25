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
#include "InventoryItemId.h"

/**
 * This class provides the description of each item of the inventory
 * with some static methods,
 * and handles their behavior when it is instanciated.
 * This inventory items does not include the dungeon items (map, compass, etc.)
 * nor the items of the quest status screen (rupee bag, heart pieces, etc.).
 */
class InventoryItem {

 private:

  InventoryItemId item_id; /**< id of this item */

  int variant;             /**< the possession state of this item when it is used */
  Game *game;              /**< the game this item is used in */
  bool finished;           /**< true if we have finished using this item */

  void start_bottle(void);
  void update_bottle(void);

 public:

  // creation and destruction
  InventoryItem(InventoryItemId item_id);
  ~InventoryItem(void);

  // static features
  static bool can_be_assigned(InventoryItemId item_id);
  static bool has_counter(InventoryItemId item_id);
  static int get_counter_index(InventoryItemId item_id);
  static bool is_bottle(InventoryItemId item_id);

  // properties
  InventoryItemId get_id(void);
  int get_variant(void);
  bool is_bottle(void);

  // state
  void start(Game *game);
  void update(void);
  void set_map(Map *map);
  bool is_finished(void);
};

#endif
