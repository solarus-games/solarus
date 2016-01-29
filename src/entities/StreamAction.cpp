/*
 * Copyright (C) 2006-2016 Christopho, Solarus - http://www.solarus-games.org
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
#include "solarus/entities/StreamAction.h"
#include "solarus/entities/Stream.h"
#include "solarus/lowlevel/System.h"
#include "solarus/movements/Movement.h"
#include "solarus/Map.h"
#include <cmath>

namespace Solarus {

/**
 * \brief Creates a stream action.
 * \param stream The stream applied.
 * \param entity_moved Entity the stream is applied to.
 */
StreamAction::StreamAction(Stream& stream, Entity& entity_moved):
  stream(std::static_pointer_cast<Stream>(stream.shared_from_this())),
  entity_moved(std::static_pointer_cast<Entity>(entity_moved.shared_from_this())),
  active(true),
  suspended(false),
  when_suspended(0),
  target(0, 0),
  next_move_date(0),
  delay(0) {

  recompute_movement();

  next_move_date = System::now() + delay;
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
const Entity& StreamAction::get_entity_moved() const {
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
 * \brief Updates the direction of the movement to the target.
 *
 * This function should be called periodically.
 */
void StreamAction::recompute_movement() {

  if (!is_active()) {
    return;
  }

  // Compute the direction of the movement and its target point.
  const int direction8 = stream->get_direction();
  const Point& xy = Entity::direction_to_xy_move(direction8);
  const int dx = xy.x;
  const int dy = xy.y;
  delay = (uint32_t) (1000 / stream->get_speed());

  target = stream->get_xy();

  // Stop 16 pixels after the stream.
  if (dx != 0) {
    // Horizontal stream.
    target.x = stream->get_x() + (dx > 0 ? 16 : -16);
  }

  if (dy != 0) {
    // Vertical stream.
    target.y = stream->get_y() + (dy > 0 ? 16 : -16);
  }

  if (target != entity_moved->get_xy()) {
    // Adjust the speed to the diagonal movement.
    delay = (uint32_t) (delay * std::sqrt(2));
  }
}

/**
 * \brief Called repeatedly by the main loop.
 * Updates the effect of the stream on the entity.
 */
void StreamAction::update() {

  // If the action is already disabled, do nothing.
  if (!is_active()) {
    return;
  }

  // First check that the stream and the entity still exist
  // and are enabled.
  if (stream->is_being_removed()) {
    stream = nullptr;
    active = false;
    return;
  }

  if (!stream->is_enabled()) {
    active = false;
    return;
  }

  if (entity_moved->is_being_removed()) {
    entity_moved = nullptr;
    active = false;
    return;
  }

  if (!entity_moved->is_enabled()) {
    active = false;
    return;
  }

  // Stop the stream action if the hero escapes a non-blocking stream.
  const Point& ground_point = entity_moved->get_ground_point();
  if (
      stream->get_allow_movement() &&
      !stream->overlaps(ground_point)  // We are no longer on the stream.
  ) {
    // Blocking streams are more special.
    // The hero cannot escape them so we don't need this.
    // Also, diagonal blocking streams continue
    // to move the entity even when it does not overlap anymore.
    // This is needed to have precise
    // exact diagonal movements of 16 pixels in stream mazes.

    if (entity_moved->get_distance(target) > 8) {
      // This last test is to avoid stopping a stream when being close to the target.
      // Indeed, in the last pixels before the target, the entity's ground
      // point is no longer on the stream. We continue anyway until the target.
      active = false;
      return;
    }
  }

  if (is_suspended()) {
    return;
  }

  // Update the position.
  recompute_movement();
  while (
      System::now() >= next_move_date &&
      is_active()
  ) {
    next_move_date += delay;

    int dx = 0;
    int dy = 0;
    if (target.x > entity_moved->get_x()) {
      dx = 1;
    }
    else if (target.x < entity_moved->get_x()) {
      dx = -1;
    }
    if (target.y > entity_moved->get_y()) {
      dy = 1;
    }
    else if (target.y < entity_moved->get_y()) {
      dy = -1;
    }

    if (test_obstacles(dx, dy)) {
      bool collision = true;
      // Collision with an obstacle: try only X or only Y.
      if (dx != 0 && dy != 0) {
        if (!test_obstacles(dx, 0)) {
          dy = 0;
          collision = false;
        }
        else if (!test_obstacles(0, dy)) {
          dx = 0;
          collision = false;
        }
      }

      if (collision) {
        if (!stream->get_allow_movement()) {
          // Stop the stream if it was a blocking one.
          active = false;
        }
        break;
      }
    }

    entity_moved->set_xy(entity_moved->get_x() + dx, entity_moved->get_y() + dy);
    entity_moved->notify_position_changed();

    // See if the entity has come outside the stream,
    // in other words, if the movement is finished.
    if (has_reached_target()) {
      // The target point is reached: stop the stream.
      active = false;
    }
    recompute_movement();
  }
}

/**
 * \brief Returns whether the entity moved has finished to follow the stream.
 * \return \c true if the target point is reached.
 */
bool StreamAction::has_reached_target() const {

  return entity_moved->get_xy() == target;
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
 * \brief Returns whether an obstacle blocks a candidate move.
 * \param dx X component of the translation to test.
 * \param dy Y component of the translation to test.
 * \return \c true if the entity cannot currently move of dx,dy because of an
 * obstacle.
 */
bool StreamAction::test_obstacles(int dx, int dy) {

  if (entity_moved->get_movement() != nullptr &&
      entity_moved->get_movement()->are_obstacles_ignored()) {
    // This entity currently ignores obstacles.
    return false;
  }

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

