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

  std::map<string, SpriteAnimation*> animations; /**< the animations */
  string default_animation_name;                 /**< name of the default animation */
  
 public:

  SpriteAnimations(SpriteAnimationsId id);
  virtual ~SpriteAnimations(void);

  SpriteAnimation *get_animation(string animation_name);
  string get_default_animation(void);

  void enable_pixel_collisions(void);

};


#endif
