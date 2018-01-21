/*
 * Copyright (C) 2006-2018 Christopho, Solarus - http://www.solarus-games.org
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
#include "solarus/core/System.h"
#include "solarus/core/Debug.h"
#include "solarus/core/Map.h"
#include "solarus/graphics/Drawable.h"
#include "solarus/entities/Entity.h"
#include "solarus/lua/LuaContext.h"
#include "solarus/movements/Movement.h"
#include <lua.hpp>

namespace Solarus {

/**
 * \brief Constructor.
 */
Movement::Movement():
  Movement(false) {

}

/**
 * \brief Constructor.
 * \param ignore_obstacles When there is a map and the movement is attached to
 * an entity of this map, indicates whether the movement should ignore
 * obstacles.
 */
Movement::Movement(bool ignore_obstacles):

  entity(nullptr),
  drawable(nullptr),
  xy(0, 0),
  last_move_date(0),
  finished(false),
  lua_notifications_enabled(true),
  suspended(false),
  when_suspended(0),
  last_collision_box_on_obstacle(-1, -1),
  default_ignore_obstacles(ignore_obstacles),
  current_ignore_obstacles(ignore_obstacles),
  finished_callback_ref() {

}

/**
 * \brief Destructor.
 */
Movement::~Movement() {
}

/**
 * \brief Returns the entity controlled by this movement (if any).
 * \return the entity controlled by this movement, or nullptr if this movement
 * is not attached to a map entity
 */
Entity* Movement::get_entity() const {
  return entity;
}

/**
 * \brief Sets the entity to be controlled by this movement object.
 *
 * This entity can be nullptr if your movement applies to something else than a map entity.
 * However, some subclasses of Movement may require a non nullptr entity because they
 * implement movements that depend on the map content (e.g. to handle the collisions).
 *
 * \param entity The entity to control, or nullptr if the movement should not be
 * attached to a map entity.
 */
void Movement::set_entity(Entity* entity) {

  Debug::check_assertion(drawable == nullptr, "This movement is already assigned to a drawable");

  this->entity = entity;

  if (entity == nullptr) {
    this->xy = { 0, 0 };
  }
  else {
    this->xy = entity->get_xy();
    notify_movement_changed();
  }
  notify_object_controlled();
}

/**
 * \brief Returns the drawable controlled by this movement (if any).
 * \return The drawable controlled by this movement, or nullptr if this movement
 * is not attached to a drawable.
 */
Drawable* Movement::get_drawable() const {
  return drawable;
}

/**
 * \brief Sets the drawable to be controlled by this movement object.
 *
 * This entity can be nullptr if your movement applies to something else than a drawable.
 *
 * \param drawable The drawable to control, or nullptr if the movement should not be
 * attached to a drawable.
 */
void Movement::set_drawable(Drawable* drawable) {

  Debug::check_assertion(entity == nullptr, "This movement is already assigned to an entity");

  this->drawable = drawable;

  if (drawable == nullptr) {
    this->xy = { 0, 0 };
  }
  else {
    this->xy = drawable->get_xy();
    notify_movement_changed();
  }
  notify_object_controlled();
}

/**
 * \brief Notifies this movement that the object it controls has changed.
 *
 * Does nothing by default.
 */
void Movement::notify_object_controlled() {
}

/**
 * \brief Returns the x position of the object controlled by this movement.
 * \return the x position of the object controlled by this movement
 */
int Movement::get_x() const {

  return get_xy().x;
}

/**
 * \brief Returns the y position of the object controlled by this movement.
 * \return the y position of the object controlled by this movement
 */
int Movement::get_y() const {

  return get_xy().y;
}

/**
 * \brief Returns the coordinates of the object controlled by this movement.
 * \return The coordinates of the object controlled by this movement.
 */
Point Movement::get_xy() const {

  if (entity != nullptr) {
    // The object controlled is a map entity.
    return entity->get_xy();
  }

  if (drawable != nullptr) {
    // The object controlled is a drawable.
    return drawable->get_xy();
  }

  // The object controlled is a point.
  return xy;
}

/**
 * \brief Sets the x position of the object controlled by this movement.
 * \param x The new x position
 */
void Movement::set_x(int x) {
  set_xy(x, get_y());
}

/**
 * \brief Sets the y position of the object controlled by this movement.
 * \param y The new y position.
 */
void Movement::set_y(int y) {
  set_xy(get_x(), y);
}

/**
 * \brief Sets the position of the object controlled by this movement.
 * \param x The new x position.
 * \param y The new y position.
 */
void Movement::set_xy(int x, int y) {
  set_xy(Point(x, y));
}

/**
 * \brief Sets the position of the object controlled by this movement.
 * \param xy The new coordinates.
 */
void Movement::set_xy(const Point& xy) {

  if (entity != nullptr) {
    // The object controlled is a map entity.
    entity->set_xy(xy);
  }

  else if (drawable != nullptr) {
    // The object controlled is a drawable.
    drawable->set_xy(xy);
  }

  // The object controlled is a point.
  this->xy = xy;

  notify_position_changed();
  last_move_date = System::now();
}

/**
 * \brief Moves the object on x.
 * \param dx number of pixels of the move
 */
void Movement::translate_x(int dx) {
  translate_xy(dx, 0);
}

/**
 * \brief Moves the object on y.
 * \param dy number of pixels of the move
 */
void Movement::translate_y(int dy) {
  translate_xy(0, dy);
}

/**
 * \brief Moves the object.
 * \param dx number of pixels of the move on x
 * \param dy number of pixels of the move on y
 */
void Movement::translate_xy(int dx, int dy) {
  set_xy(get_x() + dx, get_y() + dy);
}

/**
 * \brief Moves the object.
 * \param dxy number of pixel of the move on x and y
 */
void Movement::translate_xy(const Point& dxy) {
  translate_xy(dxy.x, dxy.y);
}

/**
 * \brief Notifies this movement that the coordinates controlled by it
 * have just been changed.
 */
void Movement::notify_position_changed() {

  LuaContext* lua_context = get_lua_context();
  if (lua_context != nullptr && are_lua_notifications_enabled()) {
    lua_context->movement_on_position_changed(*this, get_xy());
  }

  if (entity != nullptr) {
    if (!entity->is_being_removed()) {
      entity->notify_position_changed();
    }
  }
}

/**
 * \brief Notifies this movement that it just failed to apply
 * because of obstacles.
 */
void Movement::notify_obstacle_reached() {

  LuaContext* lua_context = get_lua_context();
  if (lua_context != nullptr && are_lua_notifications_enabled()) {
    lua_context->movement_on_obstacle_reached(*this);
  }

  if (entity != nullptr && !entity->is_being_removed()) {
    entity->notify_obstacle_reached();
  }
}

/**
 * \brief Notifies this movement that its characteristics (like speed or angle)
 * have changed.
 */
void Movement::notify_movement_changed() {

  LuaContext* lua_context = get_lua_context();
  if (lua_context != nullptr && are_lua_notifications_enabled()) {
    lua_context->movement_on_changed(*this);
  }

  if (entity != nullptr && !entity->is_being_removed()) {
    entity->notify_movement_changed();
  }
}

/**
 * \brief Notifies this movement that it has just finished.
 */
void Movement::notify_movement_finished() {

  LuaContext* lua_context = get_lua_context();
  if (lua_context != nullptr && are_lua_notifications_enabled()) {
    finished_callback_ref.clear_and_call("movement callback");
    lua_context->movement_on_finished(*this);
  }

  if (entity != nullptr && !entity->is_being_removed()) {
    entity->notify_movement_finished();
  }
}

/**
 * \brief Returns whether the movement is stopped.
 * \return true if the object is stopped, false otherwise
 */
bool Movement::is_stopped() const {
  return !is_started();
}

/**
 * \brief Returns whether the object is moving.
 *
 * Subclasses of Movement should redefine this function to indicate
 * whether the object is moving.
 *
 * \return true if the object is moving, false otherwise
 */
bool Movement::is_started() const {
  return false;
}

/**
 * \brief Stops the movement.
 *
 * When this function returns, is_started() should be false.
 */
void Movement::stop() {

}

/**
 * \brief Returns whether this movement is finished.
 *
 * You can redefine this function if your movement has an end.
 *
 * \return true if this movement is finished
 */
bool Movement::is_finished() const {
  return false;
}

/**
 * \brief Returns whether the movement is suspended.
 * \return true if the movement is suspended
 */
bool Movement::is_suspended() const {
  return suspended;
}

/**
 * \brief Suspends or resumes the movement.
 *
 * This function is called by the entity to suspend or resume its movement.
 *
 * \param suspended true to suspend the movement, false to resume it
 */
void Movement::set_suspended(bool suspended) {

  if (suspended != this->suspended) {
    this->suspended = suspended;

    uint32_t now = System::now();

    if (suspended) {
      // the movement is being suspended
      when_suspended = now;
    }
  }
}

/**
 * \brief Returns the date when this movement was suspended.
 *
 * This function should be called only while the movement is suspended.
 *
 * \return the date when this movement started to be suspended
 */
uint32_t Movement::get_when_suspended() const {
  return when_suspended;
}

/**
 * \brief Updates the position of the object controlled by this movement.
 *
 * This function is called repeatedly.
 * You can redefine this function.
 */
void Movement::update() {

  if (!finished && is_finished()) {
    finished = true;
    notify_movement_finished();
  }
  else if (finished && !is_finished()) {
    finished = false;
  }
}

/**
 * \brief Returns whether the entity would collide with the map
 * if it was moved a few pixels from its position.
 *
 * If the movement is not attached to an entity of a map,
 * or if obstacles are ignored, false is always returned.
 *
 * \param dx x distance between the current position and the position to check
 * \param dy y distance between the current position and the position to check
 * \return true if the entity would overlap the map obstacles in this position
 */
bool Movement::test_collision_with_obstacles(int dx, int dy) const {

  if (entity == nullptr || current_ignore_obstacles) {
    return false;
  }

  Map& map = entity->get_map();

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
 * \brief Returns whether the entity would collide with the map
 * if it was moved a few pixels from its position.
 *
 * If the movement is not attached to an entity of a map,
 * or if obstacles are ignored, false is always returned.
 *
 * \param dxy distance between the current position and the position to check
 * \return true if the entity would overlap the map obstacles in this position
 */
bool Movement::test_collision_with_obstacles(const Point& dxy) const {
  return test_collision_with_obstacles(dxy.x, dxy.y);
}

/**
 * \brief Returns the collision box of the last collision check that detected an obstacle.
 * \return the collision box of the last collision detected, or (-1, -1) if no obstacle was detected
 */
const Rectangle& Movement::get_last_collision_box_on_obstacle() const {

  return last_collision_box_on_obstacle;
}

/**
 * \brief Returns whether this movement currently ignores obstacles.
 * \return true if the obstacles are ignored
 */
bool Movement::are_obstacles_ignored() const {

  return current_ignore_obstacles;
}

/**
 * \brief Sets whether this movement has the property to traverse obstacles.
 * \param ignore_obstacles true to ignore obstacles
 */
void Movement::set_default_ignore_obstacles(bool ignore_obstacles) {

  this->default_ignore_obstacles = ignore_obstacles;
  this->current_ignore_obstacles = ignore_obstacles;
}

/**
 * \brief Overrides temporarily the ability of this movement to traverse obstacles.
 *
 * Use restore_default_ignore_obstacles() to restore the normal property of the movement.
 *
 * \param ignore_obstacles true to ignore obstacles
 */
void Movement::set_ignore_obstacles(bool ignore_obstacles) {

  this->current_ignore_obstacles = ignore_obstacles;
}

/**
 * \brief Restores the normal value of ignore_obstacles.
 */
void Movement::restore_default_ignore_obstacles() {

  this->current_ignore_obstacles = default_ignore_obstacles;
}

/**
 * \brief Returns the direction a sprite controlled by this movement should take.
 * \return the direction to use to display the object controlled by this movement (0 to 3)
 */
int Movement::get_displayed_direction4() const {
  return 3; // by default, look down
}

/**
 * \brief Returns the coordinates where an object controlled by this movement
 * should be displayed.
 *
 * By default, it returns the coordinates of the object itself. Redefine this function
 * if you want the object to be displayed at different coordinates than its real position.
 *
 * \return the coordinates to use to display the object controlled by this movement
 */
Point Movement::get_displayed_xy() const {
  return get_xy();
}

/**
 * \brief Returns the Lua registry ref to the function to call when this
 * movement finishes.
 * \return The Lua ref to a function, or an empty ref.
 */
const ScopedLuaRef& Movement::get_finished_callback() const {
  return finished_callback_ref;
}

/**
 * \brief Sets the function to call when this movement finishes.
 * \param finished_callback_ref The Lua ref to a function, or an empty ref.
 */
void Movement::set_finished_callback(const ScopedLuaRef& finished_callback_ref) {

  Debug::check_assertion(get_lua_context() != nullptr, "Undefined Lua context");

  this->finished_callback_ref = finished_callback_ref;
}

/**
 * \brief Returns whether Lua events and callbacks are enabled for this movement.
 *
 * If no, the finished callback and events
 * movement:on_position_changed(), movement:on_obstacle_reached(),
 * movement:on_changed() and movement:on_finished() won't be called.
 *
 * \return Whether Lua notifications are currently enabled.
 */
bool Movement::are_lua_notifications_enabled() const {
  return lua_notifications_enabled;
}

/**
 * \brief Sets whether Lua events and callbacks are enabled for this movement.
 *
 * If no, the finished callback and events
 * movement:on_position_changed(), movement:on_obstacle_reached(),
 * movement:on_changed() and movement:on_finished() won't be called.
 *
 * \param notify \c true to enable Lua notifications.
 */
void Movement::set_lua_notifications_enabled(bool notify) {
  this->lua_notifications_enabled = notify;
}

/**
 * \brief Returns the name identifying this type in Lua.
 * \return the name identifying this type in Lua
 */
const std::string& Movement::get_lua_type_name() const {
  return LuaContext::movement_module_name;
}

}

