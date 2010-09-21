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
  moving_speed(moving_speed), direction8(-1) {

}

/**
 * @brief Destructor.
 */
PlayerMovement::~PlayerMovement() {

}

/**
 * @brief Updates this movement.
 */
void PlayerMovement::update() {

  SmoothCollisionMovement::update();

  if (entity->get_game() == NULL) {
    return;
  }

  // someone may have stopped the movement from outside (e.g. Hero::reset_movement())
  if (is_stopped() && direction8 != -1) {
    direction8 = -1;
    compute_movement();
  }

  // check whether the wanted direction has changed
  GameControls *controls = entity->get_game()->get_controls();
  int wanted_direction8 = controls->get_wanted_direction8(); 
  if (wanted_direction8 != direction8 && !is_suspended()) {
    direction8 = wanted_direction8;
    compute_movement();
  }
}

/**
 * @brief Returns the direction this movement is trying to move towards.
 * @return the direction (0 to 7), or -1 if the player is not trying to go
 * to a direction or the movement is disabled
 */
int PlayerMovement::get_wanted_direction8() {
  return direction8;
}

/**
 * @brief Returns the moving speed of the entity.
 * @return the moving speed of the entity
 */
int PlayerMovement::get_moving_speed() {
  return moving_speed;
}

/**
 * @brief Sets the moving speed of the entity.
 * @param moving_speed the moving speed of the entity
 */
void PlayerMovement::set_moving_speed(int moving_speed) {

  this->moving_speed = moving_speed;
  set_wanted_direction();
  compute_movement();
}

/**
 * @brief Determines the direction defined by the directional keys currently pressed
 * and computes the corresponding movement.
 */
void PlayerMovement::set_wanted_direction() {

  if (entity->get_game() != NULL) {
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
void PlayerMovement::compute_movement() {

  // compute the speed vector corresponding to the direction wanted by the player

  if (direction8 == -1) {
    // no movement
    stop();
  }
  else { // the directional keys currently pressed define a valid movement
    set_speed(moving_speed);
    set_direction(direction8 * 45);
  }

  // notify the entity that its movement has just changed:
  // indeed, the entity may need to update its sprites
  entity->notify_movement_changed();
}

