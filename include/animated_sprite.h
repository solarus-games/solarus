#ifndef ZSDX_ANIMATED_SPRITE_H
#define ZSDX_ANIMATED_SPRITE_H

#include "sprite_animations.h"

/* Represent an animated sprite
 * You must inherit this class to make an animated sprite.
 * An animated sprite is represented as a set of animations, a current animation,
 * a current direction and a current frame.
 */
class AnimatedSprite {

 private:
  /* Animation set of this sprite
   */
  SpriteAnimations *animations;

  /* Current animation (first is number 0)
   */
  int current_animation;

  /* Current direction of the animation (first is number 0)
   * It can be different from the movement direction
   * of the sprite, because sometimes a sprite can
   * go backwards.
   */
  int current_direction;

  /* Current frame of the animation (first is number 0)
   */
  int current_frame;

  /* Date of the next frame
   * This date is in milliseconds since the program initialization,
   * like SDL_GetTicks()
   */
  Uint32 next_frame_date;

  /* True if the animation is suspended
   */
  bool suspended;

  /* True if the animation is over
   */
  bool over;

  /* Return the frame interval of the current animation
   */
  inline Uint32 get_frame_interval(void) {
    return animations->get_animation(current_animation)->get_frame_interval();  
  }
  
  /* Return the next frame of the current frame
   * or -1 if the animation is over
   */
  inline int get_next_frame(void) {
    return animations->get_animation(current_animation)->
      get_direction(current_direction)->get_next_frame(current_frame);    
  }

 public:
  /* Constructor
   */
  AnimatedSprite(SpriteAnimations *animations);

  /* Destructor
   */
  inline ~AnimatedSprite(void) { }

  /* Return the current animation of the sprite
   */
  inline int get_current_animation(void) { return current_animation; }

  /* Set the current animation of the sprite
   */
  inline void set_current_animation(int current_animation) {
    this->current_animation = current_animation;
    over = false;
    set_current_frame(0);
  }

  /* Return the current direction of the animation
   */
  inline int get_current_animation_direction(void) { return current_direction; }

  /* Set the current direction of the animation
   */
  inline void set_current_animation_direction(int current_direction) {
    this->current_direction = current_direction;
    set_current_frame(0);
  }

  /* Return the current frame of the animation
   */
  inline int get_current_frame(void) { return current_frame; }
  
  /* Set the current frame of the animation
   */
  inline void set_current_frame(int current_frame) {
    this->current_frame = current_frame;
  }

  /* Return true if the animation is suspended
   */
  inline bool is_suspended(void) { return suspended; }

  /* Suspend or resume the animation
   */
  void set_suspended(bool suspended);

  /* Return true if the animation is over
   */
  inline bool is_over(void) { return over; }

  /* Checks whether the frame has to be changed
   * If the frame changes, next_frame_date is updated.
   */
  void update_current_frame(void);

  /* Display the sprite on the map
   */
  void display_on_map(Map *map, SDL_Rect &where_in_map);
};

#endif
