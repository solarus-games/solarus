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
#include <cmath>

/**
 * Creates a new target movement.
 * @param target_x x coordinate of the target point
 * @param target_y y coordinate of the target point
 * @param speed speed of the movement
 */
TargetMovement::TargetMovement(int target_x, int target_y, int speed):
  target_x(target_x), target_y(target_y), target_entity(NULL), sign_x(0), sign_y(0), speed(speed),
  next_recomputation_date(SDL_GetTicks()) {

}

/**
 * Creates a new target movement.
 * @param target_entity the target entity
 * @param speed speed of the movement
 */
TargetMovement::TargetMovement(MapEntity *target_entity, int speed):
  target_x(target_entity->get_x()), target_y(target_entity->get_y()), target_entity(target_entity),
  sign_x(0), sign_y(0), speed(speed), next_recomputation_date(SDL_GetTicks()) {

}

/**
 * Destructor.
 */
TargetMovement::~TargetMovement(void) {

}

/**
 * Updates the movement.
 */
void TargetMovement::update(void) {

  int dx = target_x - get_x();
  int dy = target_y - get_y();

  if (SDL_GetTicks() >= next_recomputation_date) {
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

  int dx = target_x - get_x();
  int dy = target_y - get_y();

  double angle;
  if (dx != 0) {
    angle = atan((double) -dy / (double) dx);

    if (dx < 0) {
      angle += PI;
    }
  }
  else {
    // special case (cannot divide by zero and compute atan)
    angle = (dy > 0) ? -PI_OVER_2 : PI_OVER_2;
  }

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

