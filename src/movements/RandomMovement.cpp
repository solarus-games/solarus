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
#include "solarus/core/Geometry.h"
#include "solarus/core/Random.h"
#include "solarus/core/System.h"
#include "solarus/entities/Entity.h"
#include "solarus/lua/LuaContext.h"
#include "solarus/movements/RandomMovement.h"
#include <sstream>

namespace Solarus {

/**
 * \brief Constructor.
 * \param speed Speed of the movement in pixels per seconds.
 */
RandomMovement::RandomMovement(int speed):
  RandomMovement(speed, 0) {

}

/**
 * \brief Constructor.
 * \param speed Speed of the movement in pixels per seconds.
 * \param max_radius if the object goes further than this distance, it will come back
 */
RandomMovement::RandomMovement(int speed, int max_radius):
  StraightMovement(false, false),
  normal_speed(speed),
  max_radius(max_radius),
  next_direction_change_date(0) {

  set_speed(speed);
  set_next_direction();
}

/**
 * \brief Notifies this movement that the object it controls has changed.
 */
void RandomMovement::notify_object_controlled() {

  StraightMovement::notify_object_controlled();
  set_max_radius(max_radius);
}

/**
 * \brief Returns the speed applied when the movement is started.
 * \return The speed when moving.
 */
double RandomMovement::get_normal_speed() const {
  return normal_speed;
}

/**
 * \brief Sets the speed to apply when the movement is started.
 * \param normal_speed The new speed.
 */
void RandomMovement::set_normal_speed(double normal_speed) {

  this->normal_speed = normal_speed;

  if (is_started()) {
    StraightMovement::set_speed(normal_speed);
  }
}

/**
 * \brief Returns the maximum radius.
 * \return The distance above which the object comes back.
 */
int RandomMovement::get_max_radius() const {
  return max_radius;
}

/**
 * \brief Sets the maximum radius.
 * \param max_radius If the object goes further than this distance,
 * it will come back (0 means no limit).
 */
void RandomMovement::set_max_radius(int max_radius) {

  if (max_radius < 0) {
    std::ostringstream oss;
    oss << "Invalid max radius: " << max_radius;
    Debug::die(oss.str());
  }
  this->max_radius = max_radius;

  // restrict the movement in a rectangle
  bounds.set_xy(get_xy());
  bounds.add_xy(-max_radius, -max_radius);
  bounds.set_size(max_radius * 2, max_radius * 2);
}

/**
 * \brief Chooses a new direction for the movement.
 */
void RandomMovement::set_next_direction() {

  set_speed(normal_speed);

  double angle;
  if (get_entity() == nullptr
      || max_radius == 0 // means no limit
      || bounds.contains(get_xy())) {

    // we are inside the bounds (or there is no bound): pick a random direction
    angle = Geometry::degrees_to_radians(Random::get_number(8) * 45 + 22.5);
  }
  else {

    // we are outside the bounds: get back into the rectangle to avoid going too far
    angle = Geometry::get_angle(get_xy(), bounds.get_center());
  }
  set_angle(angle);

  next_direction_change_date = System::now() + 500 + Random::get_number(1500); // change again in 0.5 to 2 seconds

  notify_movement_changed();
}

/**
 * \brief Updates the random movement.
 *
 * This function is called repeatedly by the map.
 * This is a redefinition of Movement::update()
 * to change the direction sometimes.
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
 * \brief Suspends or resumes this movement.
 * \param suspended true to suspend the movement, false to resume it
 */
void RandomMovement::set_suspended(bool suspended) {

  StraightMovement::set_suspended(suspended);

  if (!suspended) {
    next_direction_change_date += System::now() - get_when_suspended();
  }
}

/**
 * \brief Notifies this movement that it just failed to apply
 * because of obstacles.
 */
void RandomMovement::notify_obstacle_reached() {

  StraightMovement::notify_obstacle_reached();
  set_next_direction();
}

/**
 * \brief Returns the name identifying this type in Lua.
 * \return the name identifying this type in Lua
 */
const std::string& RandomMovement::get_lua_type_name() const {
  return LuaContext::movement_random_module_name;
}

}

