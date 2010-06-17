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
#include "movements/CircleMovement.h"
#include "lowlevel/System.h"
#include "lowlevel/Geometry.h"
#include <cmath> // remove

/**
 * Creates a circle movement.
 */
CircleMovement::CircleMovement(void) {

}

/**
 * Destroys this circle movement.
 */
CircleMovement::~CircleMovement(void) {

}

/**
 * Updates the movement.
 */
void CircleMovement::update(void) {
  Movement::update();

  // update the angle
  bool angle_changed = false;
  uint32_t now = System::now();
  while (now >= next_angle_change_date) {
    current_angle += angle_increment;
    next_angle_change_date += angle_change_delay;
    angle_changed = true;
  }

  if (angle_changed || center_entity != NULL) {
    // compute the new position
    double radians = current_angle * Geometry::TWO_PI / 360.0;
    int dx = radius * cos(radians);
    int dy = -radius * sin(radians); // TODO put this in the Geometry class

    set_position(center_point.get_x() + dx, center_point.get_y() + dy);
  }
}

/**
 * Suspends or resumes the movement
 * @param suspended true to suspend the movement, false to resume it
 */
void CircleMovement::set_suspended(bool suspended) {
  Movement::set_suspended(suspended);
  // TODO suspend the angle change
}

/**
 * Returns whether the movement is finished.
 * @return true if the movement is finished
 */
bool CircleMovement::is_finished(void) {
  return false;
}

/**
 * Stops the movement right now.
 */
void CircleMovement::set_finished(void) {
  // TODO
}

/**
 * Starts the circle movement around a fixed center point.
 * @param angle_speed number of degrees per second
 * @param center_point center of the circles to make
 * @param radius radius of the circle in pixels
 */
void CircleMovement::start(int angle_speed, int radius, const Rectangle &center_point) {

  this->radius = radius;
  this->center_point = center_point;

  this->current_angle = 0;
  this->angle_increment = 1;
  this->next_angle_change_date = System::now();
  this->angle_change_delay = 1000 / angle_speed;
}

/**
 * Starts the circle movement around an entity.
 * If the entity moves, the center of the circle will follow the movement.
 * @param angle_speed the speed
 * @param radius radius of the circle in pixels
 * @param center_entity the entity around which you want to make circles
 * @param x x coordinate of where the center of the circle entity should be placed (relative to the entity's origin)
 * @param y y coordinate of where the center of the circle entity should be placed (relative to the entity's origin)
 */
void CircleMovement::start(int angle_speed, int radius, MapEntity *center_entity, int x, int y) {
  // TODO
}


