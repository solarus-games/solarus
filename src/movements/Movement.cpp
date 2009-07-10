/*
 * Copyright (C) 2009 Christopho, Zelda Solarus - http://www.zelda-solarus.com
 * 
 * Zelda: Mystery of Solarus DX is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Zelda: Mystery of Solarus DX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <cmath>
#include "movements/Movement.h"
#include "entities/MapEntity.h"

/**
 * Mathematic constants.
 * We don't use the constants from math.h
 * because they are not ANSI.
 */
const double PI = 3.14159265358979323846;
const double TWO_PI = 2.0 * PI;
const double PI_OVER_2 = PI / 2.0;
const double THREE_PI_OVER_2 = 3 * PI_OVER_2;
const double SQRT_2 = 1.41421356237309504880;

/**
 * Constructor.
 */
Movement::Movement(void):
  x_speed(0), y_speed(0),
  next_move_date_x(SDL_GetTicks()),
  next_move_date_y(SDL_GetTicks()),
  x_move(0), y_move(0),
  suspended(false), when_suspended(0) {

  set_entity(NULL);
}

/**
 * Destructor.
 */
Movement::~Movement(void) {

}

/**
 * Sets the entity to be controlled by this movement object.
 * This entity can be NULL if your movement applies to something else than a map entity.
 * However, some subclasses of Movement may require a non NULL entity because they
 * implement movements that depend on the map content (e.g. to handle the collisions).
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
 * Returns the x position of the object controlled by this movement.
 * @return the x position of the object controlled by this movement
 */
int Movement::get_x(void) {

  if (entity == NULL) {
    return x;
  }

  return entity->get_x();
}

/**
 * Returns the y position of the object controlled by this movement.
 * @return the y position of the object controlled by this movement
 */
int Movement::get_y(void) {

  if (entity == NULL) {
    return y;
  }

  return entity->get_y();
}

/**
 * Sets the x position of the object controlled by this movement.
 * @param x the new x position
 */
void Movement::set_x(int x) {
  set_position(x, get_y());
}

/**
 * Sets the y position of the object controlled by this movement.
 * @param y the new y position
 */
void Movement::set_y(int y) {
  set_position(get_x(), y);
}

/**
 * Sets the position of the entity.
 * @param x the new x position
 * @param y the new y position
 */
void Movement::set_position(int x, int y) {

  if (entity != NULL) {
    entity->set_x(x);
    entity->set_y(y);
    entity->just_moved();
  }
  else {
    this->x = x;
    this->y = y;
  }

  last_move_date = SDL_GetTicks();
}

/**
 * Moves the entity on x.
 * @param dx number of pixels of the move
 */
void Movement::translate_x(int dx) {
  translate(dx, 0);
}

/**
 * Moves the entity on y.
 * @param dy number of pixels of the move
 */
void Movement::translate_y(int dy) {
  translate(0, dy);
}

/**
 * Moves the entity.
 * @param dx number of pixels of the move on x
 * @param dy number of pixels of the move on y
 */
void Movement::translate(int dx, int dy) {
  set_position(get_x() + dx, get_y() + dy);
}

/**
 * Returns the x speed of the entity.
 * @return the x speed of the entity, between -100 and 100
 */
double Movement::get_x_speed() {
  return x_speed;
}

/**
 * Returns the y speed of the entity.
 * @return the y speed of the entity, between -100 and 100
 */
double Movement::get_y_speed() {
  return y_speed;
}

/**
 * Returns the total speed of the entity:
 * sqrt(x_speed^2 + y_speed^2)
 */
double Movement::get_speed(void) {
  return sqrt(x_speed * x_speed + y_speed * y_speed);
}

/**
 * Sets the x speed.
 * @param x_speed the x speed of the entity, between -100 and 100
 */
void Movement::set_x_speed(double x_speed) {

  this->x_speed = x_speed;
  Uint32 now = SDL_GetTicks();

  // compute x_delay, x_move and next_move_date_x
  if (x_speed == 0) {
    x_move = 0;
  }
  else {
    if (x_speed > 0) {
      x_delay = (Uint32) (100 / x_speed);
      x_move = 1;
    }
    else {
      x_delay = (Uint32) (100 / (-x_speed));
      x_move = -1;
    }
    next_move_date_x = now + x_delay;
  }
}

/**
 * Sets the y speed.
 * @param y_speed the y speed of the entity, between -100 and 100
 */
void Movement::set_y_speed(double y_speed) {

  this->y_speed = y_speed;
  Uint32 now = SDL_GetTicks();

  // compute y_delay, y_move and next_move_date_y
  if (y_speed == 0) {
    y_move = 0;
  }
  else {
    if (y_speed > 0) {
      y_delay = (Uint32) (100 / y_speed);
      y_move = 1;
    }
    else {
      y_delay = (Uint32) (100 / (-y_speed));
      y_move = -1;
    }
    next_move_date_y = now + y_delay;
  }
}

/**
 * Changes the speed, keeping the same direction of the movement.
 * x_speed and y_speed are recomputed so that the movement direction is unchanged.
 * Warning: if x_speed and y_speed are both equal to zero, there is no
 * direction so calling this function does not make sense.
 * @param speed the new speed
 */
void Movement::set_speed(double speed) {
  double angle; // angle in radians

  // compute the angle
  if (x_speed == 0) {
    angle = (y_speed > 0) ? THREE_PI_OVER_2 : PI_OVER_2;
  }
  else {
    angle = atan((double) (y_speed / x_speed));
    if (x_speed < 0) {
      angle = angle + PI;
    }
  }

  set_x_speed(speed * cos(angle));
  set_y_speed(-speed * sin(angle));
}

/**
 * Returns whether the speed is zero.
 * @return true if the entity is stopped, false otherwise
 */
bool Movement::is_stopped(void) {
  return !is_started();
}

/**
 * Returns whether the speed is not zero.
 * @return true if the entity is moving, false otherwise
 */
bool Movement::is_started(void) {
  return x_speed != 0 || y_speed != 0;
}

/**
 * Sets the speed to zero.
 */
void Movement::stop(void) {
  set_x_speed(0);
  set_y_speed(0);
  x_move = 0;
  y_move = 0;
}

/**
 * Changes the direction of the movement vector, keeping the same speed.
 * x_speed and y_speed are recomputed so that the total speed is unchanged.
 * Warning: if x_speed and y_speed are both equal to zero, this function
 * stops the program.
 * @param direction the new movement direction, between 0 and 359
 */
void Movement::set_direction(int direction) {

  double angle = direction * TWO_PI / 360.0; // angle in radians
  set_direction(angle);
}

/**
 * Changes the direction of the movement vector, keeping the same speed.
 * x_speed and y_speed are recomputed so that the total speed is unchanged.
 * Warning: if x_speed and y_speed are both equal to zero, this function
 * stops the program.
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
 * Returns true if the entity is about to try to move
 * on the next update(), i.e. if x_move is not equal to zero
 * and next_move_date_x is past, or the same thing for y.
 * @return true if the entity is about to try to move
 *
 */
bool Movement::has_to_move_now(void) {

  Uint32 now = SDL_GetTicks();
  return (x_move != 0 && now >= next_move_date_x)
    || (y_move != 0 && now >= next_move_date_y);
}

/**
 * Returns whether the movement is suspended.
 * @return true if the movement is suspended
 */
bool Movement::is_suspended(void) {
  return suspended;
}

/**
 * Suspends or resumes the movement.
 * This function is called by the entity when the game is suspended or resumed.
 * @param suspended true to suspend the movement, false to resume it
 */
void Movement::set_suspended(bool suspended) {

  this->suspended = suspended;

  Uint32 now = SDL_GetTicks();

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
 * Updates the x position of the entity if it has to be changed.
 */
void Movement::update_x(void) {

  if (x_move != 0) { // if we want to move on x

    // update the x position while next_move_date_x is past
    Uint32 now = SDL_GetTicks();
    while (now >= next_move_date_x) {
      translate_x(x_move);
      next_move_date_x += x_delay;
    }
  }
}

/**
 * Updates the y position of the entity if it has changed.
 */
void Movement::update_y(void) {

  if (y_move != 0) { // if we want to move on y

    // update the x position while next_move_date_y is past
    Uint32 now = SDL_GetTicks();
    while (now >= next_move_date_y) {
      translate_y(y_move);
      next_move_date_y += y_delay;
    }
  }
}

/**
 * Updates the entity's position.
 * This function is called repeteadly by the map.
 * By default, it calls update_x() and update_y().
 * You can redefine this function.
 */
void Movement::update(void) {

  if (!suspended) {
    update_x();
    update_y();
  }
}
