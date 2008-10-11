#include "menus/PauseSubmenuInventory.h"
#include "menus/PauseMenu.h"
#include "Sprite.h"
#include "ResourceManager.h"
#include "Sound.h"
#include "Game.h"
#include "Equipment.h"
#include "Savegame.h"
#include "Counter.h"
#include "KeysEffect.h"
#include "movements/TargetMovement.h"

/**
 * Caption text displayed when an item is selected in the inventory.
 */
static const string item_names[28][6] = {
  // TODO: load this from some external file (for future translation)

  {"Plume"},
  {"Bombes"},
  {"Arc", "Arc & Flèches"},
  {"Boomerang"},
  {"Lanterne"},
  {"Grappin"},
  {"Flacon Magique\n(Vide)", "Flacon Magique\n(Eau)", "Flacon Magique\n(Potion rouge)",
   "Flacon Magique\n(Potion verte)", "Flacon Magique\n(Potion blue)", "Flacon Magique\n(Fée)"},

  {"Bottes de Pégase"},
  {"Miroir Mystique"},
  {"Canne de Somaria"},
  {"Pommes"},
  {"Pains au Chocolat"},
  {"Croissants"},
  {"Flacon Magique\n(Vide)", "Flacon Magique\n(Eau)", "Flacon Magique\n(Potion rouge)",
   "Flacon Magique\n(Potion verte)", "Flacon Magique\n(Potion blue)", "Flacon Magique\n(Fée)"},

  {"Clé de Roc"},
  {"Clé Rouge"},
  {"Clé de Terre"},
  {"Tarte aux Pommes", "Lingots d'or", "Edelweiss", "Clé d'Os"},
  {"Palmes"},
  {"Cape Magique"},
  {"Flacon Magique\n(Vide)", "Flacon Magique\n(Eau)", "Flacon Magique\n(Potion rouge)",
   "Flacon Magique\n(Potion verte)", "Flacon Magique\n(Potion blue)", "Flacon Magique\n(Fée)"},

  {"Clé de Fer"},
  {"Clé de Pierre"},
  {"Clé de Bois"},
  {"Clé de Glace"},
  {"Poigne de Fer", "Poigne d'Or"},
  {"Pierres de Feu"},
  {"Flacon Magique\n(Vide)", "Flacon Magique\n(Eau)", "Flacon Magique\n(Potion rouge)",
   "Flacon Magique\n(Potion verte)", "Flacon Magique\n(Potion blue)", "Flacon Magique\n(Fée)"},
};

/**
 * Constructor.
 * @param pause_menu the pause menu object
 * @param game the game
 */
PauseSubmenuInventory::PauseSubmenuInventory(PauseMenu *pause_menu, Game *game):
  PauseSubmenu(pause_menu, game) {

  cursor_sprite = new Sprite("menus/pause_cursor");
  cursor_sound = ResourceManager::get_sound("cursor");

  items_img = ResourceManager::load_image("hud/inventory_items.png");

  // set the counters
  for (int k = 0; k < 28; k++) {

    // get the item, its counter property and the possession state    
    InventoryItem::ItemId item_id = (InventoryItem::ItemId) k;
    InventoryItem *item = InventoryItem::get_item(item_id);
    int variant = equipment->has_inventory_item(item_id);

    if (variant != 0 && item->has_counter()) {

      // if the player has the item and this item has a counter, we show a counter

      int amount = savegame->get_integer(item->get_counter_index());
      int x = 60 + (k % 7) * 32;
      int y = 81 + (k / 7) * 32;

      counters[k] = new Counter(2, false, x, y);
      counters[k]->set_value(amount);
    }
    else {
      counters[k] = NULL;
    }
  }

  // initialize the cursor
  set_cursor_position(savegame->get_integer(Savegame::INVENTORY_LAST_ROW),
		      savegame->get_integer(Savegame::INVENTORY_LAST_COLUMN));

  item_assigned_movement = NULL;
}

/**
 * Destructor.
 */
PauseSubmenuInventory::~PauseSubmenuInventory(void) {

  // if an item was being assigned, finish the assignement immediately
  if (is_assigning_item()) {
    finish_assigning_item();
  }

  game->get_keys_effect()->set_item_keys_enabled(true);

  // free the memory
  delete cursor_sprite;
  SDL_FreeSurface(items_img);

  for (int k = 0; k < 28; k++) {
    if (counters[k] != NULL) {
      delete counters[k];
    }
  }
}

/**
 * Sets the position of the cursor.
 * In other words, selects an item in the inventory.
 * @param row row of the cursor, between 0 and 3
 * @param column columng of the cursor, between 0 and 6
 */
void PauseSubmenuInventory::set_cursor_position(int row, int column) {

  // update the cursor position
  cursor_row = row;
  cursor_column = column;

  savegame->set_integer(Savegame::INVENTORY_LAST_ROW, row);
  savegame->set_integer(Savegame::INVENTORY_LAST_COLUMN, column);

  // update the caption text, show or hide the action icon
  KeysEffect *keys_effect = game->get_keys_effect();
  InventoryItem::ItemId item_id = (InventoryItem::ItemId) (row * 7 + column);
  InventoryItem *item = InventoryItem::get_item(item_id);
  int variant = equipment->has_inventory_item(item_id);

  if (variant != 0) {
    set_caption_text(item_names[item_id][variant - 1]);
    keys_effect->set_action_key_effect(KeysEffect::ACTION_KEY_INFO);
    keys_effect->set_item_keys_enabled(item->is_attributable());
  }
  else {
    set_caption_text("");
    keys_effect->set_action_key_effect(KeysEffect::ACTION_KEY_NONE);
    keys_effect->set_item_keys_enabled(false);
  }
}

/**
 * Returns the index of the cell currently selected in the inventory.
 * The value returned identifies an item and corresponds directly to a
 * value from the InventoryItem::ItemId enum.
 * @return the index of the selected cell, between 0 and 27
 */
int PauseSubmenuInventory::get_selected_index(void) {
  return cursor_row * 7 + cursor_column;
}

/**
 * Returns whether an item is selected, i.e. whether the cursor is on an
 * item that the player has.
 * @return true if an item is currently selected
 */
bool PauseSubmenuInventory::is_item_selected(void) {
  
  int item_id = get_selected_index();
  int variant = equipment->has_inventory_item((InventoryItem::ItemId) item_id);

  return variant != 0;
}

/**
 * This function is called when a key is pressed on this submenu.
 * @param key the key pressed
 */
void PauseSubmenuInventory::key_pressed(Controls::GameKey key) {

  KeysEffect *keys_effect = game->get_keys_effect();

  switch (key) {

  case Controls::ACTION:
    if (keys_effect->get_action_key_effect() == KeysEffect::ACTION_KEY_INFO) {
      show_info_message();
    }
    break;

  case Controls::ITEM_1:
    if (is_item_selected()) {
      assign_item(0);
    }
    break;

  case Controls::ITEM_2:
    if (is_item_selected()) {
      assign_item(1);
    }
    break;

  case Controls::LEFT:
    if (cursor_column == 0) {
      pause_menu->show_left_submenu();
    }
    else {
      cursor_sound->play();
      set_cursor_position(cursor_row, cursor_column - 1);
    }
    break;

  case Controls::RIGHT:
    if (cursor_column == 6) {
      pause_menu->show_right_submenu();
    }
    else {
      cursor_sound->play();
      set_cursor_position(cursor_row, cursor_column + 1);
    }
    break;

  case Controls::UP:
    cursor_sound->play();
    set_cursor_position((cursor_row + 3) % 4, cursor_column);
    break;

  case Controls::DOWN:
    cursor_sound->play();
    set_cursor_position((cursor_row + 1) % 4, cursor_column);
    break;

  default:
    break;
  }
}

/**
 * Updates this submenu.
 * The items displayed and their counters are not updated since the
 * equipment does not change while the game is paused.
 */
void PauseSubmenuInventory::update(void) {

  // animation of the cursor
  cursor_sprite->update();

  // item being thrown
  if (item_assigned_movement != NULL) {
    item_assigned_movement->update();

    if (item_assigned_movement->is_finished()) {
      finish_assigning_item();
    }
  }
}

/**
 * Displays this submenu.
 * @param destination the destination surface
 */
void PauseSubmenuInventory::display(SDL_Surface *destination) {

  // display the common part
  PauseSubmenu::display(destination);

  // display each inventory item
  SDL_Rect src_position = {0, 0, 16, 16};
  SDL_Rect dst_position = {0, 69, 0, 0};

  int k = 0;
  for (int i = 0; i < 4; i++) {

    dst_position.x = 56;

    for (int j = 0; j < 7; j++, k++) {

      // get the possession state of this item
      InventoryItem::ItemId item_id = (InventoryItem::ItemId) k;
      int variant = equipment->has_inventory_item(item_id);

      if (variant > 0) {

	// the player has this item, display the variant he has
	src_position.x = 16 * item_id;
	src_position.y = 16 * (variant - 1);
	SDL_BlitSurface(items_img, &src_position, destination, &dst_position);

	// display the counter (if any)
	if (counters[k] != NULL) {
	  counters[k]->display(destination);
	}
      }

      dst_position.x += 32;
    }
    dst_position.y += 32;
  }

  // display the cursor
  int x = 64 + 32 * cursor_column;
  int y = 77 + 32 * cursor_row;

  cursor_sprite->display(destination, x, y);

  // display the item being assigned
  if (item_assigned_movement != NULL) {
    
    src_position.x = 16 * item_assigned_id;
    src_position.y = 16 * (item_assigned_variant - 1);

    dst_position.x = item_assigned_movement->get_x();
    dst_position.y = item_assigned_movement->get_y();

    SDL_BlitSurface(items_img, &src_position, destination, &dst_position);
  }
}

/**
 * Shows a message describing the item currently selected.
 * The player must have this item.
 */
void PauseSubmenuInventory::show_info_message(void) {

  int item_id = get_selected_index();
  int variant = equipment->has_inventory_item((InventoryItem::ItemId) item_id);

  // exception: for a bottle, replace its real id by the id of the first bottle
  if (item_id == InventoryItem::BOTTLE_2
      || item_id == InventoryItem::BOTTLE_3
      || item_id == InventoryItem::BOTTLE_4) {
    item_id = InventoryItem::BOTTLE_1;
  }

  ostringstream oss;
  oss << "_item_description_" << item_id << '_' << variant;

  int position = (cursor_row >= 2) ? 0 : 1;

  game->show_message(oss.str(), position);
}

/**
 * Assigns the selected item to a slot.
 * The operation does not take effect immediately: the item picture is thrown to
 * its destination icon, then the assignment is done.
 * Nothing is done if the item is not assignable.
 * @param slot slot to set (0 for X or 1 for V)
 */
void PauseSubmenuInventory::assign_item(int slot) {

  InventoryItem::ItemId selected_item_id = (InventoryItem::ItemId) get_selected_index();
  InventoryItem *item = InventoryItem::get_item(selected_item_id);

  // if this item is not assignable, do nothing
  if (!item->is_attributable()) {
    return;
  }

  // if another item is being assigned, finish it immediately
  if (is_assigning_item()) {
    finish_assigning_item();
  }

  // memorize this item
  this->item_assigned_id = selected_item_id;
  this->item_assigned_variant = equipment->has_inventory_item(item_assigned_id);
  this->item_assigned_destination = slot;

  // play the sound
  ResourceManager::get_sound("throw")->play();

  // compute the movement
  int x1 = 60 + 32 * cursor_column;
  int y1 = 75 + 32 * cursor_row;
  int x2 = (slot == 0) ? 12 : 64;
  int y2 = 33;

  item_assigned_movement = new TargetMovement(50, x2, y2);
  item_assigned_movement->set_position(x1, y1);
}

/**
 * Returns whether an item is currently being thrown to an icon.
 * @return true if an item is being assigned
 */
bool PauseSubmenuInventory::is_assigning_item(void) {
  return item_assigned_movement != NULL;
}

/**
 * Stops assigning the item right now.
 * This function is called when we want to assign the item without
 * waiting for its throwing movement to end, for example when the inventory submenu
 * is being closed.
 */
void PauseSubmenuInventory::finish_assigning_item(void) {

  // if the item to assign is already assigned to the other icon, switch the two items
  int slot = item_assigned_destination;
  InventoryItem::ItemId current_item_id = equipment->get_item_assigned(slot);
  InventoryItem::ItemId other_item_id = equipment->get_item_assigned(1 - slot);

  if (other_item_id == item_assigned_id) {
    equipment->set_item_assigned(1 - slot, current_item_id);
  }
  equipment->set_item_assigned(slot, item_assigned_id);

  delete item_assigned_movement;
  item_assigned_movement = NULL;
}
