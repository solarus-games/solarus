/**
 * This module defines the class MapInitialState.
 */

#include "MapInitialState.h"


/**
 * Constructor.
 */
MapInitialState::MapInitialState(MusicID music_id, int link_x, int link_y, int link_direction):
  music_id(music_id), link_x(link_x), link_y(link_y), link_direction(link_direction) {

}

/**
 * Constructor.
 */
MapInitialState::MapInitialState(int link_x, int link_y, int link_direction):
  music_id(MUSIC_DEFAULT), link_x(link_x), link_y(link_y), link_direction(link_direction) {

}

/**
 * Returns the background music to play in this initial state.
 * @return the background music to play
 * (can be a real music, MUSIC_NONE, MUSIC_NO_CHANGE or MUSIC_DEFAULT)
 */
MusicID MapInitialState::get_music_id(void) {
  return music_id;
}

/**
 * Returns the x coordinate of Link's position.
 */
int MapInitialState::get_link_x(void) {
  return link_x;
}

/**
 * Returns the y coordinate of Link's position.
 */
int MapInitialState::get_link_y(void) {
  return link_y;
}

/**
 * Returns Link's initial direction.
 */
int MapInitialState::get_link_direction(void) {
  return link_direction;
}

