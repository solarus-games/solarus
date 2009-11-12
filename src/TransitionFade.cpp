/*
 * Copyright (C) 2009 Christopho, Zelda Solarus - http://www.zelda-solarus.com
 * 
 * Zelda: Mystery of Solarus DX is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Zelda: Mystery of Solarus DX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "TransitionFade.h"

/**
 * Constructor.
 */
TransitionFade::TransitionFade(Transition::Direction direction):
  Transition(direction), alpha(-1) {

  if (direction == OUT) {
    alpha_start = 256;
    alpha_limit = 0;
    alpha_increment = -8;
  }
  else {
    alpha_start = 0;
    alpha_limit = 256;
    alpha_increment = 8;
  }  

  set_delay(20);
}

/**
 * Destructor.
 */
TransitionFade::~TransitionFade(void) {

}

/**
 * Sets the delay between two frames.
 * The default delay is 20 ms.
 * @param delay the new delay in milliseconds
 */
void TransitionFade::set_delay(Uint32 delay) {
  this->delay = delay;
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
    next_frame_date += delay; // 20 ms between two frame updates
  }

  // display the transition effect on the surface
  int alpha_impl = std::min(alpha, 255);

  // SDL has a special handling of the alpha value 128, but it does not work correctly with my computer
  if (alpha_impl == 128) {
    alpha_impl = 127;
  }

  SDL_SetAlpha(surface, SDL_SRCALPHA, alpha_impl);
}

