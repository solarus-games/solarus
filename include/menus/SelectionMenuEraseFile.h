#ifndef ZSDX_SELECTION_MENU_ERASE_FILE_H
#define ZSDX_SELECTION_MENU_ERASE_FILE_H

#include "Common.h"
#include "menus/SelectionMenuPhase.h"

/**
 * Phase of the selection menu, where the user
 * chooses a file to erase.
 */
class SelectionMenuEraseFile: public SelectionMenuPhase {

 public:

  // creation and destruction
  SelectionMenuEraseFile(SelectionMenuPhase *previous);
  ~SelectionMenuEraseFile(void);

  // update and display
  void handle_event(const SDL_Event &event);
  void display(SDL_Surface *screen_surface);
};

#endif
