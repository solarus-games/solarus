#include <SDL/SDL.h>
#include "animation_frame_handler.h"
#include "global.h"
#include "sdl_user_events.h"

static const Uint32 frame_interval = 250; // interval in millisecond

static const short frames[2][12] = {
  {0, 1, 2, 1, 0, 1, 2, 1, 0, 1, 2, 1}, // sequence 1-2-3-2
  {0, 1, 2, 0, 1, 2, 0, 1, 2, 0, 1, 2}, // sequence 1-2-3
};

AnimationFrameHandler::AnimationFrameHandler(void) {
  frame_counter = 0;
  current_frames[0] = 0;
  current_frames[1] = 0;

  event_animation_frame_finished.type = SDL_USEREVENT;
  event_animation_frame_finished.user.code = EVENT_ANIMATION_FRAME_FINISHED;
  event_animation_frame_finished.user.data1 = NULL;
  event_animation_frame_finished.user.data2 = NULL;  
}

void AnimationFrameHandler::increment_frame_counter(void) {
  frame_counter = (frame_counter + 1) % 12;
  current_frames[0] = frames[0][frame_counter];
  current_frames[1] = frames[1][frame_counter];
}

// This function is called every 250 ms
Uint32 animation_next_frame(Uint32 interval, void *param) {

  // we increment the global frame counter
  zsdx_global.animation_frame_handler.increment_frame_counter();

  // we push an event to redraw the screen
  SDL_PushEvent(&zsdx_global.animation_frame_handler.event_animation_frame_finished);

  return interval; // should be 250
}
