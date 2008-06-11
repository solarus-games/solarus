#include <cmath>
#include "Movement8ByPlayer.h"
#include "MapEntity.h"

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
Movement8ByPlayer::Movement8ByPlayer(int speed):
  started(false), direction_mask(0), can_move(false), speed(speed) {

}

/**
 * Destructor.
 */
Movement8ByPlayer::~Movement8ByPlayer(void) {

}

/**
 * Returns the current direction of the movement.
 * The returned direction is an angle (0 to 359), or -1 if the
 * movement is stopped.
 * @returns the current movement direction
 */
int Movement8ByPlayer::get_direction(void) {
  return directions[direction_mask];
}

/**
 * Returns whether the entity is moving.
 * @return true if the entity is moving
 */
bool Movement8ByPlayer::is_started(void) {
  return started;
}

/**
 * Returns whether the player can move the entity.
 * @param true if the movements are enabled
 */
bool Movement8ByPlayer::is_moving_enabled(void) {
  return can_move;
}

/**
 * Sets whether the player can move the entity.
 * @param can_move true to enable the movements
 */
void Movement8ByPlayer::set_moving_enabled(bool can_move) {

  if (can_move != this->can_move) {

    if (can_move) {
      // if the control has just been restored, let's take
      // into account the possible arrows pressed

      /*
      stop_right();
      stop_up();
      stop_left();
      stop_down();
      */

      Uint8 *key_state = SDL_GetKeyState(NULL);

      if (key_state[SDLK_RIGHT]) {
	add_direction_mask(right_mask);
      }
      if (key_state[SDLK_UP]) {
	add_direction_mask(up_mask);
      }
      if (key_state[SDLK_LEFT]) {
	add_direction_mask(left_mask);
      }
      if (key_state[SDLK_DOWN]) {
	add_direction_mask(down_mask);
      }
    }
    else {
      set_direction_mask(0x0000);
      x_move = 0;
      y_move = 0;
    }

    this->can_move = can_move;
    compute_movement();
  }
}

/**
 * Function called when the user presses the right arrow.
 */
void Movement8ByPlayer::start_right(void) {
  if (can_move) {
    add_direction_mask(right_mask);
    compute_movement();
  }
}

void Movement8ByPlayer::start_up(void) {
  if (can_move) {
    add_direction_mask(up_mask);
    compute_movement();
  }
}

void Movement8ByPlayer::start_left(void) {
  if (can_move) {
    add_direction_mask(left_mask);
    compute_movement();
  }
}

void Movement8ByPlayer::start_down(void) {
  if (can_move) {
    add_direction_mask(down_mask);
    compute_movement();
  }
}

void Movement8ByPlayer::stop_right(void) {
  if (can_move) {
    remove_direction_mask(right_mask);
    x_move = 0;
    compute_movement();
  }
}

void Movement8ByPlayer::stop_up(void) {
  if (can_move) {
    remove_direction_mask(up_mask);
    y_move = 0;
    compute_movement();
  }
}

void Movement8ByPlayer::stop_left(void) {
  if (can_move) {
    remove_direction_mask(left_mask);
    x_move = 0;
    compute_movement();
  }
}

void Movement8ByPlayer::stop_down(void) {
  if (can_move) {
    remove_direction_mask(down_mask);
    y_move = 0;
    compute_movement();
  }
}

/**
 * Returns the direction mask.
 * The direction mask represents what arrow keys are currently
 * pressed by the player.
 * @return the direction mask
 */
Uint16 Movement8ByPlayer::get_direction_mask(void) {
  return direction_mask;
}

/**
 * Adds one of the four directions to the direction mask.
 * The direction mask represents what arrow keys are currently
 * pressed by the player.
 */
void Movement8ByPlayer::add_direction_mask(Uint16 direction_mask) {
  set_direction_mask(this->direction_mask | direction_mask);
}

/**
 * Removes one of the four directions to the direction mask.
 * The direction mask represents what arrow keys are currently
 * pressed by the player.
 */
void Movement8ByPlayer::remove_direction_mask(Uint16 direction_mask) {
  set_direction_mask(this->direction_mask & ~direction_mask);
}

/**
 * Sets the direction mask.
 * The direction mask represents what arrow keys are currently
 * pressed by the player.
 */
void Movement8ByPlayer::set_direction_mask(Uint16 direction_mask) {
  if (direction_mask != this->direction_mask) {
    this->direction_mask = direction_mask;
  }
}

/**
 * Changes the movement of the entity depending on the arrows pressed.
 * This function is called when an arrow is pressed or released
 * on the keyboard, or when the movement has just been enabled or
 * disabled (i.e. when set_moving_enabled() is called).
 */
void Movement8ByPlayer::compute_movement(void) {
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

    // we could call Movement::set_direction(direction) but with the 8
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
      DIE("Bad basic direction: " << direction);
      break;
    }
    set_x_speed(x_speed);
    set_y_speed(y_speed);
  }

  entity->movement_just_changed();
}
