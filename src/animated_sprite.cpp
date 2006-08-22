#include "animated_sprite.h"
#include <iostream>

AnimatedSprite::AnimatedSprite(int nb_animations):
  nb_animations(nb_animations), current_animation(0) {
  animations = new SpriteAnimation*[nb_animations];
}

AnimatedSprite::~AnimatedSprite(void) {
  for (int i = 0; i < nb_animations; i++) {
    delete animations[i];
  }
  delete[] animations;
}

void AnimatedSprite::display_on_map(SDL_Surface *map) {
  animations[current_animation]->display_on_map(map, where_in_map);
}
