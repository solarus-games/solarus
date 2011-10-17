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
#include "movements/TemporalMovement.h"
#include "lowlevel/Geometry.h"
#include "lowlevel/System.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"

/**
 * @brief Constructor.
 * @param speed the speed
 * @param angle angle of the movement in radians
 * @param duration duration of the movement in milliseconds
 * @param smooth true to make the movement smooth
 */
TemporalMovement::TemporalMovement(int speed, double angle, uint32_t duration, bool smooth):
  SmoothMovement(smooth) {
  start(speed, angle, duration);
}

/**
 * @brief Constructor.
 * @param speed the speed
 * @param source_xy the movement will start from this point
 * @param target_xy the movement will go into this point's direction
 * @param duration duration of the movement in milliseconds
 * @param smooth true to make the movement smooth
 */
TemporalMovement::TemporalMovement(int speed, const Rectangle &source_xy, const Rectangle &target_xy, uint32_t duration, bool smooth):
  SmoothMovement(smooth) {

  double angle = Geometry::get_angle(source_xy.get_x(), source_xy.get_y(), target_xy.get_x(), target_xy.get_y());
  start(speed, angle, duration);
}

/**
 * @brief Destructor.
 */
TemporalMovement::~TemporalMovement() {

}

/**
 * @brief Starts the straight movement into a direction.
 * @param speed the speed
 * @param angle angle of the movement in radians
 * @param duration duration of the movement in milliseconds
 */
void TemporalMovement::start(int speed, double angle, uint32_t duration) {

  this->finished = false;
  this->duration = duration;
  this->end_movement_date = System::now() + duration;
  set_speed(speed);
  if (speed != 0) {
    set_angle(angle);
  }
}

/**
 * @brief Restarts this movement for the specified duration.
 * @param duration duration of the movement in milliseconds
 */
void TemporalMovement::set_duration(uint32_t duration) {
  start((int) get_speed(), get_angle(), duration);
}

/**
 * @brief Updates the movement.
 */
void TemporalMovement::update() {

  SmoothMovement::update();

  if (!is_suspended()) {
    uint32_t now = System::now();
    if (now >= end_movement_date && !finished) {
      stop();
      finished = true;
    }
  }
}

/**
 * @brief Suspends or resumes the movement
 * @param suspended true to suspend the movement, false to resume it
 */
void TemporalMovement::set_suspended(bool suspended) {

  SmoothMovement::set_suspended(suspended);

  if (!suspended) {
    end_movement_date += System::now() - get_when_suspended();
  }
}

/**
 * @brief Returns whether the movement is finished.
 *
 * This functions returns false while the delay is not finished,
 * even if the movement was stopped by an obstacle.
 *
 * @return true if the movement is finished
 */
bool TemporalMovement::is_finished() {
  return finished;
}

/**
 * @brief Stops the movement finished even if the delay planned is not finished yet.
 */
void TemporalMovement::set_finished() {
  stop();
  this->finished = true;
}

/**
 * @brief Returns the value of a property of this movement.
 *
 * Accepted keys:
 * - speed
 * - angle
 * - duration
 * - ignore_obstacles
 * - smooth
 *
 * @param key key of the property to get
 * @return the corresponding value as a string
 */
const std::string TemporalMovement::get_property(const std::string &key) {

  std::ostringstream oss;

  if (key == "speed") {
    oss << get_speed();
  }
  else if (key == "angle") {
    oss << get_angle();
  }
  else if (key == "duration") {
    oss << duration;
  }
  else if (key == "ignore_obstacles") {
    oss << are_obstacles_ignored();
  }
  else if (key == "smooth") {
    oss << is_smooth();
  }
  else {
    Debug::die(StringConcat() << "Unknown property of TemporalMovement: '" << key << "'");
  }

  return oss.str();
}

/**
 * @brief Sets the value of a property of this movement.
 *
 * Accepted keys:
 * - speed
 * - angle
 * - duration
 * - ignore_obstacles
 * - smooth
 *
 * @param key key of the property to set (the accepted keys depend on the movement type)
 * @param value the value to set
 */
void TemporalMovement::set_property(const std::string &key, const std::string &value) {

  std::istringstream iss(value);

  if (key == "speed") {
    int speed;
    iss >> speed;
    set_speed(speed);
  }
  else if (key == "angle") {
    double angle;
    iss >> angle;
    if (get_speed() == 0) {
      set_speed(1);
    }
    set_angle(angle);
  }
  else if (key == "duration") {
    uint32_t duration;
    iss >> duration;
    set_duration(duration);
  }
  else if (key == "ignore_obstacles") {
    bool ignore_obstacles;
    iss >> ignore_obstacles;
    set_default_ignore_obstacles(ignore_obstacles);
  }
  else if (key == "smooth") {
    bool smooth;
    iss >> smooth;
    set_smooth(smooth);
  }
  else {
    Debug::die(StringConcat() << "Unknown property of TemporalMovement: '" << key << "'");
  }
}

