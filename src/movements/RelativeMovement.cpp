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
#include "solarus/entities/Entity.h"
#include "solarus/movements/RelativeMovement.h"

namespace Solarus {

/**
 * \brief Creates a follow movement.
 * \param entity_followed the entity to follow
 * \param x x coordinate of where this entity should be placed (relative to the entity followed)
 * \param y y coordinate of where this entity should be placed (relative to the entity followed)
 * \param ignore_obstacles true to make the movement ignore obstacles
 */
RelativeMovement::RelativeMovement(
    const EntityPtr& entity_followed,
    int x,
    int y,
    bool ignore_obstacles):
  RelativeMovement(entity_followed, Point(x,y), ignore_obstacles) {
}

/**
 * \brief Creates a relative movement.
 * \param entity_followed the entity to follow
 * \param entity_offset coordinate of where this entity should be placed (relative to the entity followed)
 * \param ignore_obstacles true to make the movement ignore obstacles
 */
RelativeMovement::RelativeMovement(
    const EntityPtr& entity_followed,
    const Point& entity_offset,
    bool ignore_obstacles):
  Movement(ignore_obstacles),
  entity_followed(entity_followed),
  entity_offset(entity_offset),
  finished(false) {
}

/**
 * \brief Returns whether the movement is finished.
 * \return true if there was a collision or the entity followed disappeared
 */
bool RelativeMovement::is_finished() const {
  return finished;
}

/**
 * \brief Updates the position.
 */
void RelativeMovement::update() {

  if (entity_followed == nullptr) {
    finished = true;
    return;
  }

  if (entity_followed->is_being_removed()) {
    finished = true;
    entity_followed = nullptr;
  }
  else {

    Point next = entity_followed->get_xy() + entity_offset;
    Point dnext = next - get_xy();

    if (!are_obstacles_ignored()) {

      if (!finished && (dnext.x != 0 || dnext.y != 0)) {

        if (!test_collision_with_obstacles(dnext)) {
          set_xy(next);
        }
        else {
          finished = true;
          notify_obstacle_reached();
        }
      }
    }
    else {
      set_xy(next);
    }
  }
}

/**
 * \brief Returns the coordinates where an object controlled by this movement
 * should be displayed.
 * \return the coordinates to use to display the object controlled by this movement
 */
Point RelativeMovement::get_displayed_xy() const {

  if (entity_followed == nullptr) {
    return get_xy();
  }

  // if the followed entity is displayed at a different position than its real position,
  // we apply the same difference when displaying this entity

  const Point& followed_xy = entity_followed->get_xy();
  const Point& followed_displayed_xy = entity_followed->get_displayed_xy();

  Point dxy = followed_displayed_xy - followed_xy;

  return get_xy() + dxy;
}

}

