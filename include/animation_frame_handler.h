#ifndef ZSDX_ANIMATION_FRAME_HANDLER_H
#define ZSDX_ANIMATION_FRAME_HANDLER_H

#include <SDL/SDL.h>
#include "datatypes.h"

// function called by the SDL timer every 250 ms
Uint32 animation_next_frame(Uint32 interval, void *param);

class AnimationFrameHandler {

 private:
  // current frame for all animations
  short frame_counter; /* 0 to 11 */
  short current_frames[2]; /* 0, 1 or 2 */

 public:
  AnimationFrameHandler(void);
  inline ~AnimationFrameHandler(void) { }

  SDL_Event event_animation_frame_finished;

  inline short get_current_frame(animation_sequence_t sequence) { return current_frames[sequence]; }
  void increment_frame_counter(void);
};
#endif
