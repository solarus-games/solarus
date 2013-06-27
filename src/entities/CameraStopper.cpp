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
#include "entities/CameraStopper.h"

/**
 * \brief Creates a new camera stopper.
 * \param name Name of the entity to create.
 * \param layer Layer of the entity to create on the map.
 * \param x X coordinate of the entity to create.
 * \param y Y coordinate of the entity to create.
 * \param width Width of the camera stopper (16 means a vertical stopper).
 * \param height Height of the camera stopper (16 means an horizontal stopper).
 */
CameraStopper::CameraStopper(
    const std::string& name,
    Layer layer,
    int x,
    int y,
    int width,
    int height):
  Detector(COLLISION_RECTANGLE, name, layer, x, y, width, height) {

}

/**
 * \brief Destructor.
 */
CameraStopper::~CameraStopper() {
}

/**
 * \brief Returns the type of entity.
 * \return The type of entity.
 */
EntityType CameraStopper::get_type() {
  return CAMERA_STOPPER;
}

