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
#include "hero/HeroSprites.h"
#include "entities/Hero.h"
#include "entities/CarriedItem.h"
#include "Sprite.h"
#include "SpriteAnimationSet.h"
#include "Equipment.h"
#include "Map.h"
#include "lowlevel/Sound.h"
#include "lowlevel/System.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"

/**
 * @brief Associates to each movement direction the possible directions of the hero's sprites.
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
 * @brief String constants corresponding to the sprites of the tunics.
 */
const SpriteAnimationSetId HeroSprites::tunic_sprite_ids[] = {
  "hero/tunic1", // green tunic
  "hero/tunic2", // blue tunic
  "hero/tunic3", // red tunic
};

/**
 * @brief String constants corresponding to the sprites of the swords.
 */
const SpriteAnimationSetId HeroSprites::sword_sprite_ids[] = {
  "hero/sword1",
  "hero/sword2",
  "hero/sword3",
  "hero/sword4",
};

/**
 * @brief String constants corresponding to the sprites of the stars of the swords.
 */
const SpriteAnimationSetId HeroSprites::sword_stars_sprite_ids[] = {
  "hero/sword_stars1",
  "hero/sword_stars1",
  "hero/sword_stars2",
  "hero/sword_stars2",
};

/**
 * @brief String constants corresponding to the sprites of the shields.
 */
const SpriteAnimationSetId HeroSprites::shield_sprite_ids[] = {
  "hero/shield1",
  "hero/shield2",
  "hero/shield3",
};

/**
 * @brief String constants corresponding to the sprites of the ground displayed under the hero.
 */
const SpriteAnimationSetId HeroSprites::ground_sprite_ids[] = {
  "hero/ground1",
  "hero/ground2",
};

/**
 * @brief String constants corresponding to the sounds of the swords.
 */
const SoundId HeroSprites::sword_sound_ids[] = {
  "sword1",
  "sword2",
  "sword3",
  "sword4",
};

/**
 * @brief String constants corresponding to the sounds of the ground under the hero.
 */
const SoundId HeroSprites::ground_sound_ids[] = {
  "walk_on_grass",
  "walk_on_water",
};

/**
 * @brief Constructor.
 * @param hero the hero
 * @param equipment the equipment
 */
HeroSprites::HeroSprites(Hero &hero, Equipment &equipment):
  hero(hero), equipment(equipment), tunic_sprite(NULL), sword_sprite(NULL),
  sword_stars_sprite(NULL), shield_sprite(NULL), shadow_sprite(NULL), ground_sprite(NULL), trail_sprite(NULL),
  end_blink_date(0), walking(false), clipping_rectangle(Rectangle()), lifted_item(NULL) {

}

/**
 * @brief Destructor.
 */
HeroSprites::~HeroSprites() {
  delete tunic_sprite;
  delete shadow_sprite;
  delete sword_sprite;
  delete sword_stars_sprite;
  delete shield_sprite;
  delete ground_sprite;
  delete trail_sprite;
}

/**
 * @brief Loads (or reloads) the sprites and sounds of the hero and his equipment.
 *
 * The sprites and sounds loaded depend on his tunic, sword and shield as specified in the savegame.
 * This function must be called at the game beginning
 * and as soon as the hero's equipment is changed.
 */
void HeroSprites::rebuild_equipment() {

  int animation_direction = -1;

  // the hero
  if (tunic_sprite != NULL) {
    // save the animation direction
    animation_direction = tunic_sprite->get_current_direction();
    delete tunic_sprite;
  }

  int tunic_number = equipment.get_ability("tunic");
  
  Debug::check_assertion(tunic_number > 0, StringConcat() << "Invalid tunic number: " << tunic_number);

  tunic_sprite = new Sprite(tunic_sprite_ids[tunic_number - 1]);
  tunic_sprite->enable_pixel_collisions();

  // the hero's shadow
  if (shadow_sprite == NULL) {
    shadow_sprite = new Sprite("entities/shadow");
    shadow_sprite->set_current_animation("big");
  }

  // the hero's sword
  if (sword_sprite != NULL) {
    delete sword_sprite;
    delete sword_stars_sprite;
    sword_sprite = NULL;
    sword_stars_sprite = NULL;
  }

  int sword_number = equipment.get_ability("sword");

  if (sword_number > 0) {
    // the hero has a sword: get the sprite and the sound
    sword_sprite = new Sprite(sword_sprite_ids[sword_number - 1]);
    sword_sprite->stop_animation();
    sword_sprite->enable_pixel_collisions();

    sword_sound_id = sword_sound_ids[sword_number - 1];

    sword_stars_sprite = new Sprite(sword_stars_sprite_ids[sword_number - 1]);
    sword_stars_sprite->stop_animation();
  }

  // the hero's shield
  if (shield_sprite != NULL) {
    delete shield_sprite;
    shield_sprite = NULL;
  }

  int shield_number = equipment.get_ability("shield");

  if (shield_number > 0) {
    // the hero has a shield
    shield_sprite = new Sprite(shield_sprite_ids[shield_number - 1]);
  }

  // the trail
  trail_sprite = new Sprite("hero/trail");
  trail_sprite->stop_animation();

  // restore the animation direction
  if (animation_direction != -1) {
    set_animation_direction(animation_direction);
  }
}

/**
 * @brief Returns whether the sword is currently displayed on the screen.
 * @return true if the sword is currently displayed on the screen
 */
bool HeroSprites::is_sword_visible() {
  return equipment.has_ability("sword") && sword_sprite != NULL && sword_sprite->is_animation_started();
}

/**
 * @brief Returns whether the stars of the sword are currently displayed on the screen.
 * @return true if the stars of the sword are currently displayed on the screen
 */
bool HeroSprites::is_sword_stars_visible() {
  return equipment.has_ability("sword") && sword_stars_sprite != NULL && sword_stars_sprite->is_animation_started();
}

/**
 * @brief Returns whether the shield is currently displayed on the screen.
 * @return true if the shield is currently displayed on the screen
 */
bool HeroSprites::is_shield_visible() {
  return equipment.has_ability("shield") && shield_sprite != NULL && shield_sprite->is_animation_started();
}

/**
 * @brief Returns whether the trail of dust is currently displayed.
 * @return true if the trail of dust is currently displayed
 */
bool HeroSprites::is_trail_visible() {
  return trail_sprite->is_animation_started();
}

/**
 * @brief Stops displaying the sword and the sword stars (if any).
 */
void HeroSprites::stop_displaying_sword() {

  if (is_sword_visible()) {
    sword_sprite->stop_animation();
  }
  stop_displaying_sword_stars();
}

/**
 * @brief Stops displaying the sword stars (if any).
 */
void HeroSprites::stop_displaying_sword_stars() {

  if (is_sword_stars_visible()) {
    sword_stars_sprite->stop_animation();
  }
}

/**
 * @brief Stops displaying the shield (if any).
 */
void HeroSprites::stop_displaying_shield() {

  if (equipment.has_ability("shield")) {
    shield_sprite->stop_animation();
  }
}

/**
 * @brief Stops displaying the trail (if any).
 */
void HeroSprites::stop_displaying_trail() {
  trail_sprite->stop_animation();
}


/**
 * @brief Makes the hero blink for a while.
 */
void HeroSprites::blink() {
  tunic_sprite->set_blinking(50);

  if (equipment.has_ability("shield")) {
    shield_sprite->set_blinking(50);
  }
  if (equipment.has_ability("sword")) {
    sword_sprite->set_blinking(50);
  }
  trail_sprite->set_blinking(50);

  end_blink_date = System::now() + 2000;
}

/**
 * @brief Stops making the hero's sprites blink.
 */
void HeroSprites::stop_blinking() {

  tunic_sprite->set_blinking(0);

  if (equipment.has_ability("shield")) {
    shield_sprite->set_blinking(0);
  }
  if (equipment.has_ability("sword")) {
    sword_sprite->set_blinking(0);
  }
  trail_sprite->set_blinking(0);

  end_blink_date = 0;
}

/**
 * @brief Returns whether the hero's sprites are currently blinking.
 * @return true if the hero's sprites are currently blinking
 */
bool HeroSprites::is_blinking() {
  return tunic_sprite->is_blinking();
}

/**
 * @brief Sets a rectangle of the map where the display of the hero's sprite will be restricted to.
 *
 * A (0,0,0,0) rectangle means that the sprite is not restricted to a subarea of the map.
 *
 * @param clipping_rectangle a subarea of the map to restrict the display to
 */
void HeroSprites::set_clipping_rectangle(const Rectangle &clipping_rectangle) {
  this->clipping_rectangle = clipping_rectangle;
}

/**
 * @brief Returns whether the sprites have currently a walking animation.
 * @return true if the sprites are walking
 */
bool HeroSprites::is_walking() {
  return walking;
}

/**
 * @brief Returns the direction of the hero's sprites.
 *
 * It is different from the movement direction.
 *
 * @return the direction of the sprites (0 to 3)
 */
int HeroSprites::get_animation_direction() {
  return tunic_sprite->get_current_direction();
}

/**
 * @brief Returns the direction of the hero's sprites.
 *
 * It is different from the movement direction.
 *
 * @return the direction of the sprites (0 to 7, except diagonal directions)
 */
int HeroSprites::get_animation_direction8() {
  return get_animation_direction() * 2;
}

/**
 * @brief Returns the direction that the hero's sprites should take.
 *
 * This function returns the direction that the hero's sprites should take depending on the direction wanted
 * by the player and the real movement direction.
 * For diagonal directions, the direction returned depends
 * on the current real direction of the hero's sprites.
 *
 * @param keys_direction the direction defined by the combination of directional keys pressed (0 to 7),
 * or -1 if this is not a valid direction
 * @param real_movement_direction the direction of the hero's actual movement (may be different from keys_direction)
 * @return the direction of the sprites corresponding to these arrows (0 to 3),
 * or -1 if the directional keys combination is illegal
 */
int HeroSprites::get_animation_direction(int keys_direction, int real_movement_direction) {

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
 * @brief Changes the direction of the hero's sprites.
 *
 * It is different from the movement direction.
 *
 * @param direction the direction to set (0 to 3)
 */
void HeroSprites::set_animation_direction(int direction) {

  Debug::check_assertion(direction >= 0 && direction < 4,
    StringConcat() << "Invalid direction for set_animation_direction: " << direction);

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
 * @brief Changes the direction of the hero's sprites.
 *
 * The direction specified is one of the 8 possible movement directions of the hero.
 * The hero's sprites only have four directions, so when 
 * the specified direction is a diagonal one, 
 * one of the two closest main directions is picked.
 *
 * @param direction the movement direction (0 to 7)
 */
void HeroSprites::set_animation_direction8(int direction) {

  if (get_animation_direction() != animation_directions[direction][1]) {
    set_animation_direction(animation_directions[direction][0]);
  }
}

/**
 * @brief Returns whether the sprites animations are finished.
 * @return true if the animation is finished
 */
bool HeroSprites::is_animation_finished() {
  return tunic_sprite->is_animation_finished();
}

/**
 * @brief Returns the current frame of the tunic sprite.
 * @return the current frame
 */
int HeroSprites::get_current_frame() {
  return tunic_sprite->get_current_frame();
}

/**
 * @brief Saves the current direction of the the sprite.
 *
 * Call restore_animation_direction() to restore the direction saved here.
 */
void HeroSprites::save_animation_direction() {
  this->animation_direction_saved = get_animation_direction();
}

/**
 * @brief Restores the direction of the hero's sprite saved by the last
 * call to save_animation_direction().
 */
void HeroSprites::restore_animation_direction() {
  set_animation_direction(animation_direction_saved);
}

/**
 * @brief Updates the animation of the hero's sprites if necessary.
 */
void HeroSprites::update() {

  // update the frames
  tunic_sprite->update();

  if (is_sword_visible()) {
    sword_sprite->update();
    sword_sprite->set_current_frame(tunic_sprite->get_current_frame());
    hero.get_map().check_collision_with_detectors(hero, *sword_sprite);
  }
  hero.get_map().check_collision_with_detectors(hero, *tunic_sprite);

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

  if (hero.is_ground_visible()) {
    ground_sprite->update();
  }

  if (lifted_item != NULL && walking) {
    lifted_item->get_sprite().set_current_frame(tunic_sprite->get_current_frame() % 3);
  }

  // blinking
  if (tunic_sprite->is_blinking() && System::now() >= end_blink_date) {
    stop_blinking();
  }
}

/**
 * @brief Draws the hero's sprites on the map.
 */
void HeroSprites::display_on_map() {

  int x = hero.get_x();
  int y = hero.get_y();

  Map &map = hero.get_map();

  if (clipping_rectangle.get_width() > 0) {
    // restrict the map displaying to the clipping rectangle specified (just for the hero's sprites)
    map.set_clipping_rectangle(clipping_rectangle);
  }

  if (hero.is_shadow_visible()) {
    map.display_sprite(*shadow_sprite, x, y);
  }

  const Rectangle &displayed_xy = hero.get_displayed_xy();
  x = displayed_xy.get_x();
  y = displayed_xy.get_y();

  map.display_sprite(*tunic_sprite, x, y);

  if (is_sword_visible()) {
    map.display_sprite(*sword_sprite, x, y);
  }

  if (is_sword_stars_visible()) {
    map.display_sprite(*sword_stars_sprite, x, y);
  }

  if (is_shield_visible()) {
    map.display_sprite(*shield_sprite, x, y);
  }

  if (is_trail_visible()) {
    map.display_sprite(*trail_sprite, x, y);
  }

  if (hero.is_ground_visible()) {
    map.display_sprite(*ground_sprite, x, y);
  }

  if (lifted_item != NULL) {
    lifted_item->display_on_map();
  }
  
  if (clipping_rectangle.get_width() > 0) {
    // restore the normal map displaying
    map.set_clipping_rectangle();
  }
}

/**
 * @brief Suspends or resumes the animation of the hero's sprites.
 *
 * This function is called by the map when the game is suspended or resumed.
 *
 * @param suspended true to suspend the hero's sprites, false to resume them
 */
void HeroSprites::set_suspended(bool suspended) {

  tunic_sprite->set_suspended(suspended);

  if (equipment.has_ability("sword") && sword_sprite != NULL) {
    sword_sprite->set_suspended(suspended);
    sword_stars_sprite->set_suspended(suspended);
  }

  if (equipment.has_ability("shield") && shield_sprite != NULL) {
    shield_sprite->set_suspended(suspended);
  }

  trail_sprite->set_suspended(suspended);

  // timer
  uint32_t now = System::now();
  if (suspended) {
    when_suspended = now;
  }
  else if (end_blink_date != 0) {
    end_blink_date += now - when_suspended;
  }
}

/**
 * @brief Restarts the animation of the hero's sprites.
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

  if (hero.is_ground_visible()) {
    ground_sprite->restart_animation();
  }
}

/**
 * @brief This function is called when the sprites take a "stopped" animation.
 *
 * It makes instructions common to all states having a "stopped" animation.
 * (e.g. free or carrying).
 */
void HeroSprites::set_animation_stopped_common() {

  if (hero.is_ground_visible() && hero.get_ground() != GROUND_SHALLOW_WATER) {
    ground_sprite->set_current_animation("stopped");
  }
  walking = false;
}

/**
 * @brief Starts the basic "stopped" animation of the hero's sprites.
 */
void HeroSprites::set_animation_stopped_normal() {

  set_animation_stopped_common();

  if (equipment.has_ability("shield")) {

    tunic_sprite->set_current_animation("stopped_with_shield");
    shield_sprite->set_current_animation("stopped");
    shield_sprite->set_current_direction(get_animation_direction());
  }
  else {
    tunic_sprite->set_current_animation("stopped");
  }
  stop_displaying_sword();
  stop_displaying_trail();
}

/**
 * @brief Starts the "stopped" animation of the hero's sprites with the sword loading.
 */
void HeroSprites::set_animation_stopped_sword_loading() {

  set_animation_stopped_common();

  int direction = get_animation_direction();

  tunic_sprite->set_current_animation("sword_loading_stopped");
  sword_sprite->set_current_animation("sword_loading_stopped");
  sword_sprite->set_current_direction(direction);
  sword_stars_sprite->set_current_animation("loading");
  sword_stars_sprite->set_current_direction(direction);

  if (equipment.has_ability("shield")) {

    if (direction % 2 != 0) {
      shield_sprite->set_current_animation("sword_loading_stopped");
      shield_sprite->set_current_direction(direction / 2);
    }
    else {
      stop_displaying_shield();
    }
  }
  stop_displaying_trail();
}

/**
 * @brief Starts the "stopped" animation with sprites that represent
 * the hero carrying something.
 *
 * If the hero actually carries an item, the carried item also takes a "stopped" animation.
 */
void HeroSprites::set_animation_stopped_carrying() {

  set_animation_stopped_common();
  tunic_sprite->set_current_animation("carrying_stopped");

  if (lifted_item != NULL) {
    lifted_item->set_animation_stopped();
  }
  stop_displaying_trail();
}

/**
 * @brief This function is called when the sprites take a "walking" animation.
 *
 * It makes instructions common to all states having a "walking" animation.
 * (e.g. free or carrying).
 */
void HeroSprites::set_animation_walking_common() {

  if (hero.is_ground_visible() && hero.get_ground() != GROUND_SHALLOW_WATER) {
    ground_sprite->set_current_animation("walking");
  }

  walking = true;
}

/**
 * @brief Starts the normal "walking" animation of the hero's sprites.
 */
void HeroSprites::set_animation_walking_normal() {

  set_animation_walking_common();

  if (equipment.has_ability("shield")) {

    tunic_sprite->set_current_animation("walking_with_shield");

    shield_sprite->set_current_animation("walking");
    shield_sprite->set_current_direction(get_animation_direction());
  }
  else {
    tunic_sprite->set_current_animation("walking");
  }
  stop_displaying_sword();
  stop_displaying_trail();
}

/**
 * @brief Starts the "walking" animation of the hero's sprites with the sword loading.
 */
void HeroSprites::set_animation_walking_sword_loading() {

  set_animation_walking_common();

  int direction = get_animation_direction();

  tunic_sprite->set_current_animation("sword_loading_walking");
  if (equipment.has_ability("sword")) {
    sword_sprite->set_current_animation("sword_loading_walking");
    sword_sprite->set_current_direction(direction);
    sword_stars_sprite->set_current_animation("loading");
    sword_stars_sprite->set_current_direction(direction);
  }

  if (equipment.has_ability("shield")) {

    if (direction % 2 != 0) {
      shield_sprite->set_current_animation("sword_loading_walking");
      shield_sprite->set_current_direction(direction / 2);
    }
    else {
      stop_displaying_shield();
    }
  }
  stop_displaying_trail();
}

/**
 * @brief Starts the "walking" animation with sprites that represent
 * the hero carrying something.
 *
 * If the hero actually carries an item, the carried item also takes a "walking" animation.
 */
void HeroSprites::set_animation_walking_carrying() {

  set_animation_walking_common();

  tunic_sprite->set_current_animation("carrying_walking");
  if (lifted_item != NULL) {
    lifted_item->set_animation_walking();
  }
  stop_displaying_trail();
}

/**
 * @brief Starts the "walking_diagonal" animation of the hero's sprites.
 * @param direction8 the diagonal direction to take (1, 3, 5 or 7)
 */
void HeroSprites::set_animation_walking_diagonal(int direction8) {

  stop_displaying_sword();
  stop_displaying_shield();
  stop_displaying_trail();
  tunic_sprite->set_current_animation("walking_diagonal");
  tunic_sprite->set_current_direction(direction8 / 2);
}

/**
 * @brief Starts (or restarts) the "sword" animation of the hero's sprites.
 */
void HeroSprites::set_animation_sword() {

  int direction = get_animation_direction();

  tunic_sprite->set_current_animation("sword");
  tunic_sprite->restart_animation();

  sword_sprite->set_current_animation("sword");
  sword_sprite->set_current_direction(direction);
  sword_sprite->restart_animation();
  sword_stars_sprite->stop_animation();

  if (equipment.has_ability("shield")) {

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
 * @brief Plays the sound corresponding to the current sword.
 */
void HeroSprites::play_sword_sound() {
  Sound::play(sword_sound_id);
}

/**
 * @brief Starts (or restarts) the "sword tapping" animation of the hero's sprites.
 */
void HeroSprites::set_animation_sword_tapping() {

  int direction = get_animation_direction();

  tunic_sprite->set_current_animation("sword_tapping");
  tunic_sprite->restart_animation();

  sword_sprite->set_current_animation("sword_tapping");
  sword_sprite->set_current_direction(direction);
  sword_sprite->restart_animation();
  sword_stars_sprite->stop_animation();

  if (equipment.has_ability("shield")) {

    if (direction % 2 != 0) {
      shield_sprite->set_current_animation("sword_tapping");
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
 * @brief Starts (or restarts) the "spin_attck" animation of the hero's sprites.
 */
void HeroSprites::set_animation_spin_attack() {

  tunic_sprite->set_current_animation("spin_attack");
  sword_sprite->set_current_animation("spin_attack");
  stop_displaying_sword_stars();
  stop_displaying_shield();
  stop_displaying_trail();
}

/**
 * @brief Starts the "grabbing" animation of the hero's sprites.
 */
void HeroSprites::set_animation_grabbing() {

  tunic_sprite->set_current_animation("grabbing");
  stop_displaying_shield();
  stop_displaying_trail();
}

/**
 * @brief Starts the "pulling" animation of the hero's sprites.
 */
void HeroSprites::set_animation_pulling() {

  tunic_sprite->set_current_animation("pulling");
  stop_displaying_shield();
  stop_displaying_trail();
}

/**
 * @brief Starts the "pushing" animation of the hero's sprites.
 */
void HeroSprites::set_animation_pushing() {

  tunic_sprite->set_current_animation("pushing");
  stop_displaying_shield();
  stop_displaying_trail();
}

/**
 * @brief Starts the "lifting" animation of the hero's sprites.
 */
void HeroSprites::set_animation_lifting() {

  tunic_sprite->set_current_animation("lifting");
  stop_displaying_shield();
  stop_displaying_trail();
}

/**
 * @brief Starts the "jumping" animation of the hero's sprites.
 */
void HeroSprites::set_animation_jumping() {

  tunic_sprite->set_current_animation("jumping");

  if (equipment.has_ability("shield")) {
    shield_sprite->set_current_animation("stopped");
    shield_sprite->set_current_direction(get_animation_direction());
  }
  stop_displaying_sword();
  stop_displaying_trail();
}

/**
 * @brief Starts the "hurt" animation of the hero's sprites.
 */
void HeroSprites::set_animation_hurt() {

  tunic_sprite->set_current_animation("hurt");
  stop_displaying_sword();
  stop_displaying_shield();
  stop_displaying_trail();
}

/**
 * @brief Starts the "plunging" animation of the hero's sprites.
 */
void HeroSprites::set_animation_plunging() {

  tunic_sprite->set_current_animation("plunging");
  stop_displaying_sword();
  stop_displaying_shield();
  stop_displaying_trail();
}

/**
 * @brief Starts the "falling" animation of the hero's sprites.
 */
void HeroSprites::set_animation_falling() {

  // show the animation
  tunic_sprite->set_current_animation("falling");
  stop_displaying_sword();
  stop_displaying_shield();
  stop_displaying_trail();
}

/**
 * @brief Starts the "boomerang" animation of the hero's sprites.
 */
void HeroSprites::set_animation_boomerang() {
  tunic_sprite->set_current_animation("boomerang");

  if (equipment.has_ability("shield")) {
    shield_sprite->set_current_animation("boomerang");
  }
  stop_displaying_trail();
}

/**
 * @brief Starts the "bow" animation of the hero's sprites.
 */
void HeroSprites::set_animation_bow() {

  tunic_sprite->set_current_animation("bow");
  stop_displaying_sword();
  stop_displaying_shield();
  stop_displaying_trail();
}


/**
 * @brief Starts the "brandish" animation of the hero's sprites.
 */
void HeroSprites::set_animation_brandish() {

  tunic_sprite->set_current_animation("brandish");
  tunic_sprite->set_current_direction(0);
  stop_displaying_sword();
  stop_displaying_shield();
  stop_displaying_trail();
}

/**
 * @brief Starts the "victory" animation of the hero's sprites.
 */
void HeroSprites::set_animation_victory() {

  tunic_sprite->set_current_animation("victory");
  tunic_sprite->set_current_direction(0);
  sword_sprite->set_current_animation("victory");
  sword_sprite->set_current_direction(0);
  stop_displaying_sword_stars();
  stop_displaying_shield();
  stop_displaying_trail();
}

/**
 * @brief Starts the "prepare running" animation of the hero's sprites.
 */
void HeroSprites::set_animation_prepare_running() {

  set_animation_walking_normal();
  trail_sprite->set_current_animation("running");
}

/**
 * @brief Starts the "running" animation of the hero's sprites.
 */
void HeroSprites::set_animation_running() {

  set_animation_walking_sword_loading();
  trail_sprite->set_current_animation("running");
}

/**
 * @brief Creates the ground sprite and sound corresponding to the specified ground.
 * @param ground a ground
 */
void HeroSprites::create_ground(Ground ground) {
 
  ground_sprite = new Sprite(ground_sprite_ids[ground - 1]);
  if (hero.get_ground() != GROUND_SHALLOW_WATER) {
    ground_sprite->set_current_animation(walking ? "walking" : "stopped");
  }
  ground_sound_id = ground_sound_ids[ground - 1];
}

/**
 * @brief Deletes the ground sprite.
 */
void HeroSprites::destroy_ground() {

  delete ground_sprite;
  ground_sprite = NULL;
}

/**
 * @brief Plays a sound for the ground displayed under the hero.
 */
void HeroSprites::play_ground_sound() {
  Sound::play(ground_sound_id);
}

/**
 * @brief Indicates a lifted item to display with the hero's sprites.
 *
 * Calling this function makes this class display the lifted item and
 * give it the appropriate animation, so that you don't have to make it yourself.
 * However, this class only handles displaying:
 * you still have to update it and make it follow the hero.
 *
 * @param lifted_item the item to display, or NULL to stop displaying a lifted item
 */
void HeroSprites::set_lifted_item(CarriedItem *lifted_item) {
  this->lifted_item = lifted_item;
}

