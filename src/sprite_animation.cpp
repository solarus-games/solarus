#include <SDL/SDL.h>
#include "sprite_animation.h"
#include "sprite_animation_direction.h"

SpriteAnimation::SpriteAnimation(int nb_directions, SpriteAnimationDirection **directions, Uint32 frame_interval):
  nb_directions(nb_directions), current_direction(0), directions(directions),
  frame_interval(frame_interval), next_frame_date(SDL_GetTicks() + frame_interval) {

}

  // simple constructor for animations with only one frame
SpriteAnimation::SpriteAnimation(int nb_directions, SpriteAnimationDirection **directions):
  nb_directions(nb_directions), current_direction(0), directions(directions),
  frame_interval(0) {

}

SpriteAnimation::~SpriteAnimation(void) {
  for (int i = 0; i < nb_directions; i++) {
    delete directions[i];
  }
  delete[] directions;
}

void SpriteAnimation::update_current_frame(void) {
  Uint32 now = SDL_GetTicks();
  while (now >= next_frame_date) {
    directions[current_direction]->next_frame();
    next_frame_date = now + frame_interval;
  }
}

void SpriteAnimation::display_on_map(SDL_Surface *map, SDL_Rect &where_in_map) {
  if (frame_interval != 0) {
    update_current_frame();
  }
  directions[current_direction]->display_on_map(map, where_in_map);
}
