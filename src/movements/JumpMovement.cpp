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
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"
#include <sstream>

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
 * @param length length of the jump in pixels
 * @param speed speed of the movement in pixels per second (0: default, based on the jump length)
 * @param ignore_obstacles true to make the movement ignore obstacles
 */
JumpMovement::JumpMovement(int direction8, int length, int speed, bool ignore_obstacles):
  PixelMovement("", 10, false, ignore_obstacles),
  direction8(direction8),
  length(length),
  speed(speed),
  jump_height(0) {

  restart();
}

/**
 * @brief Destructor.
 */
JumpMovement::~JumpMovement() {
}

/**
 * @brief Starts or restarts this movement.
 */
void JumpMovement::restart() {

  std::ostringstream oss;

  for (int i = 0; i < length; i++) {
    oss << basic_trajectories[direction8] << "  ";
  }

  set_trajectory(oss.str());
}

/**
 * @brief Changes the direction of this movement.
 *
 * The movement is restarted.
 *
 * @param direction8 the new direction (0 to 7)
 */
void JumpMovement::set_direction8(int direction8) {

  this->direction8 = direction8;
  restart();
}

/**
 * @brief Changes the length of this movement.
 *
 * The movement is restarted.
 *
 * @param length the new length in pixels
 */
void JumpMovement::set_length(int length) {

  this->length = length;
  restart();
}

/**
 * @brief Sets the speed of the movement.
 * @param speed the speed in pixels per second (0: default, based on the length of the jump)
 */
void JumpMovement::set_speed(int speed) {

  if (speed == 0) {
    set_delay(std::max(4, 14 - length / 10));
  }
  else {
    set_delay(1000 / speed);
  }
}

/**
 * @brief Returns the height in pixels where the entity should be displayed over its shadow.
 * @return the current height of the jump
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

/**
 * @brief Returns the value of a property of this movement.
 *
 * Accepted keys:
 * - direction8
 * - length
 * - speed
 * - ignore_obstacles
 *
 * @param key key of the property to get
 * @return the corresponding value as a string
 */
const std::string JumpMovement::get_property(const std::string &key) {

  std::ostringstream oss;

  if (key == "direction8") {
    oss << direction8;
  }
  else if (key == "length") {
    oss << length;
  }
  else if (key == "speed") {
    oss << speed;
  }
  else if (key == "ignore_obstacles") {
    oss << are_obstacles_ignored();
  }
  else {
    Debug::die(StringConcat() << "Unknown property of JumpMovement: '" << key << "'");
  }

  return oss.str();
}

/**
 * @brief Sets the value of a property of this movement.
 *
 * Accepted keys:
 * - direction8
 * - length
 * - speed
 * - ignore_obstacles
 *
 * @param key key of the property to set (the accepted keys depend on the movement type)
 * @param the value to set
 */
void JumpMovement::set_property(const std::string &key, const std::string &value) {

  std::istringstream iss(value);

  if (key == "direction8") {
    int direction8;
    iss >> direction8;
    set_direction8(direction8);
  }
  else if (key == "length") {
    int length;
    iss >> length;
    set_length(length);
  }
  else if (key == "speed") {
    int speed;
    iss >> speed;
    set_speed(speed);
  }
  else if (key == "ignore_obstacles") {
    bool ignore_obstacles;
    iss >> ignore_obstacles;
    set_default_ignore_obstacles(ignore_obstacles);
  }
  else {
    Debug::die(StringConcat() << "Unknown property of JumpMovement: '" << key << "'");
  }
}

