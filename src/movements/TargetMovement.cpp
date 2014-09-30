/*
 * Copyright (C) 2006-2014 Christopho, Solarus - http://www.solarus-games.org
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
#include "solarus/movements/TargetMovement.h"
#include "solarus/lua/LuaContext.h"
#include "solarus/entities/MapEntity.h"
#include "solarus/lowlevel/Geometry.h"
#include "solarus/lowlevel/System.h"
#include "solarus/lowlevel/Debug.h"
#include <cmath>

namespace Solarus {

const uint32_t TargetMovement::recomputation_delay = 150;

/**
 * \brief Creates a new target movement toward an entity or a fixed point.
 * \param target_entity The entity to target or nullptr.
 * \param x X of the target point, or X offset in the case of an entity.
 * \param y Y of the target point, or Y offset in the case of an entity.
 * \param moving_speed Speed of the movement when not stopped0
 * \param ignore_obstacles \c true to ignore obstacles (if on a map).
 */
TargetMovement::TargetMovement(
    const MapEntityPtr& target_entity,
    int x,
    int y,
    int moving_speed,
    bool ignore_obstacles):

  StraightMovement(ignore_obstacles, true),
  target(x, y),
  target_entity(target_entity),
  entity_offset(x, y),
  sign_x(0),
  sign_y(0),
  moving_speed(moving_speed),
  next_recomputation_date(System::now()),
  finished(false) {
}

/**
 * \brief Notifies this movement that the object it controls has changed.
 */
void TargetMovement::notify_object_controlled() {

  StraightMovement::notify_object_controlled();

  // Coordinates have changed: compute a new trajectory.
  recompute_movement();
}

/**
 * \brief Changes the target of this movement.
 * \param target_entity The entity to target or nullptr.
 * \param xy The target point, or offset in the case of an entity.
 */
void TargetMovement::set_target(
    const MapEntityPtr& target_entity,
    const Point& xy
) {
  this->target_entity = target_entity;

  if (this->target_entity != nullptr) {
    this->entity_offset = xy;
  }
  else {
    this->target = xy;
  }

  recompute_movement();
  next_recomputation_date = System::now() + recomputation_delay;
}

/**
 * \brief Returns the speed of this movement when it is not stopped.
 * \return the speed when moving, in pixels per second
 */
int TargetMovement::get_moving_speed() const {
  return moving_speed;
}

/**
 * \brief Sets the speed of this movement when it is not stopped.
 * \param moving_speed the speed when moving, in pixels per second
 */
void TargetMovement::set_moving_speed(int moving_speed) {
  this->moving_speed = moving_speed;
  recompute_movement();
}

/**
 * \brief Updates the movement.
 */
void TargetMovement::update() {

  if (target_entity != nullptr && target_entity->is_being_removed()) {
    set_target(nullptr, target);
  }

  if (System::now() >= next_recomputation_date) {
    recompute_movement();
    next_recomputation_date += recomputation_delay;
  }

  // see if the target is reached
  Point dxy = target - get_xy();
  if (dxy.x * sign_x <= 0 && dxy.y * sign_y <= 0) {
    if (!test_collision_with_obstacles(dxy)) {
      set_xy(target);  // Because the target movement may have not been very precise.
      stop();
      finished = true;
    }
  }

  StraightMovement::update();
}

/**
 * \brief Calculates the direction and the speed of the movement
 * depending on the target.
 */
void TargetMovement::recompute_movement() {

  if (target_entity != nullptr) {
    // the target may be a moving entity
    target = target_entity->get_xy() + entity_offset;
  }

  if (get_xy() != target) {
    finished = false;

    double angle = Geometry::get_angle(get_xy(), target);

    Point dxy = target - get_xy();
    sign_x = (dxy.x >= 0) ? 1 : -1;
    sign_y = (dxy.y >= 0) ? 1 : -1;

    if (std::fabs(angle - get_angle()) > 1E-6 || get_speed() < 1E-6) {
      // The angle has changed or the movement was stopped.
      set_speed(moving_speed);
      set_angle(angle);
      set_max_distance((int) Geometry::get_distance(get_xy(), target));
    }
  }
}

/**
 * \brief Returns whether the movement is finished, i.e.
 * whether the target was reached.
 */
bool TargetMovement::is_finished() const {
  return finished;
}

/**
 * \brief Returns the name identifying this type in Lua.
 * \return the name identifying this type in Lua
 */
const std::string& TargetMovement::get_lua_type_name() const {
  return LuaContext::movement_target_module_name;
}

}

