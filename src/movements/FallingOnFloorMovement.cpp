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
#include "movements/FallingOnFloorMovement.h"

/**
 * @brief Y move at each time frame.
 */
const Rectangle FallingOnFloorMovement::moves[3][26] = {

  // low
  { 
    Rectangle(0, -1),  Rectangle(0, 1),
  },

  // medium
  {
    Rectangle(0, -1),  Rectangle(0, -1),  Rectangle(0, -1),  Rectangle(0, -1),
    Rectangle(0,  1),  Rectangle(0,  1),  Rectangle(0,  1),  Rectangle(0,  1),
    Rectangle(0, -1),  Rectangle(0, -1),  Rectangle(0,  1),  Rectangle(0,  1),
  },

  // high
  {
    Rectangle(0, -4),  Rectangle(0, -3),  Rectangle(0, -3),  Rectangle(0, -2),
    Rectangle(0, -2),  Rectangle(0, -1),  Rectangle(0,  1),  Rectangle(0,  2),
    Rectangle(0,  2),  Rectangle(0,  3),  Rectangle(0,  3),  Rectangle(0,  4),
    Rectangle(0, -2),  Rectangle(0, -1),  Rectangle(0, -1),  Rectangle(0, -1),  
    Rectangle(0, -1),  Rectangle(0,  1),  Rectangle(0,  1),  Rectangle(0,  1),
    Rectangle(0,  1),  Rectangle(0,  2),  Rectangle(0, -1),  Rectangle(0, -1),
    Rectangle(0,  1),  Rectangle(0,  1),
  }
};

/**
 * @brief Number of moves for each height.
 */
const int FallingOnFloorMovement::nb_moves[3] = {2, 12, 26};

/**
 * @brief Creates a movement falling object.
 * @param height height the entity will fall from
 */
FallingOnFloorMovement::FallingOnFloorMovement(FallingHeight height):
  PixelMovement(moves[height], nb_moves[height], 30, false, true) {

}

/**
 * @brief Destructor.
 */
FallingOnFloorMovement::~FallingOnFloorMovement() {

}

