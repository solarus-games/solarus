#ifndef ZSDX_OBSTACLE_H
#define ZSDX_OBSTACLE_H

/**
 * Obstacle property for the tiles or the dynamic entities.
 * Values lower than or equal to OBSTACLE_NONE correspond to entities the hero can walk on.
 * Values higher than OBSTACLE_NONE correspond to obstacles.
 */
enum Obstacle {

  OBSTACLE_HOLE          = -3, /**< the entity is a hole */ 
  OBSTACLE_SHALLOW_WATER = -2, /**< the entity is some shallow water */
  OBSTACLE_DEEP_WATER    = -1, /**< the entity is some deep water */
  OBSTACLE_NONE          =  0, /**< the entity is normal normal ground without obstacle */
  OBSTACLE,                    /**< the entity is entirely an obstacle */
  OBSTACLE_TOP_RIGHT,          /**< the upper-right half of the entity is an obstacle */
  OBSTACLE_TOP_LEFT,           /**< the upper-left half of the entity is an obstacle */
  OBSTACLE_BOTTOM_LEFT,        /**< the lower-left half of the entity is an obstacle */
  OBSTACLE_BOTTOM_RIGHT,       /**< the lower-right half of the entity is an obstacle */
};

#endif
