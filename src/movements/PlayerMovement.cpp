#include <cmath>
#include "movements/PlayerMovement.h"
#include "entities/MapEntity.h"
#include "ZSDX.h"
#include "Game.h"
#include "Controls.h"

/*
 * Bit masks associated to each arrow on the keyboard or the joypad.
 * A combination of arrows is stored in a simple integer.
 */
const Uint16 PlayerMovement::direction_masks[4] = {
  0x0001,
  0x0002,
  0x0004,
  0x0008
};

/**
 * Associates to each possible combination of arrows
 * a movement direction in degrees: 0 to 359, or -1 to indicate
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
PlayerMovement::PlayerMovement(int speed):
  MovementWithSmoothCollision(NULL),
  started(false), speed(speed), can_move(false), direction_mask(0) {

}

/**
 * Destructor.
 */
PlayerMovement::~PlayerMovement(void) {

}

/**
 * Returns the current direction of the movement.
 * The returned direction is an angle (0 to 359), or -1 if the
 * movement is stopped.
 * @returns the current movement direction
 */
int PlayerMovement::get_direction(void) {
  return directions[direction_mask];
}

/**
 * Returns whether the entity is moving.
 * @return true if the entity is moving
 */
bool PlayerMovement::is_started(void) {
  return started;
}

/**
 * Returns whether the player can move the entity.
 * This function returns false when the player does not
 * currently have control of the entity.
 * @return true if the movements are enabled
 */
bool PlayerMovement::is_moving_enabled(void) {
  return can_move;
}

/**
 * Sets whether the player can move the entity.
 * This function permits to ignore or restore the control of the entity.
 * @param can_move true to enable the movements, false to disable them.
 */
void PlayerMovement::set_moving_enabled(bool can_move) {

  if (can_move != this->can_move) {

    if (can_move) {
      // if the control is being restored, let's take
      // into account the possible arrows pressed

      Controls *controls = zsdx->game->get_controls();

      if (controls->is_key_pressed(Controls::RIGHT)) {
	add_direction_mask(direction_masks[0]);
      }
      if (controls->is_key_pressed(Controls::UP)) {
	add_direction_mask(direction_masks[1]);
      }
      if (controls->is_key_pressed(Controls::LEFT)) {
	add_direction_mask(direction_masks[2]);
      }
      if (controls->is_key_pressed(Controls::DOWN)) {
	add_direction_mask(direction_masks[3]);
      }
    }
    else { // the control is being ignored

      // we now consider that no arrow is currently pressed
      set_direction_mask(0x0000);
    }

    this->can_move = can_move;

    // recalculate the movement since direction_mask may have changed
    compute_movement();
  }
}

/**
 * Suspends or resumes the movement.
 * @param suspended true to suspend the movement, false to resume it
 */
void PlayerMovement::set_suspended(bool suspended) {
  Movement::set_suspended(suspended);
  set_moving_enabled(!suspended);
  compute_movement(); // always recompute the movement when the game is suspended / resumed
}

/**
 * This function is called when an arrow key is pressed.
 * @param direction of the arrow pressed (0 to 3)
 */
void PlayerMovement::add_direction(int direction) {

  add_direction_mask(direction_masks[direction]);
  if (can_move) {
    compute_movement();
  }
}

/**
 * This function is called when an arrow key is released.
 * @param direction of the arrow released (0 to 3)
 */
void PlayerMovement::remove_direction(int direction) {

  remove_direction_mask(direction_masks[direction]);
  if (can_move) {
    compute_movement();
  }
}

/**
 * Returns the direction mask.
 * The direction mask represents what arrow keys are currently
 * pressed by the player.
 * @return the direction mask
 */
Uint16 PlayerMovement::get_direction_mask(void) {
  return direction_mask;
}

/**
 * Adds one of the four directions to the direction mask.
 * The direction mask represents what arrow keys are currently
 * pressed by the player.
 * @param direction_mask the direction mask to add
 */
void PlayerMovement::add_direction_mask(Uint16 direction_mask) {
  set_direction_mask(this->direction_mask | direction_mask);
}

/**
 * Removes one of the four directions to the direction mask.
 * The direction mask represents what arrow keys are currently
 * pressed by the player.
 * @param direction_mask the direction mask to remove
 */
void PlayerMovement::remove_direction_mask(Uint16 direction_mask) {
  set_direction_mask(this->direction_mask & ~direction_mask);
}

/**
 * Sets the direction mask.
 * The direction mask represents what arrow keys are currently
 * pressed by the player.
 * @param direction_mask the direction mask
 */
void PlayerMovement::set_direction_mask(Uint16 direction_mask) {
  if (direction_mask != this->direction_mask) {
    this->direction_mask = direction_mask;
  }
}

/**
 * Changes the movement of the entity depending on the arrows pressed
 * (i.e. depending on the value of direction_mask).
 * This function is called when an arrow is pressed or released,
 * or when the movement has just been enabled or
 * disabled (i.e. when set_moving_enabled() is called).
 */
void PlayerMovement::compute_movement(void) {
  int x_speed = 0;
  int y_speed = 0;

  // get the direction in degrees specified by the user (or -1)
  int direction = directions[direction_mask];

  if (direction == -1) {
    // no movement because the arrow combination is invalid
    // (e.g. left and right at the same time)
    stop();
    started = false;
  }
  else {
    // the arrows currently pressed define a valid movement

    if (!started) {
      started = true;
    }

    /* Now we need to calculte the speed vector (x_speed and y_speed) knowing the direction.
     * We could call Movement::set_direction(direction) but with the 8
     * basic directions, we don't need to make complex computations.
     */

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

  // notify the entity that its movement has just changed:
  // indeed the entity may need to update its animations and its collisions
  entity->movement_just_changed();
}
