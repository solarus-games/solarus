/**
 * This module defines the class MapEntrance.
 */

#ifndef ZSDX_MAP_ENTRANCE_H
#define ZSDX_MAP_ENTRANCE_H

#include "Types.h"

/**
 * Describes an entrance of a map, i.e. a possible state
 * of the map when it is loaded.
 */
class MapEntrance {

 private:
  const MusicID music_id;
  const Layer layer;
  const int link_x;
  const int link_y;
  const int link_direction;

 public:

  /**
   * Creates a new entrance.
   * @param music_id the background music to play in this state
   * (can be a real music, MUSIC_NONE, MUSIC_NO_CHANGE or MUSIC_DEFAULT)
   * @param layer layer of link's position in this state
   * @param link_x initial x position of Link in this state
   * @param link_y initial y position of Link in this state
   * @param link_direction initial direction of Link in this state (0 to 3)
   */
  MapEntrance(MusicID music_id, Layer layer, int link_x, int link_y, int link_direction);

  /**
   * Creates a new entrance with the default music.
   * @param layer layer of link's position in this state
   * @param link_x initial x position of Link in this state
   * @param link_y initial y position of Link in this state
   * @param link_direction initial direction of Link in this state (0 to 3)
   */
  MapEntrance(Layer layer, int link_x, int link_y, int link_direction);

  /**
   * Destructor.
   */
  inline ~MapEntrance(void) { };

  /**
   * Returns the background music to play in this initial state.
   * @return the background music to play
   * (can be a real music, MUSIC_NONE, MUSIC_NO_CHANGE or MUSIC_DEFAULT)
   */
  MusicID get_music_id(void);

  /**
   * Returns the layer of Link in this initial state.
   * @return the layer
   */
  Layer get_layer(void);

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
