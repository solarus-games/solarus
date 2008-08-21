#ifndef ZSDX_SELECTION_MENU_CHOOSE_MODE_H
#define ZSDX_SELECTION_MENU_CHOOSE_MODE_H

#include "Common.h"
#include "menus/SelectionMenuChooseMode.h"
#include "menus/SelectionMenuPhase.h"

/**
 * Phase of the selection menu where the user
 * chooses the game mode.
 */
class SelectionMenuChooseMode: public SelectionMenuPhase {

 private:

  /**
   * True if the Adventure mode if selected,
   * false if the Solarus Dreams mode is selected.
   */
  bool adventure_mode;

  SDL_Surface *img_mode;
  SDL_Surface *savegame_surface;
  
 public:

  // creation and destruction
  SelectionMenuChooseMode(SelectionMenuPhase *previous);
  ~SelectionMenuChooseMode(void);

  // update and display
  void handle_event(const SDL_Event &event);
  void update(void);
  void display(SDL_Surface *screen_surface);
};

#endif
