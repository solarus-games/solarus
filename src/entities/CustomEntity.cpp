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
#include "entities/CustomEntity.h"
#include "Sprite.h"

namespace solarus {

/**
 * \brief Creates a custom entity.
 * \param game The game.
 * \param name Name of the entity or an empty string.
 * \param direction Direction of the entity (0 to 3).
 * \param layer The layer on the map.
 * \param x X position on the map.
 * \param y Y position on the map.
 * \param width Width of the entity.
 * \param height Height of the entity.
 * \param sprite_name Animation set id of a sprite or an empty string.
 * \param model Model of custom entity or an empty string.
 */
CustomEntity::CustomEntity(
    Game& game,
    const std::string& name,
    int direction,
    Layer layer,
    int x,
    int y,
    int width,
    int height,
    const std::string& sprite_name,
    const std::string& model):

  Detector(
      COLLISION_RECTANGLE,
      name, layer, x, y, width, height),

  model(model) {

  initialize_sprite(sprite_name, direction);
}

/**
 * \brief Destructor.
 */
CustomEntity::~CustomEntity() {

}

/**
 * \brief Returns the type of entity.
 * \return The type of entity.
 */
EntityType CustomEntity::get_type() const {
  return ENTITY_CUSTOM;
}

/**
 * \brief Returns the model of this custom entity if any.
 * \return The model or an empty string.
 */
const std::string& CustomEntity::get_model() const {
  return model;
}

/**
 * \brief Creates the sprite specified.
 * \param sprite_name sprite Animation set of the entity, or an empty string
 * to create no sprite.
 * \param initial_direction Direction of the entity's sprite. Ignored if there
 * is no sprite of if the sprite has no such direction.
 */
void CustomEntity::initialize_sprite(
    const std::string& sprite_name, int initial_direction) {

  if (!sprite_name.empty()) {
    Sprite& sprite = create_sprite(sprite_name);
    if (initial_direction >= 0 && initial_direction < sprite.get_nb_directions()) {
      sprite.set_current_direction(initial_direction);
    }
  }
}

/**
 * \brief Returns whether this entity is an obstacle for another one.
 * \param other Another entity.
 * \return \c true if this entity is an obstacle for the other one.
 */
bool CustomEntity::is_obstacle_for(const MapEntity& other) const {
  // TODO
  return true;
}

}

