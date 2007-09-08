/**
 * This module defines the class MovingWithCollision.
 */

using namespace std;
#include <iostream>
#include <SDL/SDL_image.h>
#include <cmath>
#include "MovingWithSmoothCollision.h"
#include "ZSDX.h"

/**
 * Updates the position (x and y) of the entity if it wants to move
 * (i.e. if x_move or y_move are not zero).
 * This is a redefinition of Moving::update_position to make the move
 * only if there is no collision with the map.
 */
void MovingWithSmoothCollision::update_position(void) {

  // collision rectangle of the entity from the upper-left corner of the map
  SDL_Rect absolute_collision_box;

  absolute_collision_box.w = collision_box.w;
  absolute_collision_box.h = collision_box.h;

  // update x
  if (x_move != 0) { // the entity wants to move on x

    // place the collision box where the entity wants to go
    absolute_collision_box.y = position_in_map.y + collision_box.y;
    absolute_collision_box.x = position_in_map.x + collision_box.x + x_move;

    while (SDL_GetTicks() > next_move_date_x) { // while it's time to try a move

      if (!map->simple_collision(layer, absolute_collision_box)) {
	position_in_map.x += x_move; // make the move
	absolute_collision_box.x += x_move; // prepare for next loop
      }
      else if (y_move == 0) {

	// the move on x is not possible: let's try to add a move on y
	absolute_collision_box.y += 2;

	if (!map->simple_collision(layer, absolute_collision_box)) {
	  position_in_map.x += x_move;
	  position_in_map.y += 2;
	  absolute_collision_box.x += x_move;
	  next_move_date_x += (int) (x_delay * SQRT_2) - x_delay;
	}
	else {
	  absolute_collision_box.y -= 4;
	  if (!map->simple_collision(layer, absolute_collision_box)) {
	    position_in_map.x += x_move;
	    position_in_map.y -= 2;
	    absolute_collision_box.x += x_move;
	    next_move_date_x += (int) (x_delay * SQRT_2) - x_delay;
	  }
	  else {
	    absolute_collision_box.y += 2; // abort and restore the collision box
	  }
	}
      }
      next_move_date_x += x_delay;
    }
  }

  // update y
  if (y_move != 0) { // the entity wants to move on y

    // place the collision box where the entity wants to go
    absolute_collision_box.x = position_in_map.x + collision_box.x;
    absolute_collision_box.y = position_in_map.y + collision_box.y + y_move;

    while (SDL_GetTicks() > next_move_date_y) { // while it's time to try a move

      if (!map->simple_collision(layer, absolute_collision_box)) {
	position_in_map.y += y_move; // make the move
	absolute_collision_box.y += y_move; // prepare for next loop
      }
      else if (x_move == 0) {

	// the move on y is not possible: let's try to add a move on x
	absolute_collision_box.x += 2;

	if (!map->simple_collision(layer, absolute_collision_box)) {
	  position_in_map.y += y_move;
	  position_in_map.x += 2;
	  absolute_collision_box.y += y_move;
	  next_move_date_y += (int) (y_delay * SQRT_2) - y_delay;
	}
	else {
	  absolute_collision_box.x -= 4;
	  if (!map->simple_collision(layer, absolute_collision_box)) {
	    position_in_map.y += y_move;
	    position_in_map.x -= 2;
	    absolute_collision_box.y += y_move;
	    next_move_date_y += (int) (y_delay * SQRT_2) - y_delay;
	  }
	  else {
	    absolute_collision_box.x += 2; // abort and restore the collision box
	  }
	}
      }
      next_move_date_y += y_delay;
    }
  }
}
