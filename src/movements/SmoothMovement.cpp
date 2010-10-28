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
#include "movements/SmoothMovement.h"
#include "lowlevel/Geometry.h"
#include "lowlevel/System.h"

/**
 * @brief Constructor.
 * @param smooth true to actually make the movement smooth
 * (false makes the movement identical to RectilinearMovement)
 */
SmoothMovement::SmoothMovement(bool smooth):
  smooth(smooth) {

}

/**
 * @brief Destructor.
 */
SmoothMovement::~SmoothMovement() {

}

/**
 * @brief Updates the x position of the entity if it wants to move
 *
 * This is a redefinition of RectilinearMovement::update_x() to
 * handle the smooth collisions.
 */
void SmoothMovement::update_x() {

  if (!smooth) {
    RectilinearMovement::update_x();
    return;
  }

  int x_move = get_x_move();
  int y_move = get_y_move();
  uint32_t x_delay = get_x_delay();
 
  if (x_move != 0) { // the entity wants to move on x

    // by default, next_move_date_x will be incremented by x_delay,
    // unless we modify the movement in such a way that the
    // x speed needs to be fixed
    uint32_t next_move_date_x_increment = x_delay;

    uint32_t now = System::now();
    if (now >= get_next_move_date_x()) { // it's time to try a move

      if (!test_collision_with_obstacles(x_move, 0)) {

	translate_x(x_move); // make the move
	
	if (y_move != 0 && test_collision_with_obstacles(0, y_move)) {
	  // if there is also a y move, and if this y move is illegal,
	  // we still allow the x move and we give it all the speed
	  next_move_date_x_increment = (int) (x_delay / Geometry::SQRT_2);
	}
      }
      else if (y_move == 0) {
	// the move on x is not possible: let's try
	// to add a move on y to make a diagonal move

	if (!test_collision_with_obstacles(x_move, 1)) {
	  translate_xy(x_move, 1);
	  next_move_date_x_increment = (int) (x_delay * Geometry::SQRT_2); // fix the speed
	}
	else if (!test_collision_with_obstacles(x_move, -1)) {
	  translate_xy(x_move, -1);
	  next_move_date_x_increment = (int) (x_delay * Geometry::SQRT_2);
	}
	else {

	  /* The diagonal moves didn't work either.
	   * So we look for a place (up to 8 pixels up and right)
	   * where the required move would be allowed.
	   * If we find a such place, then we move towards this place.
	   */

	  bool moved = false;
	  for (int i = 1; i <= 8 && !moved; i++) {

	    if (!test_collision_with_obstacles(x_move, i) && !test_collision_with_obstacles(0, 1)) {
	      translate_y(1);
	      moved = true;
	    }
	    else if (!test_collision_with_obstacles(x_move, -i) && !test_collision_with_obstacles(0, -1)) {
	      translate_y(-1);
	      moved = true;
	    }
	  }
	}
      }
      set_next_move_date_x(get_next_move_date_x() + next_move_date_x_increment);
    }
  }
}

/**
 * @brief Updates the y position of the entity if it wants to move
 *
 * This is a redefinition of Movement::update_y() to to
 * handle the smooth collisions.
 */
void SmoothMovement::update_y() {

  if (!smooth) {
    RectilinearMovement::update_y();
    return;
  }

  int x_move = get_x_move();
  int y_move = get_y_move();
  uint32_t y_delay = get_y_delay();

  if (y_move != 0) { // the entity wants to move on y

    // by default, next_move_date_y will be incremented by y_delay,
    // unless we modify the movement in such a way that the
    // y speed needs to be fixed
    uint32_t next_move_date_y_increment = y_delay;

    uint32_t now = System::now();
    if (now >= get_next_move_date_y()) { // it's time to try a move

      if (!test_collision_with_obstacles(0, y_move)) {

	translate_y(y_move); // make the move

	if (x_move != 0 && test_collision_with_obstacles(x_move, 0)) {
	  // if there is also an x move, and if this x move is illegal,
	  // we still allow the y move and we give it all the speed
	  next_move_date_y_increment = (int) (y_delay / Geometry::SQRT_2);
	}
      }
      else if (x_move == 0) {
	// The move on y is not possible: let's try
	// to add a move on x to make a diagonal move.

	if (!test_collision_with_obstacles(1, y_move)) {
	  translate_xy(1, y_move);
	  next_move_date_y_increment = (int) (y_delay * Geometry::SQRT_2); // fix the speed
	}
	else if (!test_collision_with_obstacles(-1, y_move)) {
	  translate_xy(-1, y_move);
	  next_move_date_y_increment = (int) (y_delay * Geometry::SQRT_2);
	}
	else {
	  /* The diagonal moves didn't work either.
	   * So we look for a place (up to 8 pixels on the left and on the right)
	   * where the required move would be allowed.
	   * If we find a such place, then we move towards this place.
	   */

	  bool moved = false;
	  for (int i = 1; i <= 8 && !moved; i++) {

	    if (!test_collision_with_obstacles(i, y_move) && !test_collision_with_obstacles(1, 0)) {
	      translate_x(1);
	      moved = true;
	    }
	    else if (!test_collision_with_obstacles(-i, y_move) && !test_collision_with_obstacles(-1, 0)) {
	      translate_x(-1);
	      moved = true;
	    }
	  }
	}
      }
      set_next_move_date_y(get_next_move_date_y() + next_move_date_y_increment);
    }
  }
}

