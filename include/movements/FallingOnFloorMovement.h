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
#ifndef SOLARUS_FALLING_ON_FLOOR_MOVEMENT_H
#define SOLARUS_FALLING_ON_FLOOR_MOVEMENT_H

#include "Common.h"
#include "movements/PixelMovement.h"
#include "movements/FallingHeight.h"

namespace solarus {

/**
 * \brief Movement of an entity that falls on the floor and bounces.
 *
 * Movement of an item that falls on the floor and
 * bounces one or several times before it stops.
 * This movement is typically used by the pickable treasures
 * that appear when the player kills an enemy.
 */
class FallingOnFloorMovement: public PixelMovement {

  public:

    // construction and destruction
    FallingOnFloorMovement(FallingHeight height);
    ~FallingOnFloorMovement();

  private:

    static const std::string trajectories[];

};

}

#endif

