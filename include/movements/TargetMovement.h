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
#ifndef SOLARUS_TARGET_MOVEMENT_H
#define SOLARUS_TARGET_MOVEMENT_H

#include "Common.h"
#include "movements/RectilinearMovement.h"

/**
 * @brief Movement of an object that goes to a target point.
 *
 * The target point may be a fixed point or a moving entity.
 *
 * Properties:
 * - speed
 * - displayed_direction (read-only)
 */
class TargetMovement: public RectilinearMovement {

  protected:

    int target_x;				/**< x coordinate of the point to track */
    int target_y;				/**< y coordinate of the point to track */
    MapEntity *target_entity;			/**< the entity to track (can be NULL if only a point is targeted) */

    int sign_x;					/**< sign of the x movement (1: right, -1: left) */
    int sign_y;					/**< sign of the y movement (1: down, -1: up) */
    int speed;					/**< movement's speed */

    uint32_t next_recomputation_date;		/**< date when the movement is recalculated */
    bool finished;				/**< true if the target is reached */

    void recompute_movement();

  public:

    TargetMovement(int target_x, int target_y, int speed);
    TargetMovement(MapEntity *target_entity, int speed);
    ~TargetMovement();

    void set_target(int target_x, int target_y);
    void set_target(MapEntity *target_entity);

    bool is_finished();
    void update();

    // properties
    virtual const std::string get_property(const std::string &key);
    virtual void set_property(const std::string &key, const std::string &value);
};

#endif

