#ifndef ZSDX_GAMEOVER_SEQUENCE_H
#define ZSDX_GAMEOVER_SEQUENCE_H

#include "Common.h"

/**
 * The game over sequence displayed when the hero dies.
 */
class GameoverSequence {

 private:

  // data
  Game *game;       /**< the game */
  MusicId music_id; /**< the music played before game over */

  // graphics
  SDL_Surface *gameover_menu_img; /**< image of the game over menu */
  Sprite *hero_dead_sprite;       /**< sprite of the hero dying or dead (different from the real Link which is
				   * suspended as all map entities) */
  int hero_dead_x;                /**< x position of the hero dead sprite on the screen */
  int hero_dead_y;                /**< y position of the hero dead sprite on the screen */

  Sprite *fade_sprite;            /**< sprite of the fade in / fade out effect */
  Uint32 red_screen_color;        /**< exact color of the red screen */

  Sprite *fairy_sprite;           /**< sprite of the fairy (as rescuing fairy or cursor for the menu) */
  int fairy_x;                    /**< x position of the fairy on the screen */
  int fairy_y;                    /**< y position of the fairy on the screen */
  TargetMovement *fairy_movement; /**< movement of the fairy */

  // state
  enum State {
    DYING,          /**< the hero is dying */
    CLOSING_GAME,   /**< fade out on the game screen */
    RED_SCREEN,     /**< red screen during a small delay */
    OPENING_MENU,   /**< fade in on the game over menu */
    SAVED_BY_FAIRY, /**< the player is being saved by a fairy */
    MENU,           /**< the player can choose an option in the game over menu */
    RESUME_GAME,    /**< the game can be resumed */
   };

  State state;           /**< current state of the gameover sequence */
  Uint32 open_menu_date; /**< date when moving from state RED_SCREEN to state OPENING_MENU */

 public:

  // creation and destruction
  GameoverSequence(Game *game);
  ~GameoverSequence(void);

  // state
  bool is_finished(void);

  // update and display
  void update(void);
  void display(SDL_Surface *destination_surface);
};

#endif
