#ifdef NOT_IMPLEMENTED_YET

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
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"
#include "Map.h"
#include <cmath>

/**
 * @brief Constructor.
 * @param ignore_obstacles when there is a map and the movement is attached to an entity of this map,
 * indicates whether the movement should ignore obstacles
 */
Movement::Movement(bool ignore_obstacles):

  x_speed(0),
  y_speed(0),
  next_move_date_x(System::now()),
  next_move_date_y(System::now()),
  x_move(0),
  y_move(0),
  suspended(false),
  last_collision_box_on_obstacle(-1, -1),
  default_ignore_obstacles(ignore_obstacles),
  current_ignore_obstacles(ignore_obstacles),
  when_suspended(0) {

  set_entity(NULL);
}

/**
 * @brief Destructor.
 */
Movement::~Movement() {

}

/**
 * @brief Sets the entity to be controlled by this movement object.
 *
 * This entity can be NULL if your movement applies to something else than a map entity.
 * However, some subclasses of Movement may require a non NULL entity because they
 * implement movements that depend on the map content (e.g. to handle the collisions).
 *
 * @param entity the entity to control, or NULL if the movement should not be
 * attached to a map entity
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
int Movement::get_x() {

  return (entity != NULL) ? entity->get_x() : x;
}

/**
 * @brief Returns the y position of the object controlled by this movement.
 * @return the y position of the object controlled by this movement
 */
int Movement::get_y() {

  return (entity != NULL) ? entity->get_y() : y;
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
    notify_position_changed();
  }
  else {
    this->x = x;
    this->y = y;
  }

  last_move_date = System::now();
}

/**
 * @brief Sets the position of the entity or the point controlled by this movement.
 * @param xy the new coordinates (only x and y are used, the size of the rectangle is ignored)
 */
void Movement::set_position(const Rectangle &xy) {
  set_position(xy.get_x(), xy.get_y());
}

/**
 * @brief Notifies this movement that the coordinates controlled by it
 * have just been changed.
 * By default, the entity (if any) is notified.
 */
void Movement::notify_position_changed() {

  if (entity != NULL) {
    entity->notify_position_changed();
  }
}

/**
 * @brief Moves the object on x.
 * @param dx number of pixels of the move
 */
void Movement::translate_x(int dx) {
  translate(dx, 0);
}

/**
 * @brief Moves the object on y.
 * @param dy number of pixels of the move
 */
void Movement::translate_y(int dy) {
  translate(0, dy);
}

/**
 * @brief Moves the object.
 * @param dx number of pixels of the move on x
 * @param dy number of pixels of the move on y
 */
void Movement::translate(int dx, int dy) {
  set_position(get_x() + dx, get_y() + dy);
}

/**
 * @brief Returns the x speed of the object.
 * @return the x speed of the entity, between -100 and 100
 */
double Movement::get_x_speed() {
  return x_speed;
}

/**
 * @brief Returns the y speed of the object.
 * @return the y speed of the entity, between -100 and 100
 */
double Movement::get_y_speed() {
  return y_speed;
}

/**
 * @brief Returns the total speed of the object.
 *
 * The speed is calculated as sqrt(x_speed^2 + y_speed^2).
 */
double Movement::get_speed() {
  return sqrt(x_speed * x_speed + y_speed * y_speed);
}

/**
 * @brief Sets the x speed.
 * @param x_speed the x speed of the object, between -100 and 100
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
 * @brief Sets the y object.
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
 * @return true if the object is stopped, false otherwise
 */
bool Movement::is_stopped() {
  return !is_started();
}

/**
 * @brief Returns whether the speed is not zero.
 *
 * Subclasses of Movement that don't use the speed feature of this class
 * should redefine this function to indicate whether the object is moving.
 *
 * @return true if the entity is moving, false otherwise
 */
bool Movement::is_started() {
  return x_speed != 0 || y_speed != 0;
}

/**
 * @brief Sets the speed to zero.
 */
void Movement::stop() {

  set_x_speed(0);
  set_y_speed(0);
  set_x_move(0);
  set_y_move(0);
}

/**
 * @brief Sets the date of the next change of the x coordinate.
 * @param next_move_date_x the date in milliseconds
 */
void Movement::set_next_move_date_x(uint32_t next_move_date_x) {

  if (suspended) {
    uint32_t delay = next_move_date_x - System::now();
    this->next_move_date_x = when_suspended + delay;
  }
  else {
    this->next_move_date_x = next_move_date_x;
  }
}

/**
 * @brief Sets the date of the next change of the y coordinate.
 * @param next_move_date_y the date in milliseconds
 */
void Movement::set_next_move_date_y(uint32_t next_move_date_y) {

  if (suspended) {
    uint32_t delay = next_move_date_y - System::now();
    this->next_move_date_y = when_suspended + delay;
  }
  else {
    this->next_move_date_y = next_move_date_y;
  }
}

/**
 * @brief Returns whether this movement is finished.
 *
 * You can redefine this function if your movement has an end.
 *
 * @return true if this movement is finished
 */
bool Movement::is_finished() {
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

  Debug::assert(x_speed != 0 || y_speed != 0,
    StringConcat() << "Cannot set the direction when the speed is zero (entity: " << entity << ")");

  double speed = get_speed();
  set_x_speed(speed * cos(angle));
  set_y_speed(-speed * sin(angle));
}

/**
 * @brief Returns true if the object is about to try to move.
 *
 * This function returns true if x_move is not equal to zero
 * and next_move_date_x is past, or the same thing for y.
 *
 * @return true if the entity is about to try to move
 */
bool Movement::has_to_move_now() {

  uint32_t now = System::now();

  return (get_x_move() != 0 && now >= get_next_move_date_x())
    || (get_y_move() != 0 && now >= get_next_move_date_y());
}

/**
 * @brief Returns whether the movement is suspended.
 * @return true if the movement is suspended
 */
bool Movement::is_suspended() {
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
      uint32_t diff = now - when_suspended;
      next_move_date_x += diff;
      next_move_date_y += diff;
    }
  }
}

/**
 * @brief Updates the x position of the entity if it wants to move.
 *
 * If the movement is attached to a map entity and obstacles are not ignored,
 * the move is done only only if there is no collision with the map.
 */
void Movement::update_x() {

  uint32_t now = System::now();
  int x_move = get_x_move();
  if (x_move != 0 && now >= get_next_move_date_x()) { // if it's time to try a move

    // make the move only if there is no collision
    if (!test_collision_with_map(x_move, get_y_move())) {
      translate_x(x_move);
    }
    else {
      stop(); // also stop on y
    }
    set_next_move_date_x(get_next_move_date_x() + get_x_delay());
  }
}

/**
 * @brief Updates the y position of the entity if it wants to move
 *
 * If the movement is attached to a map entity and obstacles are not ignored,
 * the move is done only only if there is no collision with the map.
 */
void Movement::update_y() {

  uint32_t now = System::now();
  int y_move = get_y_move();
  if (y_move != 0 && now >= get_next_move_date_y()) { // if it's time to try a move

    // make the move only if there is no collision
    if (!test_collision_with_map(get_x_move(), y_move)) {
      translate_y(y_move);
    }
    else {
      stop(); // also stop on x
    }
    set_next_move_date_y(get_next_move_date_y() + get_y_delay());
  }
}

/**
 * @brief Updates the position of the object controlled by this movement.
 *
 * This function is called repeteadly.
 * By default, it calls update_x() and update_y() when necessary.
 * You can redefine this function.
 */
void Movement::update() {

  if (!is_suspended()) {
    uint32_t now = System::now();

    bool x_move_now = get_x_move() != 0 && now >= get_next_move_date_x();
    bool y_move_now = get_y_move() != 0 && now >= get_next_move_date_y();

    while (x_move_now || y_move_now) { // while it's time to move

      // save the current coordinates
      Rectangle old_xy(get_x(), get_y());

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
      else {
	update_y();
      }

      // see if the movement was successful (i.e. if the hero's coordinates have changed)
      bool success = (get_x() != old_xy.get_x() || get_y() != old_xy.get_y());

      if (!is_suspended() && entity != NULL) {
	// notify the entity
	entity->notify_movement_tried(success);
      }

      now = System::now();
      x_move_now = get_x_move() != 0 && now >= get_next_move_date_x();
      y_move_now = get_y_move() != 0 && now >= get_next_move_date_y();
    }
  }
}

/**
 * @brief Returns whether the entity would collide with the map
 * if it was moved a few pixels from its position.
 *
 * If the movement is not attached to an entity of a map,
 * or if obstacles are ignored, false is always returned.
 *
 * @param dx x distance between the current position and the position to check
 * @param dy y distance between the current position and the position to check
 * @return true if the entity would overlap the map obstacles in this position
 */
bool Movement::test_collision_with_map(int dx, int dy) {

  if (entity == NULL || current_ignore_obstacles) {
    return false;
  }

  Map &map = entity->get_map();

  // place the collision box where we want to check the collisions
  Rectangle collision_box = entity->get_bounding_box();
  collision_box.add_xy(dx, dy);

  bool collision = map.test_collision_with_obstacles(entity->get_layer(), collision_box, *entity);

  if (collision) {
    last_collision_box_on_obstacle = collision_box;
    entity->notify_movement_changed();
  }

  return collision;
}

/**
 * @brief Returns the collision box of the last collision check that detected an obstacle.
 * @return the collision box of the last collision detected, or (-1, -1) if no obstacle was detected
 */
const Rectangle& Movement::get_last_collision_box_on_obstacle() {

  return last_collision_box_on_obstacle;
}

/**
 * @brief Returns whether this movement currently ignores obstacles.
 * @return true if the obstacles are ignored
 */
bool Movement::are_obstacles_ignored() {

  return current_ignore_obstacles;
}

/**
 * @brief Sets whether this movement has the property to traverse obstacles.
 * @param ignore_obstacles true to ignore obstacles
 */
void Movement::set_default_ignore_obstacles(bool ignore_obstacles) {

  this->default_ignore_obstacles = ignore_obstacles;
  this->current_ignore_obstacles = ignore_obstacles;
}

/**
 * @brief Overrides temporarily the ability of this movement to traverse obstacles.
 *
 * Use restore_default_ignore_obstacles() to restore the normal property of the movement.
 *
 * @param ignore_obstacles true to ignore obstacles
 */
void Movement::set_ignore_obstacles(bool ignore_obstacles) {

  this->current_ignore_obstacles = ignore_obstacles;
}

/**
 * @brief Restores the normal value of ignore_obstacles.
 */
void Movement::restore_default_ignore_obstacles() {

  this->current_ignore_obstacles = default_ignore_obstacles;
}

#endif

