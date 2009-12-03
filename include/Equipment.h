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
#ifndef ZSDX_EQUIPMENT_H
#define ZSDX_EQUIPMENT_H

#include "Common.h"
#include "InventoryItemId.h"
#include "Treasure.h"

/**
 * This class represents the hero's equipment. It makes the interface
 * between the savegame and other engine classes.
 * You should call this class to know his current equipment
 * (sword, rupees, objects...) and to modify it.
 */
class Equipment {

 private:

  Savegame *savegame;

  // magic bar decrease handling
  uint32_t magic_decrease_delay;
  uint32_t next_magic_decrease_date;

  // giving some bottle content to the player
  bool giving_fairy;
  bool giving_water;
  InventoryItemId destination_bottle_id;

 public:

  Equipment(Savegame *savegame);
  ~Equipment(void);

  void update(void);

  // tunic
  int get_tunic(void);
  void set_tunic(int tunic);

  // sword
  bool has_sword(void);
  int get_sword(void);
  void set_sword(int sword);

  // shield
  bool has_shield(void);
  int get_shield(void);
  void set_shield(int shield);

  // rupees
  int get_max_rupees(void);
  void set_max_rupees(int max_rupees);

  int get_rupees(void);
  void set_rupees(int rupees);
  void add_rupees(int rupees_to_add);
  void remove_rupees(int rupees_to_remove);

  // hearts
  int get_max_hearts(void);
  void set_max_hearts(int max_hearts);
  void add_heart_container(void);

  int get_hearts(void);
  void set_hearts(int hearts);
  void add_hearts(int hearts_to_add);
  void remove_hearts(int hearts_to_remove);
  void restore_all_hearts(void);
  bool needs_hearts(void);

  int get_nb_pieces_of_heart(void);
  void add_piece_of_heart(void);

  // magic
  int get_max_magic(void);
  void set_max_magic(int max_magic);

  int get_magic(void);
  void set_magic(int magic);
  void add_magic(int magic_to_add);
  void remove_magic(int magic_to_remove);
  void restore_all_magic(void);
  bool needs_magic(void);

  bool is_magic_decreasing(void);
  void start_removing_magic(uint32_t delay);
  void stop_removing_magic(void);

  // bombs
  int get_max_bombs(void);
  void set_max_bombs(int max_bombs);

  int get_bombs(void);
  void set_bombs(int bombs);
  void add_bombs(int bombs_to_add);
  void remove_bomb(void);
  bool needs_bombs(void);

  // arrows
  int get_max_arrows(void);
  void set_max_arrows(int max_arrows);

  int get_arrows(void);
  void set_arrows(int arrows);
  void add_arrows(int arrows_to_add);
  void remove_arrow(void);
  bool needs_arrows(void);

  // inventory items
  int has_inventory_item(InventoryItemId item_id);
  void give_inventory_item(InventoryItemId item_id);
  void give_inventory_item(InventoryItemId item_id, int variant);
  void remove_inventory_item(InventoryItemId item_id);

  int get_inventory_item_amount(InventoryItemId item_id);
  void set_inventory_item_amount(InventoryItemId item_id, int amount);
  void add_inventory_item_amount(InventoryItemId item_id, int amount_to_add);
  void remove_inventory_item_amount(InventoryItemId item_id, int amount_to_remove);

  int get_inventory_item_maximum(InventoryItemId item_id);
  bool has_inventory_item_maximum(InventoryItemId item_id);

  // bottles
  void add_bottle(void);
  bool has_bottle(void);
  bool has_empty_bottle(void);
  InventoryItemId get_first_empty_bottle(void);
  bool has_bottle_with(Treasure::Content content);
  InventoryItemId get_first_bottle_with(Treasure::Content content);
  InventoryItemId get_destination_bottle(void);
  void set_bottle_content(InventoryItemId bottle_id, Treasure::Content content);
  void set_bottle_empty(InventoryItemId bottle_id);

  void found_fairy(void);
  void found_water(void);
  void found_water(InventoryItemId bottle_id);

  // item assignments
  InventoryItemId get_item_assigned(int slot);
  void set_item_assigned(int slot, InventoryItemId item_id);

  // quest status
  bool has_world_map(void);
  void add_world_map(void);

  // small keys
  bool are_small_keys_enabled(void);
  int get_small_keys_variable(void);

  bool has_small_key(void);
  int get_small_keys(void);
  void add_small_key(void);
  void remove_small_key(void);

  // ability to lift items
  bool can_lift(int weight);
};

#endif
