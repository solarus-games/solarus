#include <SDL/SDL.h>
#include "tile.h"

Tile::Tile(TileData *tile_data, SDL_Rect &where_in_map) {
  this->tile_data = tile_data;
  this->where_in_map = where_in_map;
}
