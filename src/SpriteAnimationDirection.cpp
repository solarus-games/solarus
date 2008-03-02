/**
 * This module defines the class SpriteAnimationDirection.
 */

#include "SpriteAnimationDirection.h"
#include "Map.h"

/**
 * Constructor.
 * @param nb_frames number of frames in the sequence
 * @param frames position of each frames of the sequence in the image
 * (the pointer is copied, not the array, so don't modify after calling this constructor)
 */
SpriteAnimationDirection::SpriteAnimationDirection(int nb_frames, SDL_Rect *frames):
  nb_frames(nb_frames), frames(frames) {

}

/**
 * Destructor.
 */
SpriteAnimationDirection::~SpriteAnimationDirection(void) {
  delete[] frames;
}

/**
 * Displays a specific frame on the map.
 * @param map the map
 * @param position_in_map position of the sprite
 * @param current_frame the frame to show
 * @param src_image the image from wich the frame is extracted
 */
void SpriteAnimationDirection::display_on_map(Map *map, SDL_Rect &position_in_map, int current_frame, SDL_Surface *src_image) {

//   printf("trying to blit rect (%d,%d,%d,%d) at position (%d,%d,%d,%d)\n",
// 	 frames[current_frame].x, frames[current_frame].y, frames[current_frame].w, frames[current_frame].h,
// 	 position_in_map.x, position_in_map.y, position_in_map.w, position_in_map.h);

  SDL_Rect *current_frame_rect = &frames[current_frame];
  SDL_Rect *screen_position = map->get_screen_position();
  SDL_Rect position_in_screen = position_in_map;

  position_in_screen.x -= screen_position->x;
  position_in_screen.y -= screen_position->y;

  SDL_BlitSurface(src_image, current_frame_rect, map->get_surface(), &position_in_screen);
}
