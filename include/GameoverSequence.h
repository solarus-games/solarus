#ifndef ZSDX_GAMEOVER_SEQUENCE_H
#define ZSDX_GAMEOVER_SEQUENCE_H

#include "Common.h"

/**
 * The game over sequence displayed when the hero dies.
 */
class GameoverSequence {

 private:

  // data
  Game *game; /**< the game */

  // graphics
  SDL_Surface *gameover_menu_img; /**< image of the game over menu */
  Sprite *hero_dead_sprite;       /**< sprite of the hero dying or dead (different from the real Link which is
				   * suspended as all map entities) */
  int hero_dead_x;                /**< x position of the hero dead sprite on the screen */
  int hero_dead_y;                /**< y position of the hero dead sprite on the screen */
  Sprite *fade_sprite;            /**< sprite of the fade in / fade out effect */
  // TODO fairy

  // state
  enum State {
    DYING,          /**< the hero is dying */
    CLOSING_GAME,   /**< fade out on the game screen */
    OPENING_MENU,   /**< fade in on the game over menu */
    SAVED_BY_FAIRY, /**< the player is being saved by a fairy */
    MENU,           /**< the player can choose an option in the game over menu */
   };

  State state;   /**< current state of the gameover sequence */
  bool finished; /**< true to stop the gameover sequence */

 public:

  GameoverSequence(Game *game);
  ~GameoverSequence(void);

  // update and display
  void update(void);
  void display(SDL_Surface *destination_surface);
};

#endif
