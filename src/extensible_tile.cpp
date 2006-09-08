#include "SDL/SDL.h"
#include "extensible_tile.h"
#include "map.h"

ExtensibleTile::ExtensibleTile(SDL_Rect &where_in_tileset, tile_obstacle_t obstacle):
  TileData(obstacle, where_in_tileset.w, where_in_tileset.h),
  where_in_tileset(where_in_tileset),
  repeat_x(1), repeat_y(1) {

}

ExtensibleTile::ExtensibleTile(ExtensibleTile *other, int repeat_x, int repeat_y):
  TileData(other->obstacle, other->where_in_tileset.w * repeat_x, other->where_in_tileset.h * repeat_y),
  where_in_tileset(other->where_in_tileset),
  repeat_x(repeat_x), repeat_y(repeat_y) {

}

void ExtensibleTile::display_on_map(Map *map, SDL_Rect &where_in_map) {
  SDL_Rect dst = where_in_map;
  SDL_Surface *tileset_image = map->get_tileset()->get_image();
  SDL_Surface *map_image = map->get_surface();
  for (int j = 0; j < repeat_y; j++) {
    for (int i = 0; i < repeat_x; i++) {
      SDL_BlitSurface(tileset_image, &where_in_tileset, map_image, &dst);
      dst.x += where_in_tileset.w;
    }
    dst.y += where_in_tileset.h;
    dst.x = where_in_map.x;
  }
}
