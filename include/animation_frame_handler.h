#ifndef ZSDX_ANIMATION_FRAME_HANDLER_H
#define ZSDX_ANIMATION_FRAME_HANDLER_H

#include <SDL/SDL.h>
#include "datatypes.h"

const Uint32 frame_interval = 250; // interval in millisecond

static const short frames[2][12] = {
  {0, 1, 2, 1, 0, 1, 2, 1, 0, 1, 2, 1}, // sequence 1-2-3-2
  {0, 1, 2, 0, 1, 2, 0, 1, 2, 0, 1, 2}, // sequence 1-2-3
};

// function called by the SDL timer every 250 ms
Uint32 animation_next_frame(Uint32 interval, void *param);

class AnimationFrameHandler {

 private:
  // current frame for all animations
  short frame_counter; /* 0 to 11 */
  short current_frames[2]; /* 0, 1 or 2 */

 public:
  inline AnimationFrameHandler(void) { frame_counter = current_frames[0] = current_frames[1] = 0; }
  inline ~AnimationFrameHandler(void) { }

  inline short get_current_frame(animation_sequence_t sequence) { return current_frames[sequence]; }
  inline void increment_frame_counter(void) {
    frame_counter = (frame_counter + 1) % 12;
    current_frames[0] = frames[0][frame_counter];
    current_frames[1] = frames[1][frame_counter];
  }
};

#endif
