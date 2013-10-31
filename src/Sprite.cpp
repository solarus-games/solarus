/*
 * Copyright (C) 2006-2013 Christopho, Solarus - http://www.solarus-games.org
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
#include "movements/Movement.h"
#include "lua/LuaContext.h"
#include "lowlevel/PixelBits.h"
#include "lowlevel/Color.h"
#include "lowlevel/System.h"
#include "lowlevel/Surface.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"

std::map<std::string, SpriteAnimationSet*> Sprite::all_animation_sets;

/**
 * \brief Initializes the sprites system.
 */
void Sprite::initialize() {
}

/**
 * \brief Uninitializes the sprites system.
 */
void Sprite::quit() {

  // delete the animations loaded
  std::map<std::string, SpriteAnimationSet*>::iterator it;
  for (it = all_animation_sets.begin(); it != all_animation_sets.end(); it++) {
    delete it->second;
  }
  all_animation_sets.clear();
}

/**
 * \brief Returns the sprite animation set corresponding to the specified id.
 *
 * The animation set may be created if it is new, or just retrieved from
 * memory if it way already used before.
 *
 * \param id id of the animation set
 * \return the corresponding animation set
 */
SpriteAnimationSet& Sprite::get_animation_set(const std::string& id) {

  SpriteAnimationSet* animation_set = NULL;
  std::map<std::string, SpriteAnimationSet*>::iterator it =
      all_animation_sets.find(id);
  if (it != all_animation_sets.end()) {
    animation_set = it->second;
  }
  else {
    animation_set = new SpriteAnimationSet(id);
    all_animation_sets[id] = animation_set;
  }

  Debug::check_assertion(animation_set != NULL, "No animation set");

  return *animation_set;
}

/**
 * \brief Creates a sprite with the specified animation set.
 * \param id name of an animation set
 */
Sprite::Sprite(const std::string& id):
  Drawable(),
  lua_context(NULL),
  animation_set_id(id),
  animation_set(get_animation_set(id)),
  current_animation(NULL),
  current_direction(0),
  current_frame(-1),
  suspended(false),
  ignore_suspend(false),
  paused(false),
  finished(false),
  synchronize_to(NULL),
  intermediate_surface(NULL),
  blink_delay(0) {

  set_current_animation(animation_set.get_default_animation());
}

/**
 * \brief Destructor.
 */
Sprite::~Sprite() {

  delete intermediate_surface;
}

/**
 * \brief Returns the id of the animation set of this sprite.
 * \return the animation set id of this sprite
 */
const std::string& Sprite::get_animation_set_id() const {
  return animation_set_id;
}

/**
 * \brief Returns whether the id of the animation set of this sprite
 * contains the specified string.
 * \param sequence the string to check
 * \return true if the animation set id contains this string
 */
bool Sprite::contains(const std::string& sequence) const {
  return animation_set_id.find(sequence) != std::string::npos;
}

/**
 * \brief Returns the animation set of this sprite.
 *
 * If several sprites have the same animation set, they share the same instance of animation set
 * and the same pointer is returned here.
 *
 * \return the animation set of this sprite
 */
SpriteAnimationSet& Sprite::get_animation_set() {
  return animation_set;
}

/**
 * \brief When the sprite is drawn on a map, sets the tileset.
 *
 * This function must be called if this sprite image depends on the map's tileset.
 *
 * \param tileset The tileset.
 */
void Sprite::set_tileset(Tileset& tileset) {
  animation_set.set_tileset(tileset);
}

/**
 * \brief Enables the pixel-perfect collision detection for the animation set of this sprite.
 *
 * All sprites that use the same animation set as this one will be affected.
 */
void Sprite::enable_pixel_collisions() {
  animation_set.enable_pixel_collisions();
}

/**
 * \brief Returns whether the pixel-perfect collisions are enabled for the animation set of this sprite.
 * \return true if the pixel-perfect collisions are enabled
 */
bool Sprite::are_pixel_collisions_enabled() const {
  return animation_set.are_pixel_collisions_enabled();
}

/**
 * \brief Returns the size of the current frame.
 * \return A rectangle whose size is the size of the current frame.
 * X and Y are set to zero.
 */
Rectangle Sprite::get_size() const {

  return current_animation->get_direction(current_direction)->get_size();
}

/**
 * \brief Returns the maximum frame size of the animation set of this sprite.
 * \return The maximum frame size.
 */
const Rectangle& Sprite::get_max_size() const {
  return animation_set.get_max_size();
}

/**
 * \brief Returns the origin point of a frame for the current animation and
 * the current direction.
 * \return the origin point of a frame
 */
const Rectangle& Sprite::get_origin() const {

  return current_animation->get_direction(current_direction)->get_origin();
}

/**
 * \brief Returns the frame delay of the current animation.
 *
 * A value of 0 (only for 1-frame animations) means that the
 * animation must continue to be drawn: in this case,
 * is_animation_finished() returns always false.
 *
 * \return The delay between two frames for the current animation
 * in miliseconds.
 */
uint32_t Sprite::get_frame_delay() const {
  return frame_delay;
}

/**
 * \brief Sets the frame delay of the current animation.
 *
 * A value of 0 (only for 1-frame animations) means that the
 * animation will continue to be drawn.
 *
 * \param frame_delay The delay between two frames for the current animation
 * in miliseconds.
 */
void Sprite::set_frame_delay(uint32_t frame_delay) {
  this->frame_delay = frame_delay;
}

/**
 * \brief Returns the next frame of the current frame.
 * \return The next frame of the current frame, or -1 if the animation is
 * finished.
 */
int Sprite::get_next_frame() const {
  return current_animation->get_next_frame(current_direction, current_frame);
}

/**
 * \brief Returns the current animation of the sprite.
 * \return the name of the current animation of the sprite
 */
const std::string& Sprite::get_current_animation() const {
  return current_animation_name;
}

/**
 * \brief Sets the current animation of the sprite.
 *
 * If the sprite is already playing another animation, this animation is interrupted.
 * If the sprite is already playing the same animation, nothing is done.
 *
 * \param animation_name name of the new animation of the sprite
 */
void Sprite::set_current_animation(const std::string& animation_name) {

  if (animation_name != this->current_animation_name || !is_animation_started()) {

    this->current_animation_name = animation_name;
    this->current_animation = animation_set.get_animation(animation_name);
    set_frame_delay(current_animation->get_frame_delay());

    set_current_frame(0, false);

    if (lua_context != NULL) {
      lua_context->sprite_on_animation_changed(*this, current_animation_name);
      lua_context->sprite_on_frame_changed(*this, current_animation_name, 0);
    }
  }
}

/**
 * \brief Returns whether this sprite has an animation with the specified name.
 * \param animation_name an animation name
 * \return true if this animation exists
 */
bool Sprite::has_animation(const std::string& animation_name) {
  return animation_set.has_animation(animation_name);
}

/**
 * \brief Returns the number of directions in the current animation of this
 * sprite.
 * \return The number of directions.
 */
int Sprite::get_nb_directions() const {
  return current_animation->get_nb_directions();
}

/**
 * \brief Returns the current direction of the sprite's animation.
 * \return the current direction
 */
int Sprite::get_current_direction() const {
  return current_direction;
}

/**
 * \brief Sets the current direction of the sprite's animation and restarts the animation.
 *
 * If the specified direction is the current direction, nothing is done.
 *
 * \param current_direction the current direction
 */
void Sprite::set_current_direction(int current_direction) {

  if (current_direction != this->current_direction) {

    Debug::check_assertion(current_direction >= 0
        && current_direction < get_nb_directions(),
        StringConcat() << "Invalid direction " << current_direction
        << " for sprite '" << get_animation_set_id()
        << "' in animation '" << current_animation_name << "'");

    this->current_direction = current_direction;

    set_current_frame(0, false);

    if (lua_context != NULL) {
      lua_context->sprite_on_direction_changed(*this, current_animation_name, current_direction);
      lua_context->sprite_on_frame_changed(*this, current_animation_name, 0);
    }
  }
}

/**
 * \brief Returns the number of frames in the current direction of the current
 * animation of this sprite.
 * \return The number of frames.
 */
int Sprite::get_nb_frames() const {
  return current_animation->get_direction(current_direction)->get_nb_frames();
}

/**
 * \brief Returns the current frame of the sprite's animation.
 * \return the current frame
 */
int Sprite::get_current_frame() const {
  return current_frame;
}

/**
 * \brief Sets the current frame of the sprite's animation.
 *
 * If the animation was finished, it is restarted.
 * If the animation is suspended, it remains suspended
 * but the specified frame is drawn.
 *
 * \param current_frame The current frame.
 * \param notify_script \c true to notify the Lua sprite if any.
 * Don't set notify_script to \c false unless you do the notification
 * yourself later.
 */
void Sprite::set_current_frame(int current_frame, bool notify_script) {

  finished = false;
  next_frame_date = System::now() + get_frame_delay();

  if (current_frame != this->current_frame) {
    this->current_frame = current_frame;
    set_frame_changed(true);

    if (lua_context != NULL) {
      lua_context->sprite_on_frame_changed(
          *this, current_animation_name, current_frame);
    }
  }
}

/**
 * \brief Returns the rectangle of the current frame.
 * \return The current frame's rectangle.
 */
const Rectangle& Sprite::get_current_frame_rectangle() const {

  return current_animation->get_direction(current_direction)->get_frame(current_frame);
}

/**
 * \brief Returns whether the frame of this sprite has just changed.
 * \return true if the frame of this sprite has just changed.
 */
bool Sprite::has_frame_changed() const {
  return frame_changed;
}

/**
 * \brief Sets whether the frame has just changed.
 * \param frame_changed true if the frame has just changed.
 */
void Sprite::set_frame_changed(bool frame_changed) {

  this->frame_changed = frame_changed;
}

/**
 * \brief Makes this sprite always synchronized with another one as soon as
 * they have the same animation name.
 * \param other the sprite to synchronize to, or NULL to stop any previous synchronization
 */
void Sprite::set_synchronized_to(Sprite* other) {
  this->synchronize_to = other;
}

/**
 * \brief Returns true if the animation is started.
 *
 * It can be suspended.
 *
 * \return true if the animation is started, false otherwise
 */
bool Sprite::is_animation_started() const {
  return !is_animation_finished();
}

/**
 * \brief Starts the animation.
 */
void Sprite::start_animation() {
  restart_animation();
}

/**
 * \brief Restarts the animation.
 */
void Sprite::restart_animation() {
  set_current_frame(0);
  set_paused(false);
}

/**
 * \brief Stops the animation.
 */
void Sprite::stop_animation() {
  finished = true;
}

/**
 * \brief Returns true if the game is suspended.
 * \return true if the game is suspended, false otherwise
 */
bool Sprite::is_suspended() const {
  return suspended;
}

/**
 * \brief Suspends or resumes the animation.
 *
 * Nothing is done if the parameter specified does not change.
 *
 * \param suspended true to suspend the animation, false to resume it
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

    // Also suspend or resumed the transition effect and the movement if any.
    Transition* transition = get_transition();
    if (transition != NULL) {
      transition->set_suspended(suspended);
    }

    Movement* movement = get_movement();
    if (movement != NULL) {
      movement->set_suspended(suspended);
    }
  }
}

/**
 * \brief Sets whether this sprite should keep playing its animation when the game is suspended.
 *
 * This will ignore subsequent calls to set_suspended().
 *
 * \param ignore_suspend true to make the sprite continue its animation even
 * when the game is suspended
 */
void Sprite::set_ignore_suspend(bool ignore_suspend) {
  set_suspended(false);
  this->ignore_suspend = ignore_suspend;
}

/**
 * \brief Returns true if the animation is paused.
 * \return true if the animation is paused, false otherwise
 */
bool Sprite::is_paused() const {
  return paused;
}

/**
 * \brief Pauses or resumes the animation.
 *
 * Nothing is done if the parameter specified does not change.
 *
 * \param paused true to pause the animation, false to resume it
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
 * \brief Returns true if the animation is looping.
 * \return true if the animation is looping
 */
bool Sprite::is_animation_looping() const {
  return current_animation->is_looping();
}

/**
 * \brief Returns true if the animation is finished.
 *
 * The animation is finished after the last frame is reached
 * and if the frame delay is not zero (a frame delay
 * of zero should be used only for 1-frame animations).
 *
 * \return true if the animation is finished
 */
bool Sprite::is_animation_finished() const {
  return finished;
}

/**
 * \brief Returns true if the last frame is reached.
 * \return true if the last frame is reached
 */
bool Sprite::is_last_frame_reached() const {

  return get_current_frame() == get_nb_frames() - 1;
}

/**
 * \brief Returns whether the sprite is blinking.
 * \return true if the sprite is blinking
 */
bool Sprite::is_blinking() const {
  return blink_delay != 0;
}

/**
 * \brief Sets the blink delay of this sprite.
 * \param blink_delay blink delay of the sprite in milliseconds,
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
 * \brief Tests whether this sprite's pixels are overlapping another sprite.
 * \param other Another sprite.
 * \param x1 X coordinate of this sprite's origin point.
 * \param y1 Y coordinate of this sprite's origin point.
 * \param x2 X coordinate of the other sprite's origin point.
 * \param y2 Y coordinate of the other sprite's origin point.
 * \return \c true if the sprites are overlapping.
 */
bool Sprite::test_collision(const Sprite& other, int x1, int y1, int x2, int y2) const {

  const SpriteAnimationDirection* direction1 = current_animation->get_direction(current_direction);
  const Rectangle& origin1 = direction1->get_origin();
  const Rectangle location1(x1 - origin1.get_x(), y1 - origin1.get_y());
  const PixelBits& pixel_bits1 = direction1->get_pixel_bits(current_frame);

  const SpriteAnimationDirection* direction2 = other.current_animation->get_direction(other.current_direction);
  const Rectangle& origin2 = direction2->get_origin();
  const Rectangle location2(x2 - origin2.get_x(), y2 - origin2.get_y());
  const PixelBits& pixel_bits2 = direction2->get_pixel_bits(other.current_frame);

  return pixel_bits1.test_collision(pixel_bits2, location1, location2);
}

/**
 * \brief Checks whether the frame has to be changed.
 *
 * If the frame changes, next_frame_date is updated.
 */
void Sprite::update() {

  Drawable::update();

  if (suspended || paused) {
    return;
  }

  frame_changed = false;
  uint32_t now = System::now();

  // update the current frame
  if (synchronize_to == NULL
      || current_animation_name != synchronize_to->get_current_animation()
      || synchronize_to->get_current_direction() > get_nb_directions()
      || synchronize_to->get_current_frame() > get_nb_frames()) {
    // update the frames normally (with the time)
    int next_frame;
    while (!finished && !suspended && !paused && get_frame_delay() > 0
        && now >= next_frame_date) {

      // we get the next frame
      next_frame = get_next_frame();

      // test whether the animation is finished
      if (next_frame == -1) {
        finished = true;
        if (lua_context != NULL) {
          lua_context->sprite_on_animation_finished(*this, current_animation_name);
        }
      }
      else {
        current_frame = next_frame;
        next_frame_date += get_frame_delay();
      }
      set_frame_changed(true);

      if (lua_context != NULL) {
        lua_context->sprite_on_frame_changed(*this, current_animation_name, current_frame);
      }
    }
  }
  else {
    // take the same frame as the other sprite
    if (synchronize_to->is_animation_finished()) {
      finished = true;
      if (lua_context != NULL) {
        lua_context->sprite_on_animation_finished(*this, current_animation_name);
      }
    }
    else {
      int other_frame = synchronize_to->get_current_frame();
      if (other_frame != current_frame) {
        current_frame = other_frame;
        next_frame_date = now + get_frame_delay();
        set_frame_changed(true);

        if (lua_context != NULL) {
          lua_context->sprite_on_frame_changed(*this, current_animation_name, current_frame);
        }
      }
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
}

/**
 * \brief Draws the sprite on a surface, with its current animation,
 * direction and frame.
 * \param dst_surface The destination surface.
 * \param dst_position Coordinates on the destination surface
 * (the origin will be placed at this position).
 */
void Sprite::raw_draw(
    Surface& dst_surface,
    const Rectangle& dst_position) {

  if (!is_animation_finished()
      && (blink_delay == 0 || blink_is_sprite_visible)) {

    if (intermediate_surface == NULL) {
      current_animation->draw(dst_surface, dst_position,
          current_direction, current_frame);
    }
    else {
      intermediate_surface->fill_with_color(Color::get_black());
      current_animation->draw(*intermediate_surface, get_origin(),
          current_direction, current_frame);
      Rectangle dst_position2(dst_position);
      dst_position2.add_xy(-get_origin().get_x(), -get_origin().get_y());
      intermediate_surface->draw_region(get_size(), dst_surface, dst_position2);
    }
  }
}

/**
 * \brief Draws a subrectangle of the current frame of this sprite.
 * \param region The subrectangle to draw, relative to the top-left corner
 * of the current frame.
 * \param dst_surface The destination surface.
 * \param dst_position Coordinates on the destination surface.
 */
void Sprite::raw_draw_region(
    const Rectangle& region,
    Surface& dst_surface,
    const Rectangle& dst_position) {

  if (!is_animation_finished()
      && (blink_delay == 0 || blink_is_sprite_visible)) {

    current_animation->draw(get_intermediate_surface(), get_origin(),
        current_direction, current_frame);
    Rectangle dst_position2(dst_position);
    dst_position2.add_xy(-get_origin().get_x(), -get_origin().get_y());
    get_intermediate_surface().draw_region(region, dst_surface, dst_position2);
  }
}

/**
 * \brief Draws a transition effect on this drawable object.
 * \param transition The transition effect to apply.
 */
void Sprite::draw_transition(Transition& transition) {

  transition.draw(get_intermediate_surface());
}

/**
 * \brief Returns the surface where transitions on this drawable object
 * are applied.
 * \return The surface for transitions.
 */
Surface& Sprite::get_transition_surface() {
  return get_intermediate_surface();
}

/**
 * \brief Returns the intermediate surface used for transitions and other
 * effects for this sprite.
 *
 * Creates this intermediate surface if it does not exist yet.
 *
 * \return The intermediate surface of this sprite.
 */
Surface& Sprite::get_intermediate_surface() const {

  if (intermediate_surface == NULL) {
    intermediate_surface = new Surface(get_max_size());
  }
  return *intermediate_surface;
}

/**
 * \brief Returns the Solarus Lua API.
 * \return The Lua context, or NULL if Lua callbacks are not enabled for this sprite.
 */
LuaContext* Sprite::get_lua_context() const {
  return lua_context;
}

/**
 * \brief Sets the Solarus Lua API.
 * \param lua_context The Lua context, or NULL to disable Lua callbacks
 * for this sprite.
 */
void Sprite::set_lua_context(LuaContext* lua_context) {
  this->lua_context = lua_context;
}

/**
 * \brief Returns the name identifying this type in Lua.
 * \return the name identifying this type in Lua
 */
const std::string& Sprite::get_lua_type_name() const {
  return LuaContext::sprite_module_name;
}

