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
#ifndef ZSDX_CHASE_MOVEMENT_H
#define ZSDX_CHASE_MOVEMENT_H

#include "Common.h"
#include "movements/SmoothCollisionMovement.h"

/**
 * Movement for an entity that chases a target entity (usually the hero)
 * and tries to pass the obstacles on the way.
 */
class ChaseMovement: public SmoothCollisionMovement {

 private:

  MapEntity *target;              /**< the entity targeted by this movement (usually the hero) */
  int speed;
  Uint32 next_recomputation_date;

 protected:

  void update(void);
  void recompute_movement(void);

 public:

  ChaseMovement(MapEntity *target, int speed);
  ~ChaseMovement(void);

};

#endif
