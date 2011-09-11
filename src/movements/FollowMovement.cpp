/*
 * Copyright (C) 2009 Christopho, Solarus - http://www.solarus-engine.org
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
#include "movements/FollowMovement.h"
#include "entities/MapEntity.h"

/**
 * @brief Creates a follow movement.
 * @param entity_followed the entity to follow
 * @param x x coordinate of where this entity should be placed (relative to the entity followed)
 * @param y y coordinate of where this entity should be placed (relative to the entity followed)
 * @param ignore_obstacles true to make the movement ignore obstacles
 */
FollowMovement::FollowMovement(MapEntity *entity_followed, int x, int y, bool ignore_obstacles):
  Movement(ignore_obstacles), entity_followed(entity_followed), x(x), y(y), finished(false) {

}

/**
 * @brief Destructor.
 */
FollowMovement::~FollowMovement() {

}

/**
 * @brief Returns whether the movement is finished.
 * @return true if there was a collision or the entity followed disappeared
 */
bool FollowMovement::is_finished() {
  return finished;
}

/**
 * @brief Updates the position.
 */
void FollowMovement::update() {

  if (entity_followed == NULL) {
    finished = true;
    return;
  }

  if (entity_followed->is_being_removed()) {
    finished = true;
    entity_followed = NULL;
  }
  else {

    int next_x = entity_followed->get_x() + x;
    int next_y = entity_followed->get_y() + y;

    int dx = next_x - get_x();
    int dy = next_y - get_y();

    if (!are_obstacles_ignored()) {

      if (!finished && (dx != 0 || dy != 0)) {

        if (!test_collision_with_obstacles(dx, dy)) {
          set_x(next_x);
          set_y(next_y);
        }
        else {
          finished = true;
        }
      }
    }
    else {
      set_x(next_x);
      set_y(next_y);
    }
  }
}

/**
 * @brief Returns the coordinates where an object controlled by this movement
 * should be displayed.
 * @return the coordinates to use to display the object controlled by this movement
 */
const Rectangle FollowMovement::get_displayed_xy() {

  if (entity_followed == NULL) {
    return get_xy();
  }

  // if the followed entity is displayed at a different position than its real position,
  // we apply the same difference when displaying this entity

  const Rectangle& followed_xy = entity_followed->get_xy();
  const Rectangle& followed_displayed_xy = entity_followed->get_displayed_xy();

  int dx = followed_displayed_xy.get_x() - followed_xy.get_x();
  int dy = followed_displayed_xy.get_y() - followed_xy.get_y();

  Rectangle displayed_xy = get_xy();
  displayed_xy.add_xy(dx, dy);
  return displayed_xy;
}

