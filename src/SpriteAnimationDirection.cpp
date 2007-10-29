/**
 * This module defines the class SpriteAnimationDirection.
 */

#include "SpriteAnimationDirection.h"

/**
 * Constructor.
 * @param src_image image from which the image sequence is extracted
 * @param nb_frames number of frames in the sequence
 * @param frames position of each frames of the sequence in the image
 * @param loop_on_frame number of the frame to loop on (-1 for no loop)
 */
SpriteAnimationDirection::SpriteAnimationDirection(SDL_Surface *src_image, int nb_frames,
						   SDL_Rect *frames, int loop_on_frame):
  src_image(src_image), nb_frames(nb_frames), frames(frames), loop_on_frame(loop_on_frame) {

}

/**
 * Simple constructor without specifying the number of frames.
 * You should call it if there is only one frame in your sequence.
 * @param src_image image from which the frame is extracted
 * @param frame the only frame of the sequence
 */
SpriteAnimationDirection::SpriteAnimationDirection(SDL_Surface *src_image, SDL_Rect &frame):
  src_image(src_image), nb_frames(1), loop_on_frame(-1) {
  frames = new SDL_Rect[1];
  frames[0] = frame;
}

/**
 * Destructor.
 */
SpriteAnimationDirection::~SpriteAnimationDirection(void) {
  delete[] frames;
}

/**
 * Returns the next frame of a specific frame.
 * Returns -1 if the animation is over (i.e. the frame is the
 * last one and there is no loop).
 * @param frame the frame to consider
 */
int SpriteAnimationDirection::get_next_frame(int frame) {
  int next_frame = frame + 1;

  // if we are on the last frame
  if (next_frame == nb_frames) {
    // we loop on the appropriate frame
    // or -1 if there is no loop
    next_frame = loop_on_frame;
  }

  return next_frame;
}

/**
 * Displays a specific frame on the map.
 * @param map the map
 * @param position_in_map position of the sprite
 * @param current_frame the frame to show
 */
void SpriteAnimationDirection::display_on_map(Map *map, SDL_Rect &position_in_map, int current_frame) {

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
