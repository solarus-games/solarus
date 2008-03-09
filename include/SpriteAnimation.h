/**
 * This module defines the class SpriteAnimation.
 */

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
  /**
   * Image from which the frames are extracted
   * This image is the same for
   * all directions of the sprite's animation.
   */
  SDL_Surface *src_image;

  /**
   * Number of directions of the animation.
   */
  const int nb_directions;

  /**
   * Array of directions.
   * Each direction is a sequence of images.
   */
  SpriteAnimationDirection **directions;

  /**
   * X coordinate of the sprite's origin from the
   * upper-left corner of its image.
   */
  const int x_hotspot;

  /**
   * Y coordinate of the sprite's origin from the
   * upper-left corner of its image.
   */
  const int y_hotspot;

  /**
   * Interval in milliseconds between two frames.
   * This interval is the same for all directions.
   * If there is only one frame in every direction, this field is set to zero.
   */
  const Uint32 frame_interval;
  
  /**
   * Number of the frame to loop on, or -1 to make no loop.
   */
  const int loop_on_frame;

 public:

  SpriteAnimation(string image_file_name, int nb_directions, SpriteAnimationDirection **directions,
		  int x_hotspot, int y_hotspot, Uint32 frame_interval, int loop_on_frame);
  virtual ~SpriteAnimation(void);

  /**
   * Returns a direction.
   * @param direction the direction
   * @return the sequence of images corresponding to this direction
   */
  inline SpriteAnimationDirection *get_direction(int direction) { return directions[direction]; }

  /**
   * Returns the interval between two frames for this sprite animation.
   * @return the frame interval in millisecond
   */
  inline Uint32 get_frame_interval(void) { return frame_interval; }

  int get_next_frame(int current_direction, int current_frame);
  void display(SDL_Surface *destination, const SDL_Rect &position, int current_direction, int current_frame);
};

#endif
