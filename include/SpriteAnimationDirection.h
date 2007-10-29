/**
 * This module defines the class SpriteAnimationDirection.
 */

#ifndef ZSDX_SPRITE_ANIMATION_DIRECTION
#define ZSDX_SPRITE_ANIMATION_DIRECTION

#include "Common.h"
#include "Map.h"

/**
 * An animation direction is a sequence of
 * frames representing a sprite with a particular
 * animation in a particular direction.
 */
class SpriteAnimationDirection {

 private:

  /**
   * Image from which the frames are extracted
   * Most of the time, this image is the same for
   * all directions of the sprite's animation.
   */
  SDL_Surface *src_image;

  /**
   * Number of frames in this sequence.
   */
  const int nb_frames;

  /**
   * Position of each frames of the sequence on the image.
   */
  SDL_Rect *frames;

  /**
   * Number of the frame to loop on, or -1 to make no loop.
   */
  const int loop_on_frame;

 public:

  /**
   * Constructor.
   * @param src_image image from which the image sequence is extracted
   * @param nb_frames number of frames in the sequence
   * @param frames position of each frames of the sequence in the image
   * @param loop_on_frame number of the frame to loop on (-1 for no loop)
   */
  SpriteAnimationDirection(SDL_Surface *src_image, int nb_frames, SDL_Rect *frames, int loop_on_frame);

  /**
   * Simple constructor without specifying the number of frames.
   * You should call it if there is only one frame in your sequence.
   * @param src_image image from which the frame is extracted
   * @param frame the only frame of the sequence
   */
  SpriteAnimationDirection(SDL_Surface *src_image, SDL_Rect &frame);

  /**
   * Destructor.
   */
  virtual ~SpriteAnimationDirection(void);

  /**
   * Returns the next frame of a specific frame.
   * Returns -1 if the animation is over (i.e. the frame is the
   * last one and there is no loop).
   * @param frame the frame to consider
   */
  int get_next_frame(int frame);

  /**
   * Displays a specific frame on the map.
   * @param map the map
   * @param position_in_map position of the sprite
   * @param current_frame the frame to show
   */
  void display_on_map(Map *map, SDL_Rect &position_in_map, int current_frame);
};

#endif
