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
#include "entities/StreamAction.h"
#include "entities/Stream.h"
#include "lowlevel/System.h"
#include "Map.h"
#include <cmath>

namespace solarus {

/**
 * \brief Creates a stream action.
 * \param stream The stream applied.
 * \param entity_moved Entity the stream is applied to.
 */
StreamAction::StreamAction(Stream& stream, MapEntity& entity_moved):
  stream(&stream),
  entity_moved(&entity_moved),
  active(true),
  suspended(false),
  when_suspended(0),
  dx(0),
  dy(0),
  target_x(0),
  target_y(0),
  next_move_date(0) {

  RefCountable::ref(this->stream);
  RefCountable::ref(this->entity_moved);

  // Compute the direction of the movement and its target point.
  const int direction8 = stream.get_direction();
  const Rectangle& xy = MapEntity::direction_to_xy_move(direction8);
  dx = xy.get_x();
  dy = xy.get_y();

  // The stream will stop when one of target_x or target_y is reached.
  if (dx != 0) {
    target_x = stream.get_x() + (dx > 0 ? 16 : -16);
  }

  if (dy != 0) {
    target_y = stream.get_y() + (dy > 0 ? 16 : -16);
  }

  // Compute the delay between two moves.
  delay = (uint32_t) (1000 / stream.get_speed());
  if (direction8 % 2 != 0) {
    // Diagonal movement.
    delay = (uint32_t) (delay / std::sqrt(2));
  }
  next_move_date = System::now() + delay;
}

/**
 * \brief Destructor.
 */
StreamAction::~StreamAction() {

  RefCountable::unref(this->stream);
  RefCountable::unref(this->entity_moved);
}

/**
 * \brief Returns the stream responsible from this action.
 * \return The stream.
 */
const Stream& StreamAction::get_stream() const {
  return *stream;
}

/**
 * \brief Returns the entity this action is applied to.
 * \return The entity moved by the stream.
 */
const MapEntity& StreamAction::get_entity_moved() const {
  return *entity_moved;
}

/**
 * \brief Returns whether the action is currently active.
 * The action is inactive if the stream effect was completely applied,
 * or if the stream of the entity are destroyed or disabled.
 */
bool StreamAction::is_active() const {
  return active;
}

/**
 * \brief Called repeatedly by the main loop.
 * Updates the effect of the stream on the entity.
 */
void StreamAction::update() {

  // If the action is disabled, do nothing.
  if (!is_active()) {
    return;
  }

  // First check that the stream and the entity still exist
  // and are enabled.
  if (stream->is_being_removed()) {
    RefCountable::unref(stream);
    stream = NULL;
    active = false;
  }
  else if (!stream->is_enabled()) {
    active = false;
  }

  if (entity_moved->is_being_removed()) {
    RefCountable::unref(entity_moved);
    entity_moved = NULL;
    active = false;
  }
  else if (!entity_moved->is_enabled()) {
    active = false;
  }

  if (!is_active()) {
    return;
  }

  if (is_suspended()) {
    return;
  }

  // Update the position.
  while (
      System::now() >= next_move_date &&
      is_active()
  ) {
    next_move_date += delay;

    if (test_obstacles()) {
      // Collision with an obstacle: don't move the entity.

      if (!stream->get_allow_movement()) {
        // And stop the stream if it was a blocking one.
        active = false;
      }
      break;
    }

    entity_moved->set_xy(entity_moved->get_x() + dx, entity_moved->get_y() + dy);
    entity_moved->notify_position_changed();

    // See if the entity has come outside the stream,
    // in other words, if the movement is finished.
    if (dx > 0) {
      if (entity_moved->get_x() >= target_x) {
        active = false;
      }
    }
    else if (dx < 0) {
      if (entity_moved->get_x() <= target_x) {
        active = false;
      }
    }

    if (dy > 0) {
      if (entity_moved->get_y() >= target_y) {
        active = false;
      }
    }
    else if (dy < 0) {
      if (entity_moved->get_y() <= target_y) {
        active = false;
      }
    }
  }
}

/**
 * \brief Returns whether this stream action is currently suspended.
 * \return \c true if the stream action is suspended.
 */
bool StreamAction::is_suspended() const {
  return suspended;
}

/**
 * \brief Suspends or resumes this stream action.
 * \param suspended \c true to suspend, \c false to resume.
 */
void StreamAction::set_suspended(bool suspended) {

  this->suspended = suspended;
  if (suspended) {
    when_suspended = System::now();
  }
  else {
    if (when_suspended != 0) {
      next_move_date += System::now() - when_suspended;
    }
  }
}

/**
 * \brief Returns whether an obstacle blocks the next one-pixel move.
 * \return \c true if the entity cannot currently follow the stream because of
 * an obstacle.
 */
bool StreamAction::test_obstacles() {

  Rectangle collision_box = entity_moved->get_bounding_box();
  collision_box.add_xy(dx, dy);

  Map& map = entity_moved->get_map();
  return map.test_collision_with_obstacles(
      entity_moved->get_layer(),
      collision_box,
      *entity_moved
  );

}

}

