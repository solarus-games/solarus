/*
 * Copyright (C) 2006-2012 Christopho, Solarus - http://www.solarus-games.org
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
#include "lua/LuaContext.h"
#include "entities/MapEntity.h"
#include "lowlevel/Geometry.h"
#include "lowlevel/System.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"
#include <sstream>
#include <cmath>

const uint32_t TargetMovement::recomputation_delay = 150;

/**
 * @brief Creates a new target movement towards a fixed point.
 * @param target_x x coordinate of the target point
 * @param target_y y coordinate of the target point
 * @param moving_speed speed of the movement when not stoppedstopped
 * @param ignore_obstacles true to ignore obstacles (if on a map)
 */
TargetMovement::TargetMovement(int target_x, int target_y, int moving_speed,
    bool ignore_obstacles):

  StraightMovement(ignore_obstacles, true),
  target_x(target_x),
  target_y(target_y),
  target_entity(NULL),
  sign_x(0),
  sign_y(0),
  moving_speed(moving_speed),
  next_recomputation_date(System::now()),
  finished(false) {

}

/**
 * @brief Creates a new target movement toward an entity.
 *
 * The movement will update its trajectory if the entity's position is changed.
 *
 * @param target_entity the target entity
 * @param moving_speed speed of the movement when not stopped
 * @param ignore_obstacles true to ignore obstacles (if on a map)
 */
TargetMovement::TargetMovement(MapEntity* target_entity, int moving_speed,
    bool ignore_obstacles):

  StraightMovement(ignore_obstacles, true),
  target_x(target_entity->get_x()),
  target_y(target_entity->get_y()),
  target_entity(target_entity),
  sign_x(0),
  sign_y(0),
  moving_speed(moving_speed),
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
  next_recomputation_date = System::now() + recomputation_delay;
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
  next_recomputation_date = System::now() + recomputation_delay;
}

/**
 * @brief Returns the speed of this movement when it is not stopped.
 * @return the speed when moving, in pixels per second
 */
int TargetMovement::get_moving_speed() {
  return moving_speed;
}

/**
 * @brief Sets the speed of this movement when it is not stopped.
 * @param moving_speed the speed when moving, in pixels per second
 */
void TargetMovement::set_moving_speed(int moving_speed) {
  this->moving_speed = moving_speed;
  recompute_movement();
}

/**
 * @brief Updates the movement.
 */
void TargetMovement::update() {

  int dx = target_x - get_x();
  int dy = target_y - get_y();

  if (System::now() >= next_recomputation_date) {
    recompute_movement();
    next_recomputation_date += recomputation_delay;
  }

  // see if the target is reached
  else if (dx * sign_x <= 0 && dy * sign_y <= 0) {
    set_xy(target_x, target_y); // because the target movement may have not been very precise
    stop();
    finished = true;
  }

  StraightMovement::update();
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
      // The angle has changed or the movement was stopped.
      set_speed(moving_speed);
      set_angle(angle);
      set_max_distance((int) Geometry::get_distance(
            get_x(), get_y(), target_x, target_y));
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
 * @brief Returns the name identifying this type in Lua.
 * @return the name identifying this type in Lua
 */
const std::string& TargetMovement::get_lua_type_name() const {
  return LuaContext::target_movement_module_name;
}

