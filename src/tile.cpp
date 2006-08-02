#include "map_object.h"
#include "tile_data.h"

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

// SimpleTile

SimpleTile::SimpleTile(SDL_Surface *src_image, SDL_Rect &where_in_src, tile_obstacle_t obstacle):
  TileData(obstacle), src_image(src_image), where_in_src(where_in_src) {

}

SimpleTile::~SimpleTile(void) {
  
}

void SimpleTile::display_on_map(SDL_Surface *map, SDL_Rect &where_in_map) {
  SDL_BlitSurface(src_image, &where_in_src, map, &where_in_map);
}

// ExtensibleTile

ExtensibleTile::~ExtensibleTile(void) {
  
}

ExtensibleTile::ExtensibleTile(SDL_Surface *src_image, SDL_Rect &where_in_src, tile_obstacle_t obstacle, int repeat_x, int repeat_y):
  SimpleTile(src_image, where_in_src, obstacle), repeat_x(repeat_x), repeat_y(repeat_y) {

}

void ExtensibleTile::display_on_map(SDL_Surface *map, SDL_Rect &where_in_map) {
  SDL_Rect dst = where_in_map;
  for (int j = 0; j < repeat_y; j++) {
    for (int i = 0; i < repeat_x; i++) {
      SDL_BlitSurface(src_image, &where_in_src, map, &dst);
      dst.x += where_in_src.w;
    }
    dst.y += where_in_src.h;
    dst.x = where_in_map.x;
  }
}
