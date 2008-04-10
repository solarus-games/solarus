/**
 * This module defines the class MovingWithCollision.
 */

#include <cmath>
#include "MovingWithCollision.h"
#include "ZSDX.h"
#include "Map.h"

/**
 * Sets the current map of the object.
 * @param map the map
 */
void MovingWithCollision::set_map(Map *map) {
  this->map = map;
}

/**
 * Sets the x position of the entity.
 * This is a redefinition of Moving::set_x() because we also need
 * to update the position of absolute_collision_box.
 * @param x the new x position
 */
void MovingWithCollision::set_x(int x) {
  Moving::set_x(x);
  map->entity_just_moved(this);
}

/**
 * Sets the y position of the entity.
 * This is a redefinition of Moving::set_y() because we also need
 * to update the position of absolute_collision_box.
 * @param y the new y position
 */
void MovingWithCollision::set_y(int y) {
  Moving::set_y(y);  
  map->entity_just_moved(this);
}

/**
 * Returns whether the entity would collide with the map
 * if it was moved a few pixels from its position.
 * @param dx x distance between the current position and the position to check
 * @param dy y distance between the current position and the position to check
 * @return true if the entity would overlap the map tiles in this position
 */
bool MovingWithCollision::collision_with_map(int dx, int dy) {

  // place the collision box where we want to check the collisions
  SDL_Rect collision_box = position_in_map;
  collision_box.x += dx;
  collision_box.y += dy;

  bool collision = map->collision_with_tiles(layer, collision_box);

  return collision;
}

/**
 * Updates the x position of the entity if it wants to move
 * (i.e. if x_move != 0).
 * This is a redefinition of Moving::update_x to make the move
 * only if there is no collision with the map.
 */
void MovingWithCollision::update_x(void) {

  if (x_move != 0) { // the entity wants to move on x

    Uint32 now = SDL_GetTicks();
    while (now >= next_move_date_x) { // while it's time to try a move

      // make the move only if there is no collision
      if (!collision_with_map(x_move, 0)) {
	translate_x(x_move);
      }

      next_move_date_x += x_delay;
    }
  }
}

/**
 * Updates the y position of the entity if it wants to move
 * (i.e. if y_move != 0).
 * This is a redefinition of Moving::update_y to make the move
 * only if there is no collision with the map.
 */
void MovingWithCollision::update_y(void) {

  if (y_move != 0) { // the entity wants to move on y

    Uint32 now = SDL_GetTicks();
    while (now >= next_move_date_y) { // while it's time to try a move

      // make the move only if there is no collision
      if (!collision_with_map(0, y_move)) {
	translate_y(y_move);
      }

      next_move_date_y += y_delay;
    }
  }
}
