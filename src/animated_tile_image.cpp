/**
 * This module defines the class AnimatedTileImage.
 */

#include <SDL/SDL.h>
#include "animated_tile_image.h"
#include "zsdx.h"

/**
 * Constructor.
 * @param position_in_tileset array of 3 SDL_Rect
 * representing the 3 animation frames in the tileset image
 * @param sequence animation sequence type
 * @param obstacle is the tile an obstacle?
 */
AnimatedTileImage::AnimatedTileImage(const SDL_Rect *position_in_tileset,
				     tile_animation_sequence_t sequence,
				     tile_obstacle_t obstacle):
  TileImage(obstacle, position_in_tileset[0].w, position_in_tileset[0].h), sequence(sequence) {

  this->position_in_tileset[0] = position_in_tileset[0];
  this->position_in_tileset[1] = position_in_tileset[1];
  this->position_in_tileset[2] = position_in_tileset[2];
}

/**
 * Displays the tile image on a surface.
 * This function is abstract because the way the image is displayed
 * depends on the type of tile image (animated or not).
 * @param surface the destination surface
 * @param position_in_surface position of the tile on the surface
 * @param tileset_image the tileset image of this tile
 */
void AnimatedTileImage::display(SDL_Surface *surface, SDL_Rect &position_in_surface, SDL_Surface *tileset_image) {
  short current_frame = ZSDX::get_tile_animation_manager()->get_current_frame(sequence);

  SDL_BlitSurface(tileset_image,
		  &position_in_tileset[current_frame],
		  surface,
		  &position_in_surface);

}
