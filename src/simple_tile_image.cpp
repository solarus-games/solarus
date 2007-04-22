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
 * Displays the tile image on the map.
 * @param map the map
 * @param position_in_map position of the tile on the map
 */
void SimpleTileImage::display_on_map(Map *map, SDL_Rect &position_in_map) {
  SDL_BlitSurface(map->get_tileset()->get_image(), &position_in_tileset, map->get_surface(), &position_in_map);
}
