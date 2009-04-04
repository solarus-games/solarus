/*
 * Copyright (C) 2009 Christopho, Zelda Solarus - http://www.zelda-solarus.com
 * 
 * Zelda: Mystery of Solarus DX is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Zelda: Mystery of Solarus DX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef ZSDX_FALLING_ON_FLOOR_MOVEMENT_H
#define ZSDX_FALLING_ON_FLOOR_MOVEMENT_H

#include "Common.h"
#include "PixelMovement.h"

/**
 * Movement of an item that falls on the floor and
 * bounces one or several times before it stops.
 * This movement is typically used by the pickable items
 * (rupees, hearts...) that appear when the player kill an enemy.
 */
class FallingOnFloorMovement: public PixelMovement {

 public:

  /**
   * Indicates from which height an entity falls.
   */
  enum Height {
    NONE = -1,
    LOW = 0,
    MEDIUM,
    HIGH
  };

  // construction and destruction
  FallingOnFloorMovement(Height height);
  ~FallingOnFloorMovement(void);

};

#endif
