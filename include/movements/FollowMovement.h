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
#ifndef ZSDX_FOLLOW_MOVEMENT_H
#define ZSDX_FOLLOW_MOVEMENT_H

#include "Common.h"
#include "MovementWithCollision.h"

/**
 * Movement for an entity attached to another entity.
 * At each moment, the entity is placed at a position
 * relative to the entity followed.
 */
class FollowMovement: public MovementWithCollision {

 private:

  MapEntity *entity_followed; /**< the entity followed by this movement */
  const int x;                /**< x coordinate of where this entity should be placed,
				* relative to the entity followed */
  const int y;                /**< y coordinate of where this entity should be placed,
			       * relative to the entity followed */

  const bool with_collisions; /**< indicates that this movement is sensitive to the obstacles */
  bool finished;              /**< indicates that the movement is stopped because of a collision */

 protected:

  void update(void);

 public:

  FollowMovement(Map *map, MapEntity *entity_followed, int x, int y, bool with_collisions);
  ~FollowMovement(void);

  bool is_finished(void);

};

#endif
