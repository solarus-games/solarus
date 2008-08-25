#ifndef ZSDX_SELECTION_MENU_COMMON_H
#define ZSDX_SELECTION_MENU_COMMON_H

#include "Common.h"

/**
 * This class handles the elements common to
 * all phases of the selection menu.
 * There elements are not changed between two
 * phases of the selection menu.
 */
class SelectionMenuCommon {

  friend class SelectionMenuPhase;

 private:

  /**
   * The savegames shown in the menu.
   */
  Savegame *savegames[3];

  // images
  SDL_Surface *img_cloud, *img_background;
  SDL_Surface *img_save_container, *img_option_container;
  SDL_Surface *img_numbers[3];

  Sprite *cursor;
  int cursor_position; // 1 to 5

  Uint16 background_color;

  // sounds
  Sound *cursor_sound;
  Sound *ok_sound;
  Sound *error_sound;

  // music
  Music *music;

  // icons
  KeysEffect *keys_effect;
  ActionIcon *action_icon;
  SwordIcon *sword_icon;

  // text
  TextSurface *text_option1;
  TextSurface *text_option2;
  TextSurface *text_title;

  // savegames data
  TextSurface *text_player_names[3];
  HeartsView *hearts_views[3];

  // clouds data
  SDL_Rect cloud_positions[16];
  Uint32 next_cloud_move;
  
  // initialization functions
  void initialize_clouds(void);
  void read_savegames(void);

 public:

  // creation and destruction
  SelectionMenuCommon(void);
  ~SelectionMenuCommon(void);

  // update and display
  void update(void);
  void display(SDL_Surface *destination_surface);
};

#endif
