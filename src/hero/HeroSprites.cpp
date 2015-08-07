/*
 * Copyright (C) 2006-2015 Christopho, Solarus - http://www.solarus-games.org
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
#include "solarus/hero/HeroSprites.h"
#include "solarus/entities/Hero.h"
#include "solarus/entities/CarriedItem.h"
#include "solarus/lua/LuaContext.h"
#include "solarus/Sprite.h"
#include "solarus/SpriteAnimationSet.h"
#include "solarus/Game.h"
#include "solarus/Equipment.h"
#include "solarus/Map.h"
#include "solarus/lowlevel/Sound.h"
#include "solarus/lowlevel/System.h"
#include "solarus/lowlevel/Debug.h"
#include <sstream>
#include <lua.hpp>

namespace Solarus {

/**
 * \brief Associates to each movement direction the possible directions of the hero's sprites.
 *
 * This array indicates the possible directions of the hero's animation (from 0 to 3, or -1 for no change)
 * for each movement direction.
 * Each combination of directional keys can be associated to one or two directions.
 * When one direction is specified (i.e. the second one is set to -1), the hero's sprite always takes
 * this direction.
 * When two directions are specified,
 * the hero sprite takes the first direction, unless it is already under the second one
 * (then it stays in the second one). This permits a natural behavior for diagonal directions.
 */
const int HeroSprites::animation_directions[][2] = {
  { 0, -1},  // right
  { 0,  1},  // right-up: right or up
  { 1, -1},  // up
  { 2,  1},  // left-up: left or up
  { 2, -1},  // left
  { 2,  3},  // left-down: left or down
  { 3, -1},  // down
  { 0,  3}   // right-down: right or down
};

/**
 * \brief Constructor.
 * \param hero the hero
 * \param equipment the equipment
 */
HeroSprites::HeroSprites(Hero& hero, Equipment& equipment):
  hero(hero),
  equipment(equipment),
  has_default_tunic_sprite(true),
  tunic_sprite(nullptr),
  has_default_sword_sprite(true),
  sword_sprite(nullptr),
  sword_stars_sprite(nullptr),
  has_default_sword_sound(true),
  has_default_shield_sprite(true),
  shield_sprite(nullptr),
  shadow_sprite(nullptr),
  ground_sprite(nullptr),
  trail_sprite(nullptr),
  animation_direction_saved(0),
  when_suspended(0),
  blinking(false),
  end_blink_date(0),
  walking(false),
  clipping_rectangle(Rectangle()),
  lifted_item(nullptr),
  animation_callback_ref() {

  rebuild_equipment();
}

/**
 * \brief Returns the Lua context of the hero.
 * \return The Lua context.
 */
LuaContext& HeroSprites::get_lua_context() {
  return hero.get_lua_context();
}

/**
 * \brief Loads (or reloads) the sprites and sounds of the hero and his
 * equipment.
 *
 * The sprites and sounds loaded may depend on his tunic, sword and shield as
 * specified in the savegame.
 * This function must be called at the game beginning
 * and as soon as the hero's equipment is changed.
 */
void HeroSprites::rebuild_equipment() {

  int animation_direction = -1;
  if (tunic_sprite != nullptr) {
    // Save the direction.
    animation_direction = tunic_sprite->get_current_direction();
  }

  // The hero's body.
  if (has_default_tunic_sprite) {
    set_tunic_sprite_id(get_default_tunic_sprite_id());
  }

  // The hero's shadow.
  if (shadow_sprite == nullptr) {
    shadow_sprite = std::make_shared<Sprite>("entities/shadow");
    shadow_sprite->set_current_animation("big");
  }

  // The hero's sword.
  if (has_default_sword_sprite) {
    set_sword_sprite_id(get_default_sword_sprite_id());
  }

  if (has_default_sword_sound) {
    set_sword_sound_id(get_default_sword_sound_id());
  }

  const int sword_number = equipment.get_ability(Ability::SWORD);
  if (sword_number > 0) {
    // TODO make this sprite depend on the sword sprite: sword_sprite_id + "_stars"
    std::ostringstream oss;
    oss << "hero/sword_stars" << sword_number;
    sword_stars_sprite = std::make_shared<Sprite>(oss.str());
    sword_stars_sprite->stop_animation();
  }

  // The hero's shield.
  if (has_default_shield_sprite) {
    set_shield_sprite_id(get_default_shield_sprite_id());
  }

  // The trail.
  trail_sprite = std::make_shared<Sprite>("hero/trail");
  trail_sprite->stop_animation();

  // Restore the animation direction.
  if (animation_direction != -1) {
    set_animation_direction(animation_direction);
  }
}

/**
 * \brief Returns the animation set id used for the tunic sprite.
 * \return The tunic sprite animation set id.
 */
const std::string& HeroSprites::get_tunic_sprite_id() const {

  return tunic_sprite_id;
}

/**
 * \brief Sets the animation set id to use for the tunic sprite.
 * \param sprite_id The tunic sprite animation set id.
 */
void HeroSprites::set_tunic_sprite_id(const std::string& sprite_id) {

  if (sprite_id == this->tunic_sprite_id) {
    return;
  }

  this->tunic_sprite_id = sprite_id;

  std::string animation;
  int direction = -1;
  if (tunic_sprite != nullptr) {
    // Delete the previous sprite, but save its animation and direction.
    animation = tunic_sprite->get_current_animation();
    direction = tunic_sprite->get_current_direction();
    tunic_sprite = nullptr;
  }

  tunic_sprite = std::make_shared<Sprite>(sprite_id);
  tunic_sprite->enable_pixel_collisions();
  if (!animation.empty()) {
    set_tunic_animation(animation);
    tunic_sprite->set_current_direction(direction);
  }

  has_default_tunic_sprite = (sprite_id == get_default_tunic_sprite_id());

  // Synchronize other sprites to the new tunic sprite.
  if (sword_sprite != nullptr) {
    sword_sprite->set_synchronized_to(tunic_sprite);
  }
  if (shield_sprite != nullptr) {
    shield_sprite->set_synchronized_to(tunic_sprite);
  }
}

/**
 * \brief Returns the animation set id of the default tunic sprite.
 *
 * The default tunic sprite id is "hero/tunicX" where X is the tunic level.
 *
 * \return The default tunic sprite animation set id.
 */
std::string HeroSprites::get_default_tunic_sprite_id() const {

  const int tunic_level = equipment.get_ability(Ability::TUNIC);
  std::ostringstream oss;
  oss << "hero/tunic" << tunic_level;
  return oss.str();
}

/**
 * \brief Returns the animation set id used for the sword sprite.
 * \return The sword sprite animation set id.
 */
const std::string& HeroSprites::get_sword_sprite_id() const {

  return sword_sprite_id;
}

/**
 * \brief Sets the animation set id to use for the sword sprite.
 * \param sprite_id The sword sprite animation set id.
 * An empty string means no sword sprite.
 */
void HeroSprites::set_sword_sprite_id(const std::string& sprite_id) {

  if (sprite_id == this->sword_sprite_id) {
    return;
  }

  this->sword_sprite_id = sprite_id;

  std::string animation;
  int direction = -1;
  if (sword_sprite != nullptr) {
    // Delete the previous sprite, but save its animation and direction.
    if (sword_sprite->is_animation_started()) {
      animation = sword_sprite->get_current_animation();
      direction = sword_sprite->get_current_direction();
    }
    sword_sprite = nullptr;
  }

  if (!sprite_id.empty()) {
    // There is a sword sprite specified.
    sword_sprite = std::make_shared<Sprite>(sprite_id);
    sword_sprite->enable_pixel_collisions();
    sword_sprite->set_synchronized_to(tunic_sprite);
    if (animation.empty()) {
      sword_sprite->stop_animation();
    }
    else {
      sword_sprite->set_current_animation(animation);
      sword_sprite->set_current_direction(direction);
    }
  }

  has_default_sword_sprite = (sprite_id == get_default_sword_sprite_id());
}

/**
 * \brief Returns the animation set id of the default sword sprite.
 *
 * The default sword sprite id is "hero/swordX" where X is the sword level,
 * or no sprite if the sword level is 0.
 *
 * \return The default sword sprite animation set id.
 */
std::string HeroSprites::get_default_sword_sprite_id() const {

  const int sword_level = equipment.get_ability(Ability::SWORD);
  if (sword_level == 0) {
    return "";
  }

  std::ostringstream oss;
  oss << "hero/sword" << sword_level;
  return oss.str();
}

/**
 * \brief Returns the id of the sound played when using the sword.
 * \return The sword sound id.
 */
const std::string& HeroSprites::get_sword_sound_id() const {

  return sword_sound_id;
}

/**
 * \brief Sets the sound to play when using the sword.
 * \param sound_id The sword sound id.
 * An empty string means no sword sound.
 */
void HeroSprites::set_sword_sound_id(const std::string& sound_id) {

  if (sound_id == this->sword_sound_id) {
    return;
  }

  this->sword_sound_id = sound_id;

  has_default_sword_sound = (sound_id == get_default_sword_sound_id());
}

/**
 * \brief Returns the id of default sound to play when using the sword.
 *
 * The default sword sound is "swordX" where X is the sword level,
 * or no sound if the sword level is 0.
 *
 * \return The default sword sound id.
 */
std::string HeroSprites::get_default_sword_sound_id() const {

  const int sword_level = equipment.get_ability(Ability::SWORD);
  if (sword_level == 0) {
    return "";
  }

  std::ostringstream oss;
  oss << "sword" << sword_level;
  return oss.str();
}

/**
 * \brief Returns the animation set id used for the shield sprite.
 * \return The shield sprite animation set id.
 */
const std::string& HeroSprites::get_shield_sprite_id() const {

  return shield_sprite_id;
}

/**
 * \brief Sets the animation set id to use for the shield sprite.
 * \param sprite_id The shield sprite animation set id.
 * An empty string means no sword sprite.
 */
void HeroSprites::set_shield_sprite_id(const std::string& sprite_id) {

  if (sprite_id == this->shield_sprite_id) {
    return;
  }

  this->shield_sprite_id = sprite_id;

  std::string animation;
  int direction = -1;
  if (shield_sprite != nullptr) {
    // Delete the previous sprite, but save its animation and direction.
    if (shield_sprite->is_animation_started()) {
      animation = shield_sprite->get_current_animation();
      direction = shield_sprite->get_current_direction();
    }
    shield_sprite = nullptr;
  }

  if (!sprite_id.empty()) {
    // There is a shield sprite specified.
    shield_sprite = std::make_shared<Sprite>(sprite_id);
    shield_sprite->set_synchronized_to(tunic_sprite);
    if (animation.empty()) {
      shield_sprite->stop_animation();
    }
    else {
      shield_sprite->set_current_animation(animation);
      shield_sprite->set_current_direction(direction);
    }
  }

  has_default_shield_sprite = (sprite_id == get_default_shield_sprite_id());
}

/**
 * \brief Returns the animation set id of the default shield sprite.
 *
 * The default shield sprite id is "hero/shieldX" where X is the shield level,
 * or no shield if the sword level is 0.
 *
 * \return The default shield sprite animation set id.
 */
std::string HeroSprites::get_default_shield_sprite_id() const {

  const int shield_level = equipment.get_ability(Ability::SHIELD);
  if (shield_level == 0) {
    return "";
  }

  std::ostringstream oss;
  oss << "hero/shield" << shield_level;
  return oss.str();
}

/**
 * \brief Returns whether the sword is currently displayed on the screen.
 * \return true if the sword is currently displayed on the screen
 */
bool HeroSprites::is_sword_visible() const {
  return equipment.has_ability(Ability::SWORD)
      && sword_sprite != nullptr
      && sword_sprite->is_animation_started();
}

/**
 * \brief Returns whether the stars of the sword are currently displayed on the screen.
 * \return true if the stars of the sword are currently displayed on the screen
 */
bool HeroSprites::is_sword_stars_visible() const {
  return equipment.has_ability(Ability::SWORD)
      && sword_stars_sprite != nullptr
      && sword_stars_sprite->is_animation_started();
}

/**
 * \brief Returns whether the shield is currently displayed on the screen.
 * \return true if the shield is currently displayed on the screen
 */
bool HeroSprites::is_shield_visible() const {
  return equipment.has_ability(Ability::SHIELD)
      && shield_sprite != nullptr
      && shield_sprite->is_animation_started();
}

/**
 * \brief Returns whether the trail of dust is currently displayed.
 * \return true if the trail of dust is currently displayed
 */
bool HeroSprites::is_trail_visible() const {
  return trail_sprite->is_animation_started();
}

/**
 * \brief Returns whether a special ground is displayed under the hero.
 * \return true if a ground is currently visible under the hero
 */
bool HeroSprites::is_ground_visible() const {
  return hero.is_ground_visible()
      && ground_sprite != nullptr;
}

/**
 * \brief Stops displaying the sword and the sword stars (if any).
 */
void HeroSprites::stop_displaying_sword() {

  if (is_sword_visible()) {
    sword_sprite->stop_animation();
  }
  stop_displaying_sword_stars();
}

/**
 * \brief Stops displaying the sword stars (if any).
 */
void HeroSprites::stop_displaying_sword_stars() {

  if (is_sword_stars_visible()) {
    sword_stars_sprite->stop_animation();
  }
}

/**
 * \brief Stops displaying the shield (if any).
 */
void HeroSprites::stop_displaying_shield() {

  if (equipment.has_ability(Ability::SHIELD)) {
    shield_sprite->stop_animation();
  }
}

/**
 * \brief Stops displaying the trail (if any).
 */
void HeroSprites::stop_displaying_trail() {
  trail_sprite->stop_animation();
}

/**
 * \brief Makes the hero blink for a while.
 * \param duration Delay before stopping blinking.
 * 0 means infinite.
 */
void HeroSprites::blink(uint32_t duration) {

  const uint32_t blink_delay = 50;

  blinking = true;
  tunic_sprite->set_blinking(blink_delay);

  if (equipment.has_ability(Ability::SHIELD)) {
    shield_sprite->set_blinking(blink_delay);
  }
  if (equipment.has_ability(Ability::SWORD)) {
    sword_sprite->set_blinking(blink_delay);
  }
  trail_sprite->set_blinking(blink_delay);

  if (duration == 0) {
    // No end date.
    end_blink_date = 0;
  }
  else {
    end_blink_date = System::now() + duration;
  }
}

/**
 * \brief Stops making the hero's sprites blink.
 */
void HeroSprites::stop_blinking() {

  blinking = false;
  end_blink_date = 0;

  tunic_sprite->set_blinking(0);

  if (equipment.has_ability(Ability::SHIELD)) {
    shield_sprite->set_blinking(0);
  }
  if (equipment.has_ability(Ability::SWORD)) {
    sword_sprite->set_blinking(0);
  }
  trail_sprite->set_blinking(0);
}

/**
 * \brief Returns whether the hero's sprites are currently blinking.
 * \return \c true if the hero's sprites are currently blinking.
 */
bool HeroSprites::is_blinking() const {
  return blinking;
}

/**
 * \brief Sets a rectangle of the map where the drawing of the hero's sprite will be restricted to.
 *
 * A (0,0,0,0) rectangle means that the sprite is not restricted to a subarea of the map.
 *
 * \param clipping_rectangle a subarea of the map to restrict the drawing to
 */
void HeroSprites::set_clipping_rectangle(const Rectangle& clipping_rectangle) {
  this->clipping_rectangle = clipping_rectangle;
}

/**
 * \brief Returns whether the sprites have currently a walking animation.
 * \return true if the sprites are walking
 */
bool HeroSprites::is_walking() const {
  return walking;
}

/**
 * \brief Returns the direction of the hero's sprites.
 *
 * It is different from the movement direction.
 *
 * \return the direction of the sprites (0 to 3)
 */
int HeroSprites::get_animation_direction() const {
  return tunic_sprite->get_current_direction();
}

/**
 * \brief Returns the direction of the hero's sprites.
 *
 * It is different from the movement direction.
 *
 * \return the direction of the sprites (0 to 7, except diagonal directions)
 */
int HeroSprites::get_animation_direction8() const {
  return get_animation_direction() * 2;
}

/**
 * \brief Returns the direction that the hero's sprites should take.
 *
 * This function returns the direction that the hero's sprites should take depending on the direction wanted
 * by the player and the real movement direction.
 * For diagonal directions, the direction returned depends
 * on the current real direction of the hero's sprites.
 *
 * \param keys_direction the direction defined by the combination of directional keys pressed (0 to 7),
 * or -1 if this is not a valid direction
 * \param real_movement_direction the direction of the hero's actual movement (may be different from keys_direction)
 * \return the direction of the sprites corresponding to these arrows (0 to 3),
 * or -1 if the directional keys combination is illegal
 */
int HeroSprites::get_animation_direction(
    int keys_direction, int real_movement_direction) const {

  int result;

  if (keys_direction == -1) {
    // the player is not pressing a valid combination of directional keys: don't change the sprite's direction
    result = -1;
  }
  else if (keys_direction % 2 == 0) {
    // the player is pressing only one of the four main directions
    // (i.e. he is not trying to make a diagonal move, even if the real movement is diagonal)
    // we just give the sprite this direction
    result = keys_direction / 2;
  }
  // the movement is diagonal: we have to choose between two directions
  else if (animation_directions[real_movement_direction][1] == get_animation_direction()) {
    // we choose the second direction if it was already the sprite's direction
    result = animation_directions[real_movement_direction][1];
  }
  else {
    // otherwise we choose the first direction
    result = animation_directions[real_movement_direction][0];
  }

  return result;
}

/**
 * \brief Changes the direction of the hero's sprites.
 *
 * It is different from the movement direction.
 *
 * \param direction the direction to set (0 to 3)
 */
void HeroSprites::set_animation_direction(int direction) {

  Debug::check_assertion(direction >= 0 && direction < 4,
    "Invalid direction for set_animation_direction");

  tunic_sprite->set_current_direction(direction);

  if (is_sword_visible()) {
    sword_sprite->set_current_direction(direction);
  }

  if (is_sword_stars_visible()) {
    sword_stars_sprite->set_current_direction(direction);
  }

  if (is_shield_visible()) {
    shield_sprite->set_current_direction(direction);
  }

  if (is_trail_visible()) {
    trail_sprite->set_current_direction(direction);
  }
}

/**
 * \brief Changes the direction of the hero's sprites.
 *
 * The direction specified is one of the 8 possible movement directions of the hero.
 * The hero's sprites only have four directions, so when
 * the specified direction is a diagonal one,
 * one of the two closest main directions is picked.
 *
 * \param direction the movement direction (0 to 7)
 */
void HeroSprites::set_animation_direction8(int direction) {

  if (get_animation_direction() != animation_directions[direction][1]) {
    set_animation_direction(animation_directions[direction][0]);
  }
}

/**
 * \brief Returns whether the sprites animations are finished.
 * \return true if the animation is finished
 */
bool HeroSprites::is_animation_finished() const {
  return tunic_sprite->is_animation_finished();
}

/**
 * \brief Returns the current frame of the tunic sprite.
 * \return the current frame
 */
int HeroSprites::get_current_frame() const {
  return tunic_sprite->get_current_frame();
}

/**
 * \brief Saves the current direction of the the sprite.
 *
 * Call restore_animation_direction() to restore the direction saved here.
 */
void HeroSprites::save_animation_direction() {
  this->animation_direction_saved = get_animation_direction();
}

/**
 * \brief Restores the direction of the hero's sprite saved by the last
 * call to save_animation_direction().
 */
void HeroSprites::restore_animation_direction() {
  set_animation_direction(animation_direction_saved);
}

/**
 * \brief Updates the animation of the hero's sprites if necessary.
 */
void HeroSprites::update() {

  // Keep the current sprites here in case they change from a script during the operation.
  SpritePtr tunic_sprite = this->tunic_sprite;
  SpritePtr sword_sprite = this->sword_sprite;

  // update the frames
  tunic_sprite->update();

  if (is_sword_visible()) {
    sword_sprite->update();
    sword_sprite->set_current_frame(tunic_sprite->get_current_frame());
    hero.check_collision_with_detectors(*sword_sprite);
  }
  hero.check_collision_with_detectors(*tunic_sprite);

  if (is_sword_stars_visible()) {
    // the stars are not synchronized with the other sprites
    sword_stars_sprite->update();
  }

  if (is_shield_visible()) {
    shield_sprite->update();
    if (walking) {
      shield_sprite->set_current_frame(tunic_sprite->get_current_frame());
    }
  }

  if (is_trail_visible()) {
    trail_sprite->update();
  }

  if (is_ground_visible()) {
    ground_sprite->update();
  }

  if (lifted_item != nullptr && walking) {
    lifted_item->get_sprite().set_current_frame(tunic_sprite->get_current_frame() % 3);
  }

  // blinking
  if (is_blinking()
      && end_blink_date != 0
      && System::now() >= end_blink_date) {
    stop_blinking();
  }

  // Lua callback.
  if (tunic_sprite->is_animation_finished() &&
      !animation_callback_ref.is_empty()) {
    animation_callback_ref.clear_and_call("hero animation callback");
  }
}

/**
 * \brief Draws the hero's sprites on the map.
 */
void HeroSprites::draw_on_map() {

  int x = hero.get_x();
  int y = hero.get_y();

  Map& map = hero.get_map();

  if (hero.is_shadow_visible()) {
    map.draw_sprite(*shadow_sprite, x, y, clipping_rectangle);
  }

  const Point& displayed_xy = hero.get_displayed_xy();
  x = displayed_xy.x;
  y = displayed_xy.y;

  map.draw_sprite(*tunic_sprite, x, y, clipping_rectangle);

  if (is_trail_visible()) {
    map.draw_sprite(*trail_sprite, x, y, clipping_rectangle);
  }

  if (is_ground_visible()) {
    map.draw_sprite(*ground_sprite, x, y, clipping_rectangle);
  }

  if (is_sword_visible()) {
    map.draw_sprite(*sword_sprite, x, y, clipping_rectangle);
  }

  if (is_sword_stars_visible()) {
    map.draw_sprite(*sword_stars_sprite, x, y, clipping_rectangle);
  }

  if (is_shield_visible()) {
    map.draw_sprite(*shield_sprite, x, y, clipping_rectangle);
  }

  if (lifted_item != nullptr) {
    lifted_item->draw_on_map();
  }
}

/**
 * \brief Suspends or resumes the animation of the hero's sprites.
 *
 * This function is called by the map when the game is suspended or resumed.
 *
 * \param suspended true to suspend the hero's sprites, false to resume them
 */
void HeroSprites::set_suspended(bool suspended) {

  tunic_sprite->set_suspended(suspended);

  if (equipment.has_ability(Ability::SWORD) && sword_sprite != nullptr) {
    sword_sprite->set_suspended(suspended);
    sword_stars_sprite->set_suspended(suspended);
  }

  if (equipment.has_ability(Ability::SHIELD) && shield_sprite != nullptr) {
    shield_sprite->set_suspended(suspended);
  }

  trail_sprite->set_suspended(suspended);

  if (is_ground_visible()) {
    ground_sprite->set_suspended(suspended);
  }

  // Timer.
  uint32_t now = System::now();
  if (suspended) {
    when_suspended = now;
  }
  else if (end_blink_date != 0) {
    end_blink_date += now - when_suspended;
  }
}

/**
 * \brief Notifies the hero's sprites that a map has just become active.
 */
void HeroSprites::notify_map_started() {

  // Some sprites may be tileset dependent.
  notify_tileset_changed();
}

/**
 * \brief Notifies the hero's sprites that the tileset has just changed.
 */
void HeroSprites::notify_tileset_changed() {

  // Some sprites may be tileset dependent.
  if (lifted_item != nullptr) {
    lifted_item->notify_tileset_changed();
  }

  if (is_ground_visible()) {
    ground_sprite->set_tileset(hero.get_map().get_tileset());
  }
}

/**
 * \brief Restarts the animation of the hero's sprites.
 *
 * This function is called when the sprites have to
 * get back to their first frame.
 */
void HeroSprites::restart_animation() {

  tunic_sprite->restart_animation();

  if (is_sword_visible()) {
    sword_sprite->restart_animation();
  }

  if (is_sword_stars_visible()) {
    sword_stars_sprite->restart_animation();
  }

  if (is_shield_visible()) {
    shield_sprite->restart_animation();
  }

  if (is_trail_visible()) {
    trail_sprite->restart_animation();
  }

  if (is_ground_visible()) {
    ground_sprite->restart_animation();
  }
}

/**
 * \brief Sets whether the hero's sprite should keep playing their animation
 * when the game is suspended.
 * \param ignore_suspend true to make the sprites continue their animation even
 * when the game is suspended
 */
void HeroSprites::set_ignore_suspend(bool ignore_suspend) {

  tunic_sprite->set_ignore_suspend(ignore_suspend);

  if (is_sword_visible()) {
    sword_sprite->set_ignore_suspend(ignore_suspend);
  }

  if (is_sword_stars_visible()) {
    sword_stars_sprite->set_ignore_suspend(ignore_suspend);
  }

  if (is_shield_visible()) {
    shield_sprite->set_ignore_suspend(ignore_suspend);
  }

  if (is_trail_visible()) {
    trail_sprite->set_ignore_suspend(ignore_suspend);
  }

  if (is_ground_visible()) {
    ground_sprite->set_ignore_suspend(ignore_suspend);
  }
}

/**
 * \brief This function is called when the sprites take a "stopped" animation.
 *
 * It makes instructions common to all states having a "stopped" animation.
 * (e.g. free or carrying).
 */
void HeroSprites::set_animation_stopped_common() {

  if (is_ground_visible()
      && hero.get_ground_below() != Ground::SHALLOW_WATER) {
    ground_sprite->set_current_animation("stopped");
  }
  walking = false;
}

/**
 * \brief Starts the basic "stopped" animation of the hero's sprites.
 */
void HeroSprites::set_animation_stopped_normal() {

  set_animation_stopped_common();

  if (equipment.has_ability(Ability::SHIELD)) {

    set_tunic_animation("stopped_with_shield");
    shield_sprite->set_current_animation("stopped");
    shield_sprite->set_current_direction(get_animation_direction());
  }
  else {
    set_tunic_animation("stopped");
  }
  stop_displaying_sword();
  stop_displaying_trail();
}

/**
 * \brief Starts the "stopped" animation of the hero's sprites with the sword loading.
 */
void HeroSprites::set_animation_stopped_sword_loading() {

  set_animation_stopped_common();

  int direction = get_animation_direction();

  set_tunic_animation("sword_loading_stopped");
  sword_sprite->set_current_animation("sword_loading_stopped");
  sword_sprite->set_current_direction(direction);
  sword_stars_sprite->set_current_animation("loading");
  sword_stars_sprite->set_current_direction(direction);

  if (equipment.has_ability(Ability::SHIELD)) {

    shield_sprite->set_current_animation("sword_loading_stopped");
    shield_sprite->set_current_direction(direction);
  }
  stop_displaying_trail();
}

/**
 * \brief Starts the "stopped" animation with sprites that represent
 * the hero carrying something.
 *
 * If the hero actually carries an item, the carried item also takes a "stopped" animation.
 */
void HeroSprites::set_animation_stopped_carrying() {

  set_animation_stopped_common();
  set_tunic_animation("carrying_stopped");

  if (lifted_item != nullptr) {
    lifted_item->set_animation_stopped();
  }
  stop_displaying_trail();
}

/**
 * \brief Starts the "stopped" animation with sprites that represent
 * the hero swimming.
 */
void HeroSprites::set_animation_stopped_swimming() {

  set_animation_stopped_common();
  set_tunic_animation("swimming_stopped");
  stop_displaying_sword();
  stop_displaying_shield();
  stop_displaying_trail();
}

/**
 * \brief This function is called when the sprites take a "walking" animation.
 *
 * It makes instructions common to all states having a "walking" animation.
 * (e.g. free or carrying).
 */
void HeroSprites::set_animation_walking_common() {

  if (is_ground_visible() && hero.get_ground_below() != Ground::SHALLOW_WATER) {
    ground_sprite->set_current_animation("walking");
  }

  walking = true;
}

/**
 * \brief Starts the normal "walking" animation of the hero's sprites.
 */
void HeroSprites::set_animation_walking_normal() {

  set_animation_walking_common();

  if (equipment.has_ability(Ability::SHIELD)) {

    set_tunic_animation("walking_with_shield");

    shield_sprite->set_current_animation("walking");
    shield_sprite->set_current_direction(get_animation_direction());
  }
  else {
    set_tunic_animation("walking");
  }
  stop_displaying_sword();
  stop_displaying_trail();
}

/**
 * \brief Starts the "walking" animation of the hero's sprites with the sword loading.
 */
void HeroSprites::set_animation_walking_sword_loading() {

  set_animation_walking_common();

  int direction = get_animation_direction();

  set_tunic_animation("sword_loading_walking");
  if (equipment.has_ability(Ability::SWORD)) {
    sword_sprite->set_current_animation("sword_loading_walking");
    sword_sprite->set_current_direction(direction);
    sword_stars_sprite->set_current_animation("loading");
    sword_stars_sprite->set_current_direction(direction);
  }

  if (equipment.has_ability(Ability::SHIELD)) {
    shield_sprite->set_current_animation("sword_loading_walking");
    shield_sprite->set_current_direction(direction);
  }
  stop_displaying_trail();
}

/**
 * \brief Starts the "walking" animation with sprites that represent
 * the hero carrying something.
 *
 * If the hero actually carries an item, the carried item also takes a "walking" animation.
 */
void HeroSprites::set_animation_walking_carrying() {

  set_animation_walking_common();

  set_tunic_animation("carrying_walking");

  if (lifted_item != nullptr) {
    lifted_item->set_animation_walking();
  }
  stop_displaying_shield();
  stop_displaying_trail();
}

/**
 * \brief Starts the "swimming_slow" animation of the sprites.
 */
void HeroSprites::set_animation_swimming_slow() {

  set_animation_walking_common();

  set_tunic_animation("swimming_slow");
  stop_displaying_sword();
  stop_displaying_shield();
  stop_displaying_trail();
}

/**
 * \brief Starts the "swimming_fast" animation of the sprites.
 */
void HeroSprites::set_animation_swimming_fast() {

  set_animation_walking_common();

  set_tunic_animation("swimming_fast");
  stop_displaying_sword();
  stop_displaying_shield();
  stop_displaying_trail();
}

/**
 * \brief Starts the "walking_diagonal" animation of the hero's sprites.
 * \param direction8 the diagonal direction to take (1, 3, 5 or 7)
 */
void HeroSprites::set_animation_walking_diagonal(int direction8) {

  stop_displaying_sword();
  stop_displaying_shield();
  stop_displaying_trail();
  set_tunic_animation("walking_diagonal");
  tunic_sprite->set_current_direction(direction8 / 2);
}

/**
 * \brief Starts (or restarts) the "sword" animation of the hero's sprites.
 */
void HeroSprites::set_animation_sword() {

  int direction = get_animation_direction();

  set_tunic_animation("sword");
  tunic_sprite->restart_animation();

  sword_sprite->set_current_animation("sword");
  sword_sprite->set_current_direction(direction);
  sword_sprite->restart_animation();
  sword_stars_sprite->stop_animation();

  if (equipment.has_ability(Ability::SHIELD)) {

    if (direction % 2 != 0) {
      shield_sprite->set_current_animation("sword");
      shield_sprite->set_current_direction(direction / 2);
      shield_sprite->restart_animation();
    }
    else {
      stop_displaying_shield();
    }
  }
  stop_displaying_trail();
}

/**
 * \brief Plays the sound corresponding to the current sword.
 */
void HeroSprites::play_sword_sound() {
  Sound::play(sword_sound_id);
}

/**
 * \brief Starts (or restarts) the "sword tapping" animation of the hero's sprites.
 */
void HeroSprites::set_animation_sword_tapping() {

  int direction = get_animation_direction();

  set_tunic_animation("sword_tapping");
  tunic_sprite->restart_animation();

  sword_sprite->set_current_animation("sword_tapping");
  sword_sprite->set_current_direction(direction);
  sword_sprite->restart_animation();
  sword_stars_sprite->stop_animation();

  if (equipment.has_ability(Ability::SHIELD)) {

    shield_sprite->set_current_animation("sword_tapping");
    shield_sprite->set_current_direction(direction);
    shield_sprite->restart_animation();
  }
  stop_displaying_trail();
}

/**
 * \brief Starts (or restarts) the "spin_attck" animation of the hero's sprites.
 */
void HeroSprites::set_animation_spin_attack() {

  set_tunic_animation("spin_attack");
  sword_sprite->set_current_animation("spin_attack");
  stop_displaying_sword_stars();
  stop_displaying_shield();
  stop_displaying_trail();
}

/**
 * \brief Starts (or restarts) the "super_spin_attck" animation of the hero's sprites.
 */
void HeroSprites::set_animation_super_spin_attack() {

  set_tunic_animation("super_spin_attack");
  sword_sprite->set_current_animation("super_spin_attack");
  stop_displaying_sword_stars();
  stop_displaying_shield();
  stop_displaying_trail();
}

/**
 * \brief Starts the "grabbing" animation of the hero's sprites.
 */
void HeroSprites::set_animation_grabbing() {

  set_tunic_animation("grabbing");
  stop_displaying_shield();
  stop_displaying_trail();
}

/**
 * \brief Starts the "pulling" animation of the hero's sprites.
 */
void HeroSprites::set_animation_pulling() {

  set_tunic_animation("pulling");
  stop_displaying_shield();
  stop_displaying_trail();
}

/**
 * \brief Starts the "pushing" animation of the hero's sprites.
 */
void HeroSprites::set_animation_pushing() {

  set_tunic_animation("pushing");
  stop_displaying_shield();
  stop_displaying_trail();
}

/**
 * \brief Starts the "lifting" animation of the hero's sprites.
 */
void HeroSprites::set_animation_lifting() {

  set_tunic_animation("lifting");
  stop_displaying_shield();
  stop_displaying_trail();
}

/**
 * \brief Starts the "jumping" animation of the hero's sprites.
 */
void HeroSprites::set_animation_jumping() {

  set_tunic_animation("jumping");

  if (equipment.has_ability(Ability::SHIELD)) {
    shield_sprite->set_current_animation("stopped");
    shield_sprite->set_current_direction(get_animation_direction());
  }
  stop_displaying_sword();
  stop_displaying_trail();
}

/**
 * \brief Starts the "hurt" animation of the hero's sprites.
 */
void HeroSprites::set_animation_hurt() {

  set_tunic_animation("hurt");
  stop_displaying_sword();
  stop_displaying_shield();
  stop_displaying_trail();
}

/**
 * \brief Starts the "falling" animation of the hero's sprites.
 */
void HeroSprites::set_animation_falling() {

  // show the animation
  set_tunic_animation("falling");
  stop_displaying_sword();
  stop_displaying_shield();
  stop_displaying_trail();
}

/**
 * \brief Starts the "brandish" animation of the hero's sprites.
 */
void HeroSprites::set_animation_brandish() {

  set_tunic_animation("brandish");
  tunic_sprite->set_current_direction(1);
  stop_displaying_sword();
  stop_displaying_shield();
  stop_displaying_trail();
}

/**
 * \brief Starts the "victory" animation of the hero's sprites.
 */
void HeroSprites::set_animation_victory() {

  set_tunic_animation("victory");
  tunic_sprite->set_current_direction(1);
  if (sword_sprite != nullptr) {
    sword_sprite->set_current_animation("victory");
    sword_sprite->set_current_direction(1);
  }
  stop_displaying_sword_stars();
  stop_displaying_shield();
  stop_displaying_trail();
}

/**
 * \brief Starts the "prepare running" animation of the hero's sprites.
 */
void HeroSprites::set_animation_prepare_running() {

  set_animation_walking_normal();
  trail_sprite->set_current_animation("running");
}

/**
 * \brief Starts the "running" animation of the hero's sprites.
 */
void HeroSprites::set_animation_running() {

  set_animation_walking_sword_loading();
  stop_displaying_sword_stars();
  trail_sprite->set_current_animation("running");
}

/**
 * \brief Starts the "boomerang" animation of the hero's sprites.
 * \param tunic_preparing_animation Animation name of the hero's tunic sprite
 * when preparing the boomerang.
 */
void HeroSprites::set_animation_boomerang(
    const std::string& tunic_preparing_animation) {

  set_tunic_animation(tunic_preparing_animation);

  if (shield_sprite != nullptr
      && shield_sprite->has_animation("boomerang")) {
    shield_sprite->set_current_animation("boomerang");
  }
  else {
    stop_displaying_shield();
  }
  stop_displaying_sword();
  stop_displaying_trail();
}

/**
 * \brief Returns whether the specified animation exists in the tunic sprite.
 * \param animation Name of the animation to check.
 * \return \c true if the tunic sprite has an animation with this name.
 */
bool HeroSprites::has_tunic_animation(const std::string& animation) const {

  return tunic_sprite->has_animation(animation);
}

/**
 * \brief Returns the current animation of the tunic sprite.
 * \return The animation name of the tunic sprite.
 */
const std::string& HeroSprites::get_tunic_animation() const {

  return tunic_sprite->get_current_animation();
}

/**
 * \brief Changes the animation of the tunic sprite.
 *
 * Cancels the Lua callback if any.
 *
 * \param animation The animation name of the tunic sprite.
 */
void HeroSprites::set_tunic_animation(const std::string& animation) {

  set_tunic_animation(animation, ScopedLuaRef());
}


/**
 * \brief Changes the animation of the tunic sprite.
 *
 * Cancels the Lua callback if any.
 *
 * \param animation The animation name of the tunic sprite.
 * \param callback_ref Lua ref of a function to call when the animation ends
 * or an empty ref.
 */
void HeroSprites::set_tunic_animation(
    const std::string& animation,
    const ScopedLuaRef& callback_ref
) {

  this->animation_callback_ref = callback_ref;

  tunic_sprite->set_current_animation(animation);
}

/**
 * \brief Starts a custom animation of the hero's sprites.
 *
 * All sprites of the hero that have an animation with this name take the
 * animation. The ones that don't have such an animation are not displayed.
 * Many simple animations can be started with this function.
 * More complex one have dedicated functions.
 *
 * \param animation Name of the animation to give to the hero's sprites.
 */
void HeroSprites::set_animation(const std::string& animation) {

  set_animation(animation, ScopedLuaRef());
}

/**
 * \brief Starts a custom animation of the hero's sprites.
 *
 * All sprites of the hero that have an animation with this name take the
 * animation. The ones that don't have such an animation are not displayed.
 * Many simple animations can be started with this function.
 * More complex one have dedicated functions.
 *
 * \param animation Name of the animation to give to the hero's sprites.
 * \param callback_ref Lua ref of a function to call when the animation ends
 * or an empty ref.
 */
void HeroSprites::set_animation(
    const std::string& animation,
    const ScopedLuaRef& callback_ref
) {

  if (tunic_sprite->has_animation(animation)) {
    set_tunic_animation(animation, callback_ref);
  }
  else {
    Debug::error("Sprite '" + tunic_sprite->get_animation_set_id() + "': Animation '" + animation + "' not found.");
  }

  if (shield_sprite != nullptr
      && shield_sprite->has_animation(animation)) {
    shield_sprite->set_current_animation(animation);
  }
  else {
    stop_displaying_shield();
  }

  if (sword_sprite != nullptr
      && sword_sprite->has_animation(animation)) {
    sword_sprite->set_current_animation(animation);
  }
  else {
    stop_displaying_sword();
  }

  if (sword_stars_sprite != nullptr
      && sword_stars_sprite->has_animation(animation)) {
    sword_stars_sprite->set_current_animation(animation);
  }
  else {
    stop_displaying_sword_stars();
  }

  if (trail_sprite != nullptr
      && trail_sprite->has_animation(animation)) {
    trail_sprite->set_current_animation(animation);
  }
  else {
    stop_displaying_trail();
  }
}

/**
 * \brief Creates the ground sprite and sound corresponding to the specified
 * ground.
 * \param ground A ground.
 */
void HeroSprites::create_ground(Ground ground) {

  ground_sprite = nullptr;

  std::string sprite_id;
  if (ground == Ground::GRASS) {
    sprite_id = "hero/ground1";
    ground_sound_id = "walk_on_grass";
  }
  else if (ground == Ground::SHALLOW_WATER) {
    sprite_id = "hero/ground2";
    ground_sound_id = "walk_on_water";
  }

  if (!sprite_id.empty()) {
    ground_sprite = std::make_shared<Sprite>(sprite_id);
    ground_sprite->set_tileset(hero.get_map().get_tileset());
    if (ground != Ground::SHALLOW_WATER) {
      ground_sprite->set_current_animation(walking ? "walking" : "stopped");
    }
  }
}

/**
 * \brief Deletes the ground sprite.
 */
void HeroSprites::destroy_ground() {

  ground_sprite = nullptr;
}

/**
 * \brief Plays a sound for the ground displayed under the hero.
 */
void HeroSprites::play_ground_sound() {
  Sound::play(ground_sound_id);
}

/**
 * \brief Indicates a lifted item to display with the hero's sprites.
 *
 * Calling this function makes this class display the lifted item and
 * give it the appropriate animation, so that you don't have to make it yourself.
 * However, this class only handles displaying:
 * you still have to update it and make it follow the hero.
 *
 * \param lifted_item the item to display, or nullptr to stop displaying a lifted item
 */
void HeroSprites::set_lifted_item(
    const std::shared_ptr<CarriedItem>& lifted_item
) {
  this->lifted_item = lifted_item;
}

}

