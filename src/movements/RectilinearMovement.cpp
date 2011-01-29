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
#include "movements/RectilinearMovement.h"
#include "entities/MapEntity.h"
#include "lowlevel/System.h"
#include "lowlevel/Geometry.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"
#include <cmath>

/**
 * Constructor.
 * @param ignore_obstacles when there is a map and the movement is attached to an entity of this map,
 * indicates whether the movement should ignore obstacles
 */
RectilinearMovement::RectilinearMovement(bool ignore_obstacles):
  Movement(ignore_obstacles),
  x_speed(0),
  y_speed(0),
  next_move_date_x(System::now()),
  next_move_date_y(System::now()),
  x_move(0),
  y_move(0) {

}

/**
 * Destructor.
 */
RectilinearMovement::~RectilinearMovement() {

}

/**
 * @brief Returns the x speed of the object.
 * @return the x speed of the entity, between -100 and 100
 */
double RectilinearMovement::get_x_speed() {
  return x_speed;
}

/**
 * @brief Returns the y speed of the object.
 * @return the y speed of the entity, between -100 and 100
 */
double RectilinearMovement::get_y_speed() {
  return y_speed;
}

/**
 * @brief Returns the total speed of the object.
 *
 * The speed is calculated as sqrt(x_speed^2 + y_speed^2).
 *
 * @return the speed in pixels per second
 */
double RectilinearMovement::get_speed() {
  return std::sqrt(x_speed * x_speed + y_speed * y_speed);
}

/**
 * @brief Sets the x speed.
 * @param x_speed the x speed of the object in pixels per second
 */
void RectilinearMovement::set_x_speed(double x_speed) {

  if (std::fabs(x_speed) <= 1E-6) {
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
      set_x_delay((uint32_t) (1000 / x_speed));
      set_x_move(1);
    }
    else {
      set_x_delay((uint32_t) (1000 / (-x_speed)));
      set_x_move(-1);
    }
    set_next_move_date_x(now + x_delay);
  }

  if (get_entity() != NULL) {
    get_entity()->notify_movement_changed();
  }
}

/**
 * @brief Sets the y speed.
 * @param y_speed the y speed of the object in pixels per second
 */
void RectilinearMovement::set_y_speed(double y_speed) {

  if (std::fabs(y_speed) <= 1E-6) {
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
      set_y_delay((uint32_t) (1000 / y_speed));
      set_y_move(1);
    }
    else {
      set_y_delay((uint32_t) (1000 / (-y_speed)));
      set_y_move(-1);
    }
    set_next_move_date_y(now + y_delay);
  }

  if (get_entity() != NULL) {
    get_entity()->notify_movement_changed();
  }
}

/**
 * @brief Changes the speed, keeping the same direction of the movement.
 *
 * x_speed and y_speed are recomputed so that the movement direction is unchanged.
 *
 * @param speed the new speed
 */
void RectilinearMovement::set_speed(double speed) {

  if (x_speed == 0 && y_speed == 0) {
    x_speed = 1;
  }

  // compute the new speed vector
  double angle = Geometry::get_angle(0, 0,
                                     (int) (x_speed * 100),
                                     (int) (y_speed * 100)); // angle in radians
  set_x_speed(speed * std::cos(angle));
  set_y_speed(-speed * std::sin(angle));
}

/**
 * @brief Returns whether the speed is not zero.
 * @return true if the object is moving, false otherwise
 */
bool RectilinearMovement::is_started() {
  return x_speed != 0 || y_speed != 0;
}

/**
 * @brief Sets the speed to zero.
 */
void RectilinearMovement::stop() {

  set_x_speed(0);
  set_y_speed(0);
  set_x_move(0);
  set_y_move(0);
}

/**
 * @brief Sets the date of the next change of the x coordinate.
 * @param next_move_date_x the date in milliseconds
 */
void RectilinearMovement::set_next_move_date_x(uint32_t next_move_date_x) {

  if (is_suspended()) {
    uint32_t delay = next_move_date_x - System::now();
    this->next_move_date_x = get_when_suspended() + delay;
  }
  else {
    this->next_move_date_x = next_move_date_x;
  }
}

/**
 * @brief Sets the date of the next change of the y coordinate.
 * @param next_move_date_y the date in milliseconds
 */
void RectilinearMovement::set_next_move_date_y(uint32_t next_move_date_y) {

  if (is_suspended()) {
    uint32_t delay = next_move_date_y - System::now();
    this->next_move_date_y = get_when_suspended() + delay;
  }
  else {
    this->next_move_date_y = next_move_date_y;
  }
}

/**
 * @brief Computes and returns the direction of the speed vector.
 * @return the current angle of the speed vector in degrees
 */
double RectilinearMovement::get_angle() {

  return Geometry::get_angle(0, 0, (int) (get_x_speed() * 1000), (int) (get_y_speed() * 1000));
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
void RectilinearMovement::set_angle(double angle) {

  Debug::check_assertion(x_speed != 0 || y_speed != 0,
    StringConcat() << "Cannot set the angle when the speed is zero (entity: " << get_entity() << ")");

  double speed = get_speed();
  set_x_speed(speed * std::cos(angle));
  set_y_speed(-speed * std::sin(angle));

  if (get_entity() != NULL) {
    get_entity()->notify_movement_changed();
  }
}

/**
 * @brief Returns the direction a sprite controlled by this movement should take.
 * @return the direction to use to display the object controlled by this movement (0 to 3)
 */
int RectilinearMovement::get_displayed_direction4() {

  double angle = get_angle();
  int direction = (Geometry::radians_to_degrees(angle) + 45 + 360) / 90;
  return direction % 4;
}

/**
 * @brief Returns true if the object is about to try to move.
 *
 * This function returns true if x_move is not equal to zero
 * and next_move_date_x is past, or the same thing for y.
 *
 * @return true if the entity is about to try to move
 */
bool RectilinearMovement::has_to_move_now() {

  uint32_t now = System::now();

  return (get_x_move() != 0 && now >= get_next_move_date_x())
    || (get_y_move() != 0 && now >= get_next_move_date_y());
}

/**
 * @brief Suspends or resumes the movement.
 *
 * This function is called by the entity when the game is suspended or resumed.
 *
 * @param suspended true to suspend the movement, false to resume it
 */
void RectilinearMovement::set_suspended(bool suspended) {

  Movement::set_suspended(suspended);

  if (!suspended) {

    // recalculate the next move date
    if (get_when_suspended() != 0) {
      uint32_t diff = System::now() - get_when_suspended();
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
void RectilinearMovement::update_x() {

  uint32_t now = System::now();
  int x_move = get_x_move();
  if (x_move != 0 && now >= get_next_move_date_x()) { // if it's time to try a move

    // make the move only if there is no collision
    if (!test_collision_with_obstacles(x_move, get_y_move())) {
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
void RectilinearMovement::update_y() {

  uint32_t now = System::now();
  int y_move = get_y_move();
  if (y_move != 0 && now >= get_next_move_date_y()) { // if it's time to try a move

    // make the move only if there is no collision
    if (!test_collision_with_obstacles(get_x_move(), y_move)) {
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
 * You can redefine this function.
 */
void RectilinearMovement::update() {

  Movement::update();

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

      if (!is_suspended() && get_entity() != NULL) {

        // the movement was successful if the entity's coordinates have changed
        // and the movement was not stopped
        bool success = (get_x() != old_xy.get_x() || get_y() != old_xy.get_y())
            && (get_x_move() != 0 || get_y_move() != 0);

        // notify the entity
	get_entity()->notify_movement_tried(success);
      }

      now = System::now();
      x_move_now = get_x_move() != 0 && now >= get_next_move_date_x();
      y_move_now = get_y_move() != 0 && now >= get_next_move_date_y();
    }
  }
}

/**
 * @brief Returns the value of a property of this movement.
 *
 * Accepted keys:
 * - speed
 * - angle
 * - ignore_obstacles
 *
 * @param key key of the property to get
 * @return the corresponding value as a string
 */
const std::string RectilinearMovement::get_property(const std::string &key) {

  std::ostringstream oss;

  if (key == "speed") {
    oss << get_speed();
  }
  else if (key == "angle") {
    oss << get_angle();
  }
  else if (key == "ignore_obstacles") {
    oss << are_obstacles_ignored();
  }
  else {
    Debug::die(StringConcat() << "Unknown property of RectilinearMovement: '" << key << "'");
  }

  return oss.str();
}

/**
 * @brief Sets the value of a property of this movement.
 *
 * Accepted keys:
 * - speed
 * - angle
 * - ignore_obstacles
 *
 * @param key key of the property to set (the accepted keys depend on the movement type)
 * @param value the value to set
 */
void RectilinearMovement::set_property(const std::string &key, const std::string &value) {

  std::istringstream iss(value);

  if (key == "speed") {
    int speed;
    iss >> speed;
    set_speed(speed);
  }
  else if (key == "angle") {
    double angle;
    iss >> angle;
    if (get_speed() == 0) {
      set_speed(1);
    }
    set_angle(angle);
  }
  else if (key == "ignore_obstacles") {
    bool ignore_obstacles;
    iss >> ignore_obstacles;
    set_default_ignore_obstacles(ignore_obstacles);
  }
  else {
    Debug::die(StringConcat() << "Unknown property of RectilinearMovement: '" << key << "'");
  }
}

