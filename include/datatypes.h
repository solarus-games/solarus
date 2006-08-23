#ifndef ZSDX_ZSDX_H
#define ZSDX_ZSDX_H

// TODO: remove that file?

/* Datatypes used in ZSDX
 */

/* Obstacle types for the tiles
 */
enum tile_obstacle_t {
  NO_OBSTACLE,         // the tile is not an obstacle
  OBSTACLE,            // the tile is entirely an obstacle
  OBSTACLE_UP_RIGHT,   // the upper-right half of the tile is an obstacle
  OBSTACLE_UP_LEFT,    // the upper-left half of the tile is an obstacle
  OBSTACLE_DOWN_LEFT,  // the lower-left half of the tile is an obstacle
  OBSTACLE_DOWN_RIGHT, // the lower-right half of the tile is an obstacle
};

// class declarations because of cycling dependencies
class TileData;
class Tile;

#endif
