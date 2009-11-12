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
#ifndef PAUSE_SUBMENU_INVENTORY_H
#define PAUSE_SUBMENU_INVENTORY_H

#include "Common.h"
#include "menus/PauseSubmenu.h"
#include "InventoryItemId.h"
#include "Controls.h"

/**
 * The Inventory submenu of the pause menu.
 */
class PauseSubmenuInventory: public PauseSubmenu {

 private:

  // cursor
  int cursor_row;                 /**< row of the inventory cursor, between 0 and 3 */
  int cursor_column;              /**< column of the inventory cursor, between 0 and 6 */
  Sprite *cursor_sprite;          /**< sprite of the cursor to select an item */
  Sound *cursor_sound;            /**< sound played when moving the cursor */

  // graphics
  SDL_Surface *items_img;         /**< images of the existing items */
  Counter *counters[28];          /**< counters (for some items only) */
  std::string caption_strings[28];

  // assigning an item to an icon
  InventoryItemId item_assigned_id;       /**< index of the item currently being assigned */
  int item_assigned_variant;              /**< possession state of the item being assigned */
  int item_assigned_destination;          /**< destination slot of the item being assigned: 0 for X, 1 for V */
  TargetMovement *item_assigned_movement; /**< movement of the item currently being assigned (NULL if none) */

  // cursor position
  void set_cursor_position(int row, int column);
  int get_selected_index(void);
  bool is_item_selected(void);

  // actions
  void show_info_message(void);
  void assign_item(int slot);
  bool is_assigning_item(void);
  void finish_assigning_item(void);

 public:

  // construction and destruction
  PauseSubmenuInventory(PauseMenu *pause_menu, Game *game);
  ~PauseSubmenuInventory(void);

  // main loop
  void key_pressed(Controls::GameKey key);
  void update(void);
  void display(SDL_Surface *destination);
};

#endif
