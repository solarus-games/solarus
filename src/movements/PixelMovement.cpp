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
#include "solarus/core/System.h"
#include "solarus/core/Debug.h"
#include "solarus/entities/Entity.h"
#include "solarus/lua/LuaContext.h"
#include "solarus/movements/PixelMovement.h"
#include <sstream>

namespace Solarus {

/**
 * \brief Creates a pixel movement object.
 * \param trajectory_string a string describing the succession of translations that compose this movement,
 * with the syntax "dx1 dy1  dx2 dy2  dx3 dy3 ..." (the number of spaces between values does not matter)
 * \param delay delay in milliseconds between two translations
 * \param loop true to make the movement return to the beginning once finished
 * \param ignore_obstacles true to make the movement ignore obstacles
 */
PixelMovement::PixelMovement(
    const std::string& trajectory_string,
    uint32_t delay,
    bool loop,
    bool ignore_obstacles):
  Movement(ignore_obstacles),
  next_move_date(0),
  delay(delay),
  loop(loop),
  nb_steps_done(0),
  finished(false) {

  set_trajectory(trajectory_string);
}

/**
 * \brief Returns the trajectory of this movement.
 * \return the succession of translations that compose this movement
 */
const std::list<Point>& PixelMovement::get_trajectory() const {
  return trajectory;
}

/**
 * \brief Sets the trajectory of this movement.
 *
 * This function can be called even if the object was moving with a previous trajectory.
 * The old trajectory is replaced and the movement starts the from beginning of the
 * new trajectory.
 *
 * \param trajectory a list of points describing the succession of translations that compose this movement,
 * where each point is an xy value representing a translation
 */
void PixelMovement::set_trajectory(const std::list<Point>& trajectory) {

  this->trajectory = trajectory;
  this->trajectory_string = ""; // will be computed only on demand

  restart();
}

/**
 * \brief Sets the trajectory of this movement from a string.
 *
 * This function can be called even if the object was moving with a previous trajectory.
 * The old trajectory is replaced and the movement starts the from beginning of the
 * new trajectory.
 *
 * \param trajectory_string a string describing the succession of translations that compose this movement,
 * with the syntax "dx1 dy1  dx2 dy2  dx3 dy3 ..." (the number of spaces between values does not matter)
 */
void PixelMovement::set_trajectory(const std::string& trajectory_string) {

  int dx = 0;
  int dy = 0;

  trajectory.clear();
  std::istringstream iss(trajectory_string);
  while (iss >> dx) {
    if (!(iss >> dy)) {
      Debug::die(std::string("Invalid trajectory string: '")
          + trajectory_string + "'");
    }
    trajectory.emplace_back(dx, dy);
  }
  this->trajectory_string = trajectory_string;

  restart();
}

/**
 * \brief Returns the delay between two moves.
 * \return the delay between two moves, in milliseconds
 */
uint32_t PixelMovement::get_delay() const {
  return delay;
}

/**
 * \brief Changes the delay between two moves.
 * \param delay the new delay, in milliseconds
 */
void PixelMovement::set_delay(uint32_t delay) {
  this->delay = delay;
}

/**
 * \brief Returns whether this movement loops when the end of the trajectory is reached.
 * \return true if the movement loops
 */
bool PixelMovement::get_loop() const {
  return loop;
}

/**
 * \brief Sets whether this movement loops when the end of the trajectory is reached.
 *
 * Is the movement was finished and loop is set to true, the movement restarts.
 *
 * \param loop true to make the movement loop
 */
void PixelMovement::set_loop(bool loop) {

  this->loop = loop;

  if (finished && loop) {
    restart();
  }
}

/**
 * \brief Restarts this movement to the beginning.
 */
void PixelMovement::restart() {

  if (get_length() == 0) {
    finished = true;
  }
  else {
    nb_steps_done = 0;
    finished = false;
    trajectory_iterator = trajectory.begin();

    if (next_move_date == 0) {
      // Keep the previous date if we just looped.
      next_move_date = System::now();
    }
    next_move_date += delay;

    notify_movement_changed();
  }
}

/**
 * \brief Updates the position.
 */
void PixelMovement::update() {

  uint32_t now = System::now();

  while (now >= next_move_date &&
      !is_suspended() &&
      !finished &&
      (get_entity() == nullptr || get_entity()->get_movement().get() == this)
  ) {
    Point old_xy = get_xy();
    make_next_step();

    bool success = (get_xy() != old_xy);
    if (!success) {
      notify_obstacle_reached();
    }
  }

  // Do this at last so that Movement::update() knows whether we are finished.
  Movement::update();
}

/**
 * \brief Suspends or resumes this movement.
 * \param suspended true to suspend the movement, false to resume it
 */
void PixelMovement::set_suspended(bool suspended) {

  Movement::set_suspended(suspended);

  if (!suspended
      && get_when_suspended() != 0
      && next_move_date != 0) {
    next_move_date += System::now() - get_when_suspended();
  }
}

/**
 * \brief Makes a move in the trajectory.
 *
 * This function must be called only when the trajectory is not finished yet.
 */
void PixelMovement::make_next_step() {

  bool success = false;
  const Point& dxy = *trajectory_iterator;

  if (!test_collision_with_obstacles(dxy)) {
    translate_xy(dxy);
    success = true;
  }

  ++trajectory_iterator;

  if (trajectory_iterator == trajectory.end()) {
    if (loop) {
      trajectory_iterator = trajectory.begin();
    }
    else {
      finished = true;
    }
  }

  if (!finished) {
    // Add the delay unless we are finished, because we may be looping.
    next_move_date += delay;
  }

  int step_index = nb_steps_done;
  nb_steps_done++;
  notify_step_done(step_index, success);
}

/**
 * \brief This function is called when a step of the trajectory just occurred.
 * \param step_index index of the step in the trajectory (the first one is 0)
 * \param success true if the move was made, false if the movement was stopped by an obstacle
 */
void PixelMovement::notify_step_done(int /* step_index */, bool /* success */) {
}

/**
 * \brief Returns the total number of moves in this trajectory.
 * \return the total number of moves in this trajectory
 */
int PixelMovement::get_length() const {
  return int(trajectory.size());
}

/**
 * \brief Returns whether the entity controlled by this movement is moving.
 * \return true if the entity is moving, false otherwise
 */
bool PixelMovement::is_started() const {
  return !finished;
}

/**
 * \brief Returns whether the movement is finished, i.e.
 * whether the end of the trajectory was reached.
 * \return true if the end of the trajectory was reached
 */
bool PixelMovement::is_finished() const {
  return finished;
}

/**
 * \brief Returns the name identifying this type in Lua.
 * \return the name identifying this type in Lua
 */
const std::string& PixelMovement::get_lua_type_name() const {
  return LuaContext::movement_pixel_module_name;
}

}

