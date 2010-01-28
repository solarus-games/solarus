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
#ifndef SOLARUS_OBSTACLE_H
#define SOLARUS_OBSTACLE_H

/**
 * Obstacle property for the tiles or the dynamic entities.
 * Values lower than or equal to OBSTACLE_NONE correspond to entities the hero can walk on.
 * Values higher than OBSTACLE_NONE correspond to obstacles.
 */
enum Obstacle {

  OBSTACLE_LADDER        = -4, /**< the entity is a ladder and the hero walks slower on it */
  OBSTACLE_HOLE          = -3, /**< the entity is a hole */ 
  OBSTACLE_SHALLOW_WATER = -2, /**< the entity is some shallow water */
  OBSTACLE_DEEP_WATER    = -1, /**< the entity is some deep water */
  OBSTACLE_NONE          =  0, /**< the entity is normal normal ground without obstacle */
  OBSTACLE,                    /**< the entity is entirely an obstacle */
  OBSTACLE_TOP_RIGHT,          /**< the upper-right half of the entity is an obstacle */
  OBSTACLE_TOP_LEFT,           /**< the upper-left half of the entity is an obstacle */
  OBSTACLE_BOTTOM_LEFT,        /**< the lower-left half of the entity is an obstacle */
  OBSTACLE_BOTTOM_RIGHT,       /**< the lower-right half of the entity is an obstacle */
  OBSTACLE_EMPTY               /**< there is no ground */
};

#endif

