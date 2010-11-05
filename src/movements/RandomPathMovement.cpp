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
 * @param speed speed of the movement in pixels per second
 */
RandomPathMovement::RandomPathMovement(int speed):
  PathMovement(create_random_path(), speed, false, false, false) {

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

  if (!is_suspended() && PathMovement::is_finished()) {

    // there was a collision or the random path is finished: restart with a new random path
    set_path(create_random_path());
  }
}

/**
 * @brief Returns whether the movement is finished.
 * @return always false because the movement is restarted as soon as the path is finished
 * or an obstacle is reached
 */
bool RandomPathMovement::is_finished() {
  return false;
}

