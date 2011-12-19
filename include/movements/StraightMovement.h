/*
 * Copyright (C) 2009-2011 Christopho, Solarus - http://www.solarus-engine.org
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
#ifndef SOLARUS_STRAIGHT_MOVEMENT_H
#define SOLARUS_STRAIGHT_MOVEMENT_H

#include "Common.h"
#include "movements/Movement.h"

/**
 * @brief A straight movement represented as a speed vector
 * whose properties (speed and angle) can be changed.
 *
 * Properties:
 * - speed
 * - angle
 * - max_distance
 * - ignore_obstacles
 * - smooth
 * - displayed_direction (read-only)
 */
class StraightMovement: public Movement {

  private:

    // speed vector
    double angle;                /**< angle between the speed vector and the horizontal axis in radians */
    double x_speed;              /**< X speed of the object to move in pixels per second.
                                  * 0: stopped
                                  * positive value: moving to the right
                                  * negative value: moving to the left */
    double y_speed;              /**< Y speed of the object to move in pixels per second.
                                  * 0: stopped
                                  * positive value: moving downwards
                                  * negative value: moving upwards */

    uint32_t next_move_date_x;   /**< Date of the next x move in ticks. */
    uint32_t next_move_date_y;   /**< Date of the next y move in ticks. */

    // the following fields are redundant and can be computed from x_speed and y_speed
    uint32_t x_delay;            /**< Delay in ticks between an x move of 1 pixel.
                                  * x_delay = 200 / |x_speed| */
    uint32_t y_delay;            /**< Delay in ticks between an y move of 1 pixel.
                                  * y_delay = 200 / |y_speed| */
    int x_move;                  /**< Number of pixels of the next x move : 0, 1 or -1. */
    int y_move;                  /**< Number of pixels of the next y move : 0, 1 or -1. */

    Rectangle initial_xy;        /**< Initial position when the movement started
                                  * (reset whenever the speed of the angle changes) */
    int max_distance;            /**< After this distance in pixels from the initial position,
                                  * or when an obstacle is reached, the movement will stop
                                  * (0 means no limit) */
    bool finished;               /**< When max_distance is not zero, indicates
                                  * that max_distance or an obstacle is reached */
    bool smooth;                 /**< Makes the movement adjust its trajectory
                                  * when an obstacle is close */

  protected:

    void set_next_move_date_x(uint32_t next_move_date_x);
    void set_next_move_date_y(uint32_t next_move_date_y);

    void update_x();
    void update_smooth_x();
    void update_non_smooth_x();
    void update_y();
    void update_smooth_y();
    void update_non_smooth_y();

  public:

    StraightMovement(bool ignore_obstacles, bool smooth);
    virtual ~StraightMovement();

    virtual void set_entity(MapEntity* entity);
    virtual void update();
    virtual void set_suspended(bool suspended);
    bool has_to_move_now();

    // speed vector
    double get_x_speed();
    double get_y_speed();
    double get_speed();
    void set_x_speed(double x_speed);
    void set_y_speed(double y_speed);
    void set_speed(double speed);
    double get_angle();
    void set_angle(double angle);
    int get_max_distance();
    void set_max_distance(int max_distance);
    bool is_smooth();
    void set_smooth(bool smooth);
    int get_displayed_direction4();

    // movement
    virtual bool is_started();
    bool is_finished();
    void set_finished();
    void stop();

    // properties
    virtual const std::string get_property(const std::string &key);
    virtual void set_property(const std::string &key, const std::string &value);
};

#endif

