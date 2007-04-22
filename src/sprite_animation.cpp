#include <SDL/SDL.h>
#include "sprite_animation.h"
#include "sprite_animation_direction.h"

/**
 * Constructor.
 * @param nb_directions number of directions in this animation
 * @param directions the image sequence of each direction
 * @param x_hotspot x coordinate of the sprite's origin
 * @param y_hotspot y coordinate of the sprite's origin
 * @param frame_interval interval in millisecond between two frames for this sprite animation
 */
SpriteAnimation::SpriteAnimation(int nb_directions, SpriteAnimationDirection **directions,
				 int x_hotspot, int y_hotspot, Uint32 frame_interval):
  nb_directions(nb_directions), directions(directions), x_hotspot(x_hotspot), y_hotspot(y_hotspot),
  frame_interval(frame_interval) {

}

/**
 * Simple constructor without specifying frame interval.
 * You can use this constructor if your directions contains only one frame.
 * @param nb_directions number of directions in this animation
 * @param directions the image sequence of each direction
 * @param x_hotspot x coordinate of the sprite's origin
 * @param y_hotspot y coordinate of the sprite's origin
 */
SpriteAnimation::SpriteAnimation(int nb_directions, SpriteAnimationDirection **directions
				 , int x_hotspot, int y_hotspot):
  nb_directions(nb_directions), directions(directions), x_hotspot(x_hotspot), y_hotspot(y_hotspot),
  frame_interval(0) {

}

/**
 * Destructor.
 */
SpriteAnimation::~SpriteAnimation(void) {
  for (int i = 0; i < nb_directions; i++) {
    delete directions[i];
  }

  delete[] directions;
}

/**
 * Displays a specific frame of this animation on the map.
 * @param map the map
 * @param position_in_map position of the sprite on the map
 * (the hotspot will be placed at this position)
 * @param current_direction the direction to show
 * @param current_frame the frame to show in this direction
 */
void SpriteAnimation::display_on_map(Map *map, SDL_Rect &position_in_map,
				     int current_direction, int current_frame) {
  static SDL_Rect position_in_map_up_left; // position of the sprite's upper left corner

  position_in_map_up_left.x = position_in_map.x - x_hotspot;
  position_in_map_up_left.y = position_in_map.y - y_hotspot;

  directions[current_direction]->display_on_map(map, position_in_map_up_left, current_frame);
}
