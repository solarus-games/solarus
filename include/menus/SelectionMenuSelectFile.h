#ifndef ZSDX_SELECTION_MENU_SELECT_FILE_H
#define ZSDX_SELECTION_MENU_SELECT_FILE_H

#include "Common.h"
#include "menus/SelectionMenuPhase.h"

/**
 * First phase of the selection menun
 * where the user can select a file.
 */
class SelectionMenuSelectFile: public SelectionMenuPhase {

 public:

  // creation and destruction
  SelectionMenuSelectFile(SelectionMenuPhase *previous);
  ~SelectionMenuSelectFile(void);

  // update and display
  void handle_event(const SDL_Event &event);
  void display(SDL_Surface *screen_surface);
};

#endif
