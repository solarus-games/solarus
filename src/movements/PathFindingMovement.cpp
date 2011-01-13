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
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"
#include <sstream>

/**
 * @brief Creates a chase movement.
 * @param target the entity to target
 * @param speed speed of the movement in pixels per second
 */
PathFindingMovement::PathFindingMovement(MapEntity *target, int speed):
  PathMovement("", speed, false, false, true),
  target(target), next_recomputation_date(System::now() + 100) {

}

/**
 * @brief Destructor.
 */
PathFindingMovement::~PathFindingMovement() {

}

/**
 * @brief Updates the position.
 */
void PathFindingMovement::update() {

  PathMovement::update();

  if (is_suspended()) {
    return;
  }

  if (PathMovement::is_finished()) {

    // there was a collision or the path was made
    if (System::now() >= next_recomputation_date && get_entity()->is_aligned_to_grid()) {
      recompute_movement();
    }
    else {
      set_path(create_random_path());
    }
  } 
}

/**
 * @brief Calculates the direction and the speed of the movement
 * depending on the target.
 */
void PathFindingMovement::recompute_movement() { 

  PathFinding path_finding(get_entity()->get_map(), *get_entity(), *target);
  std::string path = path_finding.compute_path();

  uint32_t min_delay;
  if (path.size() == 0) {
    // the target is too far or there is no path
    path = create_random_path();

    // no path was found: no need to try again very soon
    // (note that the A* algorithm is very costly when it explores all nodes without finding a solution)
    min_delay = 3000;
  }
  else {
    // a path was found: we need to update it frequently (and the A* algorithm is much faster in general when there is a solution)
    min_delay = 300;
  }
  // compute a new path every random delay to avoid
  // having all path-finding entities of the map compute a path at the same time
  next_recomputation_date = System::now() + min_delay + Random::get_number(200);

  set_path(path);
}

/**
 * @brief Returns whether the movement is finished.
 * @return always false because the movement is restarted as soon as the path is finished
 * or an obstacle is reached
 */
bool PathFindingMovement::is_finished() {
  return false;
}

/**
 * @brief Returns the value of a property of this movement.
 *
 * Accepted keys:
 * - speed
 *
 * @param key key of the property to get
 * @return the corresponding value as a string
 */
const std::string PathFindingMovement::get_property(const std::string &key) {

  std::ostringstream oss;

  if (key == "speed") {
    oss << get_speed();
  }
  else {
    Debug::die(StringConcat() << "Unknown property of PathFindingMovement: '" << key << "'");
  }

  return oss.str();
}

/**
 * @brief Sets the value of a property of this movement.
 *
 * Accepted keys:
 * - speed
 *
 * @param key key of the property to set (the accepted keys depend on the movement type)
 * @param value the value to set
 */
void PathFindingMovement::set_property(const std::string &key, const std::string &value) {

  std::istringstream iss(value);

  if (key == "speed") {
    int speed;
    iss >> speed;
    set_speed(speed);
  }
  else {
    Debug::die(StringConcat() << "Unknown property of PathFindingMovement: '" << key << "'");
  }
}


