/**
 * This module defines the class SimpleTileImage.
 */

#ifndef ZSDX_SIMPLE_TILE_H
#define ZSDX_SIMPLE_TILE_H

#include <SDL/SDL.h>
#include "tile_image.h"

/**
 * Static tile image.
 */
class SimpleTileImage: public TileImage {

 protected:

  /**
   * Position of the tile in the tileset image
   */
  SDL_Rect position_in_tileset;

 public:

  /**
   * Creates a simple tile.
   * @param position_in_tileset position of the tile in the tileset
   * @param obstacle is the tile an obstacle?
   */
  SimpleTileImage(SDL_Rect &position_in_tileset, tile_obstacle_t obstacle);

  /**
   * Destructor.
   */
  inline ~SimpleTileImage(void) { }

  /**
   * Displays the tile image on the map.
   * @param map the map
   * @param position_in_map position of the tile on the map
   */
  void display_on_map(Map *map, SDL_Rect &position_in_map);
};

#endif
