#ifndef ZSDX_OBSTACLE_H
#define ZSDX_OBSTACLE_H

/**
 * Obstacle property for the tiles or the active objects.
 */
enum Obstacle {
  OBSTACLE_NONE,         /**< the entity is not an obstacle */
  OBSTACLE,              /**< the entity is entirely an obstacle */
  OBSTACLE_TOP_RIGHT,    /**< the upper-right half of the entity is an obstacle */
  OBSTACLE_TOP_LEFT,     /**< the upper-left half of the entity is an obstacle */
  OBSTACLE_BOTTOM_LEFT,  /**< the lower-left half of the entity is an obstacle */
  OBSTACLE_BOTTOM_RIGHT, /**< the lower-right half of the entity is an obstacle */
};

#endif
