#ifndef ZSDX_ZSDX_H
#define ZSDX_ZSDX_H

enum tile_obstacle_t {
  NO_OBSTACLE,
  OBSTACLE,
  OBSTACLE_UP_RIGHT,
  OBSTACLE_UP_LEFT,
  OBSTACLE_DOWN_LEFT,
  OBSTACLE_DOWN_RIGHT,
};

enum animation_sequence_t {
  ANIMATION_SEQUENCE_1232,
  ANIMATION_SEQUENCE_123,
};

class TileData;
class Tile;

#endif
