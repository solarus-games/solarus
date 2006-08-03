#include <SDL/SDL.h>
#include "animation_frame_handler.h"
#include "global.h"
#include "sdl_user_events.h"

AnimationFrameHandler::AnimationFrameHandler(void) {
  frame_counter = 0;
  current_frames[0] = 0;
  current_frames[1] = 0;

  event_animation_frame_finished.type = SDL_USEREVENT;
  event_animation_frame_finished.user.code = EVENT_ANIMATION_FRAME_FINISHED;
  event_animation_frame_finished.user.data1 = NULL;
  event_animation_frame_finished.user.data2 = NULL;  
}

// This function is called every 250 ms
Uint32 animation_next_frame(Uint32 interval, void *param) {

  // we increment the global frame counter
  zsdx_global.animation_frame_handler.increment_frame_counter();

  // we push an event to redraw the screen
  SDL_PushEvent(&zsdx_global.animation_frame_handler.event_animation_frame_finished);

  return interval; // should be 250
}
