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
#include "movements/RandomPathMovement.h"

/**
 * @brief Creates a random walk movement object.
 * @param speed speed of the movement
 */
RandomPathMovement::RandomPathMovement(int speed):
  PathMovement(get_random_path(), speed, false, false, false) {

}

/**
 * @brief Destructor.
 */
RandomPathMovement::~RandomPathMovement() {

}

/**
 * @brief Updates the movements: detects the collisions
 * in order to restart the movement.
 */
void RandomPathMovement::update() {

  PathMovement::update();

  if (!is_suspended() && is_stopped()) {
    // there was a collision
    set_speed(normal_speed);
    start_next_move();
  }
}

/**
 * @brief Starts the next step of the movement.
 */
void RandomPathMovement::start_next_move() {

  if (remaining_path.size() == 0) {
    remaining_path = get_random_path();
  }

  PathMovement::start_next_move();
}

/**
 * @brief Returns whether the movement is finished.
 * @return false
 */
bool RandomPathMovement::is_finished() {
  return false;
}

