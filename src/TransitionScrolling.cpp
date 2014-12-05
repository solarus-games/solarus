/*
 * Copyright (C) 2006-2014 Christopho, Solarus - http://www.solarus-games.org
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
#include "solarus/TransitionScrolling.h"
#include "solarus/Game.h"
#include "solarus/Map.h"
#include "solarus/lowlevel/Color.h"
#include "solarus/lowlevel/System.h"
#include "solarus/lowlevel/Surface.h"
#include "solarus/lowlevel/Video.h"
#include "solarus/lowlevel/Debug.h"

namespace Solarus {

/**
 * \brief Creates a scrolling transition effect.
 * \param direction direction of the transition (opening or closing)
 */
TransitionScrolling::TransitionScrolling(Transition::Direction direction):
  Transition(direction),
  both_maps_surface(nullptr),
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

  const Size& quest_size = Video::get_quest_size();

  Rectangle dst_position(0, 0);
  if (scrolling_direction == 1) {
    // Scroll to the north.
    dst_position.set_y(quest_size.height);
  }
  else if (scrolling_direction == 2) {
    // Scroll to the west.
    dst_position.set_x(quest_size.width);
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

  // get the scrolling direction
  scrolling_direction = (get_game()->get_current_map().get_destination_side() + 2) % 4;

  const int scrolling_step = 5;
  const Size& quest_size = Video::get_quest_size();
  int width = quest_size.width;
  int height = quest_size.height;
  if (scrolling_direction % 2 == 0) {
    // right or left
    width *= 2;
    dx = (scrolling_direction == 0) ? scrolling_step : -scrolling_step;
  }
  else {
    height *= 2;
    dy = (scrolling_direction == 3) ? scrolling_step : -scrolling_step;
  }

  // create a surface with the two maps
  both_maps_surface = Surface::create(width, height);
  both_maps_surface->set_software_destination(false);

  // set the blitting rectangles

  previous_map_dst_position = get_previous_map_dst_position(scrolling_direction);
  current_map_dst_position = get_previous_map_dst_position((scrolling_direction + 2) % 4);
  current_scrolling_position = previous_map_dst_position;
  current_scrolling_position.set_size(Video::get_quest_size());

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
  current_scrolling_position.add_xy(dx, dy);
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
void TransitionScrolling::draw(Surface& dst_surface) {

  if (get_direction() == Direction::CLOSING) {
    return;
  }

  Surface* previous_surface = get_previous_surface();
  Debug::check_assertion(previous_surface != nullptr,
      "No previous surface defined for scrolling");

  // draw the old map
  previous_surface->draw(both_maps_surface, previous_map_dst_position.get_xy());

  // draw the new map
  dst_surface.draw(both_maps_surface, current_map_dst_position.get_xy());

  // blit both surfaces
  both_maps_surface->draw_region(
      current_scrolling_position,
      std::static_pointer_cast<Surface>(dst_surface.shared_from_this())
  );
}

}

