#include "animated_sprite.h"

AnimatedSprite::AnimatedSprite(SpriteAnimations *animations):
animations(animations), current_animation(0), current_direction(0),
current_frame(0), next_frame_date(SDL_GetTicks() + get_frame_interval()),
suspended(false), over(false) {

}

void AnimatedSprite::set_suspended(bool suspended) {
  this->suspended = suspended;
  if (!suspended) {
    next_frame_date = SDL_GetTicks() + get_frame_interval();
  }
}

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

void AnimatedSprite::display_on_map(Map *map, SDL_Rect &where_in_map) {
  // we update the frame if necessary
  if (!is_over() && !is_suspended()) {
    update_current_frame();
  }
  animations->display_on_map(map, where_in_map, current_animation, current_direction, current_frame);
}
