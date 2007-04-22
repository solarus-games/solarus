/**
 * This module defines the class AnimatedTileImage.
 */

#ifndef ZSDX_ANIMATED_TILE_H
#define ZSDX_ANIMATED_TILE_H

#include <SDL/SDL.h>
#include "tile_image.h"
#include "tile_animation_manager.h"

/**
 * Animated tile image.
 * An animated tile has always three frames.
 */
class AnimatedTileImage: public TileImage {

 private:

  /**
   * Animation sequence type: 0-1-2-1 or 0-1-2.
   */
  const tile_animation_sequence_t sequence;
  
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
  AnimatedTileImage(const SDL_Rect *position_in_tileset,
		    tile_animation_sequence_t sequence,
		    tile_obstacle_t obstacle);

  /**
   * Destructor.
   */
  inline ~AnimatedTileImage(void) { }

  /**
   * Displays the tile image on the map.
   * @param map the map
   * @param position_in_map position of the tile on the map
   */
  void display_on_map(Map *map, SDL_Rect &position_in_map);
};

#endif
