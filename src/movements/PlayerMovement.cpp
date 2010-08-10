/*
 * Copyright (C) 2009 Christopho, Solarus - http://www.solarus-engine.org
 * 
 * Solarus is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Solarus is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "movements/PlayerMovement.h"
#include "entities/MapEntity.h"
#include "Game.h"
#include "GameControls.h"
#include "lowlevel/Geometry.h"

/**
 * @brief Constructor.
 * @param moving_speed movement speed
 */
PlayerMovement::PlayerMovement(int moving_speed):
  moving_speed(moving_speed), moving_enabled(false), 
  direction_enabled(false), direction8(-1) {

}

/**
 * @brief Destructor.
 */
PlayerMovement::~PlayerMovement(void) {

}

/**
 * @brief Returns whether the player can move the entity.
 *
 * This function returns false when the player does not
 * currently have control of the entity.
 *
 * @return true if the movements are enabled
 */
bool PlayerMovement::is_moving_enabled(void) {
  return moving_enabled;
}

/**
 * @brief Sets whether the player can move the entity.
 *
 * This function permits to ignore or restore the control of the entity.
 *
 * @param moving_enabled true to enable the movements, false to disable them.
 * @param direction_enabled true to keep the direction control even if the movement is disabled
 * (must be true if moving_enabled is true)
 */
void PlayerMovement::set_moving_enabled(bool moving_enabled, bool direction_enabled) {

  if (moving_enabled && !direction_enabled) {
    DIE("Illegal parameters for PlayerMovement::set_moving_enabled");
  }

  if (moving_enabled != this->moving_enabled || direction_enabled != this->direction_enabled) {

    this->moving_enabled = moving_enabled;
    this->direction_enabled = direction_enabled;

    // recalculate the movement since the direction may have changed
    compute_movement();
  }
}

/**
 * @brief Returns whether the player can control the entity's direction.
 *
 * This function returns false when the player does not
 * currently have control of the entity's direction.
 *
 * @return true if the direction is enabled
 */
bool PlayerMovement::is_direction_enabled(void) {
  return direction_enabled;
}

/**
 * @brief Suspends or resumes the movement.
 * @param suspended true to suspend the movement, false to resume it
 */
void PlayerMovement::set_suspended(bool suspended) {

  Movement::set_suspended(suspended);

  // recompute the movement when the game is resumed
  if (!suspended) {
    compute_movement();
  }
}

/**
 * @brief This function is called when an arrow key is pressed.
 *
 * It changes the movement according to the new direction.
 *
 * @param direction of the directional key pressed (0 to 3)
 */
void PlayerMovement::directional_key_pressed(int direction) {
  compute_movement();
}

/**
 * @brief This function is called when a directional key is released.
 *
 * It changes the movement according to the new direction.
 *
 * @param direction of the directional key released (0 to 3)
 */
void PlayerMovement::directional_key_released(int direction) {
  compute_movement();
}

/**
 * @brief Returns the direction this movement is trying to move towards.
 * @return the direction (0 to 7), or -1 if the player is not trying to go
 * to a direction or the movement is disabled
 */
int PlayerMovement::get_wanted_direction8(void) {
  return direction8;
}

/**
 * @brief Returns the moving speed of the entity.
 * @return the moving speed of the entity
 */
int PlayerMovement::get_moving_speed(void) {
  return moving_speed;
}

/**
 * @brief Sets the moving speed of the entity.
 * @param moving_speed the moving speed of the entity
 */
void PlayerMovement::set_moving_speed(int moving_speed) {

  this->moving_speed = moving_speed;
  compute_movement();
}

/**
 * @brief Determines the direction defined by the directional keys currently pressed.
 */
void PlayerMovement::compute_wanted_direction(void) {

  if (direction_enabled && entity->get_game() != NULL) {
    GameControls *controls = entity->get_game()->get_controls();
    direction8 = controls->get_wanted_direction8();
  }
  else {
    direction8 = -1;
  }
}

/**
 * @brief Changes the movement of the entity depending on the direction wanted.
 *
 * This function is called when the direction is changed.
 */
void PlayerMovement::compute_movement(void) {

  // determine the direction wanted by the player
  compute_wanted_direction();

  // compute the corresponding speed vector
  if (direction8 == -1 || !moving_enabled) {
    // no movement
    stop();
  }
  else { // the directional keys currently pressed define a valid movement

    // calculte the speed vector (x_speed and y_speed) knowing the direction
    // without using Movement::set_direction(direction) as we just have the 8 basic directions
    int x_speed = 0;
    int y_speed = 0;

    switch (direction8) {

    case 0: // right
      x_speed = moving_speed;
      y_speed = 0;
      break;

    case 2: // up
      x_speed = 0;
      y_speed = -moving_speed;
      break;

    case 4: // left
      x_speed = -moving_speed;
      y_speed = 0;
      break;

    case 6: // down
      x_speed = 0;
      y_speed = moving_speed;
      break;

    case 1: // right up
      x_speed = (int) (moving_speed / Geometry::SQRT_2);
      y_speed = -x_speed;
      break;

    case 3: // left up
      x_speed = -(int) (moving_speed / Geometry::SQRT_2);
      y_speed = x_speed;
      break;

    case 5: // left down
      y_speed = (int) (moving_speed / Geometry::SQRT_2);
      x_speed = -y_speed;
      break;

    case 7: // right down
      x_speed = (int) (moving_speed / Geometry::SQRT_2);
      y_speed = x_speed;
      break;

    default:
      DIE("Bad basic direction: " << direction8);
      break;
    }
    set_x_speed(x_speed);
    set_y_speed(y_speed);
  }

  // notify the entity that its movement has just changed:
  // indeed, the entity may need to update its sprites
  entity->notify_movement_changed();
}

