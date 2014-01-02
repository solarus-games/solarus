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

namespace solarus {

/**
 * \brief Creates a custom entity.
 * \param game The game.
 * \param name Name of the entity or an empty string.
 * \param direction Direction of the entity.
 * \param layer The layer on the map.
 * \param x X position on the map.
 * \param y Y position on the map.
 * \param width Width of the entity.
 * \param height Height of the entity.
 * \param model Model of custom entity or an empty string.
 */
CustomEntity::CustomEntity(
    Game& game,
    const std::string& name,
    Layer layer,
    int x,
    int y,
    int width,
    int height,
    const std::string& model):

  Detector(
      COLLISION_RECTANGLE | COLLISION_SPRITE,
      name, layer, x, y, width, height),

  model(model) {

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

}

