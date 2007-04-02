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
 * Redefinition of update_x from Movable
 * to detect the collisions.
 */
void MovableWithCollision::update_x(void) {
  bool collision = false;
  SDL_Rect absolute_collision_box;
  absolute_collision_box.w = collision_box.w;
  absolute_collision_box.h = collision_box.h;
  absolute_collision_box.y = where_in_map.y + collision_box.y;

  if (x_move != 0) {
    while (SDL_GetTicks() > next_move_date_x && !collision) {

      /* check the collisions */
      absolute_collision_box.x = where_in_map.x + collision_box.x + x_move;
      if (map->collides_with(absolute_collision_box)) {
	/* cancel and stop the movement */
	stop();
	collision = true;
      }
      else {
	where_in_map.x += x_move;
	next_move_date_x += x_delay;
      }
    }
  }
}

void MovableWithCollision::update_y(void) {
  bool collision = false;
  SDL_Rect absolute_collision_box;
  absolute_collision_box.w = collision_box.w;
  absolute_collision_box.h = collision_box.h;
  absolute_collision_box.x = where_in_map.x + collision_box.x;

  if (y_move != 0) {
    while (SDL_GetTicks() > next_move_date_y && !collision) {

      /* check the collisions */
      absolute_collision_box.y = where_in_map.y + collision_box.y + y_move;
      if (map->collides_with(absolute_collision_box)) {
	/* cancel and stop the movement */
	stop();
	collision = true;
      }
      else {
	where_in_map.y += y_move;
	next_move_date_y += y_delay;
      }
    }
  }
}
