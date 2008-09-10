#ifndef PAUSE_SUBMENU_INVENTORY_H
#define PAUSE_SUBMENU_INVENTORY_H

#include "Common.h"
#include "menus/PauseSubmenu.h"

/**
 * The Inventory submenu of the pause menu.
 */
class PauseSubmenuInventory: public PauseSubmenu {

 private:

  int cursor_position;   /**< position of the inventory cursor, between 0 and 27 */
  Sprite *cursor_sprite; /**< sprite of the cursor to select an item */

 public:

  PauseSubmenuInventory(PauseMenu *pause_menu, Game *game);
  ~PauseSubmenuInventory(void);

  void key_pressed(const SDL_keysym &keysym);
  void update(void);
  void display(SDL_Surface *destination);
};

#endif
