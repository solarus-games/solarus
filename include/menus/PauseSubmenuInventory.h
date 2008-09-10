#ifndef PAUSE_SUBMENU_INVENTORY_H
#define PAUSE_SUBMENU_INVENTORY_H

#include "Common.h"
#include "menus/PauseSubmenu.h"

/**
 * The Inventory submenu of the pause menu.
 */
class PauseSubmenuInventory: public PauseSubmenu {

 private:

  Equipment *equipment;   /**< the player's equipment */

  int cursor_row;         /**< row of the inventory cursor, between 0 and 3 */
  int cursor_column;      /**< column of the inventory cursor, between 0 and 6 */
  Sprite *cursor_sprite;  /**< sprite of the cursor to select an item */
  Sound *cursor_sound;    /**< sound played when moving the cursor */

  SDL_Surface *items_img; /**< images of the existing items */
  Counter *counters[28];  /**< counters (for some items only) */

 public:

  PauseSubmenuInventory(PauseMenu *pause_menu, Game *game);
  ~PauseSubmenuInventory(void);

  void key_pressed(const SDL_keysym &keysym);
  void update(void);
  void display(SDL_Surface *destination);
};

#endif
