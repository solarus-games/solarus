#ifndef ZSDX_SELECTION_MENU_H
#define ZSDX_SELECTION_MENU_H

#include "Common.h"

/**
 * This class shows the menu to select the save file and the game mode.
 */
class SelectionMenu {

 private:

  /**
   * The savegames shown in the menu.
   */
  Savegame *savegames[3];

  /**
   * True if the user selected the Adventure mode,
   * false if he selected the Solarus Dreams mode.
   */
  bool adventure_mode;

  // images
  SDL_Surface *img_cloud, *img_background;
  SDL_Surface *img_save_container, *img_option_container;
  SDL_Surface *img_arrow, *img_letters;
  SDL_Surface *img_text_select, *img_text_erase, *img_text_confirm, *img_text_name;
  SDL_Surface *img_numbers[3];
  SDL_Surface *destination_surface;

  AnimatedSprite *cursor;
  int cursor_position; // 1 to 5
  int save_number_to_erase; // 0 to 2
  int x_letter_cursor, y_letter_cursor;
  TransitionEffect *transition;

  // sounds
  Sound *cursor_sound, *ok_sound, *letter_sound, *erase_sound, *error_sound;

  // icons
  KeysEffect *keys_effect;
  ActionIcon *action_icon;
  SwordIcon *sword_icon;

  // text
  TextDisplayed *text;

  // new player name
  char player_name[11]; // 10 letters max

  // savegames data
  TextDisplayed *text_player_names[3];
  HeartsView *hearts_views[3];

  // clouds data
  SDL_Rect cloud_positions[16];
  Uint32 next_cloud_move;
  
  // initialization functions
  void initialize_clouds(void);
  void read_saves(void);

  // common functions for all screens
  void redraw_common(void);
  void update(void);

  // management of each screen
  void show_main_screen(void);
  void redraw_main_screen(void);

  void show_erase_choice_screen(void);
  void redraw_erase_choice_screen(void);

  void show_erase_confirm_screen(void);
  void redraw_erase_confirm_screen(void);

  void show_choose_name_screen(void);
  void redraw_choose_name_screen(void);

  // cursor handling
  void move_cursor_up(void);
  void move_cursor_down(void);
  void move_cursor_left_or_right(void);

  // displaying elements
  void display_title_text(SDL_Surface *img_text);
  void display_savegame(int save_number);
  void display_options(const char *left, const char *right);
  void display_normal_cursor(void);
  void display_savegame_number(int save_number);

  // delete a file
  void delete_save_file(int save_number);

  // create a file
  bool select_letter(void);
  bool validate_player_name(void);

 public:

  SelectionMenu(void);
  ~SelectionMenu(void);

  void show(void);

  Savegame *get_selected_save(void);
  bool is_adventure_mode(void);
};

#endif
