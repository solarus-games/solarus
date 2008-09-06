#ifndef ZSDX_SIMPLE_TILE_H
#define ZSDX_SIMPLE_TILE_H

#include "Common.h"
#include "Tile.h"

/**
 * Static tile image.
 */
class SimpleTile: public Tile {

 protected:

  /**
   * Position of the tile in the tileset image.
   */
  SDL_Rect position_in_tileset;

 public:

  SimpleTile(MapEntity::Obstacle obstacle, int x, int y, int width, int height);
  ~SimpleTile(void);

  void display(SDL_Surface *surface, SDL_Rect &position_in_surface, SDL_Surface *tileset_image);
};

#endif
