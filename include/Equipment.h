#ifndef ZSDX_EQUIPMENT_H
#define ZSDX_EQUIPMENT_H

#include "Common.h"
#include "InventoryItem.h"
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
  Uint32 magic_decrease_delay;
  Uint32 next_magic_decrease_date;

  // giving a fairy to the player
  bool giving_fairy;

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

  void give_fairy(void);

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
  void start_removing_magic(Uint32 delay);
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
  int has_inventory_item(InventoryItem::ItemId item_id);
  void give_inventory_item(InventoryItem::ItemId item_id);
  void give_inventory_item(InventoryItem::ItemId item_id, int variant);
  void remove_inventory_item(InventoryItem::ItemId item_id);

  void add_bottle(void);
  bool has_empty_bottle(void);
  InventoryItem::ItemId get_first_empty_bottle(void);
  bool has_bottle_with(Treasure::Content content);
  InventoryItem::ItemId get_first_bottle_with(Treasure::Content content);
  void set_bottle_content(InventoryItem::ItemId bottle_id, Treasure::Content content);
  void set_bottle_empty(InventoryItem::ItemId bottle_id);

  int get_inventory_item_amount(InventoryItem::ItemId item_id);
  void set_inventory_item_amount(InventoryItem::ItemId item_id, int amount);
  void add_inventory_item_amount(InventoryItem::ItemId item_id, int amount_to_add);
  void remove_inventory_item_amount(InventoryItem::ItemId item_id, int amount_to_remove);

  InventoryItem::ItemId get_item_assigned(int slot);
  void set_item_assigned(int slot, InventoryItem::ItemId item_id);

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
