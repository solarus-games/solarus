/*
 * Copyright (C) 2006-2018 Christopho, Solarus - http://www.solarus-games.org
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
#include "solarus/core/Debug.h"
#include "solarus/lua/LuaContext.h"
#include "solarus/movements/JumpMovement.h"
#include <algorithm>
#include <sstream>

namespace Solarus {

/**
 * \brief Trajectory of the basic jump movement for each direction.
 */
const std::string JumpMovement::basic_trajectories[8] = {
  "1 0",    // right
  "1 -1",   // right-up
  "0 -1",   // up
  "-1 -1",  // left-up
  "-1 0",   // left
  "-1 1",   // left-down
  "0 1",    // down
  "1 1"     // right-down
};

/**
 * \brief Creates a jump movement.
 * \param direction8 of the movement (0 to 7)
 * \param distance distance of the jump in pixels
 * \param speed speed of the movement in pixels per second (0: default, based on the jump length)
 * \param ignore_obstacles true to make the movement ignore obstacles
 */
JumpMovement::JumpMovement(int direction8, int distance, int speed, bool ignore_obstacles):
  PixelMovement("", 10, false, ignore_obstacles),
  direction8(direction8),
  distance(distance),
  speed(0),
  jump_height(0) {

  if (direction8 < 0 || direction8 >= 8) {
    std::ostringstream oss;
    oss << "Invalid jump direction: " << direction8;
    Debug::die(oss.str());
  }
  set_speed(speed);
}

/**
 * \brief Starts or restarts this movement.
 */
void JumpMovement::restart() {

  std::ostringstream oss;

  for (int i = 0; i < distance; i++) {
    oss << basic_trajectories[direction8] << "  ";
  }

  set_trajectory(oss.str());
}

/**
 * \brief Returns the direction of this movement.
 * \return the direction (0 to 7)
 */
int JumpMovement::get_direction8() const {
  return direction8;
}

/**
 * \brief Changes the direction of this movement.
 *
 * The movement is restarted.
 *
 * \param direction8 the new direction (0 to 7)
 */
void JumpMovement::set_direction8(int direction8) {

  this->direction8 = direction8;
  restart();
}

/**
 * \brief Returns the distance of the jump.
 * \return the distance in pixels
 */
int JumpMovement::get_distance() const {
  return distance;
}

/**
 * \brief Changes the distance of the jump.
 *
 * The movement is restarted.
 *
 * \param distance the new distance in pixels
 */
void JumpMovement::set_distance(int distance) {

  this->distance = distance;
  restart();
}

/**
 * \brief Returns the speed of the movement.
 * \return the speed in pixels per second
 */
int JumpMovement::get_speed() const {
  return speed;
}

/**
 * \brief Sets the speed of the movement.
 * \param speed the speed in pixels per second (0 means auto, based on the length of the jump)
 */
void JumpMovement::set_speed(int speed) {

  if (speed == 0) {
    set_delay(std::max(4, 14 - distance / 10));
  }
  else {
    set_delay(1000 / speed);
  }
  this->speed = speed;
  restart();
}

/**
 * \brief Returns the direction a sprite controlled by this movement should take.
 * \return the direction to use to display the object controlled by this movement (0 to 3)
 */
int JumpMovement::get_displayed_direction4() const {

  static constexpr int displayed_directions[] = {0, 0, 1, 2, 2, 2, 3, 0};
  return displayed_directions[direction8];
}

/**
 * \brief Returns the coordinates where an object controlled by this movement
 * should be displayed.
 *
 * The movement is applied to the shadow of the entity and
 * this function returns the coordinates where the entity should be displayed instead.
 *
 * \return the coordinates to use to display the object controlled by this movement
 */
Point JumpMovement::get_displayed_xy() const {

  Point displayed_xy = get_xy();
  displayed_xy.y -= jump_height; // display the entity over its shadow
  return displayed_xy;
}

/**
 * \brief This function is called when a step of the trajectory just occured.
 *
 * If updates the height of the jump when a step of the basic movement is made.
 *
 * \param step_index index of the step in the trajectory (the first one is 0)
 * \param success true if the move was made, false if there was an obstacle
 */
void JumpMovement::notify_step_done(int step_index, bool /* success */) {

  if (step_index == 1 || step_index == get_length()) {
    jump_height = 0;
  }
  else {
    const int jump_sign = (step_index <= get_length() / 2) ? 1 : -1;
    const int jump_unit = (get_length() <= 16) ? 2 : 1;

    jump_height += jump_sign * jump_unit;
  }
}

/**
 * \brief Returns the name identifying this type in Lua.
 * \return the name identifying this type in Lua
 */
const std::string& JumpMovement::get_lua_type_name() const {
  return LuaContext::movement_jump_module_name;
}

}

