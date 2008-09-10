#include "menus/PauseMenu.h"
#include "menus/PauseSubmenuInventory.h"
#include "menus/PauseSubmenuMap.h"
#include "menus/PauseSubmenuQuestStatus.h"
#include "menus/PauseSubmenuOptions.h"
#include "Game.h"
#include "Savegame.h"
#include "KeysEffect.h"
#include "ResourceManager.h"
#include "Sound.h"

/**
 * Opens a pause menu.
 * @param game the game
 */
PauseMenu::PauseMenu(Game *game):
  game(game), savegame(game->get_savegame()), keys_effect(game->get_keys_effect()) {

  this->current_submenu = NULL;
  this->backgrounds_surface = ResourceManager::load_image("menus/pause_submenus.png");

  ResourceManager::get_sound("pause_open")->play();
  keys_effect->set_pause_key_effect(KeysEffect::PAUSE_KEY_RETURN);
  keys_effect->save_action_key_effect();
  keys_effect->save_sword_key_effect();

  set_current_submenu(savegame->get_integer(Savegame::PAUSE_LAST_SUBMENU));
}

/**
 * Destructor.
 */
PauseMenu::~PauseMenu(void) {
  delete current_submenu;
}

/**
 * Exits the pause menu.
 */
void PauseMenu::quit(void) {

  ResourceManager::get_sound("pause_closed")->play();
  keys_effect->set_pause_key_effect(KeysEffect::PAUSE_KEY_PAUSE);
  keys_effect->restore_action_key_effect();
  keys_effect->restore_sword_key_effect();
}

/**
 * This function is called when a key is pressed.
 * @param keysym the key pressed
 */
void PauseMenu::key_pressed(const SDL_keysym &keysym) {

  if (keysym.sym == SDLK_d) {
    quit();
    game->set_paused(false);
  }
  else {
    current_submenu->key_pressed(keysym);
  }
}

/**
 * Updates the pause menu.
 */
void PauseMenu::update(void) {
  current_submenu->update();
}

/**
 * Displays the pause menu.
 */
void PauseMenu::display(SDL_Surface *destination) {

  // display the background for the current submenu
  int submenu_index = savegame->get_integer(Savegame::PAUSE_LAST_SUBMENU);
  SDL_Rect src_position = {320 * submenu_index, 0, 320, 240};
  SDL_BlitSurface(backgrounds_surface, &src_position, destination, NULL);

  // display the current submenu content
  current_submenu->display(destination);
}

/**
 * Moves to the specified submenu.
 * @param submenu_index the submenu to show
 */
void PauseMenu::set_current_submenu(int submenu_index) {

  // show the default action and sword keys
  keys_effect->set_action_key_effect(KeysEffect::ACTION_KEY_NONE);
  keys_effect->set_sword_key_effect(KeysEffect::SWORD_KEY_SAVE);

  // go to the specified submenu
  savegame->set_integer(Savegame::PAUSE_LAST_SUBMENU, submenu_index);

  if (current_submenu != NULL) {
    delete current_submenu;
  }

  switch (submenu_index) {

  case 0:
    current_submenu = new PauseSubmenuInventory(this, game);
    break;

  case 1:
    current_submenu = new PauseSubmenuMap(this, game);
    break;

  case 2:
    current_submenu = new PauseSubmenuQuestStatus(this, game);
    break;

  case 3:
    current_submenu = new PauseSubmenuOptions(this, game);
    break;
  }
}

/**
 * Shows the submenu located at the left side from the current one.
 */
void PauseMenu::show_left_submenu(void) {

  ResourceManager::get_sound("pause_closed")->play();
  int submenu_index = savegame->get_integer(Savegame::PAUSE_LAST_SUBMENU);
  set_current_submenu((submenu_index + 3) % 4);
}

/**
 * Shows the submenu located at the right side from the current one.
 */
void PauseMenu::show_right_submenu(void) {

  ResourceManager::get_sound("pause_closed")->play();
  int submenu_index = savegame->get_integer(Savegame::PAUSE_LAST_SUBMENU);
  set_current_submenu((submenu_index + 1) % 4);
}
