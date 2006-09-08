#include "sprite_animations.h"

SpriteAnimations::SpriteAnimations(int nb_animations):
  nb_animations(nb_animations) {
  animations = new SpriteAnimation*[nb_animations];
}

SpriteAnimations::~SpriteAnimations(void) {
  for (int i = 0; i < nb_animations; i++) {
    delete animations[i];
  }
  delete[] animations;
}

void SpriteAnimations::display_on_map(Map *map, SDL_Rect &where_in_map,
				      int current_animation, int current_direction, int current_frame) {
  animations[current_animation]->display_on_map(map, where_in_map, current_direction, current_frame);
}
