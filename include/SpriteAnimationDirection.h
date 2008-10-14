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

  const int nb_frames;    /**< number of frames in this sequence */
  SDL_Rect *frames;       /**< position of each frame of the sequence on the image */
  SDL_Rect origin;        /**< coordinate of the sprite's origin from the
			   * upper-left corner of its image. */

  PixelBits **pixel_bits; /**< bit masks representing the non-transparent pixels of each frame,
			   * computed only if enable_pixel_collisions() is called */

 public:

  // creation and destruction
  SpriteAnimationDirection(int nb_frames, SDL_Rect *frames,
			   int x_origin, int y_origin);
  virtual ~SpriteAnimationDirection(void);

  // size and origin point
  SDL_Rect& get_size(void);
  SDL_Rect& get_origin(void);

  // frames
  int get_nb_frames(void);
  void display(SDL_Surface *destination, int x, int y,
	       int current_frame, SDL_Surface *src_image);

  void enable_pixel_collisions(SDL_Surface *src_image);
  bool are_pixel_collisions_enabled(void);
};

#endif
