/**
 * This module defines the class Game.
 */

#ifndef ZSDX_GAME_H
#define ZSDX_GAME_H

#include "GameResource.h"

/**
 * This class represents the game.
 * I think that the saves will be handled here.
 */
class Game {

 public:
  /**
   * Constructor.
   */
  inline Game(void) { }

  /**
   * Destructor.
   */
  inline ~Game(void) { }
  
  /**
   * Launches a map.
   * @param map_id id of the map to get
   */
  void launch_map(MapID map_id);
};

#endif
