#ifndef ZSDX_ZSDX_H
#define ZSDX_ZSDX_H

#include <SDL/SDL.h>

typedef Uint32 zsdx_color_t;

enum tile_obstacle_t {
  NO_OBSTACLE,
  OBSTACLE,
  OBSTACLE_UP_RIGHT,
  OBSTACLE_UP_LEFT,
  OBSTACLE_DOWN_LEFT,
  OBSTACLE_DOWN_RIGHT,
};

#endif
