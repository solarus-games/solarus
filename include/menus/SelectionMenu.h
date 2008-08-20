#ifndef ZSDX_SELECTION_MENU_H
#define ZSDX_SELECTION_MENU_H

#include "Common.h"

/**
 * This class shows the menu to select the save file and the game mode.
 */
class SelectionMenu: public Screen {

 private:

  /**
   * The differents phases of the selection menu.
   */
  enum SelectionMenuPhase {
    PHASE_SELECT_FILE,
    PHASE_ERASE_FILE,
    PHASE_CONFIRM_ERASE,
    PHASE_CHOOSE_NAME,
    PHASE_CHOOSE_MODE
  };

  /**
   * The savegames shown in the menu.
   */
  Savegame *savegames[3];

  /**
   * True if the user selected the Adventure mode,
   * false if he selected the Solarus Dreams mode.
   */
  bool adventure_mode;

  /**
   * Phase currently displayed.
   */
  SelectionMenuPhase current_phase;

  // images
  SDL_Surface *img_cloud, *img_background;
  SDL_Surface *img_save_container, *img_option_container;
  SDL_Surface *img_arrow, *img_letters;
  SDL_Surface *img_numbers[3];
  SDL_Surface *img_mode;
  SDL_Surface *destination_surface;

  Sprite *cursor;
  int cursor_position; // 1 to 5
  int save_number_to_erase; // 0 to 2
  int x_letter_cursor, y_letter_cursor;
  TransitionEffect *transition;

  // sounds
  Sound *cursor_sound;
  Sound *ok_sound;
  Sound *letter_sound;
  Sound *erase_sound;
  Sound *error_sound;

  // music
  Music *music;

  // icons
  KeysEffect *keys_effect;
  ActionIcon *action_icon;
  SwordIcon *sword_icon;

  // text
  TextDisplayed *text_option1;
  TextDisplayed *text_option2;
  TextDisplayed *text_new_player_name;
  TextDisplayed *text_title;

  static string title_strings[5];

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

  // common functions for all phases
  void redraw_common(void);
  void update_common(void);

  // management of each phase
  void start_select_file_phase(void);
  void update_select_file_phase(void);
  void display_select_file_phase(void);
  void event_select_file_phase(const SDL_Event &event);

  void start_erase_file_phase(void);
  void update_erase_file_phase(void);
  void display_erase_file_phase(void);
  void event_erase_file_phase(const SDL_Event &event);

  void start_confirm_erase_phase(void);
  void update_confirm_erase_phase(void);
  void display_confirm_erase_phase(void);
  void event_confirm_erase_phase(const SDL_Event &event);

  void start_choose_name_phase(void);
  void update_choose_name_phase(void);
  void display_choose_name_phase(void);
  void event_choose_name_phase(const SDL_Event &event);

  void start_choose_mode_phase(void);
  void update_choose_mode_phase(void);
  void display_choose_mode_phase(void);
  void event_choose_mode_phase(const SDL_Event &event);

  // cursor handling
  void move_cursor_up(void);
  void move_cursor_down(void);
  void move_cursor_left_or_right(void);

  // displaying elements
  void display_title_text(void);
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

  // creation and destruction
  SelectionMenu(void);
  ~SelectionMenu(void);

  // information about the user choices
  Savegame *get_selected_save(void);
  bool is_adventure_mode(void);

  // update and display
  void handle_event(const SDL_Event &event);
  void update(void);
  void display(SDL_Surface *screen_surface);
};

#endif
