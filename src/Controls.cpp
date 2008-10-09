#include "Controls.h"
#include "Game.h"

/**
 * Constructor.
 * @param game the game
 */
Controls::Controls(Game *game):
  game(game), link(game->get_link()) {

  // load the controls from the savegame
}

/**
 * Destructor.
 */
Controls::~Controls(void) {

}

/**
 * Returns whether the specified game key is pressed.
 * The key can be pressed from the keyboard or the joypad.
 */
bool Controls::is_key_pressed(Controls::GameKey key) {
  return false;
}

/**
 * This function is called by the game when an SDL event occurs.
 * @param event an SDL event
 */
void Controls::handle_event(const SDL_Event &event) {

  /*
   * If no key is being customized, we look for a binding
   * for this SDL event and we ignore the event if no binding was found.
   * If a key is being customized, we consider instead this event as
   * the new binding for this game key.
   */

  switch (event.type) {

    // a key is pressed
  case SDL_KEYDOWN:
    key_pressed(event.key.keysym);
    break;

    // a key is released
  case SDL_KEYUP:
    key_released(event.key.keysym);
    break;

    // joypad
  case SDL_JOYAXISMOTION:
    break;

  case SDL_JOYHATMOTION:
    break;

  case SDL_JOYBUTTONDOWN:
    break;

  case SDL_JOYBUTTONUP:
    break;
  }
}

void Controls::key_pressed(const SDL_keysym &keysym) {

}

void Controls::key_released(const SDL_keysym &keysym) {

}

// customization

/**
 * Notifies the control manager that the specified key is going to be customized.
 * After this function is called, the next keyboard or joypad event received will
 * not be treated normally; it will be considered as the new keyboard or joypad
 * binding for this game key. Then, the keyboard and joypad events will be treated
 * normally again. Call is_customization_done() to know when this happens.
 * @param key the game key to customize
 */
void Controls::customize(GameKey key) {

}

/**
 * Returns whether the customization process started by calling the customize()
 * function is done, i.e. whether the key has received a new binding
 * and the controls are now treated normally again.
 * @return true if no game key is being customized
 */
bool Controls::is_customization_done(void) {
  return true;
}
