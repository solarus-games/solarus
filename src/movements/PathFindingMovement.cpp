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
#include "movements/PathFindingMovement.h"
#include "movements/PathFinding.h"
#include "entities/MapEntity.h"
#include "Random.h"

/**
 * Creates a chase movement.
 * @param target the entity to target
 * @param speed speed of the movement
 */
PathFindingMovement::PathFindingMovement(MapEntity *target, int speed):
  PathMovement(get_random_path(), speed, false, true),
  target(target), next_recomputation_date(SDL_GetTicks()) {

}

/**
 * Destructor.
 */
PathFindingMovement::~PathFindingMovement(void) {

}

/**
 * Updates the position.
 */
void PathFindingMovement::update(void) {

  if (!suspended) {
    if (is_stopped()) {
      // there was a collision
      set_speed(initial_speed);
      start_next_move();
    }
  } 

  PathMovement::update();
}

/**
 * Calculates the direction and the speed of the movement
 * depending on the target.
 */
void PathFindingMovement::recompute_movement(void) { 

//  Uint32 start = SDL_GetTicks();

  PathFinding path_finding = PathFinding(entity->get_map(), entity, target);
  remaining_path = path_finding.compute_path();

//  Uint32 end = SDL_GetTicks();
//  std::cout << "path computed in " << (end - start) << " ms\n";

  if (remaining_path == "") {
    // the target is too far or there is no path
    remaining_path = get_random_path();
  }
}

/**
 * Starts the next step of the movement.
 */
void PathFindingMovement::start_next_move(void) {

  PathMovement::start_next_move();

  Uint32 now = SDL_GetTicks();

  if (remaining_path.size() == 0 || now >= next_recomputation_date) {
    if (entity->is_aligned_to_grid()) {
      recompute_movement();
    }
    // compute a new path every 0.5 to 1 second, randomizing in this interval to avoid
    // making all path-finding entities of the map compute a path at the same time
    next_recomputation_date += now + 500 + Random::get_number(500);
  }
}

/**
 * Returns whether the movement is finished.
 * @return false
 */
bool PathFindingMovement::is_finished(void) {
  return false;
}

