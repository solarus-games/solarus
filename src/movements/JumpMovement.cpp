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
#include "movements/JumpMovement.h"

/**
 * @brief Trajectory of the basic jump movement for each direction.
 */
const std::string JumpMovement::basic_trajectories[8] = {
    "1 0",	// right
    "1 -1",	// right-up
    "0 -1",	// up
    "-1 -1",	// left-up
    "-1 0",	// left
    "-1 1",	// left-down
    "0 1",	// down
    "1 1"	// right-down
};

/**
 * @brief Creates a jump movement.
 * @param direction8 of the movement (0 to 7)
 * @param length length of the jump
 * @param ignore_obstacles true to make the movement ignore obstacles
 * @param movement_delay delay between each one-pixel move in the jump movement in milliseconds (0: default)
 */
JumpMovement::JumpMovement(int direction8, int length, bool ignore_obstacles, uint32_t movement_delay):
  PixelMovement(compute_trajectory(direction8, length), 10, false, ignore_obstacles), jump_height(0) {

  // set the speed
  if (movement_delay == 0) {
    set_delay(std::max(4, 14 - length / 10));
  }
  else {
    set_delay(movement_delay);
  }
}

/**
 * @brief Computes the trajectory provided the direction and the length.
 * @param direction8 direction of the movement (0 to 7)
 * @param length length in pixels
 */
const std::string JumpMovement::compute_trajectory(int direction8, int length) {

  std::string trajectory;

  for (int i = 0; i < length; i++) {
    trajectory += " ";
    trajectory += basic_trajectories[direction8];
  }

  return trajectory;
}

/**
 * @brief Destructor.
 */
JumpMovement::~JumpMovement() {
}

/**
 * @brief Returns the height in pixels where the entity should be displayed over its shadow.
 * @return the height of the jump
 */
int JumpMovement::get_jump_height() {
  return jump_height;
}

/**
 * @brief This function is called when a step of the trajectory just occured.
 *
 * If updates the height of the jump when a step of the basic movement is made.
 *
 * @param step_index index of the step in the trajectory (the first one is 0)
 * @param success true if the move was made, false if there was an obstacle
 */
void JumpMovement::notify_step_done(int step_index, bool success) {

  if (step_index == 1 || step_index == get_length()) {
    jump_height = 0;
  }
  else {
    int jump_sign = (step_index <= get_length() / 2) ? 1 : -1;
    int jump_unit = (get_length() <= 16) ? 2 : 1;
    
    jump_height += jump_sign * jump_unit;
  }
}

