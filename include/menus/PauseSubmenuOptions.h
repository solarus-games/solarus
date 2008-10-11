#ifndef PAUSE_SUBMENU_OPTIONS_H
#define PAUSE_SUBMENU_OPTIONS_H

#include "Common.h"
#include "menus/PauseSubmenu.h"
#include "Controls.h"

/**
 * The Options submenu of the pause menu.
 */
class PauseSubmenuOptions: public PauseSubmenu {

 private:

  Controls *controls;

  TextSurface *fullscreen_label_text;
  TextSurface *fullscreen_answer_text;

  TextSurface *controls_text;
  TextSurface *keyboard_text;
  TextSurface *joypad_text;

  TextSurface *game_key_texts[9];
  TextSurface *keyboard_control_texts[9];
  TextSurface *joypad_control_texts[9];

  SDL_Surface *controls_surface;
  int controls_visible_y;
  int moving_visible_y;        /**< 0, -1 or 1 depending on the arrow pressed */
  Uint32 next_moving_visible_y_date;

  Sprite *up_arrow_sprite;
  Sprite *down_arrow_sprite;

  void load_control_texts(void);

 public:

  PauseSubmenuOptions(PauseMenu *pause_menu, Game *game);
  ~PauseSubmenuOptions(void);

  void key_pressed(Controls::GameKey key);
  void update(void);
  void display(SDL_Surface *destination);
};

#endif
