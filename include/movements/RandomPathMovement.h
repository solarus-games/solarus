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
#ifndef SOLARUS_RANDOM_WALK_MOVEMENT_H
#define SOLARUS_RANDOM_WALK_MOVEMENT_H

#include "Common.h"
#include "movements/PathMovement.h"

/**
 * @brief Movement of an entity that walks randomly.
 *
 * The movement is composed of small random moves. Each
 * random move goes to one of the 4 main directions
 * and with a length that is a multiple of 8 pixels.
 */
class RandomPathMovement: public PathMovement {

  protected:

    void start_next_move();

  public:

    RandomPathMovement(int speed);
    ~RandomPathMovement();

    void update();
    bool is_finished();

};

#endif

