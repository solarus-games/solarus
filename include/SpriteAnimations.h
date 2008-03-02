/**
 * This module defines the abstract class SpriteAnimations.
 */

#ifndef ZSDX_SPRITE_ANIMATIONS_H
#define ZSDX_SPRITE_ANIMATIONS_H

#include "Common.h"
#include <map>

/**
 * A set of animations.
 * Each animation correspond to a specific situation
 * of a sprite.
 *
 * For example, you can have an animation "Stopped"
 * and an animation "Walking".
 */
class SpriteAnimations {

 private:

  /**
   * The animations.
   * This structure is allocated in the constructor and
   * freed in the destructor.
   */
  std::map<string, SpriteAnimation*> animations;

  /**
   * The default animation.
   */
  SpriteAnimation *default_animation;
  
 public:

  SpriteAnimations(SpriteId id);
  virtual ~SpriteAnimations(void);

  /**
   * Returns an animation.
   * @param animation_name name of the animation to get
   * @return the specified animation
   */
  inline SpriteAnimation *get_animation(string animation_name) { return animations[animation_name]; }

  /**
   * Returns the default animation, i.e. the first one.
   * @return the default animation
   */
  SpriteAnimation *get_default_animation(void) { return default_animation; }

};


#endif
