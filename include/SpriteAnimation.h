#ifndef ZSDX_SPRITE_ANIMATION_H
#define ZSDX_SPRITE_ANIMATION_H

#include "Common.h"

/**
 * A sprite animation is a list of animated sequences.
 * Each animated sequence correspond to a direction
 * of the sprite.
 */
class SpriteAnimation {

 private:

  SDL_Surface *src_image;      /**< image from which the frames are extracted;
				* this image is the same for
				* all directions of the sprite's animation */

  const int nb_directions;     /**< number of directions of this animation */

  SpriteAnimationDirection **directions; /**< array of directions:
					  * each direction is a sequence of images */

  Uint32 frame_delay;          /**< interval in milliseconds between two frames
			        * (this delay is the same for all directions) */
  
  const int loop_on_frame;     /**< number of the frame to loop on, or -1 to make no loop */

 public:

  SpriteAnimation(string image_file_name, int nb_directions, SpriteAnimationDirection **directions,
		  Uint32 frame_interval, int loop_on_frame);
  virtual ~SpriteAnimation(void);

  SDL_Surface *get_src_image(void);

  int get_next_frame(int current_direction, int current_frame);
  void display(SDL_Surface *destination, int x, int y, int current_direction, int current_frame);

  SpriteAnimationDirection *get_direction(int direction);
  Uint32 get_frame_delay(void);
  void set_frame_delay(Uint32 frame_delay);

  void enable_pixel_collisions(void);
  bool are_pixel_collisions_enabled(void);
};

#endif
