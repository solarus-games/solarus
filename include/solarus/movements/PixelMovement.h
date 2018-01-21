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
#ifndef SOLARUS_PIXEL_MOVEMENT_H
#define SOLARUS_PIXEL_MOVEMENT_H

#include "solarus/core/Common.h"
#include "solarus/core/Point.h"
#include "solarus/movements/Movement.h"
#include <cstdint>
#include <list>
#include <string>

namespace Solarus {

/**
 * \brief Movement of an entity that follows a predetermined sequence of pixel-precise moves.
 */
class SOLARUS_API PixelMovement: public Movement {

  public:

    // creation and destruction
    PixelMovement(const std::string& trajectory_string, uint32_t delay, bool loop, bool ignore_obstacles);

    // properties
    const std::list<Point>& get_trajectory() const;
    void set_trajectory(const std::list<Point>& trajectory);
    void set_trajectory(const std::string &trajectory_string);
    uint32_t get_delay() const;
    void set_delay(uint32_t delay);
    bool get_loop() const;
    void set_loop(bool loop);

    // state
    virtual bool is_started() const override;
    virtual bool is_finished() const override;
    int get_length() const;

    virtual void update() override;
    virtual void set_suspended(bool suspended) override;

    virtual const std::string& get_lua_type_name() const override;

  protected:

    virtual void notify_step_done(int step_index, bool success);

  private:

    void make_next_step();
    void restart();

    // movement properties

    std::list<Point> trajectory;       /**< The trajectory. Each element of the
                                        * represents a move in pixels. */
    std::string trajectory_string;     /**< String representation of the trajectory, like "dx1 dy1  dx2 dy2  dx3 dy3 ..." */
    uint32_t next_move_date;           /**< Date of the next move */
    uint32_t delay;                    /**< Delay in milliseconds between two translations. */
    bool loop;                         /**< Should the trajectory return to the beginning once finished? */

    // current state

    std::list<Point>::iterator
        trajectory_iterator;           /**< Current element of the trajectory. */
    int nb_steps_done;                 /**< Number of steps already done in the trajectory */
    bool finished;                     /**< Indicates whether the object has reached the end of the trajectory
                                        * (only possible when loop is false). */

};

}

#endif

