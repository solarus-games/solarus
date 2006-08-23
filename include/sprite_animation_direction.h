#ifndef ZSDX_SPRITE_ANIMATION_DIRECTION
#define ZSDX_SPRITE_ANIMATION_DIRECTION

#include <SDL/SDL.h>

/* TODO:
 * - choose whether the sequences loops or stop at the end
 * - if the sequence loop, allow to choose on which frame it loops
 * - allow the animation to be suspended (suspend / resume)
 */

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

  /* Current frame of the sequence
   */
  int current_frame;

  /* Frames of the sequence
   */
  SDL_Rect *frames;

 public:
  /* Constructor
   */
  SpriteAnimationDirection(SDL_Surface *src_image, int nb_frames, SDL_Rect *frames);

  /* Simple constructor without nb_frames
   * You should call it if there is only one frame in your sequence
   */
  SpriteAnimationDirection(SDL_Surface *src_image, SDL_Rect &frame);

  /* Destructor
   */
  virtual ~SpriteAnimationDirection(void);

  /* Change the current frame of the sequence
   * current_frame is incremented or reset to zero when it reaches
   * the end of the sequence. This function is called when necessary by
   * SpriteAnimation::update_current_frame().
   */
  void next_frame(void);

  /* Display the current frame on the map
   */
  void display_on_map(SDL_Surface *map, SDL_Rect &where_in_map);
};

#endif
