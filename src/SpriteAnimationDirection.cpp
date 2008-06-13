#include "SpriteAnimationDirection.h"

/**
 * Constructor.
 * @param nb_frames number of frames in the sequence
 * @param frames position of each frames of the sequence in the image
 * (the pointer is copied, not the array, so don't modify after calling this constructor)
 * @param x_origin x coordinate of the sprite's origin
 * @param y_origin y coordinate of the sprite's origin
 */
SpriteAnimationDirection::SpriteAnimationDirection(int nb_frames, SDL_Rect *frames,
						   int x_origin, int y_origin):
  nb_frames(nb_frames), frames(frames),
  x_origin(x_origin), y_origin(y_origin) {

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
 * @param x x coordinate of the sprite on this surface
 * (the origin point will be displayed at this position)
 * @param y y coordinate of the sprite on this surface
 * (the origin point will be displayed at this position)
 * @param current_frame the frame to show
 * @param src_image the image from wich the frame is extracted
 */
void SpriteAnimationDirection::display(SDL_Surface *destination, int x, int y,
				       int current_frame, SDL_Surface *src_image) {

  SDL_Rect position_up_left; // position of the sprite's upper left corner

  SDL_Rect *current_frame_rect = &frames[current_frame];

  position_up_left.x = x - x_origin;
  position_up_left.y = y - y_origin;
  position_up_left.w = current_frame_rect->w;
  position_up_left.h = current_frame_rect->h;

  SDL_BlitSurface(src_image, current_frame_rect, destination, &position_up_left);
}
