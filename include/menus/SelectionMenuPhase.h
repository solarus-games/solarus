#ifndef ZSDX_SELECTION_PHASE_H
#define ZSDX_SELECTION_PHASE_H

#include "Common.h"
#include "Screen.h"

/**
 * Abstract class for a phase of the selection menu,
 * i.e. the menu to select the save file and the
 * game mode.
 */
class SelectionMenuPhase: public Screen {

 private:

  /**
   * Title text of this phase.
   */
  string title_string;

 protected:

  /**
   * The surface where the menu is drawn.
   */
  SDL_Surface *destination_surface;

  /**
   * The transition currently displayed (may be NULL).
   */
  TransitionEffect *transition;

  /**
   * The elements common to all phases.
   */
  SelectionMenuCommon *common_part;

  // creation and destruction
  SelectionMenuPhase(SelectionMenuPhase *previous,
		     string title_string);
  virtual ~SelectionMenuPhase(void);
  
  void selection_menu_finished(void);

  // savegames
  Savegame **get_savegames(void);
  Savegame *get_savegame(int save_number);
  void reload_savegames(void);

  // sounds
  void play_cursor_sound(void);
  void play_ok_sound(void);
  void play_error_sound(void);

  // keys
  KeysEffect * get_keys_effect(void);

  // cursor handling
  Sprite *get_cursor(void);
  int get_cursor_position(void);
  void set_cursor_position(int position);
  void move_cursor_up(void);
  void move_cursor_down(void);
  void move_cursor_left_or_right(void);

  // displaying elements
  void display_title_text(void);
  void display_savegame(int save_number);
  void display_savegame(int save_number, SDL_Surface *destination_surface);
  void display_savegame_number(int save_number);
  void display_savegame_number(int save_number, SDL_Surface *destination_surface);
  void display_options(const char *left_string, const char *right_string);
  void display_normal_cursor(void);

  // update and display for the common part
  virtual void update(void);
  void start_display(SDL_Surface *screen_surface);
  void finish_display(SDL_Surface *screen_surface);
};

#endif
