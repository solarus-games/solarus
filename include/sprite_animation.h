#ifndef ZSDX_SPRITE_ANIMATION_H
#define ZSDX_SPRITE_ANIMATION_H

#include <SDL/SDL.h>
#include "sprite_animation_direction.h"

/* An animation is a list of animated sequences.
 * Each animated sequence correspond to a direction
 * of the sprite.
 */
class SpriteAnimation {

 private:
  /* Number of directions of the animation
   */
  const int nb_directions;

  /* Current direction of the animation
   * It can be different from the movement direction
   * of the sprite, because sometimes a sprite can
   * go backwards.
   */
  int current_direction;

  /* Array of directions
   * Each direction is a sequence of images.
   */
  SpriteAnimationDirection **directions;

  /* Interval in milliseconds between two frames
   * This interval is the same for all directions.
   * If there is only one frame in every directions, this field is set to zero.
   */
  const Uint32 frame_interval;

  /* Date of the next frame
   * This date is in milliseconds since the program initialization,
   * like SDL_GetTicks()
   */
  Uint32 next_frame_date;

  /* Checks whether the frame has to be changed
   * If the frame changes, next_frame_date is updated.
   */
  void update_current_frame(void);
  
 public:
  /* Constructor
   */
  SpriteAnimation(int nb_directions, SpriteAnimationDirection **directions, Uint32 frame_interval);

  /* Simple constructor without frame_interval
   * You can use this constructor if your directions contains only one frame.
   */
  SpriteAnimation(int nb_directions, SpriteAnimationDirection **directions);

  /* Destructor
   */
  virtual ~SpriteAnimation(void);

  /* Return the current direction of the animation
   */
  inline int get_current_direction(void) { return current_direction; }

  /* Set the current direction of the animation
   */
  inline void set_current_direction(int current_direction) {
    this->current_direction = current_direction;
  }

  /* Display the current frame of the current direction on the map
   */
  void display_on_map(SDL_Surface *map, SDL_Rect &where_in_map);
};

#endif
