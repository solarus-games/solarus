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
#include "solarus/core/Point.h"
#include "solarus/entities/Fire.h"
#include "solarus/graphics/Sprite.h"
#include "solarus/graphics/SpriteAnimationSet.h"

namespace Solarus {

/**
 * \brief Creates some fire.
 * \param name Name identifying the entity on the map or an empty string.
 * \param layer layer of the fire entity to create
 * \param xy coordinates of the fire
 */
Fire::Fire(const std::string& name, int layer, const Point& xy):
  Entity(name, 0, layer, xy, Size(16, 16)) {

  // initialize the entity
  set_collision_modes(CollisionMode::COLLISION_OVERLAPPING | CollisionMode::COLLISION_SPRITE);

  const SpritePtr& sprite = create_sprite("entities/fire");
  sprite->enable_pixel_collisions();

  set_origin(8, 13);
}

/**
 * \brief Returns the type of entity.
 * \return the type of entity
 */
EntityType Fire::get_type() const {
  return ThisType;
}

/**
 * \brief Updates this entity.
 */
void Fire::update() {

  Entity::update();

  const SpritePtr& sprite = get_sprite();
  if (sprite != nullptr && sprite->is_animation_finished()) {
    remove_from_map();
  }
  else {
    check_collision_with_detectors();
  }
}

/**
 * \copydoc Entity::notify_collision(Entity&, Sprite&, Sprite&)
 */
void Fire::notify_collision(
    Entity& other_entity,
    Sprite& /* this_sprite */,
    Sprite& other_sprite
) {
  other_entity.notify_collision_with_fire(*this, other_sprite);
}

}
