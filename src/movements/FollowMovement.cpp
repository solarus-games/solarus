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
#include "movements/FollowMovement.h"
#include "entities/MapEntity.h"

/**
 * Creates a follow movement.
 * @param entity_followed the entity to follow
 * @param x x coordinate of where this entity should be placed (relative to the entity followed)
 * @param y y coordinate of where this entity should be placed (relative to the entity followed)
 * @param with_collisions true to make the movement sensitive to obstacles
 */
FollowMovement::FollowMovement(MapEntity *entity_followed, int x, int y, bool with_collisions):
  entity_followed(entity_followed), x(x), y(y),
  with_collisions(with_collisions), finished(false) {

}

/**
 * Destructor.
 */
FollowMovement::~FollowMovement(void) {

}

/**
 * Returns whether the movement is finished.
 * @return true if there was a collision or the entity followed disappeared
 */
bool FollowMovement::is_finished(void) {
  return finished;
}


/**
 * Updates the position.
 */
void FollowMovement::update(void) {

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

    if (with_collisions) {

      if (!finished && (dx != 0 || dy != 0)) {

	if (!test_collision_with_map(dx, dy)) {
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

