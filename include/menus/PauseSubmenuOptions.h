#ifndef PAUSE_SUBMENU_OPTIONS_H
#define PAUSE_SUBMENU_OPTIONS_H

#include "Common.h"
#include "menus/PauseSubmenu.h"

/**
 * The Options submenu of the pause menu.
 */
class PauseSubmenuOptions: public PauseSubmenu {

 public:

  PauseSubmenuOptions(PauseMenu *pause_menu, Game *game);
  ~PauseSubmenuOptions(void);

  void key_pressed(const SDL_keysym &keysym);
  void update(void);
  void display(SDL_Surface *destination);
};

#endif
