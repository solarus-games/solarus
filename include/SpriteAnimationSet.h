#ifndef ZSDX_SPRITE_ANIMATION_SET_H
#define ZSDX_SPRITE_ANIMATION_SET_H

#include "Common.h"
#include <map>

/**
 * A set of animations.
 * Each animation correspond to a specific situation
 * of a sprite.
 * For example, you can have an animation "Stopped"
 * and an animation "Walking".
 */
class SpriteAnimationSet {

 private:

  std::map<string, SpriteAnimation*> animations; /**< the animations */
  string default_animation_name;                 /**< name of the default animation */
  
 public:

  SpriteAnimationSet(SpriteAnimationSetId id);
  virtual ~SpriteAnimationSet(void);

  SpriteAnimation *get_animation(string animation_name);
  string get_default_animation(void);

  void enable_pixel_collisions(void);
  bool are_pixel_collisions_enabled(void);

};


#endif
