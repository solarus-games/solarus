#include "entities/SimpleTilePattern.h"
#include "Map.h"

/**
 * Creates a simple tile pattern.
 * @param obstacle is the tile pattern an obstacle?
 * @param x x position of the tile pattern in the tileset
 * @param y y position of the tile pattern in the tileset
 * @param width width of the tile pattern in the tileset
 * @param height height of the tile pattern in the tileset
 */
SimpleTilePattern::SimpleTilePattern(MapEntity::Obstacle obstacle, int x, int y, int width, int height):
  TilePattern(obstacle, width, height) {

  position_in_tileset.x = x;
  position_in_tileset.y = y;
  position_in_tileset.w = width;
  position_in_tileset.h = height;
}

/**
 * Destructor.
 */
SimpleTilePattern::~SimpleTilePattern(void) {

}

/**
 * Displays the tile image on a surface.
 * @param destination the destination surface
 * @param dst_position position of the tile pattern on the destination surface
 * @param tileset_image the tileset image of this tile
 */
void SimpleTilePattern::display(SDL_Surface *destination, const SDL_Rect &dst_position, SDL_Surface *tileset_image) {

  SDL_Rect dst = dst_position;
  SDL_BlitSurface(tileset_image, &position_in_tileset, destination, &dst);
}
