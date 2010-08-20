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
#include "movements/PathFindingMovement.h"
#include "movements/PathFinding.h"
#include "entities/MapEntity.h"
#include "lowlevel/Random.h"
#include "lowlevel/System.h"

/**
 * @brief Creates a chase movement.
 * @param target the entity to target
 * @param speed speed of the movement
 */
PathFindingMovement::PathFindingMovement(MapEntity *target, int speed):
  PathMovement("", speed, false, false, true),
  target(target), next_recomputation_date(System::now() + 100) {

}

/**
 * @brief Destructor.
 */
PathFindingMovement::~PathFindingMovement(void) {

}

/**
 * @brief Updates the position.
 */
void PathFindingMovement::update(void) {

  if (!is_suspended() && is_stopped()) {
    // there was a collision
    set_speed(normal_speed);
    remaining_path = get_random_path();
//    std::cout << "stopped, made random path: " << remaining_path << "\n";
    start_next_move();
  } 

  PathMovement::update();
}

/**
 * @brief Calculates the direction and the speed of the movement
 * depending on the target.
 */
void PathFindingMovement::recompute_movement(void) { 

//  uint32_t start = System::now();

  PathFinding path_finding = PathFinding(entity->get_map(), entity, target);
  remaining_path = path_finding.compute_path();

//  uint32_t end = System::now();
//  std::cout << "path computed in " << (end - start) << " ms\n";

  uint32_t min_delay;
  if (remaining_path == "") {
    // the target is too far or there is no path
    remaining_path = get_random_path();

    // no path was found: no need to try again very soon
    // (note that the A* algorithm is very costly when it explores all nodes without finding a solution)
    min_delay = 3000;
//    std::cout << "PathFindingMovement::recompute_path(): generated a random path: " << remaining_path << std::endl;
  }
  else {
    // a path was found: we need to update it frequently (and the A* algorithm is much faster in general when there is a solution)
    min_delay = 300;
//    std::cout << "PathFindingMovement::recompute_path(): calculated a path to the hero: " << remaining_path << std::endl;
  }
  // compute a new path every random delay to avoid
  // making all path-finding entities of the map compute a path at the same time
  next_recomputation_date += System::now() + min_delay + Random::get_number(200);
}

/**
 * @brief Starts the next step of the movement.
 *
 * This is a redefinition of PathMovement::start_next_move() to recalculate the path when it is finished
 * or when the recomputation delay is over.
 */
void PathFindingMovement::start_next_move(void) {

  uint32_t now = System::now();

  if (remaining_path.size() == 0 || now >= next_recomputation_date) {
    if (entity->is_aligned_to_grid()) {
      recompute_movement();
    }
  }

  PathMovement::start_next_move(); // must be done after the recomputation to update immediately the direction
}

/**
 * @brief Returns whether the movement is finished.
 * @return false
 */
bool PathFindingMovement::is_finished(void) {
  return false;
}

