#ifndef ZSDX_SPRITE_ANIMATION_DIRECTION
#define ZSDX_SPRITE_ANIMATION_DIRECTION

#include <SDL/SDL.h>
#include "map.h"

/* An animation direction is a sequence of
 * frames representing a sprite with a particular
 * animation in a given direction.
 */
class SpriteAnimationDirection {

 private:
  /* Image from which the frames are extracted
   * Most of the time, this image is the same for
   * all directions of the animation.
   */
  SDL_Surface *src_image;

  /* Number of frames of this sequence
   */
  const int nb_frames;

  /* The animation will loop on that frame if different from -1
   */
  const int loop_on_frame;

  /* Frames of the sequence
   */
  SDL_Rect *frames;

 public:
  /* Constructor
   */
  SpriteAnimationDirection(SDL_Surface *src_image, int nb_frames, int loop_on_frame, SDL_Rect *frames);

  /* Simple constructor without nb_frames
   * You should call it if there is only one frame in your sequence
   */
  SpriteAnimationDirection(SDL_Surface *src_image, SDL_Rect &frame);

  /* Destructor
   */
  virtual ~SpriteAnimationDirection(void);

  /* Return the next frame of the given frame
   * Return -1 if the animation is over
   */
  int get_next_frame(int current_frame);

  /* Display the current frame on the map
   */
  void display_on_map(Map *map, SDL_Rect &where_in_map, int current_frame);
};

#endif
