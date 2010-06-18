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
#ifndef SOLARUS_CIRCLE_MOVEMENT_H
#define SOLARUS_CIRCLE_MOVEMENT_H

#include "Common.h"
#include "movements/Movement.h"
#include "lowlevel/Rectangle.h"

/**
 * Movement for an entity that makes a circle around a center point or an entity.
 */
class CircleMovement: public Movement {

  public:

    /**
     * Possibles directions when making circles.
     */
    enum Direction {
      COUNTER_CLOCKWISE,
      CLOCKWISE
    };

  private:

    // center of the circles
    MapEntity *center_entity;                       /**< the entity to make circles around (NULL if only a point is used) */
    Rectangle center_point;                         /**< absolute coordinates of the center if only a point is used,
                                                     * or coordinates relative to the center entity otherwise */

    // angle
    int current_angle;                              /**< current angle in the circle in degrees */
    int initial_angle;                              /**< the first circle starts from this angle in degrees */
    int angle_increment;                            /**< number of degrees to add when the angle changes (1 or -1) */
    uint32_t next_angle_change_date;                /**< date when the angle changes */
    uint32_t angle_change_delay;                    /**< if not zero, time interval between two angle changes */

    // radius
    int current_radius;                             /**< current radius of the circle in pixels */
    int wanted_radius;                              /**< the current radius changes gradually towards this wanted value */
    int previous_radius;                            /**< radius before the movement stops */
    int radius_increment;                           /**< number of pixels to add when the radius is changing (1 or -1) */
    uint32_t next_radius_change_date;               /**< date of the next radius change */
    uint32_t radius_change_delay;                   /**< if not zero, time interval between two radius changes */

    // stop after an amount of time
    uint32_t duration;                              /**< if not zero, the movement will stop after this delay */
    uint32_t end_movement_date;                     /**< date when the movement stops */

    // stop after a number of rotations
    int max_rotations;                              /**< if not zero, the movement will stop after this number of rotations are done */
    int nb_rotations;                               /**< number of complete circles already done */

    // restart when stopped
    uint32_t loop_delay;                            /**< if not zero, when the movement finishes, it will start again after this delay */
    uint32_t restart_date;                          /**< date when the movement restarts */

    void recompute_position(void);

  public:

    // creation and destruction
    CircleMovement(void);
    ~CircleMovement(void);

    // state
    void update(void);
    void set_suspended(bool suspended);
    void start(void);
    void stop(void);
    bool is_started(void);

    // properties
    void set_center(const Rectangle &center_point);
    void set_center(MapEntity *center_entity, int x = 0, int y = 0);
    void set_radius(int radius, int speed = 0);
    void set_angle_speed(int angle_speed);
    void set_initial_angle(int initial_angle);
    void set_direction(Direction direction);
    void set_duration(uint32_t duration);
    void set_max_rotations(int max_rotations);
    void set_loop(uint32_t delay);

};

#endif

