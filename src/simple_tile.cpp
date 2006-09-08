#include "SDL/SDL.h"
#include "simple_tile.h"
#include "map.h"

SimpleTile::SimpleTile(SDL_Rect &where_in_tileset, tile_obstacle_t obstacle):
  TileData(obstacle, where_in_tileset.w, where_in_tileset.h),
  where_in_tileset(where_in_tileset) {

}

void SimpleTile::display_on_map(Map *map, SDL_Rect &where_in_map) {
  SDL_BlitSurface(map->get_tileset()->get_image(), &where_in_tileset, map->get_surface(), &where_in_map);
}
