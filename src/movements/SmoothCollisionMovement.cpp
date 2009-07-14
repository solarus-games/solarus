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
#include "movements/SmoothCollisionMovement.h"
#include "ZSDX.h"
#include "Geometry.h"

/**
 * Constructor.
 */
SmoothCollisionMovement::SmoothCollisionMovement(void) {

}

/**
 * Destructor.
 */
SmoothCollisionMovement::~SmoothCollisionMovement(void) {

}

/**
 * Updates the position of the entity if it wants to move.
 * This is a redefinition of Movement::update because we have
 * to call update_x() and update_y() in the right order.
 */
void SmoothCollisionMovement::update(void) {

  if (!is_suspended()) {
    Uint32 now = SDL_GetTicks();

    bool x_move_now = get_x_move() != 0 && now >= get_next_move_date_x();
    bool y_move_now = get_y_move() != 0 && now >= get_next_move_date_y();

    // TODO should be 'while' instead of 'if', but with valgrind the game
    // loops here when iterating over the obstacle dynamic entities
    while (x_move_now || y_move_now) {

      if (x_move_now) {
	// it's time to make an x move

	if (y_move_now) {
	  // but it's also time to make a y move

	  if (get_next_move_date_x() <= get_next_move_date_y()) {
	    // x move first
	    update_x();
	    update_y();
	  }
	  else {
	    // y move first
	    update_y();
	    update_x();
	  }
	}
	else {
	  update_x();
	}
      }
      else if (y_move_now) {
	update_y();
      }

      now = SDL_GetTicks();
      x_move_now = get_x_move() != 0 && now >= get_next_move_date_x();
      y_move_now = get_y_move() != 0 && now >= get_next_move_date_y();
    }
  }
}

/**
 * Updates the x position of the entity if it wants to move
 * (i.e. if x_move != 0).
 * This is a redefinition of CollisionMovement::update_x() to
 * handle the smooth collisions.
 */
void SmoothCollisionMovement::update_x(void) {

  int x_move = get_x_move();
  int y_move = get_y_move();
  Uint32 x_delay = get_x_delay();
 
  if (x_move != 0) { // the entity wants to move on x

    // by default, next_move_date_x will be incremented by x_delay,
    // unless we modify the movement in such a way that the
    // x speed needs to be fixed
    Uint32 next_move_date_x_increment = x_delay;

    Uint32 now = SDL_GetTicks();
    if (now >= get_next_move_date_x()) { // it's time to try a move

      if (!collision_with_map(x_move, 0)) {

	translate_x(x_move); // make the move
	
	if (y_move != 0 && collision_with_map(0, y_move)) {
	  // if there is also a y move, and if this y move is illegal,
	  // we still allow the x move and we give it all the speed
	  next_move_date_x_increment = (int) (x_delay / Geometry::SQRT_2);
	}
      }
      else if (y_move == 0) {
	/* The move on x is not possible: let's try
	 * to add a move on y to make a diagonal move.
	 */

	if (!collision_with_map(x_move, 1)) {
	  translate(x_move, 1);
	  next_move_date_x_increment = (int) (x_delay * Geometry::SQRT_2); // fix the speed
	}
	else if (!collision_with_map(x_move, -1)) {
	  translate(x_move, -1);
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

	    if (!collision_with_map(x_move, i) && !collision_with_map(0, 1)) {
	      translate_y(1);
	      moved = true;
	    }
	    else if (!collision_with_map(x_move, -i) && !collision_with_map(0, -1)) {
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
 * Updates the y position of the entity if it wants to move
 * (i.e. if y_move != 0).
 * This is a redefinition of CollisionMovement::update_y() to to
 * handle the smooth collisions.
 */
void SmoothCollisionMovement::update_y(void) {

  int x_move = get_x_move();
  int y_move = get_y_move();
  Uint32 y_delay = get_y_delay();

  if (y_move != 0) { // the entity wants to move on y

    // by default, next_move_date_y will be incremented by y_delay,
    // unless we modify the movement in such a way that the
    // y speed needs to be fixed
    Uint32 next_move_date_y_increment = y_delay;

    Uint32 now = SDL_GetTicks();
    if (now >= get_next_move_date_y()) { // it's time to try a move

      if (!collision_with_map(0, y_move)) {

	translate_y(y_move); // make the move

	if (x_move != 0 && collision_with_map(x_move, 0)) {
	  // if there is also an x move, and if this x move is illegal,
	  // we still allow the y move and we give it all the speed
	  next_move_date_y_increment = (int) (y_delay / Geometry::SQRT_2);
	}
      }
      else if (x_move == 0) {
	/* The move on y is not possible: let's try
	 * to add a move on x to make a diagonal move.
	 */


	if (!collision_with_map(1, y_move)) {
	  translate(1, y_move);
	  next_move_date_y_increment = (int) (y_delay * Geometry::SQRT_2); // fix the speed
	}
	else if (!collision_with_map(-1, y_move)) {
	  translate(-1, y_move);
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

	    if (!collision_with_map(i, y_move) && !collision_with_map(1, 0)) {
	      translate_x(1);
	      moved = true;
	    }
	    else if (!collision_with_map(-i, y_move) && !collision_with_map(-1, 0)) {
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
