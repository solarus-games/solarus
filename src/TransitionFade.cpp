/*
 * Copyright (C) 2006-2018 Christopho, Solarus - http://www.solarus-games.org
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
#include "solarus/TransitionFade.h"
#include "solarus/lowlevel/Surface.h"
#include "solarus/lowlevel/System.h"
#include "solarus/lowlevel/Debug.h"
#include "solarus/lowlevel/Color.h"
#include "solarus/lowlevel/Video.h"
#include <algorithm>

namespace Solarus {

/**
 * \brief Creates a fade-in or fade-out transition effect.
 * \param direction direction of the transition effect (opening or closing)
 * \param dst_surface The destination surface that will receive this
 * transition.
 */
TransitionFade::TransitionFade(Direction direction, Surface& dst_surface):
  Transition(direction),
  finished(false),
  alpha(-1),
  next_frame_date(0),
  dst_surface(&dst_surface),
  colored(true),
  transition_color(Color::black) {

  if (direction == Direction::CLOSING) {
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
 * \brief Returns whether this fade transition uses a foreground color.
 * \return \c true if there is a foreground color,
 * \c false if the transition changes the opacity of the destination surface
 * (only possible for sotware destination surfaces).
 */
bool TransitionFade::is_colored() const {
  return colored;
}

/**
 * \brief Returns the foreground color of this fade transition.
 *
 * This function only makes sense when is_colored() is \c true.
 *
 * \return The color of the transition.
 */
const Color& TransitionFade::get_color() const {
  return transition_color;
}

/**
 * \brief Sets the foreground color of this fade transition.
 *
 * The default color is black.
 *
 * \param color The color of the transition.
 */
void TransitionFade::set_color(const Color& color) {

  transition_color = color;
  colored = true;
}

/**
 * \brief Sets no foreground color for this fade transition.
 *
 * The transition effect will then change the opacity of the destination
 * surface. This is only possible for software destination surfaces, so make
 * sure you know what you are doing.
 */
void TransitionFade::clear_color() {

  colored = false;
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

    finished = (alpha == alpha_limit);
  }
}

/**
 * \brief Draws the transition effect on a surface.
 * \param dst_surface The destination surface.
 */
void TransitionFade::draw(Surface& dst_surface) {

  // Draw the transition effect on the surface.
  int alpha_impl = std::min(alpha, 255);

  if (!colored) {
    // Set the opacity on the surface.
    dst_surface.set_opacity(alpha_impl);
  }
  else {
    // Add a colored foreground surface with the appropriate opacity.
    uint8_t r, g, b, a;
    transition_color.get_components(r, g, b, a);
    // A full opaque transition corresponds to a foreground with full alpha.
    Color fade_color(r, g, b, 255 - std::min(alpha_impl, (int) a));

    dst_surface.fill_with_color(fade_color);
  }

  this->dst_surface = &dst_surface;
}

}

