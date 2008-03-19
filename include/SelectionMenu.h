#ifndef ZSDX_SELECTION_MENU_H
#define ZSDX_SELECTION_MENU_H

#include "Common.h"

/**
 * This class shows the menu to select the save file and the game mode.
 */
class SelectionMenu {

 private:

  /**
   * The savegame selected by the user, or NULL
   * if he cancelled.
   */
  Savegame *savegame;

  /**
   * True if the user selected the Adventure mode,
   * false if he selected the Solarus Dreams mode.
   */
  bool adventure_mode;

  // images
  SDL_Surface *img_cloud, *img_menu, *img_1, *img_2, *img_3;

  SDL_Rect cloud_positions[16];
  Uint32 next_cloud_move;

  void update(void);
  void redraw(void);

 public:

  SelectionMenu(void);
  ~SelectionMenu(void);

  void show(void);

  Savegame *get_selected_save(void);
  bool is_adventure_mode(void);
};

#endif
