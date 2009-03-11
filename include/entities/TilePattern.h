#ifndef ZSDX_TILE_PATTERN_H
#define ZSDX_TILE_PATTERN_H

#include "Common.h"
#include "MapEntity.h"

/**
 * Abstract class for a tile pattern.
 * A tile pattern defines a rectangle image in a tileset and has 
 * an obstacle property.
 * There are two subclasses: SimpleTilePattern and AnimatedTilePattern.
 * The width and the height of a tile pattern are always multiples or 8.
 */
class TilePattern {
  
 protected:

  const MapEntity::Obstacle obstacle; /**< is the tile an obstacle? */

  const int width;                    /**< tile width (multiple of 8) */
  const int height;                   /**< tile height (multiple of 8) */

  TilePattern(MapEntity::Obstacle obstacle, int width, int height);

 public:

  virtual ~TilePattern(void);

  int get_width(void) const;
  int get_height(void) const;
  MapEntity::Obstacle get_obstacle(void) const;

  void display_on_map(Map *map, SDL_Rect &position_in_map);
  virtual void display(SDL_Surface *surface, const SDL_Rect &position_in_surface, SDL_Surface *tileset_image) = 0;
};

#endif
