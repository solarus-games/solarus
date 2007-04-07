#include <SDL/SDL_image.h>
#include <cmath>
#include <iostream>
using namespace std;
#include "movable_with_collision.h"

MovableWithCollision::MovableWithCollision() {
}

void MovableWithCollision::set_map(Map *map) {
  this->map = map;
}

void MovableWithCollision::set_collision_box(SDL_Rect &collision_box) {
  this->collision_box = collision_box;
}

/**
 * Redefinition of update_position from Movable
 * to detect the collisions. If there is a collision, the
 * movement is stopped.
 */
void MovableWithCollision::update_position(void) {
  SDL_Rect absolute_collision_box;
  absolute_collision_box.w = collision_box.w;
  absolute_collision_box.h = collision_box.h;

  // update x
  if (x_move != 0) {
    absolute_collision_box.y = where_in_map.y + collision_box.y;
    while (SDL_GetTicks() > next_move_date_x) {

      // check the collisions
      absolute_collision_box.x = where_in_map.x + collision_box.x + x_move;
      if (map->simple_collision(absolute_collision_box)) {
	// stop the x movement
	set_x_speed(0);
      }
      else {
	where_in_map.x += x_move;
	next_move_date_x += x_delay;
      }
    }
  }

  // update y
  if (y_move != 0) {
    absolute_collision_box.x = where_in_map.x + collision_box.x;
    while (SDL_GetTicks() > next_move_date_y) {

      // check the collisions
      absolute_collision_box.y = where_in_map.y + collision_box.y + y_move;
      if (map->simple_collision(absolute_collision_box)) {
	// stop the y movement
	set_y_speed(0);
      }
      else {
	where_in_map.y += y_move;
	next_move_date_y += y_delay;
      }
    }
  }
}
