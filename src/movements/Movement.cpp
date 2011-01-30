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
#include "Map.h"
#include "lowlevel/System.h"
#include "lowlevel/Debug.h"

int Movement::next_unique_id = 0;

/**
 * @brief Constructor.
 * @param ignore_obstacles when there is a map and the movement is attached to an entity of this map,
 * indicates whether the movement should ignore obstacles
 */
Movement::Movement(bool ignore_obstacles):

  unique_id(next_unique_id++),
  entity(NULL),
  xy(0, 0),
  last_move_date(0),
  finished(false),
  suspended(false),
  when_suspended(0),
  last_collision_box_on_obstacle(-1, -1),
  default_ignore_obstacles(ignore_obstacles),
  current_ignore_obstacles(ignore_obstacles) {

}

/**
 * @brief Destructor.
 */
Movement::~Movement() {

}

/**
 * @brief Returns the unique id of this movement.
 *
 * It is guaranteed that no other movement instance will have the same id as this one
 * during the execution of the program, even after this instance is deleted.
 *
 * @return the unique id of this movement
 */
int Movement::get_unique_id() {
  return unique_id;
}

/**
 * @brief Returns the entity controlled by this movement (if any).
 * @return the entity controlled by this movement, or NULL if this movement
 * is not attached to a map entity
 */
MapEntity* Movement::get_entity() {
  return entity;
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
    this->xy.set_xy(0, 0);
  }
  else {
    this->xy.set_xy(entity->get_xy());
    entity->notify_movement_changed();
  }
}

/**
 * @brief Returns the x position of the object controlled by this movement.
 * @return the x position of the object controlled by this movement
 */
int Movement::get_x() {

  return (entity != NULL) ? entity->get_x() : xy.get_x();
}

/**
 * @brief Returns the y position of the object controlled by this movement.
 * @return the y position of the object controlled by this movement
 */
int Movement::get_y() {

  return (entity != NULL) ? entity->get_y() : xy.get_y();
}

/**
 * @brief Returns the coordinates of the object controlled by this movement.
 * @return the coordinates of the object controlled by this movement
 */
const Rectangle Movement::get_xy() {

  return (entity != NULL) ? entity->get_xy() : xy;
}

/**
 * @brief Sets the x position of the object controlled by this movement.
 * @param x the new x position
 */
void Movement::set_x(int x) {
  set_xy(x, get_y());
}

/**
 * @brief Sets the y position of the object controlled by this movement.
 * @param y the new y position
 */
void Movement::set_y(int y) {
  set_xy(get_x(), y);
}

/**
 * @brief Sets the position of the entity or the point controlled by this movement.
 * @param x the new x position
 * @param y the new y position
 */
void Movement::set_xy(int x, int y) {

  if (entity != NULL) {
    entity->set_xy(x, y);
    notify_position_changed();
  }
  else {
    this->xy.set_xy(x, y);
  }

  last_move_date = System::now();
}

/**
 * @brief Sets the position of the entity or the point controlled by this movement.
 * @param xy the new coordinates (only x and y are used, the size of the rectangle is ignored)
 */
void Movement::set_xy(const Rectangle &xy) {
  set_xy(xy.get_x(), xy.get_y());
}

/**
 * @brief Moves the object on x.
 * @param dx number of pixels of the move
 */
void Movement::translate_x(int dx) {
  translate_xy(dx, 0);
}

/**
 * @brief Moves the object on y.
 * @param dy number of pixels of the move
 */
void Movement::translate_y(int dy) {
  translate_xy(0, dy);
}

/**
 * @brief Moves the object.
 * @param dx number of pixels of the move on x
 * @param dy number of pixels of the move on y
 */
void Movement::translate_xy(int dx, int dy) {
  set_xy(get_x() + dx, get_y() + dy);
}

/**
 * @brief Moves the object.
 * @param dxy number of pixel of the move on x and y
 * (the size of the rectangle is ignored)
 */
void Movement::translate_xy(const Rectangle &dxy) {
  translate_xy(dxy.get_x(), dxy.get_y());
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
 * @brief Returns whether the movement is stopped.
 * @return true if the object is stopped, false otherwise
 */
bool Movement::is_stopped() {
  return !is_started();
}

/**
 * @brief Returns whether the object is moving.
 *
 * Subclasses of Movement should redefine this function to indicate
 * whether the object is moving.
 *
 * @return true if the object is moving, false otherwise
 */
bool Movement::is_started() {
  return false;
}

/**
 * @brief Stops the movement.
 *
 * When this function returns, is_started() should be false.
 */
void Movement::stop() {

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
}

/**
 * @brief Returns the date when this movement was suspended.
 *
 * This function should be called only while the movement is suspended.
 *
 * @return the date when this movement started to be suspended
 */
uint32_t Movement::get_when_suspended() {
  return when_suspended;
}

/**
 * @brief Updates the position of the object controlled by this movement.
 *
 * This function is called repeteadly.
 * You can redefine this function.
 */
void Movement::update() {

  if (!finished && is_finished()) {
    finished = true;
    if (entity != NULL) {
      entity->notify_movement_finished();
    }
  }
  else if (finished && !is_finished()) {
    finished = false;
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
bool Movement::test_collision_with_obstacles(int dx, int dy) {

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
  }

  return collision;
}

/**
 * @brief Returns whether the entity would collide with the map
 * if it was moved a few pixels from its position.
 *
 * If the movement is not attached to an entity of a map,
 * or if obstacles are ignored, false is always returned.
 *
 * @param dxy distance between the current position and the position to check
 * @return true if the entity would overlap the map obstacles in this position
 */
bool Movement::test_collision_with_obstacles(const Rectangle& dxy) {
  return test_collision_with_obstacles(dxy.get_x(), dxy.get_y());
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

/**
 * @brief Returns the direction a sprite controlled by this movement should take.
 * @return the direction to use to display the object controlled by this movement (0 to 3)
 */
int Movement::get_displayed_direction4() {
  return 3; // by default, look down
}

/**
 * @brief Returns the coordinates where an object controlled by this movement
 * should be displayed.
 *
 * By default, it returns the coordinates of the object itself. Redefine this function
 * if you want the object to be displayed at different coordinates than its real position.
 *
 * @return the coordinates to use to display the object controlled by this movement
 */
const Rectangle Movement::get_displayed_xy() {
  return get_xy();
}

/**
 * @brief Returns the value of a property of this movement.
 *
 * Subclasses whose movement type is available to the script API should redefine this function
 * to allow scripts to interact with the movement.
 *
 * @param key key of the property to get (the accepted keys depend on the movement type)
 * @return the corresponding value as a string
 */
const std::string Movement::get_property(const std::string &key) {

  Debug::die("Movement::get_property() is not supported by this movement type");

  return "";
}

/**
 * @brief Sets the value of a property of this movement.
 *
 * Subclasses whose movement type is available to the script API should redefine this function
 * to allow scripts to interact with the movement.
 *
 * @param key key of the property to set (the accepted keys depend on the movement type)
 * @param value the value to set
 */
void Movement::set_property(const std::string &key, const std::string &value) {

  Debug::die("Movement::set_property() is not supported by this movement type");

}

