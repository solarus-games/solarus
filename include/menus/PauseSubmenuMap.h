#ifndef PAUSE_SUBMENU_MAP_H
#define PAUSE_SUBMENU_MAP_H

#include "Common.h"
#include "menus/PauseSubmenu.h"

/**
 * The Map submenu of the pause menu.
 */
class PauseSubmenuMap: public PauseSubmenu {

 public:

  PauseSubmenuMap(Game *game);
  ~PauseSubmenuMap(void);

  void key_pressed(const SDL_keysym &keysym);
  void update(void);
  void display(SDL_Surface *destination);
};

#endif
