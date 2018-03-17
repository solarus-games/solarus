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
#ifndef SOLARUS_TARGET_MOVEMENT_H
#define SOLARUS_TARGET_MOVEMENT_H

#include "solarus/core/Common.h"
#include "solarus/core/Point.h"
#include "solarus/entities/EntityPtr.h"
#include "solarus/movements/StraightMovement.h"
#include <cstdint>
#include <string>

namespace Solarus {

/**
 * \brief Movement of an object that goes to a target point.
 *
 * The target point may be a fixed point or a moving entity.
 */
class TargetMovement: public StraightMovement {

  public:

    TargetMovement(
        const EntityPtr& target_entity,
        int x,
        int y,
        int moving_speed,
        bool ignore_obstacles
    );

    TargetMovement(
        const EntityPtr& target_entity,
        const Point& xy,
        int moving_speed,
        bool ignore_obstacles
    );

    void set_target(
        const EntityPtr& target_entity,
        const Point& xy
    );

    int get_moving_speed() const;
    void set_moving_speed(int moving_speed);

    void notify_object_controlled() override;
    void notify_position_changed() override;
    bool is_finished() const override;
    void update() override;

    virtual const std::string& get_lua_type_name() const override;

  private:

    void recompute_movement();
    void check_target_reached();

    Point target;                      /**< Coordinates of the point or entity to track. */
    EntityPtr target_entity;           /**< The entity to track (nullptr if only
                                        * a point is targeted) */
    Point entity_offset;               /**< Value to add to the entity's coordinates. */

    int sign_x;                        /**< Sign of the x movement (1: right, -1: left) */
    int sign_y;                        /**< Sign of the y movement (1: down, -1: up) */
    int moving_speed;                  /**< Speed when moving */

    uint32_t next_recomputation_date;  /**< Date when the movement is recalculated. */
    bool finished;                     /**< \c true if the target is reached. */
    bool recomputing_movement;         /**< Whether we are in \c recompute_movement(). */

};

}

#endif

