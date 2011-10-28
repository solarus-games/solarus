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
#include "movements/TargetMovement.h"
#include "entities/MapEntity.h"
#include "lowlevel/Geometry.h"
#include "lowlevel/System.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"
#include <sstream>
#include <cmath>

/**
 * @brief Creates a new target movement towards a fixed point.
 * @param target_x x coordinate of the target point
 * @param target_y y coordinate of the target point
 * @param speed speed of the movement
 */
TargetMovement::TargetMovement(int target_x, int target_y, int speed):

  RectilinearMovement(true, true),
  target_x(target_x),
  target_y(target_y),
  target_entity(NULL),
  sign_x(0),
  sign_y(0),
  speed(speed),
  next_recomputation_date(System::now()),
  finished(false) {

}

/**
 * @brief Creates a new target movement toward an entity.
 *
 * The movement will update its trajectory if the entity's position is changed.
 *
 * @param target_entity the target entity
 * @param speed speed of the movement
 */
TargetMovement::TargetMovement(MapEntity* target_entity, int speed):

  RectilinearMovement(true, true),
  target_x(target_entity->get_x()),
  target_y(target_entity->get_y()),
  target_entity(target_entity),
  sign_x(0),
  sign_y(0),
  speed(speed),
  next_recomputation_date(System::now()),
  finished(false) {

}

/**
 * @brief Destructor.
 */
TargetMovement::~TargetMovement() {

}

/**
 * @brief Changes the target of this movement.
 * @param target_x x coordinate of the target point
 * @param target_y y coordinate of the target point
 */
void TargetMovement::set_target(int target_x, int target_y) {

  this->target_x = target_x;
  this->target_y = target_y;
  recompute_movement();
  next_recomputation_date = System::now() + 200;
}

/**
 * @brief Changes the target of this movement.
 * @param target_entity the target entity
 */
void TargetMovement::set_target(MapEntity* target_entity) {

  this->target_entity = target_entity;
  this->target_x = target_x;
  this->target_y = target_y;
  recompute_movement();
  next_recomputation_date = System::now() + 200;
}

/**
 * @brief Updates the movement.
 */
void TargetMovement::update() {

  int dx = target_x - get_x();
  int dy = target_y - get_y();

  if (System::now() >= next_recomputation_date) {
    recompute_movement();
    next_recomputation_date += 200;
  }

  // see if the target is reached
  else if (dx * sign_x <= 0 && dy * sign_y <= 0) {
    set_xy(target_x, target_y); // because the target movement may have not been very precise
    stop();
    finished = true;
  }

  RectilinearMovement::update();
}

/**
 * @brief Calculates the direction and the speed of the movement
 * depending on the target.
 */
void TargetMovement::recompute_movement() { 

  if (target_entity != NULL) {
    // the target may be a moving entity
    target_x = target_entity->get_x();
    target_y = target_entity->get_y();
  }

  if (get_x() != target_x || get_y() != target_y) {
    finished = false;

    double angle = Geometry::get_angle(get_x(), get_y(), target_x, target_y);

    int dx = target_x - get_x();
    int dy = target_y - get_y();

    sign_x = (dx >= 0) ? 1 : -1;
    sign_y = (dy >= 0) ? 1 : -1;

    if (std::fabs(angle - get_angle()) > 1E-6 || get_speed() < 1E-6) {
      set_speed(speed);
      set_angle(angle);
      set_max_distance(Geometry::get_distance(get_x(), get_y(), target_x, target_y));
    }
  }
}

/**
 * @brief Returns whether the movement is finished, i.e.
 * whether the target was reached.
 */
bool TargetMovement::is_finished() {

  return finished;
}

/**
 * @brief Returns the value of a property of this movement.
 *
 * Accepted keys:
 * - speed
 * - ignore_obstacles
 * - smooth
 * - displayed_direction
 *
 * @param key key of the property to get
 * @return the corresponding value as a string
 */
const std::string TargetMovement::get_property(const std::string &key) {

  std::ostringstream oss;

  if (key == "speed") {
    oss << get_speed();
  }
  else if (key == "ignore_obstacles") {
    oss << are_obstacles_ignored();
  }
  else if (key == "smooth") {
    oss << is_smooth();
  }
  else if (key == "displayed_direction") {
    oss << get_displayed_direction4();
  }
  else {
    Debug::die(StringConcat() << "Unknown property of TargetMovement: '" << key << "'");
  }

  return oss.str();
}

/**
 * @brief Sets the value of a property of this movement.
 *
 * Accepted keys:
 * - speed
 * - ignore_obstacles
 * - smooth
 *
 * @param key key of the property to set (the accepted keys depend on the movement type)
 * @param value the value to set
 */
void TargetMovement::set_property(const std::string &key, const std::string &value) {

  std::istringstream iss(value);

  if (key == "speed") {
    int speed;
    iss >> speed;
    set_speed(speed);
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
  else if (key == "displayed_direction") {
    Debug::die("The property 'displayed_direction' of TargetMovement is read-only");
  }
  else {
    Debug::die(StringConcat() << "Unknown property of TargetMovement: '" << key << "'");
  }
}

