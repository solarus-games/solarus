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
#ifndef SOLARUS_CAMERA_H
#define SOLARUS_CAMERA_H

#include "Common.h"
#include "lowlevel/Rectangle.h"

/**
 * @brief Manages the visible area of the map.
 *
 * The camera determines the visible area of the map.
 * Most of the time, the camera follows the hero movements.
 * Occasionaly, it can be moved towards a point and then restored towards the hero.
 */
class Camera {

  private:

    Map *map;                 /**< the map */
    bool fixed_on_hero;       /**< true if the camera is fixed on the hero */
    bool restoring;           /**< true if the camera is moving back to the hero */
    Rectangle position;       /**< position of the camera on the map */
    int speed;                /**< speed of the movement */
    TargetMovement *movement; /**< movement of the camera, or NULL for no movement */

  public:

    Camera(Map *map);
    ~Camera();

    void update();
    Rectangle & get_position();

    bool is_fixed_on_hero();
    void set_speed(int speed);
    void move(int target_x, int target_y);
    void move(MapEntity *entity);
    void restore();
};

#endif

