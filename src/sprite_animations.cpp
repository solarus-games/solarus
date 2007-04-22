/**
 * This module defines the class SpriteAnimations.
 */

#include "sprite_animations.h"

/**
 * Constructor.
 * @param nb_animations number of animations to create
 */
SpriteAnimations::SpriteAnimations(int nb_animations):
  nb_animations(nb_animations) {
  animations = new SpriteAnimation*[nb_animations];
}

/**
 * Destructor.
 */
SpriteAnimations::~SpriteAnimations(void) {

  // delete the animations
  for (int i = 0; i < nb_animations; i++) {
    delete animations[i];
  }

  delete[] animations;
}

/**
 * Displays a specific frame of the animation on the map.
 * This function just calls display_on_map() on the current animation.
 * @param map the map
 * @param position_in_map position of the sprite on the map
 * @param current_animation animation number to show
 * @param current_direction the sprite's direction
 * @param current_frame the frame to show
 */
void SpriteAnimations::display_on_map(Map *map, SDL_Rect &position_in_map,
				      int current_animation, int current_direction, int current_frame) {
  animations[current_animation]->display_on_map(map, position_in_map, current_direction, current_frame);
}
