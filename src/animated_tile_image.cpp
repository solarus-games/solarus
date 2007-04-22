/**
 * This module defines the class AnimatedTileImage.
 */

#include <SDL/SDL.h>
#include "animated_tile_image.h"
#include "zsdx.h"

/**
 * Constructor.
 * @param position_in_tileset array of 3 SDL_Rect
 * representing the 3 animation frames in the tileset image
 * @param sequence animation sequence type
 * @param obstacle is the tile an obstacle?
 */
AnimatedTileImage::AnimatedTileImage(const SDL_Rect *position_in_tileset,
				     tile_animation_sequence_t sequence,
				     tile_obstacle_t obstacle):
  TileImage(obstacle, position_in_tileset[0].w, position_in_tileset[0].h), sequence(sequence) {

  this->position_in_tileset[0] = position_in_tileset[0];
  this->position_in_tileset[1] = position_in_tileset[1];
  this->position_in_tileset[2] = position_in_tileset[2];
}

/**
 * Displays the tile image on the map.
 * @param map the map
 * @param position_in_map position of the tile on the map
 */
void AnimatedTileImage::display_on_map(Map *map, SDL_Rect &position_in_map) {
  short current_frame = ZSDX::get_tile_animation_manager()->get_current_frame(sequence);

  SDL_BlitSurface(map->get_tileset()->get_image(),
		  &position_in_tileset[current_frame],
		  map->get_surface(),
		  &position_in_map);
}
