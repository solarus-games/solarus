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
#include "movements/PixelMovement.h"
#include "entities/MapEntity.h"
#include "lowlevel/System.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"
#include <sstream>

/**
 * @brief Creates a pixel movement object.
 * @param trajectory_string a string describing the succession of translations that compose this movement,
 * with the syntax "dx1 dy1  dx2 dy2  dx3 dy3 ..." (the number of spaces between values does not matter)
 * @param delay delay in milliseconds between two translations
 * @param loop true to make the movement return to the beginning once finished
 * @param ignore_obstacles true to make the movement ignore obstacles
 */
PixelMovement::PixelMovement(const std::string &trajectory_string,
			     uint32_t delay, bool loop, bool ignore_obstacles):
  Movement(ignore_obstacles),
  next_move_date(0), delay(delay), loop(loop), nb_steps_done(0), finished(false) {

  set_trajectory(trajectory_string);
}

/**
 * @brief Destructor.
 */
PixelMovement::~PixelMovement() {

}

/**
 * @brief Returns the trajectory of this movement.
 * @return a string describing the succession of translations that compose this movement
 */
const std::string& PixelMovement::get_trajectory() {
  return trajectory_string;
}

/**
 * @brief Sets the trajectory of this movement.
 *
 * This function can be called even if the object was moving with a previous trajectory.
 * The old trajectory is replaced and the movement starts the from beginning of the
 * new trajectory.
 *
 * @param trajectory_string a string describing the succession of translations that compose this movement,
 * with the syntax "dx1 dy1  dx2 dy2  dx3 dy3 ..." (the number of spaces between values does not matter)
 */
void PixelMovement::set_trajectory(const std::string &trajectory_string) {

  int dx = 0;
  int dy = 0;

  std::istringstream iss(trajectory_string);
  while (!iss.eof()) {
    if (! (iss >> dx) || ! (iss >> dy)) {
      Debug::die(StringConcat() << "Invalid trajectory string '" << trajectory_string << "'");
    }
    trajectory.push_back(Rectangle(dx, dy));
  }
  this->trajectory_string = trajectory_string;

  restart();
}

/**
 * @brief Returns the delay between two moves.
 * @return the delay between two moves, in milliseconds
 */
uint32_t PixelMovement::get_delay() {

  return delay;
}

/**
 * @brief Changes the delay between two moves.
 * @param delay the new delay, in milliseconds
 */
void PixelMovement::set_delay(uint32_t delay) {

  this->delay = delay;
}

/**
 * @brief Returns whether this movement loops when the end of the trajectory is reached.
 * @return true if the movement loops
 */
bool PixelMovement::get_loop() {

  return loop;
}

/**
 * @brief Sets whether this movement loops when the end of the trajectory is reached.
 *
 * Is the movement was finished and loops is set to true, the movement restarts.
 *
 * @param loop true to make the movement loop
 */
void PixelMovement::set_loop(bool loop) {

  this->loop = loop;

  if (finished && loop) {
    restart();
  }
}

/**
 * @brief Restarts this movement to the beginning.
 */
void PixelMovement::restart() {

  if (get_length() == 0) {
    finished = true;
  }
  else {
    nb_steps_done = 0;
    finished = false;
    trajectory_iterator = trajectory.begin();
    next_move_date = System::now();
  }
}

/**
 * @brief Updates the position.
 */
void PixelMovement::update() {

  if (is_suspended()) {
    return;
  }

  uint32_t now = System::now();

  while (now >= next_move_date && !finished) {

    Rectangle old_xy(get_x(), get_y());
    make_next_step();

    bool success = (get_x() != old_xy.get_x() || get_y() != old_xy.get_y());
    if (!is_suspended() && get_entity() != NULL) {
      get_entity()->notify_movement_tried(success);
    }
  }
}

/**
 * @brief Suspends or resumes this movement.
 * @param suspended true to suspend the movement, false to resume it
 */
void PixelMovement::set_suspended(bool suspended) {

  Movement::set_suspended(suspended);

  if (!suspended) {
    next_move_date += System::now() - get_when_suspended();
  }
}

/**
 * @brief Makes a move in the path.
 *
 * This function must be called only when the path is not finished yet.
 */
void PixelMovement::make_next_step() {

  bool success = false;
  const Rectangle &dxy = *trajectory_iterator;

  if (!test_collision_with_obstacles(dxy.get_x(), dxy.get_y())) {
    translate_xy(dxy);
    success = true;
  }

  next_move_date += delay;
  trajectory_iterator++;

  if (trajectory_iterator == trajectory.end()) {
    if (loop) {
      trajectory_iterator = trajectory.begin();
    }
    else {
      finished = true;
    }
  }

  int step_index = nb_steps_done;
  nb_steps_done++;
  notify_step_done(step_index, success);
}

/**
 * @brief This function is called when a step of the trajectory just occured.
 * @param step_index index of the step in the trajectory (the first one is 0)
 * @param success true if the move was made, false if there was an obstacle
 */
void PixelMovement::notify_step_done(int step_index, bool success) {

}

/**
 * @brief Returns the total number of moves in this trajectory.
 * @return the total number of moves in this trajectory
 */
int PixelMovement::get_length() {
  return trajectory.size();
}

/**
 * @brief Returns whether the entity controlled by this movement is moving.
 * @return true if the entity is moving, false otherwise
 */
bool PixelMovement::is_started() {
  return !finished;
}

/**
 * @brief Returns whether the movement is finished, i.e.
 * whether the end of the trajectory was reached.
 */
bool PixelMovement::is_finished() {
  return finished;
}

