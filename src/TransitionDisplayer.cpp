/**
 * This module handles the transition effects between two maps.
 */

#include "TransitionDisplayer.h"
#include "Map.h"
#include "Game.h"
#include "ZSDX.h"

/**
 * Displays a fade in or a fade out effect.
 */
void TransitionDisplayer::fade(Map *map, TransitionDirection direction) {

  Uint32 interval = 20; // interval between two updates
  Uint32 last_frame_date = SDL_GetTicks();
  Uint32 ticks;

  Uint32 alpha_start, alpha_limit, alpha_increment;
  
  if (direction == TRANSITION_IN) {
    alpha_start = 0;
    alpha_limit = 256;
    alpha_increment = 16;
  }
  else {
    alpha_start = 256;
    alpha_limit = 0;
    alpha_increment = -16;
  }
  
  // SDL main loop
  Uint32 alpha = alpha_start + alpha_increment;
  while (alpha != alpha_limit) { // loop until the transition is finished
    
    // display everything each time frame
    ticks = SDL_GetTicks();
    if (ticks >= last_frame_date + interval) {
      last_frame_date = ticks;
      
      SDL_SetAlpha(map->get_visible_surface(), SDL_SRCALPHA, alpha);
      ZSDX::game->redraw_screen(map);
      
      alpha += alpha_increment;
    }
  }

  SDL_SetAlpha(map->get_visible_surface(), SDL_SRCALPHA, 255);
}


/**
 * Displays a specified entering transition effect .
 */
void TransitionDisplayer::showTransition(Map *map, Transition transition,
					 TransitionDirection direction) {

  switch (transition) {

  case TRANSITION_IMMEDIATE:
    // nothing to do
    break;

  case TRANSITION_FADE:
    fade(map, direction);
    break;
  }
}
