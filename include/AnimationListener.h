#ifndef ZSDX_ANIMATION_LISTENER_H
#define ZSDX_ANIMATION_LISTENER_H

#include "Common.h"

/**
 * Abstract class to detect when a sprite animation is over.
 * To associate an animation listener to a sprite, use
 * the add_animation_listener function of class Sprite.
 */
class AnimationListener {

 public:
  
  inline AnimationListener(void) { };
  inline virtual ~AnimationListener(void) { };

  /**
   * This function is called when the sprite's animation finishes.
   */
  virtual void animation_finished(Sprite *sprite) = 0;

};

#endif
