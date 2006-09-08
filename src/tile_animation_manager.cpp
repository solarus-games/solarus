#include <SDL/SDL.h>
#include "tile_animation_manager.h"
#include "zsdx.h"

/* Interval in millisecond between two frames of an animation
 */
static const Uint32 frame_interval = 250;

/* Array to get the current frame (0, 1 or 2) depending on
 * the sequence type and the frame counter
 */
static const short frames[2][12] = {
  {0, 1, 2, 1, 0, 1, 2, 1, 0, 1, 2, 1}, // sequence 1-2-3-2
  {0, 1, 2, 0, 1, 2, 0, 1, 2, 0, 1, 2}, // sequence 1-2-3
};

/* Constructor
 */
TileAnimationManager::TileAnimationManager(void) {
  frame_counter = 0;
  current_frames[0] = 0;
  current_frames[1] = 0;
}

/* Increment the frame counter and updates current_frame.
 */
void TileAnimationManager::increment_frame_counter(void) {
  frame_counter = (frame_counter + 1) % 12;
  current_frames[0] = frames[0][frame_counter];
  current_frames[1] = frames[1][frame_counter];
}

/* Function called by the SDL timer every 250 ms.
 * It increments the frame counter.
 */
Uint32 tile_animation_next_frame(Uint32 interval, void *param) {

  // we increment the frame counter
  zsdx.get_tile_animation_manager()->increment_frame_counter();

  return interval; // should be 250
}
