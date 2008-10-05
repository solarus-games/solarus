#include "menus/PauseSubmenuQuestStatus.h"
#include "menus/PauseMenu.h"

/**
 * Constructor.
 * @param pause_menu the pause menu object
 * @param game the game
 */
PauseSubmenuQuestStatus::PauseSubmenuQuestStatus(PauseMenu *pause_menu, Game *game):
  PauseSubmenu(pause_menu, game) {

}

/**
 * Destructor.
 */
PauseSubmenuQuestStatus::~PauseSubmenuQuestStatus(void) {

}

/**
 * This function is called when a key is pressed on this submenu.
 * @param keysym the key pressed
 */
void PauseSubmenuQuestStatus::key_pressed(const SDL_keysym &keysym) {

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
void PauseSubmenuQuestStatus::update(void) {
  
}

/**
 * Displays this submenu.
 * @param destination the destination surface
 */
void PauseSubmenuQuestStatus::display(SDL_Surface *destination) {
  PauseSubmenu::display(destination);
}
