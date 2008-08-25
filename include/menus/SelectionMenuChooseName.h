#ifndef ZSDX_SELECTION_MENU_CHOOSE_NAME_H
#define ZSDX_SELECTION_MENU_CHOOSE_NAME_H

#include "Common.h"
#include "menus/SelectionMenuChooseName.h"
#include "menus/SelectionMenuPhase.h"

/**
 * Phase of the selection menu where the user
 * chooses the name to create its savegame.
 */
class SelectionMenuChooseName: public SelectionMenuPhase {

 private:
  
  char player_name[11];
  int x_letter_cursor;
  int y_letter_cursor;

  SDL_Surface *img_arrow;
  SDL_Surface *img_letters;

  TextDisplayed *text_new_player_name;

  // player name
  bool select_letter(void);
  bool validate_player_name(void);

 public:

  // creation and destruction
  SelectionMenuChooseName(SelectionMenuPhase *previous);
  ~SelectionMenuChooseName(void);

  // update and display
  void handle_event(const SDL_Event &event);
  void display(SDL_Surface *screen_surface);
};

#endif
