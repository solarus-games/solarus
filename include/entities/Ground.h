#ifndef ZSDX_GROUND_H
#define ZSDX_GROUND_H

/**
 * The possible kinds of ground on the map.
 * The hero cannot walk on grounds above or equal to DEEP_WATER.
 */
enum Ground {
  GROUND_NORMAL,
  GROUND_GRASS,
  GROUND_SHALLOW_WATER,
  GROUND_DEEP_WATER,
  GROUND_HOLE,
};

#endif
