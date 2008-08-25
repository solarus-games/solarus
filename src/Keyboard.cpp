#include "Keyboard.h"
#include "Game.h"
#include "DialogBox.h"
#include "entities/Link.h"
#include "movements/Movement8ByPlayer.h"

/**
 * Constructor.
 * @param game the game
 */
Keyboard::Keyboard(Game *game):
  game(game), link(game->get_link()) {

}

/**
 * Destructor.
 */
Keyboard::~Keyboard(void) {

}

/**
 * This function is called by the game when a key is pressed.
 * Depending on the game state, the appropriate object is notified.
 * @param keysym the key pressed
 */
void Keyboard::key_pressed(const SDL_keysym &keysym) {

  if (!game->is_suspended()) {    
    // if the game is not suspended, the keys apply to Link

    switch (keysym.sym) {

      // action key
    case SDLK_SPACE:
      link->action_key_pressed();
      break;

      // sword key
    case SDLK_c:
      link->sword_key_pressed();
      break;

      // move Link
    case SDLK_RIGHT:
      link->get_movement()->start_right();
      break;

    case SDLK_UP:
      link->get_movement()->start_up();
      break;

    case SDLK_LEFT:
      link->get_movement()->start_left();
      break;

    case SDLK_DOWN:
      link->get_movement()->start_down();
      break;

    default:
      break;
    }
  }
  else if (game->is_showing_message()) { // is a message being shown?

    switch (keysym.sym) {

      // action key
    case SDLK_SPACE:
      game->get_dialog_box()->action_key_pressed();
      break;

      // sword key
    case SDLK_c:
      game->get_dialog_box()->sword_key_pressed();
      break;

    case SDLK_UP:
    case SDLK_DOWN:
      game->get_dialog_box()->up_or_down_key_pressed();
      break;

    default:
      break;
    }
  }
}

/**
 * This function is called by the game when a key is released.
 * Depending on the game state, the appropriate object is notified.
 * @param keysym the key pressed
 */
void Keyboard::key_released(const SDL_keysym &keysym) {

  if (!game->is_suspended()) {
    // if the game is not suspended, the keys apply to Link

    switch (keysym.sym) {
	
      // move Link
    case SDLK_RIGHT:
      link->get_movement()->stop_right();
      break;

    case SDLK_UP:
      link->get_movement()->stop_up();
      break;

    case SDLK_LEFT:
      link->get_movement()->stop_left();
      break;

    case SDLK_DOWN:
      link->get_movement()->stop_down();
      break;

    default:
      break;
    }
  }
}
