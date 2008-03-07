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
 * @param destination the surface on which the frame will be displayed
 * @param position_in_map position of the top left corner of the sprite
 * in the destination surface
 * @param current_frame the frame to show
 * @param src_image the image from wich the frame is extracted
 */
void SpriteAnimationDirection::display(SDL_Surface *destination, SDL_Rect &position,
				       int current_frame, SDL_Surface *src_image) {

//   printf("trying to blit rect (%d,%d,%d,%d) at position (%d,%d,%d,%d)\n",
// 	 frames[current_frame].x, frames[current_frame].y, frames[current_frame].w, frames[current_frame].h,
// 	 position_in_map.x, position_in_map.y, position_in_map.w, position_in_map.h);

  SDL_Rect *current_frame_rect = &frames[current_frame];
  SDL_BlitSurface(src_image, current_frame_rect, destination, &position);
}
