/**
 * This module defines the class AnimatedTile.
 */

#include <SDL/SDL.h>
#include "AnimatedTile.h"
#include "ZSDX.h"

/**
 * Constructor.
 * @param obstacle is the tile an obstacle?
 * @param sequence animation sequence type
 * @param width width of the tile (the same for each frame of the animation)
 * @param height height of the tile (the same for each frame of the animation)
 * @param x1 x position of the first frame in the tileset
 * @param y1 y position of the first frame in the tileset
 * @param x2 x position of the second frame in the tileset
 * @param y2 y position of the second frame in the tileset
 * @param x3 x position of the third frame in the tileset
 * @param y3 y position of the third frame in the tileset
 */
AnimatedTile::AnimatedTile(Obstacle obstacle,
			   TileAnimationSequence sequence,
			   int width, int height,
			   int x1, int y1,
			   int x2, int y2,
			   int x3, int y3):
  Tile(obstacle, width, height), sequence(sequence) {

  this->position_in_tileset[0].x = x1;
  this->position_in_tileset[0].y = y1;
  this->position_in_tileset[1].x = x2;
  this->position_in_tileset[1].y = y2;
  this->position_in_tileset[2].x = x3;
  this->position_in_tileset[2].y = y3;

  for (int i = 0; i < 3; i++) {
    this->position_in_tileset[i].w = width;
    this->position_in_tileset[i].h = height;
  }
}

/**
 * Displays the tile on a surface.
 * This function is abstract because the way the image is displayed
 * depends on the type of tile image (animated or not).
 * @param surface the destination surface
 * @param position_in_surface position of the tile on the surface
 * @param tileset_image the tileset image of this tile
 */
void AnimatedTile::display(SDL_Surface *surface, SDL_Rect &position_in_surface, SDL_Surface *tileset_image) {
  short current_frame = ZSDX::get_tile_animation_manager()->get_current_frame(sequence);

  SDL_BlitSurface(tileset_image,
		  &position_in_tileset[current_frame],
		  surface,
		  &position_in_surface);

}
