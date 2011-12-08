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
#ifndef SOLARUS_FOLLOW_MOVEMENT_H
#define SOLARUS_FOLLOW_MOVEMENT_H

#include "Common.h"
#include "movements/Movement.h"

/**
 * @brief Movement of an entity attached to another entity.
 *
 * At each moment, the entity is placed at a position
 * relative to the entity followed.
 * If the movement is sensible to obstacles, it becomes finished as
 * soon as the entity cannot be placed at the requested position.
 */
class FollowMovement: public Movement {

  private:

    MapEntity *entity_followed;     /**< the entity followed by this movement */
    const int x;                    /**< x coordinate of where this entity should be placed,
                                     * relative to the entity followed */
    const int y;                    /**< y coordinate of where this entity should be placed,
                                     * relative to the entity followed */

    bool finished;                  /**< indicates that the movement is stopped because of obstacles */

  protected:

    void update();

  public:

    FollowMovement(MapEntity *entity_followed, int x, int y, bool ignore_obstacles);
    ~FollowMovement();

    bool is_finished();
    const Rectangle get_displayed_xy();
};

#endif

