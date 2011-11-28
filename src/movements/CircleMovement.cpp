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
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"
#include "entities/MapEntity.h"
#include "entities/MapEntities.h"
#include "Map.h"

/**
 * @brief Creates a circle movement.
 * @param ignore_obstacles true to ignore obstacles
 */
CircleMovement::CircleMovement(bool ignore_obstacles):

  Movement(ignore_obstacles),
  center_entity(NULL),
  center_type(ENEMY),
  current_angle(0),
  initial_angle(0),
  angle_increment(1),
  next_angle_change_date(0),
  angle_change_delay(5),
  current_radius(0),
  wanted_radius(0),
  previous_radius(0),
  next_radius_change_date(0),
  radius_change_delay(0),
  duration(0),
  end_movement_date(0),
  max_rotations(0),
  loop_delay(0),
  restart_date(0) {

}

/**
 * @brief Destroys this circle movement.
 */
CircleMovement::~CircleMovement() {

}

/**
 * @brief Sets the center of the circles.
 *
 * The movement will make circles around the specified fixed point.
 *
 * @param center_point center of the circles to make
 */
void CircleMovement::set_center(const Rectangle &center_point) {

  this->center_entity = NULL;
  this->center_point = center_point;
  recompute_position();
}

/**
 * @brief Sets the center of the circles.
 *
 * The movement will make circles around the specified (possibly moving) entity.
 *
 * @param center_entity the entity around which you want to make circles
 * @param x x coordinate of where the center should be placed relative to the entity's origin
 * @param y y coordinate of where the center should be placed relative to the entity's origin
 */
void CircleMovement::set_center(MapEntity *center_entity, int x, int y) {

  Debug::check_assertion(center_entity != NULL,  "The center entity is NULL");

  this->center_entity = center_entity;
  this->center_point.set_xy(x, y);
  recompute_position();
}

/**
 * @brief Returns the radius of the circles.
 * @return the radius in pixels
 */
int CircleMovement::get_radius() {
  return wanted_radius;
}

/**
 * @brief Sets the radius of the circles.
 * @param radius the radius in pixels
 */
void CircleMovement::set_radius(int radius) {

  Debug::check_assertion(radius >= 0, StringConcat() << "Invalid radius: " << radius);

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
 * @brief Returns the speed of the radius variations.
 * @return the speed in pixels per second, or 0 if radius variations are immediate
 */
int CircleMovement::get_radius_speed() {

  return radius_change_delay == 0 ? 0 : 1000 / radius_change_delay;
}

/**
 * @brief Sets the radius to be updated immediately or gradually towards its wanted value.
 *
 * Use set_radius() to specify the wanted value.
 *
 * @param radius_speed speed of the radius variation (number of pixels per second),
 * or 0 to update it immediately
 */
void CircleMovement::set_radius_speed(int radius_speed) {

  Debug::check_assertion(radius_speed >= 0, StringConcat() << "Invalid radius speed: " << radius_speed);

  if (radius_speed == 0) {
    this->radius_change_delay = 0;
  }
  else {
    this->radius_change_delay = 1000 / radius_speed;
  }

  set_radius(wanted_radius);
}

/**
 * @brief Returns the speed of the angle variation.
 * @return the number of degrees made per second
 */
int CircleMovement::get_angle_speed() {
  return 1000 / angle_change_delay;
}

/**
 * @brief Sets the speed of the angle variation.
 * @param angle_speed number of degrees to make per second
 */
void CircleMovement::set_angle_speed(int angle_speed) {

  Debug::check_assertion(angle_speed > 0, StringConcat() << "Invalid angle speed: " << angle_speed);

  this->angle_change_delay = 1000 / angle_speed;
  this->next_angle_change_date = System::now();
  recompute_position();
}

/**
 * @brief Returns the angle from where the first circle starts.
 * @return the angle in degrees
 */
int CircleMovement::get_initial_angle() {
  return initial_angle;
}

/**
 * @brief Sets the angle from where the first circle starts.
 * @param initial_angle angle in degrees
 */
void CircleMovement::set_initial_angle(int initial_angle) {

  Debug::check_assertion(initial_angle >= 0 && initial_angle < Geometry::TWO_PI,
      StringConcat() << "Invalid initial angle: " << initial_angle);

  this->initial_angle = initial_angle;
}

/**
 * @brief Returns the direction of the circles;
 * @return CLOCKWISE or COUNTER_CLOCKWISE
 */
CircleMovement::Direction CircleMovement::get_direction() {

  return angle_increment > 0 ? COUNTER_CLOCKWISE : CLOCKWISE;
}

/**
 * @brief Sets the direction of the circles.
 * @param direction CLOCKWISE or COUNTER_CLOCKWISE 
 */
void CircleMovement::set_direction(Direction direction) {

  this->angle_increment = (direction == CLOCKWISE) ? -1 : 1;
}

/**
 * @brief Returns the maximum duration of the movement.
 *
 * When this delay is reached, the movement stops.
 * Note that if the radius changes gradually, the movement will continue
 * for a while until the radius reaches zero.
 *
 * @return duration of the movement in milliseconds, (0 means infinite)
 */
uint32_t CircleMovement::get_duration() {
  return duration;
}

/**
 * @brief Sets the maximum duration of the movement.
 *
 * When this delay is reached, the movement stops.
 * Note that if the radius changes gradually, the movement will continue
 * for a while until the radius reaches zero.
 *
 * @param duration duration of the movement in milliseconds, or 0 to make it infinite
 */
void CircleMovement::set_duration(uint32_t duration) {

  this->duration = duration;
  if (duration != 0 && is_started()) {
    this->end_movement_date = System::now() + duration;
  }
}

/**
 * @brief Returns the number of rotations of the movement.
 *
 * When this number of rotations is reached, the movement stops.
 * Note that is the radius changes gradually, the movement will continue
 * for a while until the radius reaches zero.
 *
 * @return the number of rotations to make (0 means infinite rotations)
 */
int CircleMovement::get_max_rotations() {

  return max_rotations;
}

/**
 * @brief Sets the number of rotations of the movement.
 *
 * When this number of rotations is reached, the movement stops.
 * Note that is the radius changes gradually, the movement will continue
 * for a while until the radius reaches zero.
 *
 * @param max_rotations number of rotations to make (0 for infinite rotations)
 */
void CircleMovement::set_max_rotations(int max_rotations) {

  Debug::check_assertion(max_rotations >= 0, StringConcat() << "Invalid maximum rotations number: " << max_rotations);

  this->max_rotations = max_rotations;
  this->nb_rotations = 0;
}

/**
 * @return Returns the delay after which the movement restarts.
 * @return the delay in milliseconds (0 means no restart)
 */
uint32_t CircleMovement::get_loop() {

  return loop_delay;
}

/**
 * @brief Makes the movement restart after a delay when it is finished.
 * @param delay the movement will restart after this delay in milliseconds (0 to avoid this)
 */
void CircleMovement::set_loop(uint32_t delay) {

  this->loop_delay = delay;
  if (delay != 0 && is_stopped()) {
    this->restart_date = System::now() + delay;
  }
}

/**
 * @brief Updates the movement.
 */
void CircleMovement::update() {

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
  else if (current_radius == 0 && loop_delay != 0 && now >= restart_date && wanted_radius == 0) {
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
 * @brief Computes the position of the object controlled by this movement.
 *
 * This function should be called whenever the angle, the radius or the center changes.
 */
void CircleMovement::recompute_position() {

  Rectangle center = this->center_point;
  if (center_entity != NULL) {
    center.add_xy(center_entity->get_xy());
  }

  const Rectangle &xy = Geometry::get_xy(center, Geometry::degrees_to_radians(current_angle), current_radius);
  if (get_entity() == NULL
      || !test_collision_with_obstacles(xy.get_x() - get_entity()->get_x(), xy.get_y() - get_entity()->get_y())) {
    set_xy(xy);
    notify_position_changed();
  }
  else {
    notify_obstacle_reached();
  }
}

/**
 * @brief Suspends or resumes the movement
 * @param suspended true to suspend the movement, false to resume it
 */
void CircleMovement::set_suspended(bool suspended) {

  Movement::set_suspended(suspended);

  if (!suspended) {
    uint32_t diff = System::now() - get_when_suspended();
    next_angle_change_date += diff;
    next_radius_change_date += diff;
    end_movement_date += diff;
    restart_date += diff;
  }
}

/**
 * @brief Starts the circle movement.
 */
void CircleMovement::start() {

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
 * @brief Returns whether this movement is running.
 * @return true if the movement is started
 */
bool CircleMovement::is_started() {
  return current_radius != 0 || wanted_radius != 0;
}

/**
 * @brief Returns whether this movement is finished.
 * @return true if this movement is finished
 */
bool CircleMovement::is_finished() {
  return is_stopped();
}

/**
 * @brief Stops the movement.
 */
void CircleMovement::stop() {

  previous_radius = current_radius;
  set_radius(0);

  if (loop_delay != 0) {
    restart_date = System::now() + loop_delay;
  }
  recompute_position();
}

/**
 * @brief Returns the value of a property of this movement.
 *
 * Accepted keys:
 * - center_type
 * - center_name
 * - center_dx
 * - center_dy
 * - radius
 * - radius_speed
 * - direction
 * - initial_angle
 * - angle_speed
 * - max_rotations
 * - duration
 * - loop
 *
 * @param key key of the property to get
 * @return the corresponding value as a string
 */
const std::string CircleMovement::get_property(const std::string &key) {

  std::ostringstream oss;

  if (key == "center_type") {
    oss << (center_entity != NULL) ? center_entity->get_type() : -1;
  }
  else if (key == "center_name") {
    oss << (center_entity != NULL) ? center_entity->get_name() : "";
  }
  else if (key == "center_dx") {
    oss << center_point.get_x();
  }
  else if (key == "center_dy") {
    oss << center_point.get_y();
  }
  else if (key == "radius_speed") {
    oss << get_radius_speed();
  }
  else if (key == "radius") {
    oss << get_radius();
  }
  else if (key == "direction") {
    oss << get_direction();
  }
  else if (key == "initial_angle") {
    oss << get_initial_angle();
  }
  else if (key == "angle_speed") {
    oss << get_angle_speed();
  }
  else if (key == "max_rotations") {
    oss << get_max_rotations();
  }
  else if (key == "duration") {
    oss << get_duration();
  }
  else if (key == "loop") {
    oss << get_loop();
  }
  else {
    Debug::die(StringConcat() << "Unknown property of CircleMovement: '" << key << "'");
  }

  return oss.str();
}

/**
 * @brief Sets the value of a property of this movement.
 *
 * Accepted keys:
 * - center_type
 * - center_name
 * - center_dx
 * - center_dy
 * - radius
 * - radius_speed
 * - direction
 * - initial_angle
 * - angle_speed
 * - max_rotations
 * - duration
 * - loop
 *
 * @param key key of the property to set (the accepted keys depend on the movement type)
 * @param value the value to set
 */
void CircleMovement::set_property(const std::string &key, const std::string &value) {

  std::istringstream iss(value);

  if (key == "center_type") {
    int center_type;
    iss >> center_type;
    this->center_type = EntityType(center_type);
  }
  else if (key == "center_name") {
    std::string center_name;
    iss >> center_name;
    MapEntities& entities = get_entity()->get_map().get_entities();
    MapEntity* entity = entities.get_entity(center_type, center_name);
    set_center(entity, 0, 0);
  }
  else if (key == "center_dx") {
    int dx;
    iss >> dx;
    set_center(center_entity, dx, center_point.get_y());
  }
  else if (key == "center_dy") {
    int dy;
    iss >> dy;
    set_center(center_entity, center_point.get_x(), dy);
  }
  else if (key == "radius") {
    int radius;
    iss >> radius;
    set_radius(radius);
  }
  else if (key == "radius_speed") {
    int radius_speed;
    iss >> radius_speed;
    set_radius_speed(radius_speed);
  }
  else if (key == "direction") {
    int direction;
    iss >> direction;
    set_direction(Direction(direction));
  }
  else if (key == "initial_angle") {
    int initial_angle;
    iss >> initial_angle;
    set_initial_angle(initial_angle);
  }
  else if (key == "angle_speed") {
    int angle_speed;
    iss >> angle_speed;
    set_angle_speed(angle_speed);
  }
  else if (key == "max_rotations") {
    int max_rotations;
    iss >> max_rotations;
    set_max_rotations(max_rotations);
  }
  else if (key == "duration") {
    uint32_t duration;
    iss >> duration;
    set_duration(duration);
  }
  else if (key == "loop") {
    uint32_t loop;
    iss >> loop;
    set_loop(loop);
  }
  else {
    Debug::die(StringConcat() << "Unknown property of CircleMovement: '" << key << "'");
  }
}

