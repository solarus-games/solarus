/**
 * This module defines the class AnimatedSprite.
 */

#ifndef ZSDX_ANIMATED_SPRITE_H
#define ZSDX_ANIMATED_SPRITE_H

#include "Common.h"

/**
 * Represents an animated sprite.
 * You must inherit this class to make an animated sprite.
 * An animated sprite is represented as a set of animations, a current animation,
 * a current direction and a current frame.
 */
class AnimatedSprite {

 private:

  /**
   * Animation set of this sprite.
   */
  SpriteAnimations *animations;

  /**
   * Current animation (the first one is number 0).
   */
  int current_animation;

  /**
   * Current direction of the animation (the first one is number 0).
   * It can be different from the movement direction
   * of the entity, because sometimes a sprite can
   * go backwards.
   */
  int current_direction;

  /**
   * Current frame of the animation (the first one is number 0).
   */
  int current_frame;

  /**
   * Date of the next frame. This date is in milliseconds
   * since the program initialization, like SDL_GetTicks().
   */
  Uint32 next_frame_date;

  /**
   * True if the animation is suspended.
   */
  bool suspended;

  /**
   * True if the animation is over.
   */
  bool over;

  Uint32 get_frame_interval(void);
  int get_next_frame(void);

 public:

  AnimatedSprite(SpriteAnimations *animations);
  inline ~AnimatedSprite(void) { }

  int get_current_animation(void);
  void set_current_animation(int current_animation);
  int get_current_animation_direction(void);
  void set_current_animation_direction(int current_direction);
  int get_current_frame(void);
  void set_current_frame(int current_frame);

  void update_current_frame(void);

  bool is_suspended(void);
  void set_suspended(bool suspended);
  bool is_over(void);

  void display_on_map(Map *map, SDL_Rect &position_in_map);
};

#endif
