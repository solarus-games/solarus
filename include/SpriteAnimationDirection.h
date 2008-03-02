/**
 * This module defines the class SpriteAnimationDirection.
 */

#ifndef ZSDX_SPRITE_ANIMATION_DIRECTION
#define ZSDX_SPRITE_ANIMATION_DIRECTION

#include "Common.h"

/**
 * An animation direction is a sequence of
 * frames representing a sprite with a particular
 * animation in a particular direction.
 */
class SpriteAnimationDirection {

 private:

  /**
   * Number of frames in this sequence.
   */
  const int nb_frames;

  /**
   * Position of each frame of the sequence on the image.
   */
  SDL_Rect *frames;

 public:

  SpriteAnimationDirection(int nb_frames, SDL_Rect *frames);
  virtual ~SpriteAnimationDirection(void);

  inline int get_nb_frames() { return nb_frames; }
  void display_on_map(Map *map, SDL_Rect &position_in_map, int current_frame, SDL_Surface *src_image);
};

#endif
