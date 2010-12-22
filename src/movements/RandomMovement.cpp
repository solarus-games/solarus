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
#include "movements/RandomMovement.h"
#include "entities/MapEntity.h"
#include "lowlevel/System.h"
#include "lowlevel/Random.h"
#include "lowlevel/Geometry.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"
#include <sstream>

/**
 * @brief Constructor.
 * @param speed speed of the movement in pixels per seconds
 * @param max_distance if the object goes further than this distance, it will come back
 */
RandomMovement::RandomMovement(int speed, int max_distance):
  RectilinearMovement(true),
  max_distance(max_distance) {

  set_speed(speed);
  set_next_direction();
}

/**
 * @brief Destructor.
 */
RandomMovement::~RandomMovement() {

}

/**
 * @brief Sets the entity to be controlled by this movement object.
 * @param entity the entity to control
 */
void RandomMovement::set_entity(MapEntity *entity) {

  Movement::set_entity(entity);

  set_max_distance(max_distance);
  if (entity != NULL) {
    entity->notify_movement_changed();
  }
}

/**
 * @brief Sets the maximum distance of the movement.
 * @param max_distance if the object goes further than this distance, it will come back (0 means no limit)
 */
void RandomMovement::set_max_distance(int max_distance) {

  Debug::check_assertion(max_distance >= 0, StringConcat() << "Invalid value of max_distance: " << max_distance);
  this->max_distance = max_distance;

  // restrict the movement in a rectangle
  bounds.set_xy(get_xy());
  bounds.add_xy(-max_distance, -max_distance);
  bounds.set_size(max_distance * 2, max_distance * 2);
}

/**
 * @brief Chooses a new direction for the movement.
 */
void RandomMovement::set_next_direction() {

  double angle;
  if (get_entity() == NULL
      || max_distance == 0 // means no limit
      || bounds.contains(get_x(), get_y())) {

    // we are inside the bounds (or there is no bound): pick a random direction
    angle = Geometry::degrees_to_radians(Random::get_number(8) * 45 + 22.5);
  }
  else {

    // we are outside the bounds: get back into the rectangle to avoid going too far
    angle = Geometry::get_angle(get_x(), get_y(), bounds.get_x() + bounds.get_width() / 2, bounds.get_y() + bounds.get_height() / 2);
  }
  set_angle(angle);

  next_direction_change_date = System::now() + 500 + Random::get_number(3) * 500; // change again in 0.5 to 2 seconds

  if (get_entity() != NULL) {
    get_entity()->notify_movement_changed();
  }
}

/**
 * @brief Updates the fairy movement.
 *
 * This function is called repeatedly by the map.
 * This is a redefinition of Movement::update()
 * to change the fairy's direction sometimes.
 */
void RandomMovement::update() {

  RectilinearMovement::update();

  if (!is_suspended()) {

    uint32_t now = System::now();
    if (now >= next_direction_change_date) {
      set_next_direction();
    }
  }
}

/**
 * @brief Suspends or resumes this movement.
 * @param suspended true to suspend the movement, false to resume it
 */
void RandomMovement::set_suspended(bool suspended) {

  RectilinearMovement::set_suspended(suspended);

  if (!suspended) {
    next_direction_change_date += System::now() - get_when_suspended();
  }
}

/**
 * @brief Returns the value of a property of this movement.
 *
 * Accepted keys:
 * - speed
 * - max_distance
 *
 * @param key key of the property to get
 * @return the corresponding value as a string
 */
const std::string RandomMovement::get_property(const std::string &key) {

  std::ostringstream oss;

  if (key == "speed") {
    oss << get_speed();
  }
  else if (key == "max_distance") {
    oss << max_distance;
  }
  else if (key == "angle") {
    oss << Geometry::radians_to_degrees(get_angle());
  }
  else {
    Debug::die(StringConcat() << "Unknown property of RandomMovement: '" << key << "'");
  }

  return oss.str();
}

/**
 * @brief Sets the value of a property of this movement.
 *
 * Accepted keys:
 * - speed
 * - max_distance
 *
 * @param key key of the property to set (the accepted keys depend on the movement type)
 * @param the value to set
 */
void RandomMovement::set_property(const std::string &key, const std::string &value) {

  std::istringstream iss(value);

  if (key == "speed") {
    int speed;
    iss >> speed;
    set_speed(speed);
  }
  else if (key == "max_distance") {
    int max_distance;
    iss >> max_distance;
    set_max_distance(max_distance);
  }
  else if (key == "angle") {
    Debug::die("The property 'angle' of RandomMovement is read-only");
  }
  else {
    Debug::die(StringConcat() << "Unknown property of RandomMovement: '" << key << "'");
  }
}

