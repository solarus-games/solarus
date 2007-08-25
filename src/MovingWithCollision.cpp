/**
 * This module defines the class MovingWithCollision.
 */

using namespace std;
#include <iostream>
#include <SDL/SDL_image.h>
#include <cmath>
#include "MovingWithCollision.h"

/**
 * Sets the current map of the object.
 * @param map the map
 */
void MovingWithCollision::set_map(Map *map) {
  this->map = map;
}

/**
 * Sets the collision box of the object.
 * @param collision_box the collision box, positioned
 * from the object's upper-left corner
 */
void MovingWithCollision::set_collision_box(SDL_Rect &collision_box) {
  this->collision_box = collision_box;
}

/**
 * Updates the position (x and y) of the entity if it has moved.
 * This is a redefinition of Moving::update_position to stop
 * the movement if a collision with the map is detected.
 */
void MovingWithCollision::update_position(void) {
  // collision box from the upper-left corner of the map
  SDL_Rect absolute_collision_box;
  absolute_collision_box.w = collision_box.w;
  absolute_collision_box.h = collision_box.h;

  // update x
  if (x_move != 0) {
    absolute_collision_box.y = position_in_map.y + collision_box.y;

    while (SDL_GetTicks() > next_move_date_x) {
      absolute_collision_box.x = position_in_map.x + collision_box.x + x_move;

      // check the collisions

      next_move_date_x += x_delay;
      if (map->simple_collision(layer, absolute_collision_box)) {
	// stop the x movement
	set_x_speed(0);
	break;
      }
      else {
	// make the move
	position_in_map.x += x_move;
      }
    }
  }

  // update y
  if (y_move != 0) {
    absolute_collision_box.x = position_in_map.x + collision_box.x;
    while (SDL_GetTicks() > next_move_date_y) {
      absolute_collision_box.y = position_in_map.y + collision_box.y + y_move;

      // check the collisions

      next_move_date_y += y_delay;
      if (map->simple_collision(layer, absolute_collision_box)) {
	// stop the y movement
	set_y_speed(0);
	break;
      }
      else {
	// make the move
	position_in_map.y += y_move;
      }
    }
  }
}
