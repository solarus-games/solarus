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
#include "movements/ChaseMovement.h"
#include "entities/MapEntity.h"

/**
 * Creates a chase movement.
 * @param target the entity to target
 * @param speed speed of the movement
 */
ChaseMovement::ChaseMovement(MapEntity *target, int speed):
  target(target), speed(speed), next_recomputation_date(SDL_GetTicks()) {

}

/**
 * Destructor.
 */
ChaseMovement::~ChaseMovement(void) {

}

/**
 * Updates the position.
 */
void ChaseMovement::update(void) {

  if (SDL_GetTicks() >= next_recomputation_date) {
    recompute_movement();
    next_recomputation_date += 500;
  }

  SmoothCollisionMovement::update();
}

/**
 * Calculates the direction and the speed of the movement
 * depending on the target.
 */
void ChaseMovement::recompute_movement(void) { 

  double angle = entity->get_vector_angle(target);

  set_speed(speed);
  set_direction(angle);
}

