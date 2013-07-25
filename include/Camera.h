/*
 * Copyright (C) 2006-2013 Christopho, Solarus - http://www.solarus-games.org
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
 * \brief Manages the visible area of the map.
 *
 * The camera determines the visible area of the map.
 * Most of the time, the camera is centered on the hero movements,
 * blocking on the map borders and on camera stoppers.
 *
 * Occasionally, it can also be moved towards a point and then restored
 * towards the hero.
 */
class Camera {

  public:

    Camera(Map& map);
    ~Camera();

    void update();
    const Rectangle& get_position();

    bool is_moving();
    void set_speed(int speed);
    void move(int target_x, int target_y);
    void move(MapEntity& entity);
    void restore();

  private:

    int get_width();
    int get_height();

    void update_fixed_on_hero();
    void update_moving();

    Map& map;                     /**< The map. */
    Rectangle position;           /**< Visible area of the camera on the map. */

    // Camera centered on the hero.
    bool fixed_on_hero;           /**< \c true if the camera is fixed on the hero. */
    Rectangle stopper_scrolling_position;
    Rectangle stopper_target_position;
    int stopper_scrolling_dx;
    int stopper_scrolling_dy;
    uint32_t stopper_next_scrolling_date;

    // Camera being moved toward a point or back to the hero.
    bool restoring;               /**< \c true if the camera is moving back to the hero. */
    int speed;                    /**< Speed of the next movement. */
    TargetMovement* movement;     /**< Movement of the camera, or NULL for no movement. */
};

#endif

