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
   * Name of the current animation.
   */
  string animation_name;

  /**
   * The current animation.
   */
  SpriteAnimation *current_animation;

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
   * True if the animation has been stopped because the last frame was reached.
   */
  bool over;

  /**
   * The possible animation listener associated to this sprite.
   */
  AnimationListener *listener;

  Uint32 get_frame_interval(void);
  int get_next_frame(void);

 public:

  AnimatedSprite(SpriteAnimations *animations);
  inline ~AnimatedSprite(void) { }

  string get_current_animation(void);
  void set_current_animation(string animation_name);
  int get_current_animation_direction(void);
  void set_current_animation_direction(int current_direction);
  int get_current_frame(void);
  void set_current_frame(int current_frame);
  void restart_animation(void);

  void update_current_frame(void);

  bool is_started(void);
  void start(void);
  void stop(void);

  bool is_suspended(void);
  void set_suspended(bool suspended);
  bool is_over(void);

  void set_animation_listener(AnimationListener *listener);

  void display(SDL_Surface *destination, int x, int y);
};

#endif
