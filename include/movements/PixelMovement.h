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
#ifndef SOLARUS_PIXEL_MOVEMENT_H
#define SOLARUS_PIXEL_MOVEMENT_H

#include "Common.h"
#include "movements/Movement.h"
#include "lowlevel/Rectangle.h"
#include <list>

/**
 * @brief Movement of an entity that follows a predetermined sequence of pixel-precise moves.
 *
 * Properties:
 * - trajectory
 * - delay
 * - loop
 * - ignore_obstacles
 */
class PixelMovement: public Movement {

  private:

    // movement properties

    std::list<Rectangle> trajectory;				/**< The trajectory. Each element of the
					 			 * represents a move in pixels (only the
					 			 * x and y fields of the Rectangle are used). */
    std::string trajectory_string;				/**< String representation of the trajectory, like "dx1 dy1  dx2 dy2  dx3 dy3 ..." */
    uint32_t next_move_date;					/**< Date of the next move */
    uint32_t delay;						/**< Delay in milliseconds between two translations. */
    bool loop;							/**< Should the trajectory return to the beginning once finished? */

    // current state

    std::list<Rectangle>::iterator trajectory_iterator;		/**< Current element of the trajectory. */
    int nb_steps_done;						/**< Number of steps already done in the trajectory */
    bool finished;						/**< Indicates whether the object has reached the end of the trajectory
								 * (only possible when loop is false). */

    void make_next_step();

  protected:

    virtual void notify_step_done(int step_index, bool success);

  public:

    // creation and destruction
    PixelMovement(const std::string &trajectory_string, uint32_t delay, bool loop, bool ignore_obstacles);
    virtual ~PixelMovement();

    // properties
    const std::string &get_trajectory();
    void set_trajectory(const std::list<Rectangle> &trajectory);
    void set_trajectory(const std::string &trajectory_string);
    uint32_t get_delay();
    void set_delay(uint32_t delay);
    bool get_loop();
    void set_loop(bool loop);
    bool get_ignore_obstacles();
    void set_ignore_obstacles(bool ignore_obstacles);

    // state
    bool is_started();
    virtual bool is_finished();
    int get_length();
    void restart();

    virtual void update();
    virtual void set_suspended(bool suspended);

    // properties
    virtual const std::string get_property(const std::string &key);
    virtual void set_property(const std::string &key, const std::string &value);
};

#endif

