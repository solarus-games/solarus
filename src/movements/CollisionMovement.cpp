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
#include "movements/CollisionMovement.h"
#include "Game.h"
#include "Map.h"
#include "entities/MapEntity.h"
#include "lowlevel/System.h"

/**
 * Default constructor.
 */
CollisionMovement::CollisionMovement(void):
  stop_on_obstacles(true) {

}

/**
 * Constructor specifying whether the collisions are enabled.
 * @param stop_on_obstacles true to make the movement sensible to the obstacles.
 */
CollisionMovement::CollisionMovement(bool stop_on_obstacles):
  last_collision_box_on_obstacle(-1, -1), stop_on_obstacles(stop_on_obstacles)  {

}


/**
 * Destructor.
 */
CollisionMovement::~CollisionMovement(void) {

}

/**
 * Returns whether the entity would collide with the map
 * if it was moved a few pixels from its position.
 * @param dx x distance between the current position and the position to check
 * @param dy y distance between the current position and the position to check
 * @return true if the entity would overlap the map obstacles in this position
 */
bool CollisionMovement::test_collision_with_map(int dx, int dy) {

  if (!stop_on_obstacles) {
    return false;
  }

  Map *map = entity->get_map();

  // place the collision box where we want to check the collisions
  Rectangle collision_box = entity->get_bounding_box();
  collision_box.add_xy(dx, dy);

  bool collision = map->test_collision_with_obstacles(entity->get_layer(), collision_box, entity);

  if (collision) {
    last_collision_box_on_obstacle = collision_box;
    entity->movement_just_changed();
  }

  return collision;
}

/**
 * Updates the x position of the entity if it wants to move
 * (i.e. if x_move != 0).
 * This is a redefinition of Movement::update_x to make the move
 * only if there is no collision with the map.
 */
void CollisionMovement::update_x(void) {

  uint32_t now = System::now();
  int x_move = get_x_move();
  // if was changed to while because of problems when the game is slow
  while (x_move != 0 && now >= get_next_move_date_x()) { // while it's time to try a move

    // make the move only if there is no collision
    if (!test_collision_with_map(x_move, get_y_move())) {
      translate_x(x_move);
    }
    else {
      stop(); // also stop on y
    }
    set_next_move_date_x(get_next_move_date_x() + get_x_delay());
  }
}

/**
 * Updates the y position of the entity if it wants to move
 * (i.e. if y_move != 0).
 * This is a redefinition of Movement::update_y to make the move
 * only if there is no collision with the map.
 */
void CollisionMovement::update_y(void) {

  uint32_t now = System::now();
  int y_move = get_y_move();
  while (y_move != 0 && now >= get_next_move_date_y()) { // while it's time to try a move

    // make the move only if there is no collision
    if (!test_collision_with_map(get_x_move(), y_move)) {
      translate_y(y_move);
    }
    else {
      stop(); // also stop on x
    }
    set_next_move_date_y(get_next_move_date_y() + get_y_delay());
  }
}

/**
 * Returns the collision box of the last call
 * to test_collision_with_map() returning true.
 * @return the collision box of the last collision detected, or (-1, -1) if no obstacle was detected
 */
const Rectangle & CollisionMovement::get_last_collision_box_on_obstacle(void) {
  return last_collision_box_on_obstacle;
}

/**
 * Sets whether this movement allows to traverse obstacles.
 * @param stop_on_obstacles true to make the movement sensible to obstacles, false to ignore them
 */
void CollisionMovement::set_stop_on_obstacles(bool stop_on_obstacles) {
  this->stop_on_obstacles = stop_on_obstacles;
}

