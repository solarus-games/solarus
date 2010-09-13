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
#include "Game.h"
#include "DialogBox.h"
#include "Equipment.h"
#include "Savegame.h"
#include "Counter.h"
#include "KeysEffect.h"
#include "InventoryItem.h"
#include "ItemProperties.h"
#include "StringResource.h"
#include "lowlevel/Surface.h"

// TODO load from external data
static const std::string item_names[] = {
  "feather",
  "bombs",
  "bow",
  "boomerang",
  "lamp",
  "hookshot",
  "bottle_1",

  "speed_shoes",
  "mystic_mirror",
  "cane_of_somaria",
  "apples",
  "pains_au_chocolat",
  "croissants",
  "bottle_2",

  "rock_key",
  "red_key",
  "clay_key",
  "l4_way_bone_key",
  "flippers",
  "magic_cape",
  "bottle_3",

  "iron_key",
  "stone_key",
  "wooden_key",
  "ice_key",
  "glove",
  "fire_stones",
  "bottle_4"
};

/**
 * @brief Constructor.
 * @param pause_menu the pause menu object
 * @param game the game
 */
PauseSubmenuInventory::PauseSubmenuInventory(PauseMenu *pause_menu, Game *game):
  PauseSubmenu(pause_menu, game) {

  cursor_sprite = new Sprite("menus/pause_cursor");

  // set the sprites, counters and caption strings
  for (int k = 0; k < 28; k++) {

    // get the item, its counter property and the possession state
    const std::string &item_name = item_names[k];
    int variant = equipment->get_item_variant(item_name);
    ItemProperties *item_properties = equipment->get_item_properties(item_name);

    if (variant != 0 && item_properties->has_counter()) {

      // if the player has the item and this item has an amount, we show a counter

      int amount = equipment->get_item_amount(item_name);
      int maximum = equipment->get_item_maximum(item_name);
      int x = 60 + (k % 7) * 32;
      int y = 81 + (k / 7) * 32;

      counters[k] = new Counter(2, false, x, y);
      counters[k]->set_maximum(maximum);
      counters[k]->set_value(amount);
    }
    else {
      counters[k] = NULL;
    }

    // initialize the sprite and the caption string
    if (variant != 0) {

      std::ostringstream oss;

      // sprite
      sprites[k] = new Sprite("hud/inventory_item");
      sprites[k]->set_current_animation(item_name);
      sprites[k]->set_current_direction(variant - 1);

      // caption string
      oss.str("");
      oss << "inventory.caption.item." << item_name << "." << variant;
      caption_strings[k] = StringResource::get_string(oss.str());
    }
    else {
      sprites[k] = NULL;
    }
  }

  // initialize the cursor
  int index = savegame->get_integer(Savegame::INVENTORY_LAST_ITEM_INDEX);
  int row = index / 7;
  int column = index % 7;
  set_cursor_position(row, column);

  item_assigned_movement = NULL;
}

/**
 * @brief Destructor.
 */
PauseSubmenuInventory::~PauseSubmenuInventory(void) {

  // if an item was being assigned, finish the assignement immediately
  if (is_assigning_item()) {
    finish_assigning_item();
  }

  game->get_keys_effect()->set_item_keys_enabled(true);

  // free the memory
  delete cursor_sprite;

  for (int k = 0; k < 28; k++) {
    delete sprites[k];
    delete counters[k];
  }
}

/**
 * @brief Sets the position of the cursor.
 *
 * In other words, selects an item in the inventory.
 *
 * @param row row of the cursor, between 0 and 3
 * @param column columng of the cursor, between 0 and 6
 */
void PauseSubmenuInventory::set_cursor_position(int row, int column) {

  // update the cursor position
  cursor_row = row;
  cursor_column = column;

  int index = row * 7 + column;
  savegame->set_integer(Savegame::INVENTORY_LAST_ITEM_INDEX, index);

  // update the caption text, show or hide the action icon
  KeysEffect *keys_effect = game->get_keys_effect();
  const std::string item_name = item_names[index];
  int variant = equipment->get_item_variant(item_name);

  set_caption_text(caption_strings[index]);
  if (variant != 0) {
    keys_effect->set_action_key_effect(KeysEffect::ACTION_KEY_INFO);
    keys_effect->set_item_keys_enabled(equipment->get_item_properties(item_name)->can_be_assigned());
  }
  else {
    keys_effect->set_action_key_effect(KeysEffect::ACTION_KEY_NONE);
    keys_effect->set_item_keys_enabled(false);
  }
}

/**
 * @brief Returns the index of the cell currently selected in the inventory.
 *
 * The value returned identifies an item.
 *
 * @return the index of the selected cell, between 0 and 27
 */
int PauseSubmenuInventory::get_selected_index(void) {
  return cursor_row * 7 + cursor_column;
}

/**
 * @brief Returns whether an item is selected.
 *
 * This function returns whether whether the cursor is on an
 * item that the player has.
 *
 * @return true if an item is currently selected
 */
bool PauseSubmenuInventory::is_item_selected(void) {
  
  return equipment->has_item(item_names[get_selected_index()]);
}

/**
 * @brief This function is called when a key is pressed on this submenu.
 * @param key the key pressed
 */
void PauseSubmenuInventory::key_pressed(GameControls::GameKey key) {

  KeysEffect *keys_effect = game->get_keys_effect();

  switch (key) {

  case GameControls::ACTION:
    if (keys_effect->get_action_key_effect() == KeysEffect::ACTION_KEY_INFO) {
      show_info_message();
    }
    break;

  case GameControls::ITEM_1:
    if (is_item_selected()) {
      assign_item(0);
    }
    break;

  case GameControls::ITEM_2:
    if (is_item_selected()) {
      assign_item(1);
    }
    break;

  case GameControls::LEFT:
    if (cursor_column == 0) {
      pause_menu->show_left_submenu();
    }
    else {
      play_cursor_sound();
      set_cursor_position(cursor_row, cursor_column - 1);
    }
    break;

  case GameControls::RIGHT:
    if (cursor_column == 6) {
      pause_menu->show_right_submenu();
    }
    else {
      play_cursor_sound();
      set_cursor_position(cursor_row, cursor_column + 1);
    }
    break;

  case GameControls::UP:
    play_cursor_sound();
    set_cursor_position((cursor_row + 3) % 4, cursor_column);
    break;

  case GameControls::DOWN:
    play_cursor_sound();
    set_cursor_position((cursor_row + 1) % 4, cursor_column);
    break;

  default:
    break;
  }
}

/**
 * @brief Updates this submenu.
 *
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
 * @brief Displays this submenu.
 * @param destination the destination surface
 */
void PauseSubmenuInventory::display(Surface *destination) {

  // display the common part
  PauseSubmenu::display(destination);

  // display each inventory item
  Rectangle dst_xy(0, 69);

  int k = 0;
  for (int i = 0; i < 4; i++) {

    dst_xy.set_x(56);

    for (int j = 0; j < 7; j++, k++) {

      // get the possession state of this item
      const std::string item_name = item_names[k];

      if (equipment->has_item(item_name)) {

	// the player has this item, display the variant he has
	sprites[k]->display(destination, dst_xy.get_x(), dst_xy.get_y());

	// display the counter (if any)
	if (counters[k] != NULL) {
	  counters[k]->display(destination);
	}
      }

      dst_xy.add_x(32);
    }
    dst_xy.add_y(32);
  }

  // display the cursor
  int x = 64 + 32 * cursor_column;
  int y = 77 + 32 * cursor_row;

  cursor_sprite->display(destination, x, y);

  // display the item being assigned
  if (item_assigned_movement != NULL) {
    
    item_assigned_sprite->display(destination, item_assigned_movement->get_x(), item_assigned_movement->get_y());
  }
}

/**
 * @brief Shows a message describing the item currently selected.
 *
 * The player must have this item.
 */
void PauseSubmenuInventory::show_info_message(void) {

  const std::string &item_name = item_names[get_selected_index()];
  int variant = equipment->get_item_variant(item_name);

  std::ostringstream oss;
  oss << "_item_description." << item_name << '.' << variant;

  DialogBox::VerticalPosition vertical_position = (cursor_row >= 2) ? DialogBox::POSITION_TOP : DialogBox::POSITION_BOTTOM;

  game->get_dialog_box()->start_dialog(oss.str(), vertical_position);
}

/**
 * @brief Assigns the selected item to a slot.
 *
 * The operation does not take effect immediately: the item picture is thrown to
 * its destination icon, then the assignment is done.
 * Nothing is done if the item is not assignable.
 *
 * @param slot slot to set (0 for X or 1 for V)
 */
void PauseSubmenuInventory::assign_item(int slot) {

  int index = get_selected_index();
  const std::string &item_name = item_names[index];

  // if this item is not assignable, do nothing
  if (!equipment->get_item_properties(item_name)->can_be_assigned()) {
    return;
  }

  // if another item is being assigned, finish it immediately
  if (is_assigning_item()) {
    finish_assigning_item();
  }

  // memorize this item
  this->item_assigned_name = item_name;
  this->item_assigned_sprite = sprites[index];
  this->item_assigned_variant = equipment->has_item(item_assigned_name);
  this->item_assigned_destination = slot;

  // play the sound
  game->play_sound("throw");

  // compute the movement
  int x1 = 60 + 32 * cursor_column;
  int y1 = 75 + 32 * cursor_row;
  int x2 = (slot == 0) ? 12 : 64;
  int y2 = 33;

  item_assigned_movement = new TargetMovement(x2, y2, 50);
  item_assigned_movement->set_position(x1, y1);
}

/**
 * @brief Returns whether an item is currently being thrown to an icon.
 * @return true if an item is being assigned
 */
bool PauseSubmenuInventory::is_assigning_item(void) {
  return item_assigned_movement != NULL;
}

/**
 * @brief Stops assigning the item right now.
 *
 * This function is called when we want to assign the item without
 * waiting for its throwing movement to end, for example when the inventory submenu
 * is being closed.
 */
void PauseSubmenuInventory::finish_assigning_item(void) {

  // if the item to assign is already assigned to the other icon, switch the two items
  int slot = item_assigned_destination;
  const std::string &current_item_name = equipment->get_item_assigned(slot);
  const std::string &other_item_name = equipment->get_item_assigned(1 - slot);

  if (other_item_name == item_assigned_name) {
    equipment->set_item_assigned(1 - slot, current_item_name);
  }
  equipment->set_item_assigned(slot, item_assigned_name);

  delete item_assigned_movement;
  item_assigned_movement = NULL;
}

