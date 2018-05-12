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
#include "solarus/core/Debug.h"
#include "solarus/core/Game.h"
#include "solarus/core/Map.h"
#include "solarus/core/System.h"
#include "solarus/graphics/Color.h"
#include "solarus/graphics/Surface.h"
#include "solarus/graphics/TransitionScrolling.h"
#include <memory>

namespace Solarus {

/**
 * \brief Creates a scrolling transition effect.
 * \param direction direction of the transition (opening or closing)
 */
TransitionScrolling::TransitionScrolling(Transition::Direction direction):
  Transition(direction),
  scrolling_direction(0),
  next_scroll_date(0),
  dx(0),
  dy(0) {

}

/**
 * \brief Returns where the previous map should be blitted on
 * both_maps_surface, for the specified scrolling direction.
 * \param scrolling_direction The scrolling direction (0 to 3).
 */
Rectangle TransitionScrolling::get_previous_map_dst_position(
    int scrolling_direction) {

  const Surface* previous_map_surface = get_previous_surface();
  Debug::check_assertion(previous_map_surface != nullptr, "Missing previous surface for scrolling");
  const Size& camera_size = previous_map_surface->get_size();

  Rectangle dst_position(0, 0);
  if (scrolling_direction == 1) {
    // Scroll to the north.
    dst_position.set_y(camera_size.height);
  }
  else if (scrolling_direction == 2) {
    // Scroll to the west.
    dst_position.set_x(camera_size.width);
  }

  return dst_position;
}

/**
 * \brief Starts this transition effect.
 */
void TransitionScrolling::start() {

  if (get_direction() == Direction::CLOSING) {
    return;
  }

  const Game* game = get_game();
  Debug::check_assertion(game != nullptr, "Missing game for scrolling transition");
  const Surface* previous_map_surface = get_previous_surface();
  Debug::check_assertion(previous_map_surface != nullptr, "Missing previous surface for scrolling");

  const Map& map = get_game()->get_current_map();

  // get the scrolling direction
  scrolling_direction = (map.get_destination_side() + 2) % 4;

  const int scrolling_step = 5;

  const Size& camera_size = previous_map_surface->get_size();
  int width = camera_size.width;
  int height = camera_size.height;
  if (scrolling_direction % 2 == 0) {
    // right or left
    width *= 2;
    dx = (scrolling_direction == 0) ? scrolling_step : -scrolling_step;
  }
  else {
    height *= 2;
    dy = (scrolling_direction == 3) ? scrolling_step : -scrolling_step;
  }

  // set the blitting rectangles

  previous_map_dst_position = get_previous_map_dst_position(scrolling_direction);
  current_map_dst_position = get_previous_map_dst_position((scrolling_direction + 2) % 4);
  current_scrolling_position = previous_map_dst_position;
  current_scrolling_position.set_size(camera_size);

  next_scroll_date = System::now();
}

/**
 * \brief Returns whether this transition effect needs the previous surface.
 * \return true
 */
bool TransitionScrolling::needs_previous_surface() const {
  return true;
}

/**
 * \brief Returns whether the transition effect is started.
 * \return false
 */
bool TransitionScrolling::is_started() const {
  return !is_finished();
}

/**
 * \brief Returns whether the transition effect is finished.
 * \return true
 */
bool TransitionScrolling::is_finished() const {

  if (get_direction() == Direction::CLOSING) {
    return true;
  }

  return current_scrolling_position.get_x() == current_map_dst_position.get_x()
    && current_scrolling_position.get_y() == current_map_dst_position.get_y();
}

/**
 * \brief Makes a scrolling step.
 */
void TransitionScrolling::scroll() {

  // Add dx,dy to current_scrolling_position, but don't exceed the final position.

  if (dx > 0) {
    current_scrolling_position.set_x(std::min(
        current_scrolling_position.get_x() + dx,
        current_map_dst_position.get_x())
    );
  }
  else {
    current_scrolling_position.set_x(std::max(
        current_scrolling_position.get_x() + dx,
        current_map_dst_position.get_x())
    );
  }

  if (dy > 0) {
    current_scrolling_position.set_y(std::min(
        current_scrolling_position.get_y() + dy,
        current_map_dst_position.get_y())
    );
  }
  else {
    current_scrolling_position.set_y(std::max(
        current_scrolling_position.get_y() + dy,
        current_map_dst_position.get_y())
    );
  }
}

/**
 * \brief Notifies the transition effect that it was just suspended
 * or resumed.
 * \param suspended true if suspended, false if resumed.
 */
void TransitionScrolling::notify_suspended(bool suspended) {

  if (!suspended) {
    next_scroll_date += System::now() - get_when_suspended();
  }
}

/**
 * \brief Updates this transition effect.
 *
 * This function is called repeatedly while the transition exists.
 */
void TransitionScrolling::update() {

  if (!is_started() || is_suspended()) {
    return;
  }

  uint32_t now = System::now();
  while (now >= next_scroll_date && !is_finished()) {
    scroll();
    next_scroll_date += 10;
  }
}

/**
 * \brief Draws the transition effect on a surface.
 * \param dst_surface the surface to draw
 */
void TransitionScrolling::draw(Surface& dst_surface, const Surface &src_surface, const DrawInfos &infos) const {

  if (get_direction() == Direction::CLOSING) {
    return;
  }

  Surface* previous_surface = get_previous_surface();
  Debug::check_assertion(previous_surface != nullptr,
      "No previous surface defined for scrolling");

  // draw the old map
  infos.proxy.draw(dst_surface,*previous_surface,
                   DrawInfos(infos,
                             Rectangle(Point(),previous_surface->get_size()),
                             previous_map_dst_position.get_xy()-current_scrolling_position.get_xy()));

  // draw the new map
  infos.proxy.draw(dst_surface,src_surface,
                   DrawInfos(infos,
                             Rectangle(Point(),src_surface.get_size()),
                             current_map_dst_position.get_xy()-current_scrolling_position.get_xy()));
}
}
