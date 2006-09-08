#include <SDL/SDL.h>
#include "animated_tile.h"
#include "zsdx.h"

AnimatedTile::AnimatedTile(const SDL_Rect *where_in_src,
			   tile_animation_sequence_t sequence,
			   tile_obstacle_t obstacle):
  TileData(obstacle, where_in_src[0].w, where_in_tileset[0].h), sequence(sequence) {
  this->where_in_tileset[0] = where_in_tileset[0];
  this->where_in_tileset[1] = where_in_tileset[1];
  this->where_in_tileset[2] = where_in_tileset[2];
}

void AnimatedTile::display_on_map(Map *map, SDL_Rect &where_in_map) {
  short current_frame = zsdx.get_tile_animation_manager()->get_current_frame(sequence);
  SDL_BlitSurface(map->get_tileset()->get_image(), &where_in_tileset[current_frame], map->get_surface(), &where_in_map);
}
