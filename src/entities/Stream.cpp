/*
 * Copyright (C) 2006-2014 Christopho, Solarus - http://www.solarus-games.org
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
#include "entities/Stream.h"
#include "Map.h"
#include "Sprite.h"
#include "lowlevel/FileTools.h"

namespace solarus {

/**
 * \brief Creates a new stream.
 * \param name Name identifying the entity on the map or an empty string.
 * \param layer Layer of the entity to create.
 * \param x X coordinate of the entity to create.
 * \param y Y coordinate of the entity to create.
 * \param direction Direction of the stream (0 to 7).
 * \param sprite_name Animation set id of a sprite or an empty string.
 */
Stream::Stream(
    const std::string& name,
    Layer layer,
    int x,
    int y,
    int direction,
    const std::string& sprite_name
):
  Detector(COLLISION_OVERLAPPING, name, layer, x, y, 16, 16),
  speed(40),
  allow_movement(true),
  allow_attack(true),
  allow_item(true) {

  set_origin(8, 13);
  if (!sprite_name.empty()) {
    create_sprite(sprite_name);
    get_sprite().set_current_direction(direction);
  }
  set_direction(direction);
}

/**
 * \brief Destructor.
 */
Stream::~Stream() {

}

/**
 * \brief Returns the type of entity.
 * \return the type of entity
 */
EntityType Stream::get_type() const {
  return ENTITY_STREAM;
}

/**
 * \brief Returns the speed of this stream.
 * \return The speed in pixels per second.
 */
int Stream::get_speed() const {
  return speed;
}

/**
 * \brief Sets the speed of this stream.
 * \param speed The speed in pixels per second.
 */
void Stream::set_speed(int speed) {
  this->speed = speed;
}

/**
 * \brief Returns whether the player can move the hero in this stream.
 * \return \c true if the hero can move, \c false if this is a blocking stream.
 */
bool Stream::get_allow_movement() const {
  return allow_movement;
}

/**
 * \brief Sets whether the player can move the hero in this stream.
 * \param allow_movement \c true to allow the hero to move, \c false if
 * this is a blocking stream.
 */
void Stream::set_allow_movement(bool allow_movement) {
  this->allow_movement = allow_movement;
}

/**
 * \brief Returns whether the hero can use the sword in this stream.
 * \return \c true if the hero can use the sword.
 */
bool Stream::get_allow_attack() const {
  return allow_attack;
}

/**
 * \brief Sets whether the hero can use the sword in this stream.
 * \param allow_attack \c true to allow the hero to use the sword.
 */
void Stream::set_allow_attack(bool allow_attack) {
  this->allow_attack = allow_attack;
}

/**
 * \brief Returns whether the hero can use equipment items in this stream.
 * \return \c true if the hero can use equipment items.
 */
bool Stream::get_allow_item() const {
  return allow_item;
}

/**
 * \brief Sets whether the hero can use equipment items in this stream.
 * \param allow_attack \c true to allow the hero to use equipment items.
 */
void Stream::set_allow_item(bool allow_item) {
  this->allow_item = allow_item;
}

/**
 * \copydoc MapEntity::is_obstacle_for
 */
bool Stream::is_obstacle_for(MapEntity& other) {

  return other.is_stream_obstacle(*this);
}

/**
 * \copydoc Detector::notify_collision
 */
void Stream::notify_collision(MapEntity& entity_overlapping, CollisionMode collision_mode) {

  const Rectangle& xy_move = direction_to_xy_move(get_direction());
  entity_overlapping.notify_collision_with_stream(*this, xy_move.get_x(), xy_move.get_y());
}

}

