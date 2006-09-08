#include <SDL/SDL.h>
#include "sprite_animation.h"
#include "sprite_animation_direction.h"

SpriteAnimation::SpriteAnimation(int nb_directions, SpriteAnimationDirection **directions, Uint32 frame_interval):
  nb_directions(nb_directions), directions(directions),
  frame_interval(frame_interval) {

}

  // simple constructor for animations with only one frame
SpriteAnimation::SpriteAnimation(int nb_directions, SpriteAnimationDirection **directions):
  nb_directions(nb_directions), directions(directions),
  frame_interval(0) {

}

SpriteAnimation::~SpriteAnimation(void) {
  for (int i = 0; i < nb_directions; i++) {
    delete directions[i];
  }
  delete[] directions;
}

void SpriteAnimation::display_on_map(Map *map, SDL_Rect &where_in_map,
				     int current_direction, int current_frame) {
  directions[current_direction]->display_on_map(map, where_in_map, current_frame);
}
