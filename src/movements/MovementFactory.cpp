/*
 * Copyright (C) 2009 Christopho, Solarus - http://www.solarus-engine.org
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
#include "movements/MovementFactory.h"
#include "movements/PixelMovement.h"

MovementFactory::CreationFunction MovementFactory::creation_functions[] = {
  MovementFactory::create_pixel_movement
};

std::map<std::string, MovementFactory::MovementType> MovementFactory::strings_to_types;

/**
 * Initialized strings_to_types.
 */
void MovementFactory::initialize_map() {

  strings_to_types["pixel_movement"] = PIXEL_MOVEMENT;
}


Movement* MovementFactory::create(MovementType type) {

  return creation_functions[type]();
}

Movement* MovementFactory::create(const std::string type) {

  if (strings_to_types.size() == 0) {
    initialize_map();
  }

  return create(strings_to_types[type]);
}

Movement* MovementFactory::create_pixel_movement() {

  return new PixelMovement("", 10, false, false);
}
