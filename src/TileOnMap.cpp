/**
 * This module defines the class TileOnMap.
 */

using namespace std;
#include <iostream>
#include "TileOnMap.h"

/**
 * Creates a new tile on a map without repeating its pattern.
 * It is equivalent to TileOnMap(tile_image, position_in_map, 1, 1).
 * @param tile image of the tile (SimpleTile or AnimatedTile)
 * @param position_in_map position of the tile on the map
   * @param layer layer of the tile
 */
TileOnMap::TileOnMap(Tile *tile, SDL_Rect &position_in_map, Layer layer):
  tile(tile), layer(layer), repeat_x(1), repeat_y(1) {

  this->position_in_map.x = position_in_map.x;
  this->position_in_map.y = position_in_map.y;
  this->position_in_map.w = tile->get_width();
  this->position_in_map.h = tile->get_height();
};

/**
 * Creates a new tile on a map.
 * @param tile_image image of the tile (SimpleTileImage or AnimatedTileImage)
 * @param position_in_map position of the tile on the map
 * @param repeat_x number of times the pattern is repeated on x
 * @param repeat_y number of times the pattern is repeated on y
 */
TileOnMap::TileOnMap(Tile *tile, SDL_Rect &position_in_map, Layer layer, int repeat_x, int repeat_y):
  tile(tile), layer(layer),repeat_x(repeat_x), repeat_y(repeat_y) {

  this->position_in_map.x = position_in_map.x;
  this->position_in_map.y = position_in_map.y;
  this->position_in_map.w = tile->get_width();
  this->position_in_map.h = tile->get_height();
};

/**
 * Displays the tile on the map.
 * @param map the map
 */
void TileOnMap::display_on_map(Map *map) {
  SDL_Rect dst;

  dst.w = position_in_map.w;
  dst.h = position_in_map.h;

  int limit_x = position_in_map.x + position_in_map.w * repeat_x;
  int limit_y = position_in_map.y + position_in_map.h * repeat_y;
  
  for (dst.y = position_in_map.y; dst.y < limit_y; dst.y += position_in_map.h) {
    dst.x = position_in_map.x;
    for (dst.x = position_in_map.x; dst.x < limit_x; dst.x += position_in_map.w) {
      tile->display(map->get_surface(), dst, map->get_tileset()->get_image());
    }
  }
}
