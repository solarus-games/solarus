#include <SDL/SDL.h>
#include "animation_frame_handler.h"
#include "global.h"
#include "sdl_user_events.h"

/**
 * Interval in millisecond between two frames of an animation
 */
static const Uint32 frame_interval = 250;

/**
 * Array to get the current frame (0, 1 or 2) depending on
 * the sequence type and the frame counter
 */
static const short frames[2][12] = {
  {0, 1, 2, 1, 0, 1, 2, 1, 0, 1, 2, 1}, // sequence 1-2-3-2
  {0, 1, 2, 0, 1, 2, 0, 1, 2, 0, 1, 2}, // sequence 1-2-3
};

/**
 * Constructor.
 */
AnimationFrameHandler::AnimationFrameHandler(void) {
  frame_counter = 0;
  current_frames[0] = 0;
  current_frames[1] = 0;

  event_animation_frame_finished.type = SDL_USEREVENT;
  event_animation_frame_finished.user.code = EVENT_ANIMATION_FRAME_FINISHED;
  event_animation_frame_finished.user.data1 = NULL;
  event_animation_frame_finished.user.data2 = NULL;  
}

/**
 * Increment the frame counter and updates current_frame.
 */
void AnimationFrameHandler::increment_frame_counter(void) {
  frame_counter = (frame_counter + 1) % 12;
  current_frames[0] = frames[0][frame_counter];
  current_frames[1] = frames[1][frame_counter];
}

/**
 * Function called by the SDL timer every 250 ms.
 * It increments the frame counter and pushes an event
 * to redraw the screen.
 */
Uint32 animation_next_frame(Uint32 interval, void *param) {

  // we increment the frame counter
  zsdx_global.animation_frame_handler.increment_frame_counter();

  // we push an event to redraw the screen
  SDL_PushEvent(&zsdx_global.animation_frame_handler.event_animation_frame_finished);

  return interval; // should be 250
}
