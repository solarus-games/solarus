#ifndef ZSDX_SPRITE_H
#define ZSDX_SPRITE_H

#include "Common.h"

/**
 * Represents an animated sprite.
 * An animated sprite is represented as a set of animations (an
 * instance of SpriteAnimations), a current animation,
 * a current direction and a current frame.
 * Several sprites can have the same animation set (i.e. they share
 * the same SpriteAnimations object).
 *
 * A sprite can be displayed directly on a surface, or it can
 * be attached to a map entity, allowing it to have a position on
 * the map and a movement.
 */
class Sprite {

 private:

  // animation set

  /**
   * Animation set of this sprite.
   */
  SpriteAnimations *animations;

  // current state of the sprite

  /**
   * Name of the current animation.
   */
  string current_animation_name;

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

  // blink

  /**
   * Blink delay of the sprite, or zero if the sprite is not blinking.
   */
  Uint16 blink_delay;

  /**
   * When blinking, true if the sprite is visible or false if it is invisible.
   */
  bool blink_is_sprite_visible;

  /**
   * Date of the next change when blinking: visible or not.
   */
  Uint16 blink_next_change_date;

  Uint32 get_frame_interval(void);
  int get_next_frame(void);

 public:

  // creation and destruction
  Sprite(SpriteAnimationsId id);
  Sprite(SpriteAnimations *animations);
  ~Sprite(void);

  // size and origin point
  SDL_Rect& get_size(void);
  SDL_Rect& get_origin(void);

  // current animation, direction and frame
  string get_current_animation(void);
  void set_current_animation(string animation_name);
  int get_current_direction(void);
  void set_current_direction(int current_direction);
  int get_current_frame(void);
  void set_current_frame(int current_frame);

  // animation state
  bool is_animation_started(void);
  void start_animation(void);
  void restart_animation(void);
  void stop_animation(void);

  bool is_suspended(void);
  void set_suspended(bool suspended);
  bool is_over(void);

  // blink
  bool is_blinking(void);
  void set_blinking(Uint16 blink_delay);

  // animation listener
  void set_animation_listener(AnimationListener *listener);

  // udpate and display
  void update_current_frame(void);
  void display(SDL_Surface *destination, int x, int y);
};

#endif
