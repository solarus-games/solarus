/**
 * This module defines the class TileImage.
 */

#ifndef ZSDX_TILE_IMAGE_H
#define ZSDX_TILE_IMAGE_H

#include <SDL/SDL.h>
#include "map.h"

/**
 * Abstract class for a tile image.
 * There are two subclasses: SimpleTile and AnimatedTile.
 * The width and the height of a tile image are always multiples or 8.
 */
class TileImage {
  
 protected:

  /**
   * Is the tile an obstacle?
   */
  const tile_obstacle_t obstacle;

  /**
   * Tile width (multiple of 8).
   */
  const int width;

  /**
   * Tile height (multiple of 8).
   */
  const int height;

  /**
   * Constructor.
   * It is called by the subclasses. 
   */
  inline TileImage(tile_obstacle_t obstacle, int width, int height):
    obstacle(obstacle), width(width), height(height) { }

 public:

  /**
   * Destructor.
   */
  inline virtual ~TileImage(void) { };

  /**
   * Returns the width of the tile image.
   * @return the width of the tile image 
   */
  inline int get_width(void) const { return width; }

  /**
   * Returns the height of the tile image.
   * @return the height of the tile image 
   */
  inline int get_height(void) const { return height; }
  
  /**
   * Returns the obstacle property of the tile.
   * @return the obstacle property of the tile: 
   */
  inline tile_obstacle_t get_obstacle(void) const { return obstacle; }

  /**
   * Displays the tile image on a surface.
   * This function is abstract because the way the image is displayed
   * depends on the type of tile image (animated or not).
   * @param surface the destination surface
   * @param position_in_surface position of the tile on the surface
   * @param tileset_image the tileset image of this tile
   */
  virtual void display(SDL_Surface *surface, SDL_Rect &position_in_surface, SDL_Surface *tileset_image) = 0;
};

#endif
