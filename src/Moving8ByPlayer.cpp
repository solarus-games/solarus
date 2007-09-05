/**
 * This module defines the class Moving8ByPlayer.
 */

using namespace std;
#include <iostream>
#include <SDL/SDL_image.h>
#include <cmath>
#include "Moving8ByPlayer.h"

/**
 * Bit masks associated to each arrow on the keyboard.
 * A combination of arrows is stored in a simple integer.
 */
static const Uint16 right_mask = 0x0001;
static const Uint16 up_mask = 0x0002;
static const Uint16 left_mask = 0x0004;
static const Uint16 down_mask = 0x0008;

/**
 * Associates to each combination of keyboard arrows
 * a movement direction: 0 to 359, or -1 to indicate
 * that the movement is stopped.
 *
 * For example:
 *   Uint16 arrows_pressed = right_mask | up_mask;
 *   int angle = directions[arrows_pressed];
 * Here the angle is 45°.
 */
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

/**
 * Constructor.
 * @param speed movement speed
 */
Moving8ByPlayer::Moving8ByPlayer(int speed):
  started(false), direction_mask(0), speed(speed) {

}

/**
 * Returns the current direction of the movement.
 * The returned direction is an angle (0 to 359), or -1 if the
 * movement is stopped.
 */
int Moving8ByPlayer::get_direction(void) {
  return directions[direction_mask];
}

/**
 * Function called when the user presses the right arrow
 */
void Moving8ByPlayer::start_right(void) {
  direction_mask |= right_mask;
  update_movement();
}

void Moving8ByPlayer::start_up(void) {
  direction_mask |= up_mask;
  update_movement();
}

void Moving8ByPlayer::start_left(void) {
  direction_mask |= left_mask;
  update_movement();
}

void Moving8ByPlayer::start_down(void) {
  direction_mask |= down_mask;
  update_movement();
}

void Moving8ByPlayer::stop_right(void) {
  direction_mask &= ~right_mask;
  x_move = 0;
  update_movement();
}

void Moving8ByPlayer::stop_up(void) {
  direction_mask &= ~up_mask;
  y_move = 0;
  update_movement();
}

void Moving8ByPlayer::stop_left(void) {
  direction_mask &= ~left_mask;
  x_move = 0;
  update_movement();
}

void Moving8ByPlayer::stop_down(void) {
  direction_mask &= ~down_mask;
  y_move = 0;
  update_movement();
}

/**
 * Updates the movement of the entity.
 * This function is called when an arrow is pressed or released
 * on the keyboard.
 */
void Moving8ByPlayer::update_movement(void) {
  int x_speed = 0;
  int y_speed = 0;

  // direction in degrees specified by the user (or -1)
  int direction = directions[direction_mask];

  if (direction == -1) {
    stop();
    started = false;
  }
  else {
    if (!started) {
      started = true;
    }

    // we could call Moving::set_direction(direction) but with the 8
    // basic directions, we don't need to make complex computations

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

/**
 * Updates the position (x and y) of the entity if it has moved.
 * This is a redefinition of MovingWithCollision::update_position to update
 * x_move and y_move depending on the arrows pressed.
 */
void Moving8ByPlayer::update_position(void) {
  
  MovingWithCollision::update_position();
}
