/*
 * Copyright (C) 2009 Christopho, Zelda Solarus - http://www.zelda-solarus.com
 * 
 * Zelda: Mystery of Solarus DX is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Zelda: Mystery of Solarus DX is distributed in the hope that it will be useful,
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

/**
 * Creates a new target movement towards a fixed point.
 * @param target_x x coordinate of the target point
 * @param target_y y coordinate of the target point
 * @param speed speed of the movement
 */
TargetMovement::TargetMovement(int target_x, int target_y, int speed):
  target_x(target_x), target_y(target_y), target_entity(NULL), sign_x(0), sign_y(0), speed(speed),
  next_recomputation_date(System::now()) {

}

/**
 * Creates a new target movement toward an entity.
 * The movement will update its trajectory if the entity's position is changed.
 * @param target_entity the target entity
 * @param speed speed of the movement
 */
TargetMovement::TargetMovement(MapEntity *target_entity, int speed):
  target_x(target_entity->get_x()), target_y(target_entity->get_y()), target_entity(target_entity),
  sign_x(0), sign_y(0), speed(speed), next_recomputation_date(System::now()) {

}

/**
 * Destructor.
 */
TargetMovement::~TargetMovement(void) {

}

/**
 * Changes the target of this movement.
 * @param target_x x coordinate of the target point
 * @param target_y y coordinate of the target point
 */
void TargetMovement::set_target(int target_x, int target_y) {
  this->target_x = target_x;
  this->target_y = target_y;
  next_recomputation_date = System::now();
}

/**
 * Changes the target of this movement.
 * @param target_entity the target entity
 */
void TargetMovement::set_target(MapEntity *target_entity) {
  this->target_entity = target_entity;
  this->target_x = target_entity->get_x();
  this->target_y = target_entity->get_y();
  next_recomputation_date = System::now();
}


/**
 * Updates the movement.
 */
void TargetMovement::update(void) {

  int dx = target_x - get_x();
  int dy = target_y - get_y();

  if (System::now() >= next_recomputation_date) {
    recompute_movement();
    next_recomputation_date += 100;
  }

  // see if the target is reached
  else if (dx * sign_x <= 0 && dy * sign_y <= 0) {
    set_position(target_x, target_y); // because the target movement may have not been very precise
    stop();
  }

  Movement::update();
}

/**
 * Calculates the direction and the speed of the movement
 * depending on the target.
 */
void TargetMovement::recompute_movement(void) { 

  if (target_entity != NULL) {
    // the target is not necessarily an entity
    target_x = target_entity->get_x();
    target_y = target_entity->get_y();
  }

  double angle = Geometry::get_angle(get_x(), get_y(), target_x, target_y);

  int dx = target_x - get_x();
  int dy = target_y - get_y();

  sign_x = dx >= 0 ? 1 : -1;
  sign_y = dy >= 0 ? 1 : -1;

  set_speed(speed);
  set_direction(angle);
}

/**
 * Returns whether the movement is finished, i.e.
 * whether the target was reached.
 */
bool TargetMovement::is_finished(void) {
  return is_stopped();
}

