#ifndef ZSDX_PAUSE_SUBMENU_H
#define ZSDX_PAUSE_SUBMENU_H

#include "Common.h"

/**
 * Abstract class for a submenu of the pause menu.
 */
class PauseSubmenu {

 protected:

  Game *game;

  PauseSubmenu(Game *game);

 public:

  virtual ~PauseSubmenu(void);

  virtual void key_pressed(const SDL_keysym &keysym) = 0;
  virtual void update(void) = 0;
  virtual void display(SDL_Surface *destination) = 0;
};

#endif
