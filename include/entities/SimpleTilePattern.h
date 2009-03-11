#ifndef ZSDX_SIMPLE_TILE_PATTERN_H
#define ZSDX_SIMPLE_TILE_PATTERN_H

#include "Common.h"
#include "entities/TilePattern.h"

/**
 * Non-animated tile pattern.
 */
class SimpleTilePattern: public TilePattern {

 protected:

  /**
   * Position of the tile pattern in the tileset image.
   */
  SDL_Rect position_in_tileset;

 public:

  SimpleTilePattern(MapEntity::Obstacle obstacle, int x, int y, int width, int height);
  ~SimpleTilePattern(void);

  void display(SDL_Surface *destination, const SDL_Rect &destination_position, SDL_Surface *tileset_image);
};

#endif
