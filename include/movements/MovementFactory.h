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
#ifndef SOLARUS_MOVEMENT_FACTORY_H
#define SOLARUS_MOVEMENT_FACTORY_H

#include "Common.h"
#include <map>

/**
 * @brief Utility class to create various types of movements.
 */
class MovementFactory {

  public:

    enum MovementType {
      PIXEL_MOVEMENT
      // TODO others
    };

  private:

    typedef Movement* (*CreationFunction)();

    static CreationFunction creation_functions[];
    static std::map<std::string, MovementType> strings_to_types;

    MovementFactory(); // don't instantiate this class
    ~MovementFactory();

    static void initialize_map();

  public:

    static Movement* create(MovementType type);
    static Movement* create(const std::string type);

    static Movement* create_pixel_movement();
};

#endif

