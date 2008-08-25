#ifndef ZSDX_KEYBOARD_H
#define ZSDX_KEYBOARD_H

#include "Common.h"

/**
 * This class receives the SDL keyboard events
 * that occur during the game and notifies the
 * appropriate objects that a key was pressed.
 */
class Keyboard {

 private:
  
  Game *game;
  Link *link;

 public:

  Keyboard(Game *game);
  ~Keyboard(void);

  void key_pressed(const SDL_keysym &keysym);
  void key_released(const SDL_keysym &keysym);
};

#endif
