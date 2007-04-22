/**
 * This module defines the class Tile.
 */

using namespace std;
#include <iostream>
#include "tile.h"

/**
 * Creates a new tile without repeating its pattern.
 * It is equivalent to Tile(tile_image, position_in_map, 1, 1).
 * @param tile_image image of the tile (SimpleTileImage or AnimatedTileImage)
 * @param position_in_map position of the tile on the map
 */
Tile::Tile(TileImage *tile_image, SDL_Rect &position_in_map):
  tile_image(tile_image), repeat_x(1), repeat_y(1) {

  this->position_in_map.x = position_in_map.x;
  this->position_in_map.y = position_in_map.y;
  this->position_in_map.w = tile_image->get_width();
  this->position_in_map.h = tile_image->get_height();
};

/**
 * Creates a new tile.
 * @param tile_image image of the tile (SimpleTileImage or AnimatedTileImage)
 * @param position_in_map position of the tile on the map
 * @param repeat_x number of times the pattern is repeated on x
 * @param repeat_y number of times the pattern is repeated on y
 */
Tile::Tile(TileImage *tile_image, SDL_Rect &position_in_map, int repeat_x, int repeat_y):
  tile_image(tile_image), repeat_x(repeat_x), repeat_y(repeat_y) {

  this->position_in_map.x = position_in_map.x;
  this->position_in_map.y = position_in_map.y;
  this->position_in_map.w = tile_image->get_width();
  this->position_in_map.h = tile_image->get_height();
};

/**
 * Displays the tile on the map.
 * @param map the map
 */
void Tile::display_on_map(Map *map) {
  SDL_Rect dst;

  dst.w = position_in_map.w;
  dst.h = position_in_map.h;

  int limit_x = position_in_map.x + position_in_map.w * repeat_x;
  int limit_y = position_in_map.y + position_in_map.h * repeat_y;
  
  for (dst.y = position_in_map.y; dst.y < limit_y; dst.y += position_in_map.h) {
    dst.x = position_in_map.x;
    for (dst.x = position_in_map.x; dst.x < limit_x; dst.x += position_in_map.w) {
      tile_image->display(map->get_surface(), dst, map->get_tileset()->get_image());
    }
  }
}
