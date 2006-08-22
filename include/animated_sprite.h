#ifndef ZSDX_ANIMATED_H
#define ZSDX_ANIMATED_H

#include "map_object.h"
#include "sprite_animation.h"

enum animation_direction_t {
  ANIMATION_DIRECTION_RIGHT,
  ANIMATION_DIRECTION_UP,
  ANIMATION_DIRECTION_LEFT,
  ANIMATION_DIRECTION_DOWN
};

class AnimatedSprite: virtual public MapObject {

 protected:
  const int nb_animations;
  int current_animation;
  SpriteAnimation **animations;

  virtual void create_animations(void) = 0;

  AnimatedSprite(int nb_animations);
  virtual ~AnimatedSprite(void);

  void display_on_map(SDL_Surface *map);
};

#endif
