#include "SDL/SDL.h"
#include "map_object.h"
#include "tile_data.h"
#include "global.h"

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

ExtensibleTile::ExtensibleTile(SDL_Surface *src_image, SDL_Rect &where_in_src, tile_obstacle_t obstacle):
  SimpleTile(src_image, where_in_src, obstacle), repeat_x(1), repeat_y(1) {

}

ExtensibleTile::ExtensibleTile(ExtensibleTile *other, int repeat_x, int repeat_y):
  SimpleTile(other->src_image, other->where_in_src, other->obstacle), repeat_x(repeat_x), repeat_y(repeat_y) {

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

// AnimatedTile

AnimatedTile::~AnimatedTile(void) {

}

AnimatedTile::AnimatedTile(SDL_Surface *src_image,
			   const SDL_Rect *where_in_src,
			   animation_sequence_t sequence,
			   tile_obstacle_t obstacle):
  TileData(obstacle), src_image(src_image), sequence(sequence) {
  this->where_in_src[0] = where_in_src[0];
  this->where_in_src[1] = where_in_src[1];
  this->where_in_src[2] = where_in_src[2];
}

void AnimatedTile::display_on_map(SDL_Surface *map, SDL_Rect &where_in_map) {
  short current_frame = zsdx_global.animation_frame_handler.get_current_frame(sequence);
  SDL_BlitSurface(src_image, &where_in_src[current_frame], map, &where_in_map);
}
