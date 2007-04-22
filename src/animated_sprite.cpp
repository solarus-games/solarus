/**
 * This module defines the class AnimatedSprite.
 */

#include "animated_sprite.h"

/**
 * Constructor.
 * @param animations the sprite's animations
 */
AnimatedSprite::AnimatedSprite(SpriteAnimations *animations):
animations(animations), current_animation(0), current_direction(0),
current_frame(0), next_frame_date(SDL_GetTicks() + get_frame_interval()),
suspended(false), over(false) {

}

/**
 * Suspends or resumes the animation.
 * @param suspended true to suspend the animation, false to resume it
 */
void AnimatedSprite::set_suspended(bool suspended) {
  this->suspended = suspended;

  // compte next_frame_date if necessary
  if (!suspended) {
    next_frame_date = SDL_GetTicks() + get_frame_interval();
  }
}

/**
 * Checks whether the frame has to be changed.
 * If the frame changes, next_frame_date is updated.
 */
void AnimatedSprite::update_current_frame(void) {
  int next_frame;
  Uint32 now = SDL_GetTicks();

  //  printf("update current frame: now = %d, next_frame_date = %d, frame_interval = %d\n", now, next_frame_date, get_frame_interval());

  while (!over && now >= next_frame_date) {

    // we get the next frame
    next_frame = get_next_frame();

    // test whether the animation is over
    over = (next_frame == -1);

    if (!over) {
      current_frame = next_frame;
      next_frame_date += get_frame_interval();
    }
  }

  //  printf("current frame: %d\n", current_frame);
}

/**
 * Displays the sprite on the map with its current animation, direction and frame.
 * @param map the map
 * @param position_in_map position of the sprite on the map
 */
void AnimatedSprite::display_on_map(Map *map, SDL_Rect &position_in_map) {
  // we update the frame if necessary
  if (!is_over() && !is_suspended()) {
    update_current_frame();
  }
  animations->display_on_map(map, position_in_map, current_animation, current_direction, current_frame);
}
