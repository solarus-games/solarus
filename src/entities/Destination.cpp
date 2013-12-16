/*
 * Copyright (C) 2006-2013 Christopho, Solarus - http://www.solarus-games.org
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
#include "entities/Destination.h"
#include "Sprite.h"
#include "Game.h"
#include "lowlevel/FileTools.h"

namespace solarus {

/**
 * \brief Constructor.
 * \param name Name of the destination to create.
 * \param layer The layer.
 * \param x X position of the destination point to create.
 * \param y Y position of the destination point to create.
 * \param hero_direction Initial direction of the hero in this state
 * (0 to 3, or -1 to indicate that the hero's direction is not changed).
 * \param sprite_name Animation set id of a sprite to represent this
 * destination, or an empty string.
 * \param is_default \c true to make this destination the default one.
 */
Destination::Destination(
    const std::string& name,
    Layer layer,
    int x,
    int y,
    int hero_direction,
    const std::string& sprite_name,
    bool is_default):
  MapEntity(name, hero_direction, layer, x, y, 16, 16),
  is_default_destination(is_default) {

  set_origin(8, 13);

  if (!sprite_name.empty()) {
    create_sprite(sprite_name);
  }
}

/**
 * \brief Destructor.
 */
Destination::~Destination() {
}

/**
 * \brief Returns the type of entity.
 * \return the type of entity
 */
EntityType Destination::get_type() const {
  return ENTITY_DESTINATION;
}

/**
 * \brief Returns whether entities of this type can be obstacles for other entities.
 * \return \c true if this type of entity can be obstacle for other entities.
 */
bool Destination::can_be_obstacle() const {
  return false;
}

/**
 * \brief Returns whether this destination is the default one of the map.
 * \return \c true if this destination is the default one.
 */
bool Destination::is_default() const {
  return is_default_destination;
}

}

