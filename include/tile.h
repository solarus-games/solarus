#ifndef ZSDX_TILE_H
#define ZSDX_TILE_H

#include "map_object.h"
#include "tile_data.h"

class Tile: public MapObject {

 private:
  TileData *tile_data;

 public:
  Tile(TileData *tile_data, SDL_Rect &where_in_map);
  inline ~Tile() { }
  inline void display_on_map(SDL_Surface *map) { tile_data->display_on_map(map, where_in_map); }
};

#endif
