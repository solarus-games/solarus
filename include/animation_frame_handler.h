#ifndef ZSDX_ANIMATION_FRAME_HANDLER_H
#define ZSDX_ANIMATION_FRAME_HANDLER_H

#include <SDL/SDL.h>
#include "datatypes.h"

/**
 * Animation sequence type: 1-2-3-2 or 1-2-3
 */
enum animation_sequence_t {
  ANIMATION_SEQUENCE_1232,
  ANIMATION_SEQUENCE_123,
};

/**
 * Function called by the SDL timer every 250 ms
 */
Uint32 animation_next_frame(Uint32 interval, void *param);

/**
 * This class updates during the game the animation frame
 * of the animated tiles and the simple sprites.
 * Two kinds of animation sequences are handled: 1-2-3-2 and 1-2-3.
 */
class AnimationFrameHandler {

  /**
   * Function called by the SDL timer every 250 ms
   */
  friend Uint32 animation_next_frame(Uint32 interval, void *param);

 private:
  // Frame counter (0 to 11), increased every 250 ms
  short frame_counter;

  // Current frame for each sequence
  short current_frames[2];

  void increment_frame_counter(void);

 public:
  AnimationFrameHandler(void);
  inline ~AnimationFrameHandler(void) { }

  inline short get_current_frame(animation_sequence_t sequence) { return current_frames[sequence]; }
};
#endif
