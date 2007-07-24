/**
 * This module handles the animation of the tiles.
 * The animated tiles are not treated as sprites because
 * they are simpler (don't have an animation or a direction).
 *
 * There are two possibles sequences of animation for a tile:
 * 0-1-2-1... or 0-1-2...
 */

#ifndef ZSDX_TILE_ANIMATION_MANAGER_H
#define ZSDX_TILE_ANIMATION_MANAGER_H

#include <SDL/SDL.h>

/**
 * Tile animation sequence type: 0-1-2 or 0-1-2-1
 */
enum tile_animation_sequence_t {
  ANIMATION_SEQUENCE_012,
  ANIMATION_SEQUENCE_0121,
};

/**
 * This class updates during the game the animation frame
 * of the animated tiles.
 * The two kinds of animation sequences are handled: 0-1-2-1 and 0-1-2.
 * When a tile is displayed, it calls the method get_current_frame()
 * of this class to know its current frame number. 
 */
class TileAnimationManager {

 private:

  /**
   * Frame counter (0 to 11), increased every 250 ms.
   */
  static short frame_counter;

  /**
   * Current frame (0 to 2) for both sequences
   */
  static short current_frames[];

 public:

  /**
   * Increments the frame counter and updates the current frame.
   * This function is called every 250 ms by the SDL timer.
   * @param interval interval in millisecond since the last call (should be 250)
   * @param param additional parameters (not used)
   * @return the interval to the next call (250)
   */
  static Uint32 increment_frame_counter(Uint32 interval, void *param);

  /**
   * Returns the current_frame (0, 1 or 2) of a sequence 0-1-2-1 or 0-1-2.
   * @param sequence the kind of sequence: ANIMATION_SEQUENCE_0121 or ANIMATION_SEQUENCE_012
   */
  static inline short get_current_frame(tile_animation_sequence_t sequence) { return current_frames[sequence]; }
};

#endif
