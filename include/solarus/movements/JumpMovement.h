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
#ifndef SOLARUS_JUMP_MOVEMENT_H
#define SOLARUS_JUMP_MOVEMENT_H

#include "solarus/core/Common.h"
#include "solarus/movements/PixelMovement.h"
#include <string>

namespace Solarus {

/**
 * \brief Movement of an entity that jumps towards a direction.
 *
 * TODO: inherit StraightMovement instead?
 */
class JumpMovement: public PixelMovement {

  public:

    JumpMovement(int direction8, int distance, int speed, bool ignore_obstacles);

    int get_direction8() const;
    void set_direction8(int direction8);
    int get_distance() const;
    void set_distance(int distance);
    int get_speed() const;
    void set_speed(int speed);

    virtual int get_displayed_direction4() const override;
    virtual Point get_displayed_xy() const override;

    virtual const std::string& get_lua_type_name() const override;

  protected:

    virtual void notify_step_done(int step_index, bool success) override;

  private:

    void restart();

    static const std::string basic_trajectories[];  /**< one-pixel trajectory of each direction */

    // properties
    int direction8;                                 /**< direction of the jump (0 to 7) */
    int distance;                                   /**< jump length in pixels */
    int speed;                                      /**< speed in pixels per second */

    // state
    int jump_height;                                /**< current height of the object while jumping */

};

}

#endif

