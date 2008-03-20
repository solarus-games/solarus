/**
 * This module defines the class AnimatedSprite.
 */

#include "AnimatedSprite.h"
#include "SpriteAnimations.h"
#include "SpriteAnimation.h"
#include "SpriteAnimationDirection.h"
#include "AnimationListener.h"

/**
 * Constructor.
 * @param animations the sprite's animations
 */
AnimatedSprite::AnimatedSprite(SpriteAnimations *animations):
animations(animations), current_direction(0),
suspended(false), over(false), listener(NULL) {
  
  set_current_animation(animations->get_default_animation());
}

/**
 * Returns the frame interval of the current animation
 * @return the delay between two frames for the current animation
 */
Uint32 AnimatedSprite::get_frame_interval(void) {
  return current_animation->get_frame_interval();  
}

/**
 * Returns the next frame of the current frame.
 * @return the next frame of the current frame (or -1 if the animation is over)
 */
int AnimatedSprite::get_next_frame(void) {
  /*
  if (animation_name == "sword") {
    cout << "next frame of frame " << current_frame << ": "
	 << current_animation->get_next_frame(current_direction, current_frame) << "\n";
  }
*/
  return current_animation->get_next_frame(current_direction, current_frame);    
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

  while (!over && now >= next_frame_date) {

    // we get the next frame
    next_frame = get_next_frame();

    // test whether the animation is over
    if (next_frame == -1) {

      over = true;

      // tell the listener the animation is over
      if (listener != NULL) {
	listener->animation_over(this);
      }
    }
    else {
      current_frame = next_frame;
      next_frame_date += get_frame_interval();
    }
  }
}

/**
 * Displays the sprite on a surface, with its current animation, direction and frame.
 * @param destination the surface on which the sprite will be displayed
 * @param x x coordinate of the sprite on this surface
 * (the hotspot will be placed at this position)
 * @param y y coordinate of the sprite on this surface
 * (the hotspot will be placed at this position)
 */
void AnimatedSprite::display(SDL_Surface *destination, int x, int y) {

  current_animation->display(destination, x, y, current_direction, current_frame);
}

/**
 * Returns the current animation of the sprite.
 * @return the name of the current animation of the sprite
 */
string AnimatedSprite::get_current_animation(void) { return animation_name; }

/**
 * Sets the current animation of the sprite.
 * @param animation_name name of the new animation of the sprite
 */
void AnimatedSprite::set_current_animation(string animation_name) {

  SpriteAnimation *animation = animations->get_animation(animation_name);
  
  if (animation == NULL) {
    cerr << "Unknown animation '" << animation_name << "'\n";
    exit(1);
  }
  
  this->animation_name = animation_name;
  this->current_animation = animation;
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
  over = false;
  next_frame_date = SDL_GetTicks() + get_frame_interval();
}

/**
 * Restarts the animation.
 */
void AnimatedSprite::restart_animation(void) {
  set_current_frame(0);
  set_suspended(false);
}

/**
 * Returns true if the animation is suspended.
 * @return true if the animation is suspended, false otherwise
 */
bool AnimatedSprite::is_suspended(void) {
  return suspended;
}

/**
 * Returns true if the animation is finished.
 * @return true if the animation is finished
 */
bool AnimatedSprite::is_over(void) {
  return over;
}

/**
 * Associates an animation listener to this sprite.
 * @param listener the listener to associate,
 * or NULL to remove the previous listener
 */
void AnimatedSprite::set_animation_listener(AnimationListener *listener) {
  this->listener = listener;
}
