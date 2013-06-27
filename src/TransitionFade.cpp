/*
 * Copyright (C) 2006-2013 Christopho, Solarus - http://www.solarus-games.org
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
#include "lowlevel/Debug.h"

/**
 * \brief Creates a fade-in or fade-out transition effect.
 * \param direction direction of the transition effect (in or out)
 */
TransitionFade::TransitionFade(Transition::Direction direction):
  Transition(direction),
  finished(false),
  alpha(-1),
  dst_surface(NULL) {

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
 * \brief Destructor.
 */
TransitionFade::~TransitionFade() {
}

/**
 * \brief Sets the delay between two frames.
 *
 * The default delay is 20 ms.
 *
 * \param delay the new delay in milliseconds
 */
void TransitionFade::set_delay(uint32_t delay) {
  this->delay = delay;
}

/**
 * \brief Starts this transition effect.
 */
void TransitionFade::start() {
  alpha = alpha_start;
  next_frame_date = System::now();
}

/**
 * \brief Returns whether the transition effect is started and not finished yet.
 * \return true if the transition effect is started
 */
bool TransitionFade::is_started() const {

  return alpha != -1 && !is_finished();
}

/**
 * \brief Returns whether the transition effect is finished.
 * \return true if the transition effect is finished
 */
bool TransitionFade::is_finished() const {

  return finished;
}

/**
 * \brief Notifies the transition effect that it was just suspended
 * or resumed.
 * \param suspended true if suspended, false if resumed.
 */
void TransitionFade::notify_suspended(bool suspended) {

  if (!suspended) {
    next_frame_date += System::now() - get_when_suspended();
  }
}

/**
 * \brief Updates this transition effect.
 *
 * This function should be called repeatedly while the transition exists.
 */
void TransitionFade::update() {

  if (!is_started() || is_suspended()) {
    return;
  }

  uint32_t now = System::now();

  // update the transition effect if needed
  while (now >= next_frame_date && !finished) {
    alpha += alpha_increment;
    next_frame_date += delay; // 20 ms between two frame updates

    if (dst_surface != NULL) {
      // make sure the final opacity is applied to the surface
      int alpha_impl = std::min(alpha, 255);
      dst_surface->set_opacity(alpha_impl);
    }

    finished = (alpha == alpha_limit);
  }
}

/**
 * \brief Draws the transition effect on a surface.
 * \param dst_surface the destination surface
 */
void TransitionFade::draw(Surface& dst_surface) {

  Debug::check_assertion(
      this->dst_surface == NULL || this->dst_surface == &dst_surface,
      "Unexpected surface for transition");

  // draw the transition effect on the surface
  int alpha_impl = std::min(alpha, 255);
  dst_surface.set_opacity(alpha_impl);
  this->dst_surface = &dst_surface;
}

