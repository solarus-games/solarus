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
#ifndef SOLARUS_PATH_FINDING_MOVEMENT_H
#define SOLARUS_PATH_FINDING_MOVEMENT_H

#include "solarus/core/Common.h"
#include "solarus/entities/EntityPtr.h"
#include "solarus/movements/PathMovement.h"
#include <cstdint>
#include <string>

namespace Solarus {

/**
 * \brief Movement for an entity that looks for a path to another entity.
 *
 * This movement is typically used by enemies that try to reach the hero.
 * The entity tries to find a path and to avoid the obstacles on the way.
 * To this end, the PathFinding class (i.e. an implementation of the A* algorithm) is used.
 * If the target entity is too far or not reachable, the movement is a random walk.
 */
class SOLARUS_API PathFindingMovement: public PathMovement {

  public:

    explicit PathFindingMovement(int speed);

    void set_target(const EntityPtr& target);
    virtual bool is_finished() const override;

    virtual const std::string& get_lua_type_name() const override;

  protected:

    virtual void update() override;
    void recompute_movement();

  private:

    EntityPtr target;               /**< the entity targeted by this movement (usually the hero) */
    uint32_t next_recomputation_date;

};

}

#endif

