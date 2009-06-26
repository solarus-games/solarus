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
#ifndef ZSDX_TARGET_MOVEMENT_H
#define ZSDX_TARGET_MOVEMENT_H

#include "Common.h"
#include "movements/Movement.h"

/**
 * Movement of an object that goes from an initial point to a target point.
 */
class TargetMovement: public Movement {

 protected:

  int target_x;
  int target_y;
  MapEntity *target_entity; /**< can be NULL if only a point is targeted */

  int sign_x;   /**< sign of the x movement (1: right, -1: left) */
  int sign_y;   /**< sign of the y movement (1: up, -1: down) */
  int speed;

  Uint32 next_recomputation_date;

  void recompute_movement(void);

 public:

  TargetMovement(int target_x, int target_y, int speed);
  TargetMovement(MapEntity *target_entity, int speed);
  ~TargetMovement(void);

  bool is_finished(void);
  void update(void);
};

#endif

