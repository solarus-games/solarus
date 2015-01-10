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
#include "solarus/entities/Fire.h"
#include "solarus/lowlevel/Point.h"
#include "solarus/Sprite.h"
#include "solarus/SpriteAnimationSet.h"

namespace Solarus {

/**
 * \brief Creates some fire.
 * \param name Name identifying the entity on the map or an empty string.
 * \param layer layer of the fire entity to create
 * \param xy coordinates of the fire
 */
Fire::Fire(const std::string& name, Layer layer, const Point& xy):
  Detector(COLLISION_OVERLAPPING | COLLISION_SPRITE, name, layer, xy, Size(16, 16)) {

  // initialize the entity
  create_sprite("entities/fire");

  get_sprite().enable_pixel_collisions();
  set_origin(8, 13);
}

/**
 * \brief Returns the type of entity.
 * \return the type of entity
 */
EntityType Fire::get_type() const {
  return EntityType::FIRE;
}

/**
 * \brief Returns whether entities of this type can be obstacles for other entities.
 *
 * If yes, the function is_obstacle_for() will be called
 * to determine whether this particular entity is an obstacle or not.
 *
 * \return true if this type of entity can be obstacle for other entities
 */
bool Fire::can_be_obstacle() const {
  return false;
}

/**
 * \brief Updates this entity.
 */
void Fire::update() {

  Detector::update();
  if (get_sprite().is_animation_finished()) {
    remove_from_map();
  }
  else {
    check_collision_with_detectors();
  }
}

/**
 * \copydoc Detector::notify_collision(MapEntity&, Sprite&, Sprite&)
 */
void Fire::notify_collision(
    MapEntity& other_entity,
    Sprite& /* this_sprite */,
    Sprite& other_sprite
) {
  other_entity.notify_collision_with_fire(*this, other_sprite);
}

}
