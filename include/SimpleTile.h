/**
 * This module defines the class SimpleTile.
 */

#ifndef ZSDX_SIMPLE_TILE_H
#define ZSDX_SIMPLE_TILE_H

#include <SDL/SDL.h>
#include "Tile.h"

/**
 * Static tile image.
 */
class SimpleTile: public Tile {

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
  SimpleTile(SDL_Rect &position_in_tileset, tile_obstacle_t obstacle);

  /**
   * Destructor.
   */
  inline ~SimpleTile(void) { }

  /**
   * Displays the tile on a surface.
   * @param surface the destination surface
   * @param position_in_surface position of the tile on the surface
   * @param tileset the tileset of this tile
   */
  void display(SDL_Surface *surface, SDL_Rect &position_in_surface, SDL_Surface *tileset_image);
};

#endif
