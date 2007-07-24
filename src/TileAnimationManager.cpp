/**
 * This module handles the animation of the tiles.
 * The animated tiles are not treated as sprites because
 * they are simpler (don't have an animation or a direction).
 *
 * There are two possibles sequences of animation for a tile:
 * 0-1-2-1... or 0-1-2...
 */

#include <SDL/SDL.h>
#include "TileAnimationManager.h"
#include "ZSDX.h"

/**
 * Interval in millisecond between two frames of an animation.
 */
static const Uint32 TILE_FRAME_INTERVAL = 250;

/**
 * Array to associate the current frame (0, 1 or 2) depending on
 * the sequence type and the frame counter (0 to 11).
 */
static const short frames[2][12] = {
  {0, 1, 2, 1, 0, 1, 2, 1, 0, 1, 2, 1}, // sequence 0-1-2-1
  {0, 1, 2, 0, 1, 2, 0, 1, 2, 0, 1, 2}, // sequence 0-1-2
};

/**
 * Frame counter (0 to 11), increased every 250 ms.
 */
short TileAnimationManager::frame_counter = 0;

/**
 * Current frame (0 to 2) for both sequences
 */
short TileAnimationManager::current_frames[2] = {0, 0};

/**
 * Increments the frame counter and updates the current frame.
 * This function is called every 250 ms by the SDL timer.
 * @param interval interval in millisecond since the last call (should be 250)
 * @param param additional parameters (not used)
 * @return the interval to the next call (250)
 */
Uint32 TileAnimationManager::increment_frame_counter(Uint32 interval, void *param) {
  // increment the frame counter
  frame_counter = (frame_counter + 1) % 12;
  current_frames[0] = frames[0][frame_counter];
  current_frames[1] = frames[1][frame_counter];

  return interval; // should be 250
}
