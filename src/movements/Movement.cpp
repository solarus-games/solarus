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
#include "movements/Movement.h"
#include "entities/MapEntity.h"
#include "lowlevel/Geometry.h"
#include "lowlevel/System.h"
#include <cmath>

/**
 * @brief Constructor.
 */
Movement::Movement(void):
  x_speed(0), y_speed(0),
  next_move_date_x(System::now()),
  next_move_date_y(System::now()),
  x_move(0), y_move(0),
  suspended(false), when_suspended(0) {

  set_entity(NULL);
}

/**
 * @brief Destructor.
 */
Movement::~Movement(void) {

}

/**
 * @brief Sets the entity to be controlled by this movement object.
 *
 * This entity can be NULL if your movement applies to something else than a map entity.
 * However, some subclasses of Movement may require a non NULL entity because they
 * implement movements that depend on the map content (e.g. to handle the collisions).
 *
 * @param entity the entity to control
 */
void Movement::set_entity(MapEntity *entity) {
  this->entity = entity;

  if (entity == NULL) {
    this->x = 0;
    this->y = 0;
  }
  else {
    this->x = entity->get_x();
    this->y = entity->get_y();
  }
}

/**
 * @brief Returns the x position of the object controlled by this movement.
 * @return the x position of the object controlled by this movement
 */
int Movement::get_x(void) {

  if (entity == NULL) {
    return x;
  }

  return entity->get_x();
}

/**
 * @brief Returns the y position of the object controlled by this movement.
 * @return the y position of the object controlled by this movement
 */
int Movement::get_y(void) {

  if (entity == NULL) {
    return y;
  }

  return entity->get_y();
}

/**
 * @brief Sets the x position of the object controlled by this movement.
 * @param x the new x position
 */
void Movement::set_x(int x) {
  set_position(x, get_y());
}

/**
 * @brief Sets the y position of the object controlled by this movement.
 * @param y the new y position
 */
void Movement::set_y(int y) {
  set_position(get_x(), y);
}

/**
 * @brief Sets the position of the entity or the point controlled by this movement.
 * @param x the new x position
 * @param y the new y position
 */
void Movement::set_position(int x, int y) {

  if (entity != NULL) {
    entity->set_x(x);
    entity->set_y(y);
    entity->notify_position_changed();
  }
  else {
    this->x = x;
    this->y = y;
  }

  last_move_date = System::now();
}

/**
 * @brief Sets the position of the entity.
 * @param xy the new coordinates
 */
void Movement::set_position(const Rectangle &xy) {
  set_position(xy.get_x(), xy.get_y());
}

/**
 * @brief Moves the entity on x.
 * @param dx number of pixels of the move
 */
void Movement::translate_x(int dx) {
  translate(dx, 0);
}

/**
 * @brief Moves the entity on y.
 * @param dy number of pixels of the move
 */
void Movement::translate_y(int dy) {
  translate(0, dy);
}

/**
 * @brief Moves the entity.
 * @param dx number of pixels of the move on x
 * @param dy number of pixels of the move on y
 */
void Movement::translate(int dx, int dy) {
  set_position(get_x() + dx, get_y() + dy);
}

/**
 * @brief Returns the x speed of the entity.
 * @return the x speed of the entity, between -100 and 100
 */
double Movement::get_x_speed() {
  return x_speed;
}

/**
 * @brief Returns the y speed of the entity.
 * @return the y speed of the entity, between -100 and 100
 */
double Movement::get_y_speed() {
  return y_speed;
}

/**
 * @brief Returns the total speed of the entity.
 *
 * The speed is calculated as sqrt(x_speed^2 + y_speed^2).
 */
double Movement::get_speed(void) {
  return sqrt(x_speed * x_speed + y_speed * y_speed);
}

/**
 * @brief Sets the x speed.
 * @param x_speed the x speed of the entity, between -100 and 100
 */
void Movement::set_x_speed(double x_speed) {

  if (fabs(x_speed) <= 1E-6) {
    x_speed = 0;
  }

  this->x_speed = x_speed;
  uint32_t now = System::now();

  // compute x_delay, x_move and next_move_date_x
  if (x_speed == 0) {
    set_x_move(0);
  }
  else {
    if (x_speed > 0) {
      set_x_delay((uint32_t) (100 / x_speed));
      set_x_move(1);
    }
    else {
      set_x_delay((uint32_t) (100 / (-x_speed)));
      set_x_move(-1);
    }
    set_next_move_date_x(now + x_delay);
  }
}

/**
 * @brief Sets the y speed.
 * @param y_speed the y speed of the entity, between -100 and 100
 */
void Movement::set_y_speed(double y_speed) {

  if (fabs(y_speed) <= 1E-6) {
    y_speed = 0;
  }

  this->y_speed = y_speed;
  uint32_t now = System::now();

  // compute y_delay, y_move and next_move_date_y
  if (y_speed == 0) {
    set_y_move(0);
  }
  else {
    if (y_speed > 0) {
      set_y_delay((uint32_t) (100 / y_speed));
      set_y_move(1);
    }
    else {
      set_y_delay((uint32_t) (100 / (-y_speed)));
      set_y_move(-1);
    }
    set_next_move_date_y(now + y_delay);
  }
}

/**
 * @brief Changes the speed, keeping the same direction of the movement.
 *
 * x_speed and y_speed are recomputed so that the movement direction is unchanged.
 *
 * @param speed the new speed
 */
void Movement::set_speed(double speed) {

  if (x_speed == 0 && y_speed == 0) {
    x_speed = 1;
  }

  // compute the new speed vector
  double angle = Geometry::get_angle(0, 0, (int) (x_speed * 100), (int) (y_speed * 100)); // angle in radians
  set_x_speed(speed * cos(angle));
  set_y_speed(-speed * sin(angle));
}

/**
 * @brief Returns whether the speed is zero.
 * @return true if the entity is stopped, false otherwise
 */
bool Movement::is_stopped(void) {
  return !is_started();
}

/**
 * @brief Returns whether the speed is not zero.
 * @return true if the entity is moving, false otherwise
 */
bool Movement::is_started(void) {
  return x_speed != 0 || y_speed != 0;
}

/**
 * @brief Sets the speed to zero.
 */
void Movement::stop(void) {
  set_x_speed(0);
  set_y_speed(0);
  set_x_move(0);
  set_y_move(0);
}

/**
 * @brief Returns whether this movement is finished.
 *
 * You can redefine this function if your movement has an end.
 *
 * @return true if this movement is finished
 */
bool Movement::is_finished(void) {
  return false;
}

/**
 * @brief Changes the direction of the movement vector, keeping the same speed.
 *
 * x_speed and y_speed are recomputed so that the total speed is unchanged.
 * Warning: if x_speed and y_speed are both equal to zero, this function
 * stops the program on an error message.
 *
 * @param direction the new movement direction, between 0 and 359
 */
void Movement::set_direction(int direction) {

  double angle = Geometry::degrees_to_radians(direction);
  set_direction(angle);
}

/**
 * @brief Changes the direction of the movement vector, keeping the same speed.
 *
 * x_speed and y_speed are recomputed so that the total speed is unchanged.
 * Warning: if x_speed and y_speed are both equal to zero, this function
 * stops the program on an error message.
 *
 * @param angle the new movement direction in radians
 */
void Movement::set_direction(double angle) {

  if (x_speed == 0 && y_speed == 0) {
    if (entity == NULL) {
      DIE("Cannot set the direction when the speed is zero");
    }
    else {
      DIE("Cannot set the direction when the speed is zero for entity '" << entity->get_name() << "'");
    }
  }

  double speed = get_speed();
  set_x_speed(speed * cos(angle));
  set_y_speed(-speed * sin(angle));
}

/**
 * @brief Returns true if the entity is about to try to move
 * on the next update().
 *
 * This function returns true if x_move is not equal to zero
 * and next_move_date_x is past, or the same thing for y.
 *
 * @return true if the entity is about to try to move
 */
bool Movement::has_to_move_now(void) {

  uint32_t now = System::now();

  return (x_move != 0 && now >= next_move_date_x)
    || (y_move != 0 && now >= next_move_date_y);
}

/**
 * @brief Returns whether the movement is suspended.
 * @return true if the movement is suspended
 */
bool Movement::is_suspended(void) {
  return suspended;
}

/**
 * @brief Suspends or resumes the movement.
 *
 * This function is called by the entity when the game is suspended or resumed.
 *
 * @param suspended true to suspend the movement, false to resume it
 */
void Movement::set_suspended(bool suspended) {

  this->suspended = suspended;

  uint32_t now = System::now();

  if (suspended) {
    // the movement is being suspended
    when_suspended = now;
  }
  else {
    // recalculate the next move date
    if (when_suspended != 0) {
      next_move_date_x += now - when_suspended;
      next_move_date_y += now - when_suspended;
    }
  }
}

/**
 * @brief Updates the x position of the entity if it has to be changed.
 */
void Movement::update_x(void) {

  if (x_move != 0) { // if we want to move on x

    // update the x position while next_move_date_x is past
    uint32_t now = System::now();
    while (now >= next_move_date_x) {
      translate_x(x_move);
      set_next_move_date_x(next_move_date_x + x_delay);
    }
  }
}

/**
 * @brief Updates the y position of the entity if it has changed.
 */
void Movement::update_y(void) {

  if (y_move != 0) { // if we want to move on y

    // update the x position while next_move_date_y is past
    uint32_t now = System::now();
    while (now >= next_move_date_y) {
      translate_y(y_move);
      set_next_move_date_y(next_move_date_y + y_delay);
    }
  }
}

/**
 * @brief Updates the entity's position.
 *
 * This function is called repeteadly by the map.
 * By default, it calls update_x() and update_y().
 * You can redefine this function.
 */
void Movement::update(void) {

  if (!is_suspended()) {
    uint32_t now = System::now();

    bool x_move_now = get_x_move() != 0 && now >= get_next_move_date_x();
    bool y_move_now = get_y_move() != 0 && now >= get_next_move_date_y();

    while (x_move_now || y_move_now) { // while it's time to move

      if (x_move_now) {
	// it's time to make an x move

	if (y_move_now) {
	  // but it's also time to make a y move

	  if (get_next_move_date_x() <= get_next_move_date_y()) {
	    // x move first
	    update_x();
	    update_y();
	  }
	  else {
	    // y move first
	    update_y();
	    update_x();
	  }
	}
	else {
	  update_x();
	}
      }
      else if (y_move_now) {
	update_y();
      }

      now = System::now();
      x_move_now = get_x_move() != 0 && now >= get_next_move_date_x();
      y_move_now = get_y_move() != 0 && now >= get_next_move_date_y();
    }
  }
}

/**
 * @brief Returns the collision box of the last collision check that detected an obstacle.
 *
 * This function is useful only for subclasses of Movement that handle collisions.
 *
 * @return the collision box of the last collision detected, or (-1, -1) if no obstacle was detected
 */
const Rectangle & Movement::get_last_collision_box_on_obstacle(void) {
  static const Rectangle collision_box(-1, -1);
  return collision_box;
}

