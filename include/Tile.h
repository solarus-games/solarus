/**
 * This module defines the class Tile.
 */

#ifndef ZSDX_TILE_IMAGE_H
#define ZSDX_TILE_IMAGE_H

#include <SDL/SDL.h>
#include "Map.h"

/**
 * Abstract class for a tile image.
 * There are two subclasses: SimpleTile and AnimatedTile.
 * The width and the height of a tile are always multiples or 8.
 */
class Tile {
  
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
  inline Tile(tile_obstacle_t obstacle, int width, int height):
    obstacle(obstacle), width(width), height(height) { }

 public:

  /**
   * Destructor.
   */
  inline virtual ~Tile(void) { };

  /**
   * Returns the width of the tile.
   * @return the width of the tile 
   */
  inline int get_width(void) const { return width; }

  /**
   * Returns the height of the tile.
   * @return the height of the tile 
   */
  inline int get_height(void) const { return height; }
  
  /**
   * Returns the obstacle property of the tile.
   * @return the obstacle property of the tile: OBSTACLE_NONE, OBSTACLE,
   * OBSTACLE_TOP_RIGHT, OBSTACLE_TOP_LEFT, OBSTACLE_BOTTOM_LEFT or OBSTACLE_BOTTOM_RIGHT
   */
  inline tile_obstacle_t get_obstacle(void) const { return obstacle; }

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
