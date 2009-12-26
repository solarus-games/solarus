/*
 * Copyright (C) 2009 Christopho, Zelda Solarus - http://www.zelda-solarus.com
 * 
 * Zelda: Mystery of Solarus DX is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Zelda: Mystery of Solarus DX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "Sprite.h"
#include "SpriteAnimationSet.h"
#include "SpriteAnimation.h"
#include "SpriteAnimationDirection.h"
#include "ResourceManager.h"
#include "ZSDX.h"
#include "Game.h"
#include "Map.h"
#include "lowlevel/PixelBits.h"
#include "lowlevel/Color.h"
#include "lowlevel/System.h"
#include "lowlevel/Surface.h"

Surface *Sprite::alpha_surface = NULL;

/**
 * Initializes the sprites system.
 */
void Sprite::initialize(void) {

  // create only once an intermediary surface that will be used by transparent sprites
  alpha_surface = new Surface(320, 240);
  alpha_surface->set_transparency_color(Color::get_black());
}

/**
 * Uninitializes the sprites system.
 */
void Sprite::quit(void) {
  delete alpha_surface;
}

/**
 * Creates a sprite with the specified animation set.
 * @param id name of an animation set
 */
Sprite::Sprite(const SpriteAnimationSetId &id):
  animation_set_id(id), current_direction(0), current_frame(-1),
  suspended(false), ignore_suspend(false), paused(false), finished(false),
  blink_delay(0), alpha(255), alpha_next_change_date(0) {
  
  animation_set = ResourceManager::get_sprite_animation_set(id);
  set_current_animation(animation_set->get_default_animation());
}

/**
 * Destructor.
 */
Sprite::~Sprite(void) {

}

/**
 * Returns the id of the animation set of this sprite.
 * @return the animation set id of this sprite
 */
const SpriteAnimationSetId& Sprite::get_animation_set_id(void) {
  return animation_set_id;
}

/**
 * Returns wether the id of the animation set of this sprite
 * contains the specified string.
 * @param s the string to check
 * @return true if the animation set id contains this string
 */
bool Sprite::contains(const std::string s) {
  return animation_set_id.find(s) != std::string::npos;
}

/**
 * Returns the animation set of this sprite.
 * If several sprites have the same animation set, they share the same instance of animation set
 * and the same pointer is returned here.
 * @return the animation set of this sprite
 */
SpriteAnimationSet * Sprite::get_animation_set(void) {
  return animation_set;
}

/**
 * Returns the size of a frame for the current animation and the current direction.
 * @return the size of a frame
 */
const Rectangle & Sprite::get_size(void) {

  SpriteAnimation *animation = animation_set->get_animation(current_animation_name);
  return animation->get_direction(current_direction)->get_size();
}

/**
 * Returns the origin point of a frame for the current animation and the current direction.
 * @return the origin point of a frame
 */
const Rectangle & Sprite::get_origin(void) {

  SpriteAnimation *animation = animation_set->get_animation(current_animation_name);
  return animation->get_direction(current_direction)->get_origin();
}

/**
 * Returns the frame delay of the current animation.
 * A value of 0 (only for 1-frame animations) means that the
 * animation must continue to be displayed: in this case,
 * is_animation_finished() returns always false.
 * @return the delay between two frames for the current animation (in miliseconds)
 */
uint32_t Sprite::get_frame_delay(void) {
  return frame_delay;  
}

/**
 * Sets the frame delay of the current animation.
 * A value of 0 (only for 1-frame animations) means that the
 * animation will continue to be displayed.
 * @param frame_delay the delay between two frames for the current animation (in miliseconds)
 */
void Sprite::set_frame_delay(uint32_t frame_delay) {
  this->frame_delay = frame_delay;  
}

/**
 * Returns the next frame of the current frame.
 * @return the next frame of the current frame (or -1 if the animation is finished)
 */
int Sprite::get_next_frame(void) {
  return current_animation->get_next_frame(current_direction, current_frame);    
}

/**
 * Returns the current animation of the sprite.
 * @return the name of the current animation of the sprite
 */
const std::string & Sprite::get_current_animation(void) {
  return current_animation_name;
}

/**
 * Sets the current animation of the sprite.
 * If the sprite is already playing another animation, this animation is interrupted.
 * If the sprite is already playing the same animation, nothing is done.
 * @param animation_name name of the new animation of the sprite
 */
void Sprite::set_current_animation(const std::string &animation_name) {

  if (animation_name != this->current_animation_name || !is_animation_started()) {

    SpriteAnimation *animation = animation_set->get_animation(animation_name);

    if (animation == NULL) {
      DIE("Unknown animation '" << animation_name << "' for animation set '" << animation_set_id << "'");
    }

    this->current_animation_name = animation_name;
    this->current_animation = animation;
    set_frame_delay(animation->get_frame_delay());
    set_current_frame(0);
  }
}

/**
 * Returns the current direction of the sprite's animation.
 * @return the current direction
 */
int Sprite::get_current_direction(void) {
  return current_direction;
}

/**
 * Sets the current direction of the sprite's animation and restarts the animation.
 * If the specified direction is the current direction, nothing is done.
 * @param current_direction the current direction
 */
void Sprite::set_current_direction(int current_direction) {

  if (current_direction != this->current_direction) {

    if (current_direction < 0) {
      DIE("Invalid sprite direction: " << current_direction);
    }

    this->current_direction = current_direction;
    set_current_frame(0);
  }
}

/**
 * Returns the current frame of the sprite's animation.
 * @return the current frame
 */
int Sprite::get_current_frame(void) {
  return current_frame;
}

/**
 * Sets the current frame of the sprite's animation.
 * If the animation was finished, it is restarted.
 * If the animation is suspended, it remains suspended
 * but the specified frame is displayed. 
 * @param current_frame the current frame
 */
void Sprite::set_current_frame(int current_frame) {

  finished = false;
  next_frame_date = System::now() + get_frame_delay();

  frame_changed = (current_frame != this->current_frame);

  this->current_frame = current_frame;
}

/**
 * Returns whether the frame of this sprite has just changed.
 * @return true if the frame of this sprite has just changed.
 */
bool Sprite::has_frame_changed(void) {
  return frame_changed;
}

/**
 * Returns true if the animation is started.
 * It can be suspended.
 * @return true if the animation is started, false otherwise
 */
bool Sprite::is_animation_started(void) {
  return !is_animation_finished();
}

/**
 * Starts the animation.
 */
void Sprite::start_animation(void) {
  restart_animation();
}

/**
 * Restarts the animation.
 */
void Sprite::restart_animation(void) {
  set_current_frame(0);
  set_suspended(false);
  set_paused(false);
}

/**
 * Stops the animation.
 */
void Sprite::stop_animation(void) {
  finished = true;
}

/**
 * Returns true if the game is suspended.
 * @return true if the game is suspended, false otherwise
 */
bool Sprite::is_suspended(void) {
  return suspended;
}

/**
 * Suspends or resumes the animation.
 * Nothing is done if the parameter specified does not change.
 * @param suspended true to suspend the animation, false to resume it
 */
void Sprite::set_suspended(bool suspended) {

  if (suspended != this->suspended && !ignore_suspend) {
    this->suspended = suspended;

    // compte next_frame_date if the animation is being resumed
    if (!suspended) {
      uint32_t now = System::now();
      next_frame_date = now + get_frame_delay();
      blink_next_change_date = now;
    }
    else {
      blink_is_sprite_visible = true;
    }
  }
}

/**
 * Sets whether this sprite should keep playing its animation when the game is suspended.
 * This will ignore subsequent calls to set_suspended(). 
 * @param ignore_suspend true to make the sprite continue its animation even
 * when the game is suspended
 */
void Sprite::set_ignore_suspend(bool ignore_suspend) {
  set_suspended(false);
  this->ignore_suspend = ignore_suspend;
}

/**
 * Returns true if the animation is paused.
 * @return true if the animation is paused, false otherwise
 */
bool Sprite::is_paused(void) {
  return paused;
}

/**
 * Pauses or resumes the animation.
 * Nothing is done if the parameter specified does not change.
 * @param paused true to pause the animation, false to resume it
 */
void Sprite::set_paused(bool paused) {

  if (paused != this->paused) {
    this->paused = paused;
 
    // compte next_frame_date if the animation is being resumed
    if (!paused) {
      uint32_t now = System::now();
      next_frame_date = now + get_frame_delay();
      blink_next_change_date = now;
    }
    else {
      blink_is_sprite_visible = true;
    }
  }
}

/**
 * Returns true if the animation is looping.
 * @return true if the animation is looping
 */
bool Sprite::is_animation_looping(void) {
  return current_animation->is_looping();
}

/**
 * Returns true if the animation is finished.
 * The animation is finished after the last frame is reached
 * and if the frame delay is not zero (a frame delay
 * of zero should be used only for 1-frame animations).
 * @return true if the animation is finished
 */
bool Sprite::is_animation_finished(void) {
  return finished;
}

/**
 * Returns true if the last frame is reached.
 * @return true if the last frame is reached
 */
bool Sprite::is_last_frame_reached(void) {

  SpriteAnimationDirection *direction = current_animation->get_direction(current_direction);
  return get_current_frame() == direction->get_nb_frames() - 1;
}

/**
 * Returns whether the sprite is blinking.
 * @return true if the sprite is blinking
 */
bool Sprite::is_blinking(void) {
  return blink_delay != 0;
}

/**
 * Sets the blink delay of this sprite.
 * @param blink_delay blink delay of the sprite in milliseconds,
 * or zero to stop blinking.
 */
void Sprite::set_blinking(uint32_t blink_delay) {
  this->blink_delay = blink_delay;

  if (blink_delay > 0) {
    blink_is_sprite_visible = false;
    blink_next_change_date = System::now();
  }
}
 
/**
 * Returns the alpha value currently applied to the sprite.
 * @return the transparency rate: 0 (tranparent) to 255 (opaque)
 */
int Sprite::get_alpha(void) {
  return alpha;
}

/**
 * Sets the alpha value applied to the sprite.
 * @param alpha the opacity rate: 0 (tranparent) to 255 (opaque)
 */
void Sprite::set_alpha(int alpha) {
  this->alpha = alpha;
  alpha_surface->set_opacity(alpha);
}

/**
 * Returns whether the entity's sprites are currently displaying a fade-in or fade-out effect.
 * @return true if there is currently a fade effect
 */
bool Sprite::is_fading(void) {
  return alpha_next_change_date != 0;
}

/**
 * Starts a fade-in effect on this sprite.
 * @param direction direction of the effect (0: fade-in, 1: fade-out)
 */
void Sprite::start_fading(int direction) {
  alpha_next_change_date = System::now();
  alpha_increment = (direction == 0) ? 20 : -20;
  set_alpha((direction == 0) ? 0 : 255);
}

/**
 * Tests whether this sprite's pixels are overlapping another sprite.
 * @param other another sprite
 * @param x1 x coordinate of this sprite's origin point
 * @param y1 y coordinate of this sprite's origin point
 * @param x2 x coordinate of the other sprite's origin point
 * @param y2 y coordinate of the other sprite's origin point
 * @return true if the sprites are overlapping
 */
bool Sprite::test_collision(Sprite *other, int x1, int y1, int x2, int y2) {

  SpriteAnimationDirection *direction1 = current_animation->get_direction(current_direction);
  const Rectangle &origin1 = direction1->get_origin();
  Rectangle location1(x1 - origin1.get_x(), y1 - origin1.get_y());
  PixelBits *pixel_bits1 = direction1->get_pixel_bits(current_frame);

  SpriteAnimationDirection *direction2 = other->current_animation->get_direction(other->current_direction);
  const Rectangle &origin2 = direction2->get_origin();
  Rectangle location2(x2 - origin2.get_x(), y2 - origin2.get_y());
  PixelBits *pixel_bits2 = direction2->get_pixel_bits(other->current_frame);

  return pixel_bits1->test_collision(pixel_bits2, location1, location2);
}

/**
 * Checks whether the frame has to be changed.
 * If the frame changes, next_frame_date is updated.
 */
void Sprite::update(void) {

  if (suspended || paused) {
    return;
  }

  frame_changed = false;
  uint32_t now = System::now();

  // update the current frame
  int next_frame;
  while (!finished && !suspended && !paused && get_frame_delay() > 0
	 && now >= next_frame_date) {

    // we get the next frame
    next_frame = get_next_frame();

    // test whether the animation is finished
    if (next_frame == -1) {
      finished = true;
    }
    else {
      current_frame = next_frame;
      next_frame_date += get_frame_delay();
      frame_changed = true;
    }
  }

  // update the special effects
  if (is_blinking()) {
    // the sprite is blinking

    while (now >= blink_next_change_date) {
      blink_is_sprite_visible = !blink_is_sprite_visible;
      blink_next_change_date += blink_delay;
    }
  }

  if (is_fading() && now >= alpha_next_change_date) {
    // the sprite is fading

    int rate = get_alpha();
    rate += alpha_increment;
    rate = std::max(0, std::min(255, rate));
    if (rate == 0 || rate == 255) { // fade finished
      alpha_next_change_date = 0;
    }
    else {
      alpha_next_change_date += 40;
    }
    set_alpha(rate);
  }
}

/**
 * Displays the sprite on a surface, with its current animation, direction and frame.
 * @param destination the surface on which the sprite will be displayed
 * @param x x coordinate of the sprite on this surface
 * (the origin will be placed at this position)
 * @param y y coordinate of the sprite on this surface
 * (the origin will be placed at this position)
 */
void Sprite::display(Surface *destination, int x, int y) {

  if (!is_animation_finished() && (blink_delay == 0 || blink_is_sprite_visible)) {

    if (get_alpha() >= 255) {
      // opaque
      current_animation->display(destination, x, y, current_direction, current_frame);
    }
    else {
      // semi transparent
      alpha_surface->fill_with_color(Color::get_black());
      current_animation->display(alpha_surface, x, y, current_direction, current_frame);
      alpha_surface->blit(destination);
    }
  }
}

