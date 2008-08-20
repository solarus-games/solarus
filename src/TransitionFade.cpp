#include "TransitionFade.h"

/**
 * Constructor.
 */
TransitionFade::TransitionFade(TransitionDirection direction):
TransitionEffect(direction), alpha(-1) {

  if (direction == TRANSITION_OUT) {
    alpha_start = 256;
    alpha_limit = 0;
    alpha_increment = -16;
  }
  else {
    alpha_start = 0;
    alpha_limit = 256;
    alpha_increment = 16;
  }  
}

/**
 * Destructor.
 */
TransitionFade::~TransitionFade(void) {

}

/**
 * Starts this transition effect.
 */
void TransitionFade::start(void) {
  alpha = alpha_start;
  next_frame_date = SDL_GetTicks();
}

/**
 * Returns whether the transition effect is started and not finished yet.
 * @return true if the transition effect is started
 */
bool TransitionFade::is_started(void) {
  return alpha != -1 && !is_over();
}

/**
 * Returns whether the transition effect is over.
 * @return true
 */
bool TransitionFade::is_over(void) {
  return alpha == alpha_limit;
}

/**
 * Displays the transition effect on a surface.
 */
void TransitionFade::display(SDL_Surface *surface) {

  Uint32 now = SDL_GetTicks();

  // update the transition effect if needed
  while (now >= next_frame_date && alpha != alpha_limit) {
    alpha += alpha_increment;
    next_frame_date += 20; // 20 ms between two frame updates
  }

  // display the transition effect on the surface
  SDL_SetAlpha(surface, SDL_SRCALPHA, MIN(alpha, 255));
}
