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
#include "solarus/core/Game.h"
#include "solarus/core/QuestFiles.h"
#include "solarus/entities/Destination.h"
#include "solarus/graphics/Sprite.h"

namespace Solarus {

/**
 * \brief Constructor.
 * \param name Name of the destination to create.
 * \param layer The layer.
 * \param xy Coordinates of the destination to create.
 * \param hero_direction Initial direction of the hero in this state
 * (0 to 3, or -1 to indicate that the hero's direction is not changed).
 * \param sprite_name Animation set id of a sprite to represent this
 * destination, or an empty string.
 * \param is_default \c true to make this destination the default one.
 */
Destination::Destination(
    const std::string& name,
    int layer,
    const Point& xy,
    int hero_direction,
    const std::string& sprite_name,
    bool is_default
):
  Entity(name, hero_direction, layer, xy, Size(16, 16)),
  starting_location_mode(StartingLocationMode::WHEN_WORLD_CHANGES),
  is_default_destination(is_default) {

  set_origin(8, 13);

  if (!sprite_name.empty()) {
    create_sprite(sprite_name);
  }
}

/**
 * \brief Returns the type of entity.
 * \return the type of entity
 */
EntityType Destination::get_type() const {
  return ThisType;
}

/**
 * \brief Returns whether this destination is the default one of the map.
 * \return \c true if this destination is the default one.
 */
bool Destination::is_default() const {
  return is_default_destination;
}

/**
 * \brief Returns whether this destination updates the starting location.
 * \return The starting location mode.
 */
StartingLocationMode Destination::get_starting_location_mode() const {
  return starting_location_mode;
}

/**
 * \brief Sets whether this destination updates the starting location.
 * \param mode The starting location mode.
 */
void Destination::set_starting_location_mode(StartingLocationMode mode) {
  this->starting_location_mode = mode;
}

}

