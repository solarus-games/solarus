#ifndef ZSDX_PAUSE_MENU_H
#define ZSDX_PAUSE_MENU_H

#include "Common.h"

/**
 * The menus displayed when the game is paused.
 */
class PauseMenu {

 private:

  Game *game;
  Savegame *savegame;
  Equipment *equipment;
  DungeonEquipment *dungeon_equipment;
  KeysEffect *keys_effect;

  int current_submenu_index;
  PauseSubmenu *submenus[4];
  SDL_Surface *backgrounds_surface;

  PauseSubmenu *get_current_submenu(void);

 public:

  PauseMenu(Game *game);
  ~PauseMenu(void);
  void quit(void);

  void key_pressed(const SDL_keysym &keysym);
  void update(void);
  void display(SDL_Surface *destination);
};

#endif
