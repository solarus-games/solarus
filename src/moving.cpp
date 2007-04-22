/**
 * This module defines the class Moving.
 */

using namespace std;
#include <iostream>
#include <SDL/SDL.h>
#include <cmath>
#include "moving.h"

/**
 * Mathematic constants.
 * We don't use the constants from math.h
 * because they are not ANSI.
 */
const double PI = 3.14159265358979323846;
const double TWO_PI = 2.0 * PI;
const double PI_OVER_2 = PI / 2.0;
const double THREE_PI_OVER_2 = 3 * PI_OVER_2;
const double SQRT_2 = 1.41421356237309504880;

/**
 * Constructor.
 */
Moving::Moving(void):
  x_speed(0), y_speed(0), x_move(0), y_move(0) {
  
}

/**
 * Updates the position (x and y) of the sprite if it has changed.
 */
void Moving::update_position(void) {
  update_x();
  update_y();
}

/**
 * Updates the x position of the entity if it has changed.
 */
void Moving::update_x(void) {
  if (x_move != 0) { // if there is a move on x

    // update the x position while next_move_date_x is past
    while (SDL_GetTicks() > next_move_date_x) {
      position_in_map.x += x_move;
      next_move_date_x += x_delay;
    }
  }
}

/**
 * Updates the y position of the entity if it has changed.
 */
void Moving::update_y(void) {
  if (y_move != 0) { // if there is a move on y

    // update the x position while next_move_date_y is past
    while (SDL_GetTicks() > next_move_date_y) {
      position_in_map.y += y_move;
      next_move_date_y += y_delay;
    }
  }
}

/**
 * Returns the total speed of the entity:
 * sqrt(x_speed^2 + y_speed^2)
 */
int Moving::get_speed(void) {
  return (int) sqrt(x_speed * x_speed + y_speed * y_speed);
}

/**
 * Sets the x speed.
 * @param x_speed the x speed of the entity, between -100 and 100
 */
void Moving::set_x_speed(int x_speed) {
  this->x_speed = x_speed;

  // compute x_delay, x_move and next_move_date_x
  if (x_speed > 0) {
    x_delay = 200 / x_speed;
    x_move = 2;
    next_move_date_x = SDL_GetTicks() + x_delay;
  }
  else if (x_speed < 0) {
    x_delay = 200 / (-x_speed);
    x_move = -2;
    next_move_date_x = SDL_GetTicks() + x_delay;
  }
  else {
    x_move = 0;
  }
}

/**
 * Sets the y speed.
 * @param y_speed the y speed of the entity, between -100 and 100
 */
void Moving::set_y_speed(int y_speed) {
  this->y_speed = y_speed;

  // compute y_delay, y_move and next_move_date_y
  if (y_speed > 0) {
    y_delay = 200 / y_speed;
    y_move = 2;
    next_move_date_y = SDL_GetTicks() + y_delay;
  }
  else if (y_speed < 0) {
    y_delay = 200 / (-y_speed);
    y_move = -2;
    next_move_date_y = SDL_GetTicks() + y_delay;
  }
  else {
    y_move = 0;
  }
}

/**
 * Changes the direction of the movement, keeping the same speed.
 * x_speed and y_speed are recomputed so that the total speed is unchanged.
 * Warning: if x_speed and y_speed are both equal to zero, this function has no effect.
 * @param direction the new movement direction, between 0 and 359
 */
void Moving::set_direction(int direction) {
  int speed = get_speed();
  double angle = direction * TWO_PI / 360.0; // angle in radians

  set_x_speed((int) (speed * cos(angle)));
  set_y_speed((int) (speed * sin(angle)));
}

/**
 * Changes the speed, keeping the same direction of the movement.
 * x_speed and y_speed are recomputed so that the movement direction is unchanged.
 * Warning: if x_speed and y_speed are both equal to zero, there is no
 * direction so you're getting into trouble.
 * @param speed the new speed
 */
void Moving::set_speed(int speed) {
  double angle; // angle in radians

  // compute the angle
  if (x_speed == 0) {
    angle = (y_speed > 0) ? THREE_PI_OVER_2 : PI_OVER_2;
  }
  else {
    angle = atan(y_speed / x_speed);
    if (x_speed < 0) {
      angle = angle + PI;
    }
  }

  set_x_speed((int) (speed * cos(angle)));
  set_y_speed((int) (speed * sin(angle)));
}
