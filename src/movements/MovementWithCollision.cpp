#include "movements/MovementWithCollision.h"
#include "ZSDX.h"
#include "Map.h"
#include "entities/MapEntity.h"

/**
 * Constructor.
 */
MovementWithCollision::MovementWithCollision(void):
  map(NULL) {

}

/**
 * Destructor.
 */
MovementWithCollision::~MovementWithCollision(void) {

}

/**
 * Sets the current map of the object.
 * @param map the map
 */
void MovementWithCollision::set_map(Map *map) {
  this->map = map;
}

/**
 * Sets the x position of the entity.
 * This is a redefinition of Movement::set_x() because we also need
 * to update the position of absolute_collision_box.
 * @param x the new x position
 */
void MovementWithCollision::set_x(int x) {
  Movement::set_x(x);
  entity->just_moved();
}

/**
 * Sets the y position of the entity.
 * This is a redefinition of Movement::set_y() because we also need
 * to update the position of absolute_collision_box.
 * @param y the new y position
 */
void MovementWithCollision::set_y(int y) {
  Movement::set_y(y);  
  entity->just_moved();
}

/**
 * Returns whether the entity would collide with the map
 * if it was moved a few pixels from its position.
 * @param dx x distance between the current position and the position to check
 * @param dy y distance between the current position and the position to check
 * @return true if the entity would overlap the map tiles in this position
 */
bool MovementWithCollision::collision_with_map(int dx, int dy) {

  // place the collision box where we want to check the collisions
  SDL_Rect collision_box = *entity->get_position_in_map();
  collision_box.x += dx;
  collision_box.y += dy;

  bool collision = map->collision_with_obstacles(entity->get_layer(), collision_box);

  return collision;
}

/**
 * Updates the x position of the entity if it wants to move
 * (i.e. if x_move != 0).
 * This is a redefinition of Movement::update_x to make the move
 * only if there is no collision with the map.
 */
void MovementWithCollision::update_x(void) {

  if (x_move != 0) { // the entity wants to move on x

    Uint32 now = SDL_GetTicks();
    while (now >= next_move_date_x) { // while it's time to try a move

      // make the move only if there is no collision
      if (!collision_with_map(x_move, 0)) {
	translate_x(x_move);
      }
      else {
	stop(); // also stop on y
      }
      next_move_date_x += x_delay;
    }
  }
}

/**
 * Updates the y position of the entity if it wants to move
 * (i.e. if y_move != 0).
 * This is a redefinition of Movement::update_y to make the move
 * only if there is no collision with the map.
 */
void MovementWithCollision::update_y(void) {

  if (y_move != 0) { // the entity wants to move on y

    Uint32 now = SDL_GetTicks();
    while (now >= next_move_date_y) { // while it's time to try a move

      // make the move only if there is no collision
      if (!collision_with_map(0, y_move)) {
	translate_y(y_move);
      }
      else {
	stop(); // also stop on x
      }
      next_move_date_y += y_delay;
    }
  }
}
