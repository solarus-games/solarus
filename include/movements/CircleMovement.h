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

  private:

    // movement info
    int radius;                                     /**< circle radius in pixels */
    Rectangle center_point;                         /**< coordinates of the center of the circles */
    MapEntity *center_entity;                       /**< the entity to circle around (NULL if only a point is used) */

    // state
    int current_angle;                           /**< */
    int angle_increment;
    uint32_t next_angle_change_date;
    uint32_t angle_change_delay;

  public:

    CircleMovement(void);
    ~CircleMovement(void);

    void update(void);
    void set_suspended(bool suspended);
    bool is_finished(void);
    void set_finished(void);

    void start(int angle_speed, int radius, const Rectangle &center_point);
    void start(int angle_speed, int radius, MapEntity *center_entity, int x = 0, int y = 0);
};

#endif

