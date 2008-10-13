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

  TextSurface *video_mode_text;

  TextSurface *controls_text;
  TextSurface *keyboard_text;
  TextSurface *joypad_text;

  TextSurface *game_key_texts[9];
  TextSurface *keyboard_control_texts[9];
  TextSurface *joypad_control_texts[9];

  SDL_Surface *controls_surface;
  int highest_visible_key; // 1 to 9
  int controls_visible_y;

  Sprite *up_arrow_sprite;
  Sprite *down_arrow_sprite;

  Sprite *cursor_sprite;
  SDL_Rect cursor_sprite_position;

  Sound *cursor_sound;
  Sound *ok_sound;
  int cursor_position; /**< 0: fullscreen, 1 to 9: key 1 to 9 */
  bool customizing;

  void load_control_texts(void);
  void set_cursor_position(int position);
  void action_key_pressed(void);

  void display_cursor(SDL_Surface *destination);

 public:

  PauseSubmenuOptions(PauseMenu *pause_menu, Game *game);
  ~PauseSubmenuOptions(void);

  void key_pressed(Controls::GameKey key);
  void update(void);
  void display(SDL_Surface *destination);
};

#endif
