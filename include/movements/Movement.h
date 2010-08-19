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
#ifndef SOLARUS_MOVEMENT_H
#define SOLARUS_MOVEMENT_H

#include "Common.h"

/**
 * @brief Base class for representing a movement.
 *
 * This is the parent class of all kinds of movement.
 * It implements a simple speed vector and provides basic functions
 * such as setting the speed and the direction of a straight movement.
 * Subclassses can use this speed vector and extend it or implement totally different movements.
 *
 * An instance of Movement can be applied to a map entity during the game,
 * or to any arbitrary coordinates.
 * A moving map entity has an instance of Movement that modifies its coordinates.
 */
class Movement {

  protected:

    MapEntity *entity;         /**< The entity controlled by this movement.
                                * If it is NULL, the movement is applied to the internal x and y fields below. */

  private:

    int x;                     /**< X coordinate of the object controlled by this movement. */
    int y;                     /**< Y coordinate of the object controlled by this movement. */

    // speed vector

    double x_speed;            /**< X speed of the entity, between -100 and 100.
				* 0: stopped
				* positive value: moving to the right
				* negative value: moving to the left */

    double y_speed;            /**< Y speed of the entity, between -100 and 100.
				* 0: stopped
				* positive value: moving downwards
				* negative value: moving upwards */

    uint32_t next_move_date_x; /**< Date of the next x move in ticks. */
    uint32_t next_move_date_y; /**< Date of the next y move in ticks. */

    uint32_t last_move_date;   /**< Date of the last x or y move. */

    // the following fields are redundant and can be computed
    // with x_speed and y_speed

    uint32_t x_delay;          /**< Delay in ticks between an x move of 1 pixel.
				* x_delay = 200 / |x_speed|  */
    uint32_t y_delay;          /**< Delay in ticks between an y move of 1 pixel.
				* y_delay = 200 / |y_speed|
				*/

    int x_move;                /**< Number of pixels of the next x move : 0, 1 or -1. */
    int y_move;                /**< Number of pixels of the next y move : 0, 1 or -1. */

    bool suspended;            /**< Indicates whether the movement is suspended. */

  protected:

    uint32_t when_suspended;   /**< Indicates when the movement is currently suspended. */

    inline int get_x_move(void)        { return x_move; }
    inline int get_y_move(void)        { return y_move; }
    inline void set_x_move(int x_move) { this->x_move = x_move; }
    inline void set_y_move(int y_move) { this->y_move = y_move; }

    inline uint32_t get_next_move_date_x(void)                  { return next_move_date_x; }
    inline uint32_t get_next_move_date_y(void)                  { return next_move_date_y; }
    void set_next_move_date_x(uint32_t next_move_date_x);
    void set_next_move_date_y(uint32_t next_move_date_y);

    inline uint32_t get_x_delay(void)         { return x_delay; }
    inline uint32_t get_y_delay(void)         { return y_delay; }
    inline void set_x_delay(uint32_t x_delay) { this->x_delay = x_delay; }
    inline void set_y_delay(uint32_t y_delay) { this->y_delay = y_delay; }

    void translate_x(int dx);
    void translate_y(int dy);
    void translate(int dx, int dy);

    virtual void update_x(void);
    virtual void update_y(void);

  public:

    Movement(void);
    virtual ~Movement(void);

    // entity
    virtual void set_entity(MapEntity *entity);

    // position
    int get_x(void);
    int get_y(void);
    void set_x(int x);
    void set_y(int y);
    virtual void set_position(int x, int y);
    void set_position(const Rectangle &xy);
    virtual void notify_position_changed(void);

    virtual void update(void);
    bool has_to_move_now(void);

    // movement

    double get_x_speed();
    double get_y_speed();
    double get_speed(void);
    bool is_stopped(void);
    virtual bool is_started(void);
    virtual bool is_finished(void);

    void set_x_speed(double x_speed);
    void set_y_speed(double y_speed);
    virtual void set_speed(double speed);
    void stop(void);

    bool is_suspended(void);
    virtual void set_suspended(bool suspended);

    void set_direction(int direction);
    void set_direction(double angle);

    // collisions
    virtual const Rectangle & get_last_collision_box_on_obstacle(void);
};

#endif

