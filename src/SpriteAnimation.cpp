#include "SpriteAnimation.h"
#include "SpriteAnimationDirection.h"
#include "ResourceManager.h"

/**
 * Constructor.
 * @param image_file_name the image from which the frames are extracted
 * @param nb_directions number of directions in this animation
 * @param directions the image sequence of each direction
 * @param frame_delay delay in millisecond between two frames for this sprite animation
 * (or 0 to make no animation, for example when you have only one frame)
 * @param loop_on_frame frame to loop on after the last frame (or -1 to make no loop)
 */
SpriteAnimation::SpriteAnimation(string image_file_name, int nb_directions,
				 SpriteAnimationDirection **directions,
				 Uint32 frame_delay, int loop_on_frame):
  nb_directions(nb_directions), directions(directions),
  frame_delay(frame_delay), loop_on_frame(loop_on_frame) {

  src_image = ResourceManager::load_image(image_file_name);
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
 * Returns the image from which the frames are extracted.
 * @return the source image of this animation
 */
SDL_Surface * SpriteAnimation::get_src_image(void) {
  return src_image;
}

/**
 * Returns a direction.
 * @param direction the direction
 * @return the sequence of images corresponding to this direction
 */
SpriteAnimationDirection * SpriteAnimation::get_direction(int direction) {
  return directions[direction];
}

/**
 * Returns the delay between two frames for this sprite animation.
 * @return the frame delay in milliseconds
 */
Uint32 SpriteAnimation::get_frame_delay(void) {
  return frame_delay;
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
 * @param x x coordinate of the sprite on this surface
 * (the origin point will be displayed at this position)
 * @param y y coordinate of the sprite on this surface
 * (the origin point will be displayed at this position)
 * @param current_direction the direction to show
 * @param current_frame the frame to show in this direction
 */
void SpriteAnimation::display(SDL_Surface *destination, int x, int y,
			      int current_direction, int current_frame) {

  directions[current_direction]->display(destination, x, y, current_frame, src_image);
}

/**
 * Enables the pixel-perfect collision detection for this animation.
 */
void SpriteAnimation::enable_pixel_collisions(void) {
  
  for (int i = 0; i < nb_directions; i++) {
    directions[i]->enable_pixel_collisions(src_image);
  }
}

/**
 * Returns whether the pixel-perfect collisions are enabled for this animations.
 * @return true if the pixel-perfect collisions are enabled
 */
bool SpriteAnimation::are_pixel_collisions_enabled(void) {
  return directions[0]->are_pixel_collisions_enabled();
}
