/**
 * This module handles the transition effects between two maps.
 */

#include "TransitionDisplayer.h"
#include "Map.h"
#include "Game.h"
#include "ZSDX.h"

// TODO factorize the code of fadeIn and fadeOut

/**
 * Displays a fade in effect from a black screen.
 */
void TransitionDisplayer::fadeIn(Map *map) {

  Uint32 interval = 20; // interval between two updates
  Uint32 last_frame_date = SDL_GetTicks();
  Uint32 ticks;
  Uint32 alpha = 32;
  
  // SDL main loop
  while (alpha < 256) { // loop until the transition is finished
    
    // display everything each time frame
    ticks = SDL_GetTicks();
    if (ticks >= last_frame_date + interval) {
      last_frame_date = ticks;
      
      SDL_SetAlpha(map->get_surface(), SDL_SRCALPHA, alpha);
      ZSDX::game->redraw_screen(map);

      alpha += 16;
    }
  }
}


/**
 * Displays a fade out effect from the current screen to a black screen.
 */
void TransitionDisplayer::fadeOut(Map *map) {

  Uint32 interval = 20; // interval between two updates
  Uint32 last_frame_date = SDL_GetTicks();
  Uint32 ticks;
  Uint32 alpha = 256;
  
  // SDL main loop
  while (alpha > 0) { // loop until the transition is finished
    
    // display everything each time frame
    ticks = SDL_GetTicks();
    if (ticks >= last_frame_date + interval) {
      last_frame_date = ticks;
      
      alpha -= 16;

      SDL_SetAlpha(map->get_surface(), SDL_SRCALPHA, alpha);
      ZSDX::game->redraw_screen(map);
    }
  }
}


/**
 * Displays a specified entering transition effect .
 */
void TransitionDisplayer::showInTransition(Map *map, Transition transition) {

  switch (transition) {

  case TRANSITION_IMMEDIATE:
    // nothing to do
    break;

  case TRANSITION_FADE:
    fadeIn(map);
    break;
  }

  SDL_SetAlpha(map->get_surface(), SDL_SRCALPHA, 255);
}

/**
 * Displays a specified exit transition effect.
 */
void TransitionDisplayer::showOutTransition(Map *map, Transition transition) {

  switch (transition) {

  case TRANSITION_IMMEDIATE:
    // nothing to do
    break;

  case TRANSITION_FADE:
    fadeOut(map);
    break;
  }

}
