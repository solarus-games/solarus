#include <SDL/SDL.h>
#include <cmath>
#include <iostream>
using namespace std;
#include "movable.h"

static const double PI = M_PI;
static const double TWO_PI = 2.0 * M_PI;
static const double PI_OVER_2 = M_PI_2;
static const double THREE_PI_OVER_2 = 3 * M_PI_2;
static const double SQRT_2 = M_SQRT2;

Movable::Movable(void):
  x_speed(0), y_speed(0), x_move(0), y_move(0) {
}

int Movable::get_x(void) {
  update_x();
  return where_in_map.x;
}

int Movable::get_y(void) {
  update_y();
  return where_in_map.y;
}

void Movable::update_x(void) {
  cout << "update_x de Movable\n";
  if (x_move != 0) {
    while (SDL_GetTicks() > next_move_date_x) {
      where_in_map.x += x_move;
      next_move_date_x += x_delay;
    }
  }
}

void Movable::update_y(void) {
  if (y_move != 0) {
    while (SDL_GetTicks() > next_move_date_y) {
      where_in_map.y += y_move;
      next_move_date_y += y_delay;
    }
  }
}

int Movable::get_speed(void) {
  return (int) sqrt(x_speed * x_speed + y_speed * y_speed);
}

void Movable::set_x_speed(int x_speed) {
  this->x_speed = x_speed;
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

void Movable::set_y_speed(int y_speed) {
  this->y_speed = y_speed;
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

// direction: 0 to 359
void Movable::set_direction(int direction) {
  int speed = get_speed();
  double angle = direction * TWO_PI / 360.0;

  set_x_speed(speed * (int) cos(angle));
  set_y_speed(speed * (int) sin(angle));
}

// change the speed keeping the same direction
void Movable::set_speed(int speed) {
  double angle;

  if (x_speed == 0) {
    angle = (y_speed > 0) ? THREE_PI_OVER_2 : PI_OVER_2;
  }
  else {
    angle = atan(y_speed / x_speed);
    if (x_speed < 0) {
      angle = angle + PI;
//       if (angle >= TWO_PI) {
// 	angle -= TWO_PI;
//       }
    }
  }

  set_x_speed(speed * (int) cos(angle));
  set_y_speed(speed * (int) sin(angle));
}
