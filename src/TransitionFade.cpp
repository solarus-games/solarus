/*
 * Copyright (C) 2009-2011 Christopho, Solarus - http://www.solarus-engine.org
 * 
 * Solarus is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Solarus is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "TransitionFade.h"
#include "lowlevel/Surface.h"
#include "lowlevel/System.h"

/**
 * @brief Creates a fade-in or fade-out transition effect.
 * @param direction direction of the transition effect (in or out)
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
 * @brief Destructor.
 */
TransitionFade::~TransitionFade() {

}

/**
 * @brief Sets the delay between two frames.
 *
 * The default delay is 20 ms.
 *
 * @param delay the new delay in milliseconds
 */
void TransitionFade::set_delay(uint32_t delay) {
  this->delay = delay;
}

/**
 * @brief Starts this transition effect.
 */
void TransitionFade::start() {
  alpha = alpha_start;
  next_frame_date = System::now();
}

/**
 * @brief Returns whether the transition effect is started and not finished yet.
 * @return true if the transition effect is started
 */
bool TransitionFade::is_started() {
  return alpha != -1 && !is_finished();
}

/**
 * @brief Returns whether the transition effect is finished.
 * @return true
 */
bool TransitionFade::is_finished() {
  return alpha == alpha_limit;
}

/**
 * @brief Updates this transition effect.
 *
 * This function should be called repeatedly while the transition exists.
 */
void TransitionFade::update() {

  if (!is_started()) {
    return;
  }

  uint32_t now = System::now();

  // update the transition effect if needed
  while (now >= next_frame_date && alpha != alpha_limit) {
    alpha += alpha_increment;
    next_frame_date += delay; // 20 ms between two frame updates
  }
}

/**
 * @brief Displays the transition effect on a surface.
 * @param surface the destination surface
 */
void TransitionFade::display(Surface *surface) {

  // display the transition effect on the surface
  int alpha_impl = std::min(alpha, 255);
  surface->set_opacity(alpha_impl);
}

