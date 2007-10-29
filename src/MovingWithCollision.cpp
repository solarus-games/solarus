/**
 * This module defines the class MovingWithCollision.
 */

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
  this->absolute_collision_box.w = collision_box.w;
  this->absolute_collision_box.h = collision_box.h;
}

/**
 * Sets the x position of the entity.
 * This is a redefinition of Moving::set_x() because we also need
 * to update the position of absolute_collision_box.
 * @param x the new x position
 */
void MovingWithCollision::set_x(int x) {
  Moving::set_x(x);
  absolute_collision_box.x = x + collision_box.x;
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
  absolute_collision_box.y = y + collision_box.y;
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
  absolute_collision_box.x += dx;
  absolute_collision_box.y += dy;

  bool collision = map->collision_with_tiles(layer, absolute_collision_box);

  // restore the collision box
  absolute_collision_box.x -= dx;
  absolute_collision_box.y -= dy;

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

    while (SDL_GetTicks() > next_move_date_x) { // while it's time to try a move

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

    while (SDL_GetTicks() > next_move_date_y) { // while it's time to try a move

      // make the move only if there is no collision
      if (!collision_with_map(0, y_move)) {
	translate_y(y_move);
      }

      next_move_date_y += y_delay;
    }
  }
}
