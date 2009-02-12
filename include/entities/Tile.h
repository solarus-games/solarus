#ifndef ZSDX_TILE_H
#define ZSDX_TILE_H

#include "Common.h"
#include "MapEntity.h"

/**
 * Abstract class for a tile image.
 * There are two subclasses: SimpleTile and AnimatedTile.
 * The width and the height of a tile are always multiples or 8.
 */
class Tile {
  
 protected:

  const MapEntity::Obstacle obstacle; /**< is the tile an obstacle? */

  const int width;                    /**< tile width (multiple of 8) */
  const int height;                   /**< tile height (multiple of 8) */

  Tile(MapEntity::Obstacle obstacle, int width, int height);

 public:

  virtual ~Tile(void);

  int get_width(void) const;
  int get_height(void) const;
  MapEntity::Obstacle get_obstacle(void) const;

  /**
   * Displays the tile on a surface.
   * This function is abstract because the way the image is displayed
   * depends on the type of tile (animated or not).
   * @param surface the destination surface
   * @param position_in_surface position of the tile on the surface
   * @param tileset_image the tileset image of this tile
   */
  virtual void display(SDL_Surface *surface, SDL_Rect &position_in_surface, SDL_Surface *tileset_image) = 0;
};

#endif
