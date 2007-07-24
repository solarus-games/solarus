/**
 * This module defines the class AnimatedSprite.
 */

#ifndef ZSDX_ANIMATED_SPRITE_H
#define ZSDX_ANIMATED_SPRITE_H

#include "SpriteAnimations.h"

/**
 * Represents an animated sprite
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

  /**
   * Returns the frame interval of the current animation
   * @return the delay between two frames for the current animation
   */
  inline Uint32 get_frame_interval(void) {
    return animations->get_animation(current_animation)->get_frame_interval();  
  }
  
  /**
   * Returns the next frame of the current frame.
   * @return the next frame of the current frame (or -1 if the animation is over)
   */
  inline int get_next_frame(void) {
    return animations->get_animation(current_animation)->
      get_direction(current_direction)->get_next_frame(current_frame);    
  }

 public:

  /**
   * Constructor.
   * @param animations the sprite's animations
   */
  AnimatedSprite(SpriteAnimations *animations);

  /**
   * Destructor.
   */
  inline ~AnimatedSprite(void) { }

  /**
   * Returns the current animation of the sprite.
   * @return the current animation of the sprite
   */
  inline int get_current_animation(void) { return current_animation; }

  /**
   * Sets the current animation of the sprite.
   * @param current_animation the new current animation of the sprite
   */
  inline void set_current_animation(int current_animation) {
    this->current_animation = current_animation;
    over = false;
    set_current_frame(0);
  }

  /**
   * Returns the current direction of the sprite's animation.
   * @return the current direction
   */
  inline int get_current_animation_direction(void) { return current_direction; }

  /**
   * Sets the current direction of the sprite's animation.
   * @param current_direction the current direction
   */
  inline void set_current_animation_direction(int current_direction) {
    this->current_direction = current_direction;
    set_current_frame(0);
  }

  /**
   * Returns the current frame of the sprite's animation.
   * @return the current frame
   */
  inline int get_current_frame(void) { return current_frame; }
  
  /**
   * Sets the current frame of the sprite's animation.
   * @param current_frame the current frame
   */
  inline void set_current_frame(int current_frame) {
    this->current_frame = current_frame;
  }

  /**
   * Returns true if the animation is suspended.
   * @return true if the animation is suspended, false otherwise
   */
  inline bool is_suspended(void) { return suspended; }

  /**
   * Suspends or resumes the animation.
   * @param suspended true to suspend the animation, false to resume it
   */
  void set_suspended(bool suspended);

  /**
   * Return true if the animation is over.
   * @return true if the animation is over, false otherwise
   */
  inline bool is_over(void) { return over; }

  /**
   * Checks whether the frame has to be changed.
   * If the frame changes, next_frame_date is updated.
   */
  void update_current_frame(void);

  /**
   * Displays the sprite on the map with its current animation, direction and frame.
   * @param map the map
   * @param position_in_map position of the sprite on the map
   */
  void display_on_map(Map *map, SDL_Rect &position_in_map);
};

#endif
