/**
 * This module defines the class MovingWithCollision.
 */

using namespace std;
#include <iostream>
#include <SDL/SDL_image.h>
#include <cmath>
#include "MovingWithCollision.h"
#include "ZSDX.h"

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
 * Updates the position (x and y) of the entity if it wants to move
 * (i.e. if x_move or y_move are not zero).
 * This is a redefinition of Moving::update_position to make the move
 * only if there is no collision with the map.
 */
void MovingWithCollision::update_position(void) {

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

      // make the move only if there is no collision
      if (!map->simple_collision(layer, absolute_collision_box)) {
	position_in_map.x += x_move;
	absolute_collision_box.x += x_move;
      }

      next_move_date_x += x_delay;
    }
  }

  // update y
  if (y_move != 0) {

    absolute_collision_box.x = position_in_map.x + collision_box.x;
    absolute_collision_box.y = position_in_map.y + collision_box.y + y_move;

    while (SDL_GetTicks() > next_move_date_y) { // while it's time to try a move

      // make the move only if there is no collision
      if (!map->simple_collision(layer, absolute_collision_box)) {
	position_in_map.y += y_move;
	absolute_collision_box.y += y_move;
      }

      next_move_date_y += y_delay;
    }
  }
}
