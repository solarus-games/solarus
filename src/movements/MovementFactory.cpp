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
#include "movements/RandomMovement.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"

MovementFactory::CreationFunction MovementFactory::creation_functions[] = {
  MovementFactory::create_pixel_movement,
  MovementFactory::create_random_movement
};

std::map<std::string, MovementFactory::MovementType> MovementFactory::strings_to_types;

/**
 * @brief Initializes strings_to_types.
 */
void MovementFactory::initialize_map() {

  strings_to_types["pixel_movement"] = PIXEL_MOVEMENT;
  strings_to_types["random_movement"] = RANDOM_MOVEMENT;
}

/**
 * @brief Creates a movement of the specified type with some default properties.
 * @param type type of movement
 */
Movement* MovementFactory::create(MovementType type) {

  return creation_functions[type]();
}

/**
 * @brief Creates a movement of the specified type with some default properties.
 * @param type a string describing the type of movement:
 * "pixel_movement" or "random_movement"
 */
Movement* MovementFactory::create(const std::string type) {

  if (strings_to_types.size() == 0) {
    initialize_map();
  }

  Debug::assert(strings_to_types.find(type) != strings_to_types.end(),
      StringConcat() << "Invalid movement type: '" << type << "'");

  return create(strings_to_types[type]);
}

Movement* MovementFactory::create_pixel_movement() {

  return new PixelMovement("", 10, false, false);
}

Movement* MovementFactory::create_random_movement() {

  return new RandomMovement(32);
}
