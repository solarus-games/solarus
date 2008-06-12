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
   * The name of the default animation.
   */
  string default_animation_name;
  
 public:

  SpriteAnimations(SpriteAnimationsId id);
  virtual ~SpriteAnimations(void);

  SpriteAnimation *get_animation(string animation_name);
  string get_default_animation(void);

};


#endif
