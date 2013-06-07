/*
 * Copyright (C) 2006-2013 Christopho, Solarus - http://www.solarus-games.org
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
#include "lua/LuaContext.h"
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
  StraightMovement(false, false),
  normal_speed(speed),
  max_distance(max_distance),
  next_direction_change_date(0) {

  set_next_direction();
}

/**
 * @brief Destructor.
 */
RandomMovement::~RandomMovement() {

}

/**
 * @brief Notifies this movement that the object it controls has changed.
 */
void RandomMovement::notify_object_controlled() {

  StraightMovement::notify_object_controlled();
  set_max_distance(max_distance);
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

  set_speed(normal_speed);

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

  next_direction_change_date = System::now() + 500 + Random::get_number(1500); // change again in 0.5 to 2 seconds

  notify_movement_changed();
}

/**
 * @brief Updates the fairy movement.
 *
 * This function is called repeatedly by the map.
 * This is a redefinition of Movement::update()
 * to change the fairy's direction sometimes.
 */
void RandomMovement::update() {

  StraightMovement::update();

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

  StraightMovement::set_suspended(suspended);

  if (!suspended) {
    next_direction_change_date += System::now() - get_when_suspended();
  }
}

/**
 * @brief Notifies this movement that it just failed to apply
 * because of obstacles.
 */
void RandomMovement::notify_obstacle_reached() {

  StraightMovement::notify_obstacle_reached();
  set_next_direction();
}

/**
 * @brief Returns the name identifying this type in Lua.
 * @return the name identifying this type in Lua
 */
const std::string& RandomMovement::get_lua_type_name() const {
  return LuaContext::movement_random_module_name;
}

