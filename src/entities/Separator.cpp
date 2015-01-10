/*
 * Copyright (C) 2006-2012 Christopho, Solarus - http://www.solarus-games.org
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
#include "solarus/entities/Separator.h"
#include "solarus/lowlevel/Debug.h"
#include "solarus/lua/LuaContext.h"

namespace Solarus {

/**
 * \brief Creates a new separator.
 * \param name Name of the entity to create.
 * \param layer Layer of the entity to create on the map.
 * \param x X coordinate of the entity to create.
 * \param y Y coordinate of the entity to create.
 * \param width Width of the separator (16 means a vertical separator).
 * \param height Height of the separator (16 means an horizontal separator).
 */
Separator::Separator(
    const std::string& name,
    Layer layer,
    const Point& xy,
    const Size& size
):
  Detector(COLLISION_CUSTOM, name, layer, xy, size) {

  Debug::check_assertion((size.width == 16 && size.height > 16)
      || (size.width > 16 && size.height == 16), "Invalid separator size");

  set_layer_independent_collisions(true);
}

/**
 * \copydoc MapEntity::get_type
 */
EntityType Separator::get_type() const {
  return EntityType::SEPARATOR;
}

/**
 * \brief Returns whether this is an horizontal separator.
 * \return \c true if this separator is horizontal, \c false if it is vertical.
 */
bool Separator::is_horizontal() const {
  return get_height() == 16;
}

/**
 * \brief Returns whether this is an vertical separator.
 * \return \c true if this separator is vertical, \c false if it is horizontal.
 */
bool Separator::is_vertical() const {
  return get_width() == 16;
}

/**
 * \copydoc MapEntity::is_obstacle_for
 */
bool Separator::is_obstacle_for(MapEntity& other) {
  return other.is_separator_obstacle(*this);
}

/**
 * \copydoc Detector::test_collision_custom
 */
bool Separator::test_collision_custom(MapEntity& entity) {

  // Trigger the collision if the center point crosses the middle of the
  // separator.

  const Point& separator_center = get_center_point();
  const Point& center = entity.get_center_point();

  if (!overlaps(center)) {
    return false;
  }

  if (is_horizontal()) {
    if (center.y < separator_center.y) {
      // The entity is above the separator.
      return center.y == separator_center.y - 1;
    }
    else {
      // The entity is below the separator.
      return center.y == separator_center.y;
    }
  }
  else {
    if (center.x < separator_center.x) {
      // The entity is west of the separator.
      return center.x == separator_center.x - 1;
    }
    else {
      // The entity is east of the separator.
      return center.x == separator_center.x;
    }
  }
}

/**
 * \copydoc Detector::notify_collision
 */
void Separator::notify_collision(
    MapEntity& entity_overlapping, CollisionMode collision_mode) {

  entity_overlapping.notify_collision_with_separator(*this, collision_mode);
}

/**
 * \brief This function is called when this separator starts being traversed.
 * \param direction4 Direction of the traversal.
 */
void Separator::notify_activating(int direction4) {

  get_lua_context().separator_on_activating(*this, direction4);
}

/**
 * \brief This function is called when the camera has finished to traverse
 * this separator.
 * \param direction4 Direction of the traversal.
 */
void Separator::notify_activated(int direction4) {

  get_lua_context().separator_on_activated(*this, direction4);
}

}

