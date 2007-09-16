/**
 * This module defines the class Tile.
 */

#ifndef ZSDX_TILE_H
#define ZSDX_TILE_H

#include <SDL/SDL.h>
#include "Types.h"
#include "Map.h"

/**
 * Abstract class for a tile image.
 * There are two subclasses: SimpleTile and AnimatedTile.
 * The width and the height of a tile are always multiples or 8.
 */
class Tile {
  
 protected:

  /**
   * Is the tile an obstacle: OBSTACLE_NONE, OBSTACLE, OBSTACLE_TOP_RIGHT,
   * OBSTACLE_TOP_LEFT, OBSTACLE_BOTTOM_LEFT or OBSTACLE_BOTTOM_RIGHT.
   */
  const Obstacle obstacle;

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
   * @param obstacle type of obstacle: OBSTACLE_NONE, OBSTACLE, OBSTACLE_TOP_RIGHT,
   * OBSTACLE_TOP_LEFT, OBSTACLE_BOTTOM_LEFT or OBSTACLE_BOTTOM_RIGHT
   * @param width width of the tile in pixels (must be a multiple of 8)
   * @param height height of the tile in pixels (must be a multiple of 8)
   */
  Tile(Obstacle obstacle, int width, int height);

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
  inline Obstacle get_obstacle(void) const { return obstacle; }

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
