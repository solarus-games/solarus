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
#ifndef SOLARUS_PATH_FINDING_MOVEMENT_H
#define SOLARUS_PATH_FINDING_MOVEMENT_H

#include "Common.h"
#include "movements/PathMovement.h"

/**
 * Movement for an entity that finds a path to another entity,
 * avoiding the obstacles on the way.
 * If the target entity is too far, the movement is a random walk.
 */
class PathFindingMovement: public PathMovement {

 private:

  MapEntity *target;              /**< the entity targeted by this movement (usually the hero) */
  uint32_t next_recomputation_date;

 protected:

  void update(void);
  void start_next_move(void);
  void recompute_movement(void);

 public:

  PathFindingMovement(MapEntity *target, int speed);
  ~PathFindingMovement(void);

  bool is_finished(void);
};

#endif

