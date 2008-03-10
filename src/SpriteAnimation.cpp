#include "SpriteAnimation.h"
#include "SpriteAnimationDirection.h"
#include "FileTools.h"

/**
 * Constructor.
 * @param image_file_name the image from which the frames are extracted
 * @param nb_directions number of directions in this animation
 * @param directions the image sequence of each direction
 * @param frame_interval interval in millisecond between two frames for this sprite animation
 */
SpriteAnimation::SpriteAnimation(string image_file_name, int nb_directions,
				 SpriteAnimationDirection **directions,
				 Uint32 frame_interval, int loop_on_frame):
  nb_directions(nb_directions), directions(directions),
  frame_interval(frame_interval), loop_on_frame(loop_on_frame) {

  src_image = IMG_Load(FileTools::data_file_add_prefix(image_file_name.c_str()));
}

/**
 * Destructor.
 */
SpriteAnimation::~SpriteAnimation(void) {
  for (int i = 0; i < nb_directions; i++) {
    delete directions[i];
  }

  delete[] directions;

  SDL_FreeSurface(src_image);
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
 * @param position position of the sprite on this surface (only x and y
 * are considered), the hotspot will be placed at this position
 * @param current_direction the direction to show
 * @param current_frame the frame to show in this direction
 */
void SpriteAnimation::display(SDL_Surface *destination, const SDL_Rect &position,
			      int current_direction, int current_frame) {

  directions[current_direction]->display(destination, position, current_frame, src_image);
}
