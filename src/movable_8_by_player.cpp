#include <SDL/SDL_image.h>
#include <cmath>
#include <iostream>
using namespace std;
#include "movable_8_by_player.h"

static const double SQRT_2 = M_SQRT2;

static const short right_mask = 0x0001;
static const short up_mask = 0x0002;
static const short left_mask = 0x0004;
static const short down_mask = 0x0008;

static const int directions[] = {
  -1,  // none: stop
  0,   // right
  90,  // up
  45,  // right + up
  180, // left
  -1,  // left + right: stop
  135, // left + up
  -1,  // left + right + up: stop
  270, // down
  315, // down + right
  -1,  // down + up: stop
  -1,  // down + right + up: stop
  225, // down + left
  -1,  // down + left + right: stop
  -1,  // down + left + up: stop
  -1,  // down + left + right + up: stop
};

Movable8ByPlayer::Movable8ByPlayer(int speed):
  started(false), direction_mask(0), speed(speed) {

}

int Movable8ByPlayer::get_direction(void) {
  return directions[direction_mask];
}

void Movable8ByPlayer::start_right(void) {
  direction_mask |= right_mask;
  update_movement();
}

void Movable8ByPlayer::start_up(void) {
  direction_mask |= up_mask;
  update_movement();
}

void Movable8ByPlayer::start_left(void) {
  direction_mask |= left_mask;
  update_movement();
}

void Movable8ByPlayer::start_down(void) {
  direction_mask |= down_mask;
  update_movement();
}

void Movable8ByPlayer::stop_right(void) {
  direction_mask &= ~right_mask;
  update_movement();
}

void Movable8ByPlayer::stop_up(void) {
  direction_mask &= ~up_mask;
  update_movement();
}

void Movable8ByPlayer::stop_left(void) {
  direction_mask &= ~left_mask;
  update_movement();
}

void Movable8ByPlayer::stop_down(void) {
  direction_mask &= ~down_mask;
  update_movement();
}

void Movable8ByPlayer::update_movement(void) {
  int direction = directions[direction_mask];
  int x_speed, y_speed;
  if (direction == -1) {
    stop();
    started = false;
  }
  else {
    if (!started) {
      started = true;
    }

    switch (direction) {
    case 0: // right
      x_speed = speed;
      y_speed = 0;
      break;
    case 90: // up
      x_speed = 0;
      y_speed = -speed;
      break;
    case 180: // left
      x_speed = -speed;
      y_speed = 0;
      break;
    case 270: // down
      x_speed = 0;
      y_speed = speed;
      break;
    case 45: // right up
      x_speed = (int) (speed / SQRT_2);
      y_speed = -x_speed;
      break;
    case 135: // left up
      x_speed = -(int) (speed / SQRT_2);
      y_speed = x_speed;
      break;
    case 225: // left down
      y_speed = (int) (speed / SQRT_2);
      x_speed = -y_speed;
      break;
    case 315: // right down
      x_speed = (int) (speed / SQRT_2);
      y_speed = x_speed;
      break;
    default:
      cerr << "Bad basic direction: " << direction << endl;
      break;
    }
    set_x_speed(x_speed);
    set_y_speed(y_speed);
  }
}
