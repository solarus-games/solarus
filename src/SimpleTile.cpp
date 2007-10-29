/**
 * This module defines the class SimpleTileImage.
 */

#include "SimpleTile.h"
#include "Map.h"

/**
 * Creates a simple tile.
 * @param x x position of the tile in the tileset
 * @param y y position of the tile in the tileset
 * @param width width of the tile in the tileset
 * @param height height of the tile in the tileset
 * @param obstacle is the tile an obstacle?
 */
SimpleTile::SimpleTile(Obstacle obstacle, int x, int y, int width, int height):
  Tile(obstacle, width, height) {

  position_in_tileset.x = x;
  position_in_tileset.y = y;
  position_in_tileset.w = width;
  position_in_tileset.h = height;
}

/**
 * Displays the tile image on a surface.
 * @param surface the destination surface
 * @param position_in_surface position of the tile on the surface
 * @param tileset_image the tileset image of this tile
 */
void SimpleTile::display(SDL_Surface *surface, SDL_Rect &position_in_surface, SDL_Surface *tileset_image) {
  SDL_Rect dst = position_in_surface;
  SDL_BlitSurface(tileset_image, &position_in_tileset, surface, &dst);
}
