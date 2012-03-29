/*
 * Copyright (C) 2006-2012 Christopho, Solarus - http://www.solarus-engine.org
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
#ifndef SOLARUS_PATH_FINDING_MOVEMENT_H
#define SOLARUS_PATH_FINDING_MOVEMENT_H

#include "Common.h"
#include "movements/PathMovement.h"

/**
 * @brief Movement for an entity that looks for a path to another entity.
 *
 * This movement is typically used by enemies that try to reach the hero.
 * The entity tries to find a path and to avoid the obstacles on the way.
 * To this end, the PathFinding class (i.e. an implementation of the A* algorithm) is used.
 * If the target entity is too far or not reachable, the movement is a random walk.
 *
 * Properties:
 * - speed
 * - displayed_direction (read-only)
 */
class PathFindingMovement: public PathMovement {

  private:

    MapEntity* target;              /**< the entity targeted by this movement (usually the hero) */
    uint32_t next_recomputation_date;

  protected:

    void update();
    void recompute_movement();

  public:

    PathFindingMovement(int speed);
    ~PathFindingMovement();

    void set_target(MapEntity& target);
    bool is_finished();

    virtual const std::string& get_lua_type_name() const;
};

#endif

