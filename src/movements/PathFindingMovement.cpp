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
#include "solarus/core/Random.h"
#include "solarus/core/System.h"
#include "solarus/entities/Entity.h"
#include "solarus/lua/LuaContext.h"
#include "solarus/movements/PathFinding.h"
#include "solarus/movements/PathFindingMovement.h"

namespace Solarus {

/**
 * \brief Creates a chase movement.
 * \param speed speed of the movement in pixels per second
 */
PathFindingMovement::PathFindingMovement(int speed):
  PathMovement("", speed, false, false, true),
  target(),
  next_recomputation_date(0) {

}

/**
 * \brief Sets the entity to target with this movement.
 */
void PathFindingMovement::set_target(const EntityPtr& target) {

  this->target = target;
  next_recomputation_date = System::now() + 100;
}

/**
 * \brief Updates the position.
 */
void PathFindingMovement::update() {

  PathMovement::update();

  if (target != nullptr && target->is_being_removed()) {
    target = nullptr;
  }

  if (is_suspended()) {
    return;
  }

  if (PathMovement::is_finished()) {

    // there was a collision or the path was made
    if (target != nullptr
        && System::now() >= next_recomputation_date
        && get_entity()->is_aligned_to_grid()) {
      recompute_movement();
    }
    else {
      set_path(create_random_path());
    }
  }
}

/**
 * \brief Calculates the direction and the speed of the movement
 * depending on the target.
 */
void PathFindingMovement::recompute_movement() {

  if (target != nullptr) {
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
}

/**
 * \brief Returns whether the movement is finished.
 * \return always false because the movement is restarted as soon as the path is finished
 * or an obstacle is reached
 */
bool PathFindingMovement::is_finished() const {
  return false;
}

/**
 * \brief Returns the name identifying this type in Lua.
 * \return the name identifying this type in Lua
 */
const std::string& PathFindingMovement::get_lua_type_name() const {
  return LuaContext::movement_path_finding_module_name;
}

}

