#include <SDL/SDL.h>
#include "sprite_animation.h"
#include "sprite_animation_direction.h"

SpriteAnimation::SpriteAnimation(int nb_directions, SpriteAnimationDirection **directions,
				 int x_hotspot, int y_hotspot, Uint32 frame_interval):
  nb_directions(nb_directions), directions(directions), x_hotspot(x_hotspot), y_hotspot(y_hotspot),
  frame_interval(frame_interval) {

}

  // simple constructor for animations with only one frame
SpriteAnimation::SpriteAnimation(int nb_directions, SpriteAnimationDirection **directions
				 , int x_hotspot, int y_hotspot):
  nb_directions(nb_directions), directions(directions), x_hotspot(x_hotspot), y_hotspot(y_hotspot),
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
  static SDL_Rect where_in_map_up_left;
  where_in_map_up_left.x = where_in_map.x - x_hotspot;
  where_in_map_up_left.y = where_in_map.y - y_hotspot;
  directions[current_direction]->display_on_map(map, where_in_map_up_left, current_frame);

  //  directions[current_direction]->display_on_map(map, where_in_map, current_frame);
}
