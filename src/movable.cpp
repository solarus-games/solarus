#include <SDL/SDL.h>
#include <cmath>
#include <iostream>
using namespace std;
#include "movable.h"
#include "sdl_user_events.h"

static const double PI = M_PI;
static const double TWO_PI = 2.0 * M_PI;
static const double PI_OVER_2 = M_PI_2;
static const double THREE_PI_OVER_2 = 3 * M_PI_2;
static const double SQRT_2 = M_SQRT2;

//static inline int abs(int n) { return (n >= 0) ? n : -n; }

static Uint32 move_x(Uint32 interval, Movable *movable);
static Uint32 move_y(Uint32 interval, Movable *movable);

static SDL_Event event_sprite_move;

Movable::Movable(void):
  x_speed(0), y_speed(0) {
}

void Movable::init(void) {
  event_sprite_move.type = SDL_USEREVENT;
  event_sprite_move.user.code = EVENT_SPRITE_MOVE;
  event_sprite_move.user.data1 = NULL;
  event_sprite_move.user.data2 = NULL;
}

int Movable::get_speed(void) {
  return (int) sqrt(x_speed * x_speed + y_speed * y_speed);
}

void Movable::set_x_speed(int x_speed) {
  if (this->x_speed != 0) {
    SDL_RemoveTimer(x_timer_id);
  }

  if (x_speed != 0) {
//     cout << "create a timer: x_speed = " << x_speed << ", abs(x_speed) = " << abs(x_speed) << ", interval: " << (2000/abs(x_speed)) << endl
      ;
    x_timer_id = SDL_AddTimer(200 / abs(x_speed), (Uint32 (*)(Uint32, void*)) move_x, this);
  }

  this->x_speed = x_speed;
}

void Movable::set_y_speed(int y_speed) {
  if (this->y_speed != 0) {
    SDL_RemoveTimer(y_timer_id);
  }

  if (y_speed != 0) {
//     cout << "create a timer: y_speed = " << y_speed << ", abs(y_speed) = " << abs(y_speed) << ", interval: " << (2000/abs(y_speed)) << endl
      ;
    y_timer_id = SDL_AddTimer(200 / abs(y_speed), (Uint32 (*)(Uint32, void*)) move_y, this);
  }

  this->y_speed = y_speed;
}

// direction: 0 to 359
void Movable::set_direction(int direction) {
  int speed = get_speed();
  double angle = direction * TWO_PI / 360.0;

  set_x_speed(speed * (int) cos(angle));
  set_y_speed(speed * (int) sin(angle));
}

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

static Uint32 move_x(Uint32 interval, Movable *movable) {
  if (movable->get_x_speed() > 0) {
    movable->set_x(movable->get_x() + 2);
  }
  else {
    movable->set_x(movable->get_x() - 2);
  }
  SDL_PushEvent(&event_sprite_move);
//   cout << "x interval: " << interval << endl;
  return interval;
}

static Uint32 move_y(Uint32 interval, Movable *movable) {
  if (movable->get_y_speed() > 0) {
    movable->set_y(movable->get_y() + 2);
  }
  else {
    movable->set_y(movable->get_y() - 2);
  }
  SDL_PushEvent(&event_sprite_move);
//   cout << "y interval: " << interval << endl;
  return interval;
}
