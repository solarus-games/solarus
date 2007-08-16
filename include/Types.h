/**
 * Some definition of tiles.
 */

#ifndef ZSDX_TYPES_H
#define ZSDX_TYPES_H

/**
 * Obstacle property for the tiles or the active objects.
 */
enum Obstacle {
  OBSTACLE_NONE,         // the entity is not an obstacle
  OBSTACLE,              // the entity is entirely an obstacle
  OBSTACLE_TOP_RIGHT,    // the upper-right half of the entity is an obstacle
  OBSTACLE_TOP_LEFT,     // the upper-left half of the entity is an obstacle
  OBSTACLE_BOTTOM_LEFT,  // the lower-left half of the entity is an obstacle
  OBSTACLE_BOTTOM_RIGHT, // the lower-right half of the entity is an obstacle
};

/**
 * Layer of a tile or an active object.
 */
enum Layer {
  LAYER_LOW,             // the entity is always below Link (floor, walls, chests,
                         // enemies and 99% of the tiles and active objects)
  LAYER_INTERMEDIATE,    // Link can be below or above the entity (platforms or objects on a plaform)
  LAYER_HIGH,            // the entity is always above Link (trees, top of doors...)
  LAYER_NB               // number of layers
};

#endif
