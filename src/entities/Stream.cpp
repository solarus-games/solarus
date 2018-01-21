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
#include "solarus/core/Map.h"
#include "solarus/core/QuestFiles.h"
#include "solarus/entities/Stream.h"
#include "solarus/entities/StreamAction.h"
#include "solarus/graphics/Sprite.h"
#include <memory>

namespace Solarus {

/**
 * \brief Creates a new stream.
 * \param name Name identifying the entity on the map or an empty string.
 * \param layer Layer of the entity to create.
 * \param xy Coordinates of the entity to create.
 * \param direction Direction of the stream (0 to 7).
 * \param sprite_name Animation set id of a sprite or an empty string.
 */
Stream::Stream(
    const std::string& name,
    int layer,
    const Point& xy,
    int direction,
    const std::string& sprite_name
):
  Entity(name, 0, layer, xy, Size(16, 16)),
  speed(64),
  allow_movement(true),
  allow_attack(true),
  allow_item(true) {

  set_collision_modes(CollisionMode::COLLISION_OVERLAPPING);
  set_origin(8, 13);
  set_direction(direction);
  if (!sprite_name.empty()) {
    create_sprite(sprite_name);
    notify_direction_changed();
  }
}

/**
 * \brief Returns the type of entity.
 * \return the type of entity
 */
EntityType Stream::get_type() const {
  return ThisType;
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
 * \param allow_item \c true to allow the hero to use equipment items.
 */
void Stream::set_allow_item(bool allow_item) {
  this->allow_item = allow_item;
}

/**
 * \copydoc Entity::notify_direction_changed
 */
void Stream::notify_direction_changed() {

  Entity::notify_direction_changed();

  // Give the correct direction to the sprite if any.
  int direction8 = get_direction();
  const SpritePtr& sprite = get_sprite();
  if (sprite != nullptr &&
      sprite->get_nb_directions() >= 8) {
    // There is a sprite with all necessary directions.
    sprite->set_current_direction(direction8);
  }
}

/**
 * \copydoc Entity::is_obstacle_for
 */
bool Stream::is_obstacle_for(Entity& other) {

  return other.is_stream_obstacle(*this);
}

/**
 * \copydoc Entity::notify_collision
 */
void Stream::notify_collision(Entity& entity_overlapping, CollisionMode /* collision_mode */) {

  const Point& xy_move = direction_to_xy_move(get_direction());
  entity_overlapping.notify_collision_with_stream(*this, xy_move.x, xy_move.y);
}

/**
 * \brief Applies this stream on an entity.
 * \param target The entity to move. It should overlap the stream.
 */
void Stream::activate(Entity& target) {

  target.start_stream_action(std::unique_ptr<StreamAction>(
      new StreamAction(*this, target)
  ));
}

}

