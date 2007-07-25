/**
 * This module defines the class AnimatedTile.
 */

#ifndef ZSDX_ANIMATED_TILE_H
#define ZSDX_ANIMATED_TILE_H

#include <SDL/SDL.h>
#include "Tile.h"
#include "TileAnimationManager.h"

/**
 * Animated tile.
 * An animated tile has always three frames.
 */
class AnimatedTile: public Tile {

 private:

  /**
   * Animation sequence type: 0-1-2-1 or 0-1-2.
   */
  const TileAnimationSequence sequence;
  
  /**
   * Array of 3 SDL_Rect representing the 3 animation frames
   * in the tileset image.
   * The 3 frames should have the same width and height.
   */
  SDL_Rect position_in_tileset[3];

 public:

  /**
   * Constructor.
   * @param position_in_tileset array of 3 SDL_Rect
   * representing the 3 animation frames in the tileset image
   * @param sequence animation sequence type
   * @param obstacle is the tile an obstacle?
   */
  AnimatedTile(const SDL_Rect *position_in_tileset,
		    TileAnimationSequence sequence,
		    Obstacle obstacle);

  /**
   * Destructor.
   */
  inline ~AnimatedTile(void) { }

  /**
   * Displays the tile image on a surface.
   * This function is abstract because the way the image is displayed
   * depends on the type of tile image (animated or not).
   * @param surface the destination surface
   * @param position_in_surface position of the tile on the surface
   * @param tileset_image the tileset image of this tile
   */
  void display(SDL_Surface *surface, SDL_Rect &position_in_surface, SDL_Surface *tileset_image);

};

#endif
