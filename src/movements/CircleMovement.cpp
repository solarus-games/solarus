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
#include "entities/MapEntity.h"

/**
 * Creates a circle movement.
 */
CircleMovement::CircleMovement(void):
  center_entity(NULL), current_angle(0), initial_angle(0), angle_increment(1), next_angle_change_date(0), angle_change_delay(5),
  current_radius(0), wanted_radius(0), previous_radius(0), next_radius_change_date(0), radius_change_delay(0),
  duration(0), end_movement_date(0), max_rotations(0), loop_delay(0), restart_date(0) {

}

/**
 * Destroys this circle movement.
 */
CircleMovement::~CircleMovement(void) {

}

/**
 * Sets the center of the circles.
 * The movement will make circles around the specified fixed point.
 * @param center_point center of the circles to make
 */
void CircleMovement::set_center(const Rectangle &center_point) {

  this->center_entity = NULL;
  this->center_point = center_point;
  recompute_position();
}

/**
 * Sets the center of the circles.
 * The movement will make circles around the specified (possibly moving) entity.
 * @param center_entity the entity around which you want to make circles
 * @param x x coordinate of where the center should be placed relative to the entity's origin
 * @param y y coordinate of where the center should be placed relative to the entity's origin
 */
void CircleMovement::set_center(MapEntity *center_entity, int x, int y) {

  if (center_entity == NULL) {
    DIE("The center entity is NULL");
  }

  this->center_entity = center_entity;
  this->center_point.set_xy(x, y);
  recompute_position();
}

/**
 * Sets the radius of the circles.
 * @param radius the radius in pixels
 */
void CircleMovement::set_radius(int radius) {

  if (radius < 0) {
    DIE("Invalid radius: " << radius);
  }

  this->wanted_radius = radius;
  if (radius_change_delay == 0) {
    if (is_started()) {
      this->current_radius = wanted_radius;
    }
  }
  else {
    this->radius_increment = (radius > this->current_radius) ? 1 : -1;
    if (is_started()) {
      this->next_radius_change_date = System::now();
    }
  }
  recompute_position();
}

/**
 * Sets the radius to be updated immediately or gradually towards its wanted value when you call set_radius().
 * @param radius_speed speed of the radius variation (number of pixels per second), or 0 to update it immediately
 */
void CircleMovement::set_radius_speed(int radius_speed) {

  if (radius_speed < 0) {
    DIE("Invalid speed: " << radius_speed);
  }

  if (radius_speed == 0) {
    this->radius_change_delay = 0;
  }
  else {
    this->radius_change_delay = 1000 / radius_speed;
  }

  set_radius(wanted_radius);
}


/**
 * Sets the speed of the angle variation.
 * @param angle_speed number of degrees to make per second
 */
void CircleMovement::set_angle_speed(int angle_speed) {

  if (angle_speed <= 0) {
    DIE("Invalid angle speed: " << angle_speed);
  }

  this->angle_change_delay = 1000 / angle_speed;
  this->next_angle_change_date = System::now();
  recompute_position();
}

/**
 * Sets the angle where the first circle starts from.
 * @param initial_angle angle in degrees (0 to 359)
 */
void CircleMovement::set_initial_angle(int initial_angle) {

  if (initial_angle < 0 || initial_angle >= 360) {
    DIE("Invalid initial angle: " << initial_angle);
  }

  this->initial_angle = initial_angle;
}

/**
 * Sets the direction of the circles.
 * @param direction CLOCKWISE or COUNTER_CLOCKWISE 
 */
void CircleMovement::set_direction(Direction direction) {

  this->angle_increment = (direction == CLOCKWISE) ? -1 : 1;
}

/**
 * Sets the maximum duration of the movement.
 * When this delay is reached, the movement stops.
 * Note that is the radius changes gradually, the movement will continue
 * for a while until the radius reaches zero.
 * @param duration duration of the movement in milliseconds, or 0 to make it infinite
 */
void CircleMovement::set_duration(uint32_t duration) {

  this->duration = duration;
  if (duration != 0 && is_started()) {
    this->end_movement_date = System::now() + duration;
  }
}

/**
 * Sets the number of rotations of the movement.
 * When this number of rotations is reached, the movement stops.
 * Note that is the radius changes gradually, the movement will continue
 * for a while until the radius reaches zero.
 * @param max_rotations number of rotations to make (0 for infinite rotations)
 */
void CircleMovement::set_max_rotations(int max_rotations) {

  if (max_rotations < 0) {
    DIE("Invalid maximum rotations number: " << max_rotations);
  }

  this->max_rotations = max_rotations;
  this->nb_rotations = 0;
}

/**
 * Makes the movement restart after a delay when it is finished.
 * @param delay the movement will restart after this delay in milliseconds (0 to avoid this)
 */
void CircleMovement::set_loop(uint32_t delay) {

  this->loop_delay = delay;
  if (delay != 0 && is_stopped()) {
    this->restart_date = System::now() + delay;
  }
}

/**
 * Updates the movement.
 */
void CircleMovement::update(void) {
  Movement::update();

  if (is_suspended()) {
    return;
  }

  bool update_needed = false;
  uint32_t now = System::now();

  // maybe it is time to stop or to restart
  if (current_radius != 0 && duration != 0 && now >= end_movement_date && wanted_radius != 0) {
    stop();
  }
  else if (current_radius == 0 && loop_delay != 0 && now >= restart_date) {
    set_radius(previous_radius);
    start();
  }

  // update the angle
  if (is_started()) {
    while (now >= next_angle_change_date) {

      current_angle += angle_increment;
      current_angle = (360 + current_angle) % 360;
      if (current_angle == initial_angle) {
	nb_rotations++;

	if (nb_rotations == max_rotations) {
	  stop();
	}
      }

      next_angle_change_date += angle_change_delay;
      update_needed = true;
    }
  }

  // update the radius
  while (current_radius != wanted_radius && now >= next_radius_change_date) {

    current_radius += radius_increment;

    next_radius_change_date += radius_change_delay;
    update_needed = true;
  }

  // the center may have moved
  if (center_entity != NULL) {
    update_needed = true;
  }

  if (update_needed) {
    recompute_position();
  }
}

/**
 * Computes the position of the object controlled by this movement.
 * This function should be called whenever the angle, the radius or the center changes.
 */
void CircleMovement::recompute_position(void) {

  Rectangle center = this->center_point;
  if (center_entity != NULL) {
    center.add_xy(center_entity->get_xy());
  }

  const Rectangle &xy = Geometry::get_xy(center, Geometry::degrees_to_radians(current_angle), current_radius);
  set_position(xy);
}

/**
 * Suspends or resumes the movement
 * @param suspended true to suspend the movement, false to resume it
 */
void CircleMovement::set_suspended(bool suspended) {
  Movement::set_suspended(suspended);

  if (!suspended) {
    uint32_t diff = System::now() - when_suspended;
    next_angle_change_date += diff;
    next_radius_change_date += diff;
    end_movement_date += diff;
    restart_date += diff;
  }
}

/**
 * Starts the circle movement.
 */
void CircleMovement::start(void) {

  current_angle = initial_angle;
  next_angle_change_date = System::now();
  nb_rotations = 0;

  if (duration != 0) {
    end_movement_date = System::now() + duration;
  }

  if (radius_change_delay == 0) {
    current_radius = wanted_radius;
  }
  else {
    next_radius_change_date = System::now();
  }
  recompute_position();
}

/**
 * Returns whether this movement is running.
 * @return true if the movement is started
 */
bool CircleMovement::is_started(void) {
  return current_radius != 0 || wanted_radius != 0;
}

/**
 * Stops the movement.
 */
void CircleMovement::stop(void) {

  previous_radius = current_radius;
  set_radius(0);

  if (loop_delay != 0) {
    restart_date = System::now() + loop_delay;
  }
  recompute_position();
}

