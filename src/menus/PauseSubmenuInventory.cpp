/*
 * Copyright (C) 2009 Christopho, Solarus - http://www.solarus-engine.org
 * 
 * Solarus is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Solarus is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "menus/PauseSubmenuInventory.h"
#include "menus/PauseMenu.h"
#include "movements/TargetMovement.h"
#include "Sprite.h"
#include "ResourceManager.h"
#include "lowlevel/Sound.h"
#include "Game.h"
#include "DialogBox.h"
#include "Equipment.h"
#include "Savegame.h"
#include "Counter.h"
#include "KeysEffect.h"
#include "InventoryItem.h"
#include "StringResource.h"
#include "lowlevel/Surface.h"

/**
 * Constructor.
 * @param pause_menu the pause menu object
 * @param game the game
 */
PauseSubmenuInventory::PauseSubmenuInventory(PauseMenu *pause_menu, Game *game):
  PauseSubmenu(pause_menu, game) {

  cursor_sprite = new Sprite("menus/pause_cursor");
  cursor_sound = ResourceManager::get_sound("cursor");

  items_img = new Surface("hud/inventory_items.png");

  // set the counters and the caption strings
  for (int k = 0; k < 28; k++) {

    // get the item, its counter property and the possession state    
    InventoryItemId item_id = InventoryItemId(k);
    int variant = equipment->has_inventory_item(item_id);

    if (variant != 0 && InventoryItem::has_counter(item_id)) {

      // if the player has the item and this item has a counter, we show a counter

      int amount = equipment->get_inventory_item_amount(item_id);
      int maximum = equipment->get_inventory_item_maximum(item_id);
      int x = 60 + (k % 7) * 32;
      int y = 81 + (k / 7) * 32;

      counters[k] = new Counter(2, false, x, y);
      counters[k]->set_maximum(maximum);
      counters[k]->set_value(amount);
    }
    else {
      counters[k] = NULL;
    }

    // initialize the caption strings
    if (variant != 0) {
      int id = InventoryItem::is_bottle(item_id) ? INVENTORY_BOTTLE_1 : item_id;
      std::ostringstream oss;
      oss << "inventory.caption.item_" << id << "_" << variant;
      caption_strings[k] = StringResource::get_string(oss.str());
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
  delete items_img;

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
  InventoryItemId item_id = InventoryItemId(row * 7 + column);
  int variant = equipment->has_inventory_item(item_id);

  set_caption_text(caption_strings[item_id]);
  if (variant != 0) {
    keys_effect->set_action_key_effect(KeysEffect::ACTION_KEY_INFO);
    keys_effect->set_item_keys_enabled(InventoryItem::can_be_assigned(item_id));
  }
  else {
    keys_effect->set_action_key_effect(KeysEffect::ACTION_KEY_NONE);
    keys_effect->set_item_keys_enabled(false);
  }
}

/**
 * Returns the index of the cell currently selected in the inventory.
 * The value returned identifies an item and corresponds directly to a
 * value from the InventoryItemId enum.
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
  
  InventoryItemId item_id = InventoryItemId(get_selected_index());
  int variant = equipment->has_inventory_item(item_id);

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
void PauseSubmenuInventory::display(Surface *destination) {

  // display the common part
  PauseSubmenu::display(destination);

  // display each inventory item
  Rectangle src_position(0, 0, 16, 16);
  Rectangle dst_position(0, 69, 0, 0);

  int k = 0;
  for (int i = 0; i < 4; i++) {

    dst_position.set_x(56);

    for (int j = 0; j < 7; j++, k++) {

      // get the possession state of this item
      InventoryItemId item_id = InventoryItemId(k);
      int variant = equipment->has_inventory_item(item_id);

      if (variant > 0) {

	// the player has this item, display the variant he has
	src_position.set_xy(16 * item_id, 16 * (variant - 1));
	items_img->blit(src_position, destination, dst_position);

	// display the counter (if any)
	if (counters[k] != NULL) {
	  counters[k]->display(destination);
	}
      }

      dst_position.add_x(32);
    }
    dst_position.add_y(32);
  }

  // display the cursor
  int x = 64 + 32 * cursor_column;
  int y = 77 + 32 * cursor_row;

  cursor_sprite->display(destination, x, y);

  // display the item being assigned
  if (item_assigned_movement != NULL) {
    
    src_position.set_xy(16 * item_assigned_id, 16 * (item_assigned_variant - 1));
    dst_position.set_xy(item_assigned_movement->get_x(), item_assigned_movement->get_y());
    items_img->blit(src_position, destination, dst_position);
  }
}

/**
 * Shows a message describing the item currently selected.
 * The player must have this item.
 */
void PauseSubmenuInventory::show_info_message(void) {

  InventoryItemId item_id = InventoryItemId(get_selected_index());
  int variant = equipment->has_inventory_item(item_id);

  // exception: for a bottle, replace its real id by the id of the first bottle
  if (item_id == INVENTORY_BOTTLE_2
      || item_id == INVENTORY_BOTTLE_3
      || item_id == INVENTORY_BOTTLE_4) {
    item_id = INVENTORY_BOTTLE_1;
  }

  std::ostringstream oss;
  oss << "_item_description_" << item_id << '_' << variant;

  DialogBox::VerticalPosition vertical_position = (cursor_row >= 2) ? DialogBox::POSITION_TOP : DialogBox::POSITION_BOTTOM;

  game->get_dialog_box()->start_message_sequence(oss.str(), vertical_position);
}

/**
 * Assigns the selected item to a slot.
 * The operation does not take effect immediately: the item picture is thrown to
 * its destination icon, then the assignment is done.
 * Nothing is done if the item is not assignable.
 * @param slot slot to set (0 for X or 1 for V)
 */
void PauseSubmenuInventory::assign_item(int slot) {

  InventoryItemId selected_item_id = InventoryItemId(get_selected_index());

  // if this item is not assignable, do nothing
  if (!InventoryItem::can_be_assigned(selected_item_id)) {
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

  item_assigned_movement = new TargetMovement(x2, y2, 50);
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
  InventoryItemId current_item_id = equipment->get_item_assigned(slot);
  InventoryItemId other_item_id = equipment->get_item_assigned(1 - slot);

  if (other_item_id == item_assigned_id) {
    equipment->set_item_assigned(1 - slot, current_item_id);
  }
  equipment->set_item_assigned(slot, item_assigned_id);

  delete item_assigned_movement;
  item_assigned_movement = NULL;
}

