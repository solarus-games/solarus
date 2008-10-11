#ifndef PAUSE_SUBMENU_QUEST_STATUS_H
#define PAUSE_SUBMENU_QUEST_STATUS_H

#include "Common.h"
#include "menus/PauseSubmenu.h"
#include "Controls.h"

/**
 * The Quest Status submenu of the pause menu.
 */
class PauseSubmenuQuestStatus: public PauseSubmenu {

 private:

  SDL_Surface *quest_items_surface;

 public:

  PauseSubmenuQuestStatus(PauseMenu *pause_menu, Game *game);
  ~PauseSubmenuQuestStatus(void);

  void key_pressed(Controls::GameKey key);
  void update(void);
  void display(SDL_Surface *destination);
};

#endif
