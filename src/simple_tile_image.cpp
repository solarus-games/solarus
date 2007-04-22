/**
 * This module defines the class SimpleTileImage.
 */

#include "SDL/SDL.h"
#include "simple_tile_image.h"
#include "map.h"

/**
 * Creates a simple tile.
 * @param position_in_tileset position of the tile in the tileset
 * @param obstacle is the tile an obstacle?
 */
SimpleTileImage::SimpleTileImage(SDL_Rect &position_in_tileset, tile_obstacle_t obstacle):
  TileImage(obstacle, position_in_tileset.w, position_in_tileset.h),
  position_in_tileset(position_in_tileset) {

}

/**
 * Displays the tile image on a surface.
 * This function is abstract because the way the image is displayed
 * depends on the type of tile image (animated or not).
 * @param surface the destination surface
 * @param position_in_surface position of the tile on the surface
 * @param tileset_image the tileset image of this tile
 */
void SimpleTileImage::display(SDL_Surface *surface, SDL_Rect &position_in_surface, SDL_Surface *tileset_image) {
  SDL_BlitSurface(tileset_image, &position_in_tileset, surface, &position_in_surface);

}
