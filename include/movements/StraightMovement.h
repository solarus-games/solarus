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
#ifndef ZSDX_STRAIGHT_MOVEMENT_H
#define ZSDX_STRAIGHT_MOVEMENT_H

#include "Common.h"
#include "movements/MovementWithCollision.h"

/**
 * Movement for an entity that follows a straight trajectory
 * for a given amount of time.
 */
class StraightMovement: public MovementWithCollision {

 private:

  Uint32 end_movement_date;
  bool finished;

 public:

  StraightMovement(int speed, int direction, Uint32 time);
  StraightMovement(int speed, double direction, Uint32 time);
  ~StraightMovement(void);

  void update(void);
  void set_suspended(bool suspended);
  bool is_finished(void);
  void start(int speed, int direction, Uint32 time);
  void start(int speed, double direction, Uint32 time);
};

#endif
