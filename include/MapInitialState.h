/**
 * This module defines the class MapInitialState.
 */

#ifndef ZSDX_MAP_INITIAL_STATE_H
#define ZSDX_MAP_INITIAL_STATE_H

#include "Types.h"

// TODO: add the layer in the initial state, rename class to MapEntrance?

/**
 * Describes a possible state of the map when it is loaded.
 */
class MapInitialState {

 private:
  const MusicID music_id;
  const int link_x;
  const int link_y;
  const int link_direction;

 public:

  /**
   * Constructor.
   * @param music_id the background music to play in this state
   * (can be a real music, MUSIC_NONE, MUSIC_NO_CHANGE or MUSIC_DEFAULT)
   * @param link_x initial x position of Link in this state
   * @param link_y initial y position of Link in this state
   * @param link_direction initial direction of Link in this state (0 to 3)
   */
  MapInitialState(MusicID music_id, int link_x, int link_y, int link_direction);

  /**
   * Constructor.
   * @param link_x initial x position of Link in this state
   * @param link_y initial y position of Link in this state
   * @param link_direction initial direction of Link in this state (0 to 3)
   */
  MapInitialState(int link_x, int link_y, int link_direction);

  /**
   * Destructor.
   */
  inline ~MapInitialState(void) { };

  /**
   * Returns the background music to play in this initial state.
   * @return the background music to play
   * (can be a real music, MUSIC_NONE, MUSIC_NO_CHANGE or MUSIC_DEFAULT)
   */
  MusicID get_music_id(void);

  /**
   * Returns the x coordinate of Link's position in this initial state.

   */
  int get_link_x(void);

  /**
   * Returns the y coordinate of Link's position in this initial state.
   */
  int get_link_y(void);

  /**
   * Returns Link's initial direction in this initial state.
   */
  int get_link_direction(void);

};

#endif
