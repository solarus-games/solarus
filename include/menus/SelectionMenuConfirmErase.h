#ifndef ZSDX_SELECTION_MENU_CONFIRM_ERASE_H
#define ZSDX_SELECTION_MENU_CONFIRM_ERASE_H

#include "Common.h"
#include "menus/SelectionMenuPhase.h"

/**
 * First phase of the selection menu,
 * where the user can select a file.
 */
class SelectionMenuConfirmErase: public SelectionMenuPhase {

 private:
  
  int save_number_to_erase;
  void delete_save_file(int save_number);

 public:

  // creation and destruction
  SelectionMenuConfirmErase(SelectionMenuPhase *previous,
			    int save_number_to_erase);
  ~SelectionMenuConfirmErase(void);

  // update and display
  void handle_event(const SDL_Event &event);
  void display(SDL_Surface *screen_surface);
};

#endif
