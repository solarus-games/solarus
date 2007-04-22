/**
 * This module defines the abstract class SpriteAnimations.
 */

#ifndef ZSDX_SPRITE_ANIMATIONS_H
#define ZSDX_SPRITE_ANIMATIONS_H

#include <SDL/SDL.h>
#include "sprite_animation.h"
#include "map.h"

/**
 * Abstract class for a set of animations.
 * Each animation correspond to a specific situation
 * of a sprite.
 *
 * For example, you can have an animation "Stopped"
 * and an animation "Walking".
 */
class SpriteAnimations {

 protected:

  /**
   * Number of animations.
   */
  const int nb_animations;

  /**
   * Array of SpriteAnimation* containing the animations.
   * This array is allocated in the constructor and
   * freed in the destructor. It is filled in the
   * abstract function create_animations.
   */
  SpriteAnimation **animations;

  /**
   * Constructor.
   * @param nb_animations number of animations to create
   */
  SpriteAnimations(int nb_animations);

  /**
   * Destructor.
   */
  virtual ~SpriteAnimations(void);

  /**
   * Creates the animations.
   * This function should be called by the constructor
   * of your subclass.
   */
  virtual void create_animations(void) = 0;

 public:

  /**
   * Returns an animation.
   * @param animation_number animation number (the first one is number 0)
   * @return the specified animation
   */
  inline SpriteAnimation *get_animation(int animation_number) { return animations[animation_number]; }

  /**
   * Displays a specific frame of the animation on the map.
   * This function just calls display_on_map() on the current animation.
   * @param map the map
   * @param position_in_map position of the sprite on the map
   * @param current_animation animation number to show
   * @param current_direction the sprite's direction
   * @param current_frame the frame to show
   */
  void display_on_map(Map *map, SDL_Rect &position_in_map,
		      int current_animation, int current_direction, int current_frame);
};


#endif
