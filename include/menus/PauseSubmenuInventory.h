#ifndef PAUSE_SUBMENU_INVENTORY_H
#define PAUSE_SUBMENU_INVENTORY_H

#include "Common.h"
#include "menus/PauseSubmenu.h"
#include "InventoryItem.h"

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

  // assigning an item to an icon
  InventoryItem::ItemId item_assigned_id;     /**< index of the item currently being assigned */
  int item_assigned_variant;                  /**< possession state of the item being assigned */
  int item_assigned_destination;              /**< destination slot of the item being assigned: 0 for X, 1 for V */
  TargetMovement *item_assigned_movement;     /**< movement of the item currently being assigned (NULL if none) */

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
  void key_pressed(const SDL_keysym &keysym);
  void update(void);
  void display(SDL_Surface *destination);
};

#endif
