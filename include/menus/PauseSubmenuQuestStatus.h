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

  Sprite *cursor_sprite;
  int cursor_position; /**< 0 to 7 */
  SDL_Rect cursor_sprite_position;

  string caption_texts[8];

  void set_cursor_position(int position);

 public:

  PauseSubmenuQuestStatus(PauseMenu *pause_menu, Game *game);
  ~PauseSubmenuQuestStatus(void);

  void key_pressed(Controls::GameKey key);
  void update(void);
  void display(SDL_Surface *destination);
};

#endif
