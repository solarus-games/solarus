/*
 * Copyright (C) 2009 Christopho, Solarus - http://www.solarus-engine.org
 * 
 * Solarus is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Solarus is distributed in the hope that it will be useful,
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
#include "Game.h"
#include "Map.h"
#include "lowlevel/PixelBits.h"
#include "lowlevel/Color.h"
#include "lowlevel/System.h"
#include "lowlevel/Surface.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"

int Sprite::next_unique_id = 0;
std::map<SpriteAnimationSetId, SpriteAnimationSet*> Sprite::all_animation_sets;
Surface *Sprite::alpha_surface = NULL;

/**
 * @brief Initializes the sprites system.
 */
void Sprite::initialize() {

  // create only once an intermediary surface that will be used by transparent sprites
  alpha_surface = new Surface(320, 240);
  alpha_surface->set_transparency_color(Color::get_black());
}

/**
 * @brief Uninitializes the sprites system.
 */
void Sprite::quit() {

  // delete the animations loaded
  std::map<SpriteAnimationSetId, SpriteAnimationSet*>::iterator it;
  for (it = all_animation_sets.begin(); it != all_animation_sets.end(); it++) {
    delete it->second;
  }
  all_animation_sets.clear();

  // delete the static alpha_surface
  delete alpha_surface;
}

/**
 * @brief Returns the sprite animation set corresponding to the specified id.
 *
 * The animation set may be created if it is new, or just retrieved from
 * memory if it way already used before.
 *
 * @param id id of the animation set
 * @return the corresponding animation set
 */
SpriteAnimationSet& Sprite::get_animation_set(const SpriteAnimationSetId &id) {

  if (all_animation_sets.count(id) == 0) {
    all_animation_sets[id] = new SpriteAnimationSet(id);
  }

  return *all_animation_sets[id];
}

/**
 * @brief Creates a sprite with the specified animation set.
 * @param id name of an animation set
 */
Sprite::Sprite(const SpriteAnimationSetId &id):

  unique_id(next_unique_id++),
  animation_set_id(id),
  animation_set(get_animation_set(id)),
  current_direction(0),
  current_frame(-1),
  suspended(false),
  ignore_suspend(false),
  paused(false),
  finished(false),
  synchronize_to(NULL),
  blink_delay(0),
  alpha(255),
  alpha_next_change_date(0) {
 
  set_current_animation(animation_set.get_default_animation());
}

/**
 * @brief Destructor.
 */
Sprite::~Sprite() {

}

/**
 * @brief Returns the unique id of this sprite.
 *
 * It is guaranteed that no other sprite instance will have the same id as this one
 * during the execution of the program, even after this sprite is deleted.
 *
 * @return the unique id of this movement
 */
int Sprite::get_unique_id() const {
  return unique_id;
}

/**
 * @brief Returns the id of the animation set of this sprite.
 * @return the animation set id of this sprite
 */
const SpriteAnimationSetId& Sprite::get_animation_set_id() const {
  return animation_set_id;
}

/**
 * @brief Returns wether the id of the animation set of this sprite
 * contains the specified string.
 * @param s the string to check
 * @return true if the animation set id contains this string
 */
bool Sprite::contains(const std::string &s) const {
  return animation_set_id.find(s) != std::string::npos;
}

/**
 * @brief Returns the animation set of this sprite.
 *
 * If several sprites have the same animation set, they share the same instance of animation set
 * and the same pointer is returned here.
 *
 * @return the animation set of this sprite
 */
SpriteAnimationSet& Sprite::get_animation_set() {
  return animation_set;
}

/**
 * @brief When the sprite is displayed on a map, sets the map.
 *
 * This function must be called if this sprite image depends on the map's tileset.
 *
 * @param map the map
 */
void Sprite::set_map(Map &map) {
  animation_set.set_map(map);
}


/**
 * @brief Enables the pixel-perfect collision detection for the animation set of this sprite.
 *
 * All sprites that use the same animation set as this one will be affected.
 */
void Sprite::enable_pixel_collisions() {
  animation_set.enable_pixel_collisions();
}

/**
 * @brief Returns whether the pixel-perfect collisions are enabled for the animation set of this sprite.
 * @return true if the pixel-perfect collisions are enabled
 */
bool Sprite::are_pixel_collisions_enabled() const {
  return animation_set.are_pixel_collisions_enabled();
}

/**
 * @brief Returns the size of a frame for the current animation and the current direction.
 * @return the size of a frame
 */
const Rectangle& Sprite::get_size() const {

  const SpriteAnimation *animation = animation_set.get_animation(current_animation_name);
  return animation->get_direction(current_direction)->get_size();
}

/**
 * @brief Returns the origin point of a frame for the current animation and the current direction.
 * @return the origin point of a frame
 */
const Rectangle& Sprite::get_origin() const {

  const SpriteAnimation *animation = animation_set.get_animation(current_animation_name);
  return animation->get_direction(current_direction)->get_origin();
}

/**
 * @brief Returns the frame delay of the current animation.
 *
 * A value of 0 (only for 1-frame animations) means that the
 * animation must continue to be displayed: in this case,
 * is_animation_finished() returns always false.
 *
 * @return the delay between two frames for the current animation (in miliseconds)
 */
uint32_t Sprite::get_frame_delay() const {
  return frame_delay;  
}

/**
 * @brief Sets the frame delay of the current animation.
 *
 * A value of 0 (only for 1-frame animations) means that the
 * animation will continue to be displayed.
 *
 * @param frame_delay the delay between two frames for the current animation (in miliseconds)
 */
void Sprite::set_frame_delay(uint32_t frame_delay) {
  this->frame_delay = frame_delay;  
}

/**
 * @brief Returns the next frame of the current frame.
 * @return the next frame of the current frame (or -1 if the animation is finished)
 */
int Sprite::get_next_frame() const {
  return current_animation->get_next_frame(current_direction, current_frame);    
}

/**
 * @brief Returns the current animation of the sprite.
 * @return the name of the current animation of the sprite
 */
const std::string & Sprite::get_current_animation() const {
  return current_animation_name;
}

/**
 * @brief Sets the current animation of the sprite.
 *
 * If the sprite is already playing another animation, this animation is interrupted.
 * If the sprite is already playing the same animation, nothing is done.
 *
 * @param animation_name name of the new animation of the sprite
 */
void Sprite::set_current_animation(const std::string &animation_name) {

  if (animation_name != this->current_animation_name || !is_animation_started()) {

    SpriteAnimation *animation = animation_set.get_animation(animation_name);

    this->current_animation_name = animation_name;
    this->current_animation = animation;
    set_frame_delay(animation->get_frame_delay());
    set_current_frame(0);
  }
}

/**
 * @brief Returns whether this sprite has an animation with the specified name.
 * @param animation_name an animation name
 * @return true if this animation exists
 */
bool Sprite::has_animation(const std::string& animation_name) {
  return animation_set.has_animation(animation_name);
}

/**
 * @brief Returns the current direction of the sprite's animation.
 * @return the current direction
 */
int Sprite::get_current_direction() const {
  return current_direction;
}

/**
 * @brief Sets the current direction of the sprite's animation and restarts the animation.
 *
 * If the specified direction is the current direction, nothing is done.
 *
 * @param current_direction the current direction
 */
void Sprite::set_current_direction(int current_direction) {

  if (current_direction != this->current_direction) {

    Debug::check_assertion(current_direction >= 0
        && current_direction < current_animation->get_nb_directions(),
        StringConcat() << "Invalid direction of sprite '" << get_animation_set_id()
        << "': " << current_direction);

    this->current_direction = current_direction;
    set_current_frame(0);
  }
}

/**
 * @brief Returns the current frame of the sprite's animation.
 * @return the current frame
 */
int Sprite::get_current_frame() const {
  return current_frame;
}

/**
 * @brief Sets the current frame of the sprite's animation.
 *
 * If the animation was finished, it is restarted.
 * If the animation is suspended, it remains suspended
 * but the specified frame is displayed.
 *
 * @param current_frame the current frame
 */
void Sprite::set_current_frame(int current_frame) {

  finished = false;
  next_frame_date = System::now() + get_frame_delay();

  frame_changed = (current_frame != this->current_frame);

  this->current_frame = current_frame;
}

/**
 * @brief Returns whether the frame of this sprite has just changed.
 * @return true if the frame of this sprite has just changed.
 */
bool Sprite::has_frame_changed() const {
  return frame_changed;
}

/**
 * @brief Makes this sprite always synchronized with another one as soon as
 * they have the same animation name.
 * @param other the sprite to synchronize to, or NULL to stop any previous synchronization
 */
void Sprite::set_synchronized_to(Sprite* other) {
  this->synchronize_to = other;
}

/**
 * @brief Returns true if the animation is started.
 *
 * It can be suspended.
 *
 * @return true if the animation is started, false otherwise
 */
bool Sprite::is_animation_started() const {
  return !is_animation_finished();
}

/**
 * @brief Starts the animation.
 */
void Sprite::start_animation() {
  restart_animation();
}

/**
 * @brief Restarts the animation.
 */
void Sprite::restart_animation() {
  set_current_frame(0);
  set_paused(false);
}

/**
 * @brief Stops the animation.
 */
void Sprite::stop_animation() {
  finished = true;
}

/**
 * @brief Returns true if the game is suspended.
 * @return true if the game is suspended, false otherwise
 */
bool Sprite::is_suspended() const {
  return suspended;
}

/**
 * @brief Suspends or resumes the animation.
 *
 * Nothing is done if the parameter specified does not change.
 *
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
 * @brief Sets whether this sprite should keep playing its animation when the game is suspended.
 *
 * This will ignore subsequent calls to set_suspended().
 *
 * @param ignore_suspend true to make the sprite continue its animation even
 * when the game is suspended
 */
void Sprite::set_ignore_suspend(bool ignore_suspend) {
  set_suspended(false);
  this->ignore_suspend = ignore_suspend;
}

/**
 * @brief Returns true if the animation is paused.
 * @return true if the animation is paused, false otherwise
 */
bool Sprite::is_paused() const {
  return paused;
}

/**
 * @brief Pauses or resumes the animation.
 *
 * Nothing is done if the parameter specified does not change.
 *
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
 * @brief Returns true if the animation is looping.
 * @return true if the animation is looping
 */
bool Sprite::is_animation_looping() const {
  return current_animation->is_looping();
}

/**
 * @brief Returns true if the animation is finished.
 *
 * The animation is finished after the last frame is reached
 * and if the frame delay is not zero (a frame delay
 * of zero should be used only for 1-frame animations).
 *
 * @return true if the animation is finished
 */
bool Sprite::is_animation_finished() const {
  return finished;
}

/**
 * @brief Returns true if the last frame is reached.
 * @return true if the last frame is reached
 */
bool Sprite::is_last_frame_reached() const {

  const SpriteAnimationDirection *direction = current_animation->get_direction(current_direction);
  return get_current_frame() == direction->get_nb_frames() - 1;
}

/**
 * @brief Returns whether the sprite is blinking.
 * @return true if the sprite is blinking
 */
bool Sprite::is_blinking() const {
  return blink_delay != 0;
}

/**
 * @brief Sets the blink delay of this sprite.
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
 * @brief Returns the alpha value currently applied to the sprite.
 * @return the transparency rate: 0 (tranparent) to 255 (opaque)
 */
int Sprite::get_alpha() const {
  return alpha;
}

/**
 * @brief Sets the alpha value applied to the sprite.
 * @param alpha the opacity rate: 0 (tranparent) to 255 (opaque)
 */
void Sprite::set_alpha(int alpha) {
  this->alpha = alpha;
  alpha_surface->set_opacity(alpha);
}

/**
 * @brief Returns whether the entity's sprites are currently displaying a fade-in or fade-out effect.
 * @return true if there is currently a fade effect
 */
bool Sprite::is_fading() const {
  return alpha_next_change_date != 0;
}

/**
 * @brief Starts a fade-in or fade_out effect on this sprite.
 * @param direction direction of the effect (0: fade-in, 1: fade-out)
 */
void Sprite::start_fading(int direction) {
  alpha_next_change_date = System::now();
  alpha_increment = (direction == 0) ? 20 : -20;
  set_alpha((direction == 0) ? 0 : 255);
}

/**
 * @brief Tests whether this sprite's pixels are overlapping another sprite.
 * @param other another sprite
 * @param x1 x coordinate of this sprite's origin point
 * @param y1 y coordinate of this sprite's origin point
 * @param x2 x coordinate of the other sprite's origin point
 * @param y2 y coordinate of the other sprite's origin point
 * @return true if the sprites are overlapping
 */
bool Sprite::test_collision(Sprite& other, int x1, int y1, int x2, int y2) const {

  const SpriteAnimationDirection* direction1 = current_animation->get_direction(current_direction);
  const Rectangle &origin1 = direction1->get_origin();
  const Rectangle location1(x1 - origin1.get_x(), y1 - origin1.get_y());
  const PixelBits& pixel_bits1 = direction1->get_pixel_bits(current_frame);

  const SpriteAnimationDirection* direction2 = other.current_animation->get_direction(other.current_direction);
  const Rectangle& origin2 = direction2->get_origin();
  const Rectangle location2(x2 - origin2.get_x(), y2 - origin2.get_y());
  const PixelBits& pixel_bits2 = direction2->get_pixel_bits(other.current_frame);

  return pixel_bits1.test_collision(pixel_bits2, location1, location2);
}

/**
 * @brief Checks whether the frame has to be changed.
 *
 * If the frame changes, next_frame_date is updated.
 */
void Sprite::update() {

  if (suspended || paused) {
    return;
  }

  frame_changed = false;
  uint32_t now = System::now();

  // update the current frame
  if (synchronize_to == NULL
      || current_animation_name != synchronize_to->get_current_animation()) {
    // update the frames normally (with the time)
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
      }
      frame_changed = true;
    }
  }
  else {
    // take the same frame as the other sprite
    int other_frame = synchronize_to->get_current_frame();
    if (other_frame != current_frame) {
      current_frame = other_frame;
      next_frame_date = now + get_frame_delay();
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
 * @brief Displays the sprite on a surface, with its current animation, direction and frame.
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

