#include <SDL/SDL.h>
#include "map_object.h"

// Tile

Tile::Tile(TileData *tile_data, SDL_Rect &where_in_map) {
  this->tile_data = tile_data;
  this->where_in_map = where_in_map;
}

/* Tile::Tile(Tile &tile) { */
/*   this->tile_data = tile.tile_data; */
/*   this->where_int_map = tile->where_in_map; // hum */
/*   this->obstacle = tile.obstacle; */
/* } */
