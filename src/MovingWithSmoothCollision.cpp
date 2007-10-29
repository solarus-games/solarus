/**
 * This module defines the class MovingWithCollision.
 */

#include <cmath>
#include "MovingWithSmoothCollision.h"
#include "ZSDX.h"

/**
 * Updates the x position of the entity if it wants to move
 * (i.e. if x_move != 0).
 * This is a redefinition of MovingWithCollision::update_x to
 * handle the smooth collisions.
 */
void MovingWithSmoothCollision::update_x(void) {

  if (x_move != 0) { // the entity wants to move on x

    while (SDL_GetTicks() > next_move_date_x) { // while it's time to try a move

      if (!collision_with_map(x_move, 0)) {
	translate_x(x_move); // make the move
      }
      else if (y_move == 0) {
	/* The move on x is not possible: let's try
	 * to add a move on y to make a diagonal move.
	 */

	if (!collision_with_map(x_move, 2)) {
	  translate(x_move, 2);
	  next_move_date_x += (int) (x_delay * SQRT_2) - x_delay; // fix the speed
	}
	else if (!collision_with_map(x_move, -2)) {
	  translate(x_move, -2);
	  next_move_date_x += (int) (x_delay * SQRT_2) - x_delay;
	}
	else {
	  /* The diagonal moves didn't work either.
	   * So we look for a place (up to 8 pixels on the left and on the right)
	   * where the required move would be allowed.
	   * If we find a such place, then we move towards this place.
	   */

	  bool moved = false;
	  for (int i = 4; i <= 8 && !moved; i += 2) {

	    if (!collision_with_map(x_move, i)) {
	      translate_y(2);
	      moved = true;
	    }
	    else if (!collision_with_map(x_move, -i)) {
	      translate_y(-2);
	      moved = true;
	    }
	  }
	}
      }
      next_move_date_x += x_delay;
     }
   }
}

/**
 * Updates the y position of the entity if it wants to move
 * (i.e. if y_move != 0).
 * This is a redefinition of MovingWithCollision::update_y to to
 * handle the smooth collisions.
 */
void MovingWithSmoothCollision::update_y(void) {

  if (y_move != 0) { // the entity wants to move on x

    while (SDL_GetTicks() > next_move_date_y) { // while it's time to try a move

      if (!collision_with_map(0, y_move)) {
	translate_y(y_move); // make the move
      }
      else if (x_move == 0) {
	/* The move on x is not possible: let's try
	 * to add a move on y to make a diagonal move.
	 */

	if (!collision_with_map(2, y_move)) {
	  translate(2, y_move);
	  next_move_date_y += (int) (y_delay * SQRT_2) - y_delay; // fix the speed
	}
	else if (!collision_with_map(-2, y_move)) {
	  translate(-2, y_move);
	  next_move_date_y += (int) (y_delay * SQRT_2) - y_delay;
	}
	else {
	  /* The diagonal moves didn't work either.
	   * So we look for a place (up to 8 pixels on the left and on the right)
	   * where the required move would be allowed.
	   * If we find a such place, then we move towards this place.
	   */

	  bool moved = false;
	  for (int i = 4; i <= 8 && !moved; i += 2) {

	    if (!collision_with_map(i, y_move)) {
	      translate_x(2);
	      moved = true;
	    }
	    else if (!collision_with_map(-i, y_move)) {
	      translate_x(-2);
	      moved = true;
	    }
	  }
	}
      }
      next_move_date_y += y_delay;
     }
   }
}
