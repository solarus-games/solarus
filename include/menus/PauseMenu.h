#ifndef ZSDX_PAUSE_MENU_H
#define ZSDX_PAUSE_MENU_H

#include "Common.h"
#include "Controls.h"

/**
 * The menus displayed when the game is paused.
 */
class PauseMenu {

 private:

  Game *game;
  Savegame *savegame;
  KeysEffect *keys_effect;

  SDL_Surface *backgrounds_surface;
  PauseSubmenu *current_submenu;

  void set_current_submenu(int submenu_index);

 public:

  PauseMenu(Game *game);
  ~PauseMenu(void);
  void quit(void);

  void key_pressed(Controls::GameKey key);
  void update(void);
  void display(SDL_Surface *destination);

  void show_left_submenu(void);
  void show_right_submenu(void);
};

#endif
