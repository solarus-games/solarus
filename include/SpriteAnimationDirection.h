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

  /**
   * Coordinate of the sprite's origin from the
   * upper-left corner of its image.
   */
  SDL_Rect origin;

 public:

  // creation and destruction
  SpriteAnimationDirection(int nb_frames, SDL_Rect *frames,
			   int x_origin, int y_origin);
  virtual ~SpriteAnimationDirection(void);

  // size and origin point
  SDL_Rect& get_size(void);
  SDL_Rect& get_origin(void);

  // frames
  inline int get_nb_frames() { return nb_frames; }
  void display(SDL_Surface *destination, int x, int y,
	       int current_frame, SDL_Surface *src_image);
};

#endif
