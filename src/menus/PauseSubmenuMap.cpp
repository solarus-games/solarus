#include "menus/PauseSubmenuMap.h"
#include "menus/PauseMenu.h"

/**
 * Constructor.
 * @param pause_menu the pause menu object
 * @param game the game
 */
PauseSubmenuMap::PauseSubmenuMap(PauseMenu *pause_menu, Game *game):
  PauseSubmenu(pause_menu, game) {


}

/**
 * Destructor.
 */
PauseSubmenuMap::~PauseSubmenuMap(void) {

}

/**
 * This function is called when a key is pressed on this submenu.
 * @param keysym the key pressed
 */
void PauseSubmenuMap::key_pressed(const SDL_keysym &keysym) {

  switch (keysym.sym) {

  case SDLK_LEFT:
    pause_menu->show_left_submenu();
    break;

  case SDLK_RIGHT:
    pause_menu->show_right_submenu();
    break;

  default:
    break;
  }
}

/**
 * Updates this submenu.
 */
void PauseSubmenuMap::update(void) {
  
}

/**
 * Displays this submenu.
 */
void PauseSubmenuMap::display(SDL_Surface *destination) {
  PauseSubmenu::display(destination);
}
