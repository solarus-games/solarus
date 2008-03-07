#include "SpriteAnimation.h"
#include "SpriteAnimationDirection.h"

/**
 * Constructor.
 * @param src_image the image from which the frames are extracted
 * @param nb_directions number of directions in this animation
 * @param directions the image sequence of each direction
 * @param x_hotspot x coordinate of the sprite's origin
 * @param y_hotspot y coordinate of the sprite's origin
 * @param frame_interval interval in millisecond between two frames for this sprite animation
 */
SpriteAnimation::SpriteAnimation(SDL_Surface *src_image, int nb_directions,
				 SpriteAnimationDirection **directions,
				 int x_hotspot, int y_hotspot, Uint32 frame_interval, int loop_on_frame):
  src_image(src_image), nb_directions(nb_directions), directions(directions),
  x_hotspot(x_hotspot), y_hotspot(y_hotspot),
  frame_interval(frame_interval), loop_on_frame(loop_on_frame) {

}

/**
 * Destructor.
 */
SpriteAnimation::~SpriteAnimation(void) {
  for (int i = 0; i < nb_directions; i++) {
    delete directions[i];
  }

  delete[] directions;
}

/**
 * Returns the next frame of the current frame.
 * @param current_direction the current direction
 * @param current_frame the current frame
 * @return the next frame of the current frame in this direction
 * (or -1 if the animation is over)
 */
int SpriteAnimation::get_next_frame(int current_direction, int current_frame) {

  int next_frame = current_frame + 1;

  // if we are on the last frame
  if (next_frame == directions[current_direction]->get_nb_frames()) {
    // we loop on the appropriate frame
    // or -1 if there is no loop
    next_frame = loop_on_frame;
  }

  return next_frame;
}

/**
 * Displays a specific frame of this animation on a surface.
 * @param destination the surface on which the sprite will be displayed
 * @param position position of the sprite on this surface
 * (the hotspot will be placed at this position)
 * @param current_direction the direction to show
 * @param current_frame the frame to show in this direction
 */
void SpriteAnimation::display(SDL_Surface *destination, const SDL_Rect &position,
			      int current_direction, int current_frame) {
  SDL_Rect position_up_left; // position of the sprite's upper left corner

  position_up_left.x = position.x - x_hotspot;
  position_up_left.y = position.y - y_hotspot;

  directions[current_direction]->display(destination, position_up_left, current_frame, src_image);
}
