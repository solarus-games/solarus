/**
 * This module defines the class AnimatedSprite.
 */

#include "AnimatedSprite.h"

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
 * Returns the frame interval of the current animation
 * @return the delay between two frames for the current animation
 */
Uint32 AnimatedSprite::get_frame_interval(void) {
  return animations->get_animation(current_animation)->get_frame_interval();  
}
  
/**
 * Returns the next frame of the current frame.
 * @return the next frame of the current frame (or -1 if the animation is over)
 */
int AnimatedSprite::get_next_frame(void) {

  return animations->get_animation(current_animation)->
    get_direction(current_direction)->get_next_frame(current_frame);    
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

  if (suspended) {
    return;
  }

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

  animations->display_on_map(map, position_in_map, current_animation, current_direction, current_frame);
}

/**
 * Returns the current animation of the sprite.
 * @return the current animation of the sprite
 */
int AnimatedSprite::get_current_animation(void) { return current_animation; }

/**
 * Sets the current animation of the sprite.
 * @param current_animation the new current animation of the sprite
 */
void AnimatedSprite::set_current_animation(int current_animation) {
  this->current_animation = current_animation;
  over = false;
  set_current_frame(0);
}

/**
 * Returns the current direction of the sprite's animation.
 * @return the current direction
 */
int AnimatedSprite::get_current_animation_direction(void) {
  return current_direction;
}

/**
 * Sets the current direction of the sprite's animation.
 * @param current_direction the current direction
 */
void AnimatedSprite::set_current_animation_direction(int current_direction) {

  this->current_direction = current_direction;
  set_current_frame(0);
}

/**
 * Returns the current frame of the sprite's animation.
 * @return the current frame
 */
int AnimatedSprite::get_current_frame(void) {
  return current_frame;
}

/**
 * Sets the current frame of the sprite's animation.
 * @param current_frame the current frame
 */
void AnimatedSprite::set_current_frame(int current_frame) {
  this->current_frame = current_frame;
}

/**
 * Returns true if the animation is suspended.
 * @return true if the animation is suspended, false otherwise
 */
bool AnimatedSprite::is_suspended(void) {
  return suspended;
}
