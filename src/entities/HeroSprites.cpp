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
#include "entities/HeroSprites.h"
#include "entities/Hero.h"
#include "entities/CarriedItem.h"
#include "Sprite.h"
#include "SpriteAnimationSet.h"
#include "Equipment.h"
#include "Map.h"
#include "ResourceManager.h"
#include "lowlevel/Sound.h"
#include "lowlevel/System.h"

/**
 * Indicates the possible directions of the hero's animation (from 0 to 3, or -1 for no change)
 * for movement direction.
 * Each combination of directional keys can be associated to one or two directions.
 * When one direction is specified (i.e. the second one is set to -1), the hero's sprite takes
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
 * String constants corresponding to the sprites of the tunics.
 */
const SpriteAnimationSetId HeroSprites::tunic_sprite_ids[] = {
  "hero/tunic0", // green tunic
  "hero/tunic1", // blue tunic
  "hero/tunic2", // red tunic
};

/**
 * String constants corresponding to the sprites of the swords.
 */
const SpriteAnimationSetId HeroSprites::sword_sprite_ids[] = {
  "hero/sword1",
  "hero/sword2",
  "hero/sword3",
  "hero/sword4",
};

/**
 * String constants corresponding to the sprites of the stars of the swords.
 */
const SpriteAnimationSetId HeroSprites::sword_stars_sprite_ids[] = {
  "hero/sword_stars1",
  "hero/sword_stars1",
  "hero/sword_stars2",
  "hero/sword_stars2",
};

/**
 * String constants corresponding to the sprites of the shields.
 */
const SpriteAnimationSetId HeroSprites::shield_sprite_ids[] = {
  "hero/shield1",
  "hero/shield2",
  "hero/shield3",
};

/**
 * String constants corresponding to the sprites of the ground displayed under the hero.
 */
const SpriteAnimationSetId HeroSprites::ground_sprite_ids[] = {
  "hero/ground1",
  "hero/ground2",
};

/**
 * String constants corresponding to the sounds of the swords.
 */
const SoundId HeroSprites::sword_sound_ids[] = {
  "sword1",
  "sword2",
  "sword3",
  "sword4",
};

/**
 * String constants corresponding to the sounds of the ground under the hero.
 */
const SoundId HeroSprites::ground_sound_ids[] = {
  "walk_on_grass",
  "walk_on_water",
};

/**
 * Constructor.
 * @param hero the hero
 * @param equipment the equipment
 */
HeroSprites::HeroSprites(Hero *hero, Equipment *equipment):
  hero(hero), equipment(equipment), tunic_sprite(NULL), sword_sprite(NULL),
  sword_stars_sprite(NULL), shield_sprite(NULL), shadow_sprite(NULL), ground_sprite(NULL),
  end_blink_date(0), walking(false) {

}

/**
 * Destructor.
 */
HeroSprites::~HeroSprites(void) {
  delete tunic_sprite;
  delete shadow_sprite;
  delete sword_sprite;
  delete sword_stars_sprite;
  delete shield_sprite;
  delete ground_sprite;
}

/**
 * Loads (or reloads) the sprites and sounds of the hero and his equipment,
 * depending on its tunic, sword and shield as specified in the savegame.
 * This function must be called at the game beginning
 * and as soon as the hero's equipment is changed.
 */
void HeroSprites::rebuild_equipment(void) {

  int animation_direction = -1;

  // the hero
  if (tunic_sprite != NULL) {
    // save the animation direction
    animation_direction = tunic_sprite->get_current_direction();
    delete tunic_sprite;
  }

  int tunic_number = equipment->get_tunic();

  tunic_sprite = new Sprite(tunic_sprite_ids[tunic_number]);
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

  int sword_number = equipment->get_sword();

  if (sword_number > 0) {
    // the hero has a sword: get the sprite and the sound
    sword_sprite = new Sprite(sword_sprite_ids[sword_number - 1]);
    sword_sprite->stop_animation();
    sword_sprite->enable_pixel_collisions();

    sword_sound = ResourceManager::get_sound(sword_sound_ids[sword_number - 1]);

    sword_stars_sprite = new Sprite(sword_stars_sprite_ids[sword_number - 1]);
    sword_stars_sprite->stop_animation();
  }

  // the hero's shield
  if (shield_sprite != NULL) {
    delete shield_sprite;
    shield_sprite = NULL;
  }

  int shield_number = equipment->get_shield();

  if (shield_number > 0) {
    // the hero has a shield
    shield_sprite = new Sprite(shield_sprite_ids[shield_number - 1]);
  }

  // restore the animation direction
  if (animation_direction != -1) {
    set_animation_direction(animation_direction);
  }
}

/**
 * Returns whether the hero is currently visible.
 * Note that this function is used only for specific purposes of class HeroSprites and has
 * nothing to do with MapEntity::is_visible().
 * @return true if the hero is currently visible
 */
bool HeroSprites::is_visible(void) {
  return hero->is_visible() && hero->get_state() != Hero::RETURNING_TO_SOLID_GROUND;
}

/**
 * Returns whether the sword is currently displayed on the screen.
 * @return true if the sword is currently displayed on the screen
 */
bool HeroSprites::is_sword_visible(void) {
  return is_visible() && equipment->has_sword() && sword_sprite != NULL && sword_sprite->is_animation_started();
}

/**
 * Returns whether the stars of the sword are currently displayed on the screen.
 * @return true if the stars of the sword are currently displayed on the screen
 */
bool HeroSprites::is_sword_stars_visible(void) {
  return equipment->has_sword() && sword_stars_sprite != NULL && sword_stars_sprite->is_animation_started();
}

/**
 * Returns whether the shield is currently displayed on the screen.
 * @return true if the shield is currently displayed on the screen
 */
bool HeroSprites::is_shield_visible(void) {
  return equipment->has_shield() && shield_sprite != NULL && shield_sprite->is_animation_started();
}

/**
 * Returns whether the shadow is currently displayed, separated from the tunic sprite.
 * @return true if the shadow is currently displayed, separated from the tunic sprite
 */
bool HeroSprites::is_shadow_visible(void) {
  return hero->get_state() == Hero::JUMPING;
}

/**
 * Stops displaying the sword (if any).
 */
void HeroSprites::stop_displaying_sword(void) {

  if (is_sword_visible()) {
    sword_sprite->stop_animation();
    sword_stars_sprite->stop_animation();
  }
}

/**
 * Stops displaying the shield (if any).
 */
void HeroSprites::stop_displaying_shield(void) {
  shield_sprite->stop_animation();
}

/**
 * Makes the hero blink for a while.
 */
void HeroSprites::blink(void) {
  tunic_sprite->set_blinking(50);

  if (equipment->has_shield()) {
    shield_sprite->set_blinking(50);
  }
  if (equipment->has_sword()) {
    sword_sprite->set_blinking(50);
  }

  end_blink_date = System::now() + 2000;
}

/**
 * Stops making the hero's sprites blink.
 */
void HeroSprites::stop_blinking() {

  tunic_sprite->set_blinking(0);

  if (equipment->has_shield()) {
    shield_sprite->set_blinking(0);
  }
  if (equipment->has_sword()) {
    sword_sprite->set_blinking(0);
  }

  end_blink_date = 0;
}

/**
 * Returns whether the hero's sprites are currently blinking.
 * @return true if the hero's sprites are currently blinking
 */
bool HeroSprites::is_blinking(void) {
  return tunic_sprite->is_blinking();
}

/**
 * Returns whether the sprites have currently a walking animation.
 * @return true if the sprites are walking
 */
bool HeroSprites::is_walking(void) {
  return walking;
}

/**
 * Returns the direction of the hero's sprites.
 * It is different from the movement direction.
 * @return the direction of the sprites (0 to 3)
 */
int HeroSprites::get_animation_direction(void) {
  return tunic_sprite->get_current_direction();
}

/**
 * Returns the direction the hero's sprite should take depending on the direction wanted
 * by the player and the real movement direction.
 * For diagonal directions, the direction returned depends
 * on the current real direction of the hero's sprites.
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
 * Changes the direction of the hero's sprites.
 * It is different from the movement direction.
 * @param direction the direction to set (0 to 3)
 */
void HeroSprites::set_animation_direction(int direction) {

  if (direction < 0 || direction >= 4) {
    DIE("Invalid direction for set_animation_direction: " << direction);
  }

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
}

/**
 * Changes the direction of the hero's sprites,
 * specifying one of the 8 possible movement directions
 * of the hero.
 * The hero's sprites only have four directions, so when 
 * the specified direction is a diagonal one, 
 * one of the two closest main directions is picked.
 * @param direction the movement direction (0 to 7)
 */
void HeroSprites::set_animation_direction8(int direction) {

  if (get_animation_direction() != animation_directions[direction][1]) {
    set_animation_direction(animation_directions[direction][0]);
  }
}

/**
 * Returns whether the sprites animations are finished.
 * @return true if the animation is finished
 */
bool HeroSprites::is_animation_finished(void) {
  return tunic_sprite->is_animation_finished();
}

/**
 * Returns the current frame of the tunic sprite.
 * @return the current frame
 */
int HeroSprites::get_current_frame(void) {
  return tunic_sprite->get_current_frame();
}

/**
 * Saves the current direction of the the sprite.
 * Call restore_animation_direction() to restore the direction saved here.
 */
void HeroSprites::save_animation_direction(void) {
  this->animation_direction_saved = get_animation_direction();
}

/**
 * Restores the direction of the hero's sprite saved by the last
 * save_animation_direction() call.
 */
void HeroSprites::restore_animation_direction(void) {
  set_animation_direction(animation_direction_saved);
}

/**
 * Updates the animation of the hero's sprites if necessary.
 */
void HeroSprites::update(void) {

  // update the frames
  tunic_sprite->update();

  if (is_sword_visible()) {
    sword_sprite->update();
    sword_sprite->set_current_frame(tunic_sprite->get_current_frame());
    hero->get_map()->check_collision_with_detectors(hero, sword_sprite);
  }
  hero->get_map()->check_collision_with_detectors(hero, tunic_sprite);

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

  if (hero->get_state() == Hero::CARRYING && walking) {
    CarriedItem *lifted_item = hero->get_lifted_item();
    lifted_item->get_sprite()->set_current_frame(tunic_sprite->get_current_frame() % 3);
  }

  if (hero->is_ground_visible()) {
    ground_sprite->update();
  }

  // blinking
  if (tunic_sprite->is_blinking() && System::now() >= end_blink_date) {
    stop_blinking();
  }
}

/**
 * Draws the hero's sprites on the map.
 */
void HeroSprites::display_on_map(void) {

  int x = hero->get_x();
  int y = hero->get_y();

  Map *map = hero->get_map();
  if (hero->get_state() == Hero::JUMPING) {
    map->display_sprite(shadow_sprite, x, y);
    map->display_sprite(tunic_sprite, x, hero->get_jump_y());
    if (equipment->has_shield()) {
      map->display_sprite(shield_sprite, x, hero->get_jump_y());
    }
  }
  else {
    map->display_sprite(tunic_sprite, x, y);

    if (is_sword_visible()) {
      map->display_sprite(sword_sprite, x, y);
    }

    if (is_sword_stars_visible()) {
      map->display_sprite(sword_stars_sprite, x, y);
    }

    if (is_shield_visible()) {
      map->display_sprite(shield_sprite, x, y);
    }

    if (hero->is_ground_visible()) {
      map->display_sprite(ground_sprite, x, y);
    }
  }
}

/**
 * Suspends or resumes the animation of the hero's sprites.
 * This function is called by the map when the game is suspended or resumed.
 * @param suspended true to suspend the hero's sprites, false to resume them
 */
void HeroSprites::set_suspended(bool suspended) {

  tunic_sprite->set_suspended(suspended);

  if (equipment->has_sword() && sword_sprite != NULL) {
    sword_sprite->set_suspended(suspended);
    sword_stars_sprite->set_suspended(suspended);
  }

  if (equipment->has_shield() && shield_sprite != NULL) {
    shield_sprite->set_suspended(suspended);
  }

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
 * Restarts the animation of the hero's sprites.
 * This function is called when the sprites have to
 * get back to their first frame.
 */
void HeroSprites::restart_animation(void) {
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

  if (hero->is_ground_visible()) {
    ground_sprite->restart_animation();
  }
}

/**
 * Starts the "stopped" animation of the hero's sprites.
 */
void HeroSprites::set_animation_stopped(void) {

  int direction = tunic_sprite->get_current_direction();

  switch (hero->get_state()) {

   case Hero::SWORD_LOADING:

    tunic_sprite->set_current_animation("sword_loading_stopped");

    sword_sprite->set_current_animation("sword_loading_stopped");
    sword_sprite->set_current_direction(direction);

    sword_stars_sprite->set_current_animation("loading");
    sword_stars_sprite->set_current_direction(direction);

    if (equipment->has_shield()) {

      if (direction % 2 != 0) {
	shield_sprite->set_current_animation("sword_loading_stopped");
	shield_sprite->set_current_direction(direction / 2);
      }
      else {
	shield_sprite->stop_animation();
      }
    }

    break;

  case Hero::CARRYING:
    {
      tunic_sprite->set_current_animation("carrying_stopped");
      CarriedItem *lifted_item = hero->get_lifted_item();
      lifted_item->set_animation_stopped();
      break;
    }

  default:
    stop_displaying_sword();

    if (equipment->has_shield()) {

      tunic_sprite->set_current_animation("stopped_with_shield");

      shield_sprite->set_current_animation("stopped");
      shield_sprite->set_current_direction(direction);
    }
    else {
      tunic_sprite->set_current_animation("stopped");
    }

    break;
  }

  if (hero->is_ground_visible() && hero->get_ground() != GROUND_SHALLOW_WATER) {
    ground_sprite->set_current_animation("stopped");
  }

  walking = false;
}

/**
 * Starts the "walking" animation of the hero's sprites.
 */
void HeroSprites::set_animation_walking(void) {

  int direction = tunic_sprite->get_current_direction();

  switch (hero->get_state()) {

   case Hero::FREE:

    stop_displaying_sword();

    if (equipment->has_shield()) {

      tunic_sprite->set_current_animation("walking_with_shield");

      shield_sprite->set_current_animation("walking");
      shield_sprite->set_current_direction(direction);
    }
    else {
      tunic_sprite->set_current_animation("walking");
    }

    break;

  case Hero::SWORD_LOADING:

    tunic_sprite->set_current_animation("sword_loading_walking");

    sword_sprite->set_current_animation("sword_loading_walking");
    sword_sprite->set_current_direction(direction);

    sword_stars_sprite->set_current_animation("loading");
    sword_stars_sprite->set_current_direction(direction);

    if (equipment->has_shield()) {

      if (direction % 2 != 0) {
	shield_sprite->set_current_animation("sword_loading_walking");
	shield_sprite->set_current_direction(direction / 2);
      }
      else {
	shield_sprite->stop_animation();
      }
    }

    break;

  case Hero::CARRYING:
    {
      tunic_sprite->set_current_animation("carrying_walking");
      CarriedItem *lifted_item = hero->get_lifted_item();
      lifted_item->set_animation_walking();
      break;
    }

  default:
    break;
  }

  if (hero->is_ground_visible() && hero->get_ground() != GROUND_SHALLOW_WATER) {
    ground_sprite->set_current_animation("walking");
  }

  walking = true;
}

/**
 * Starts (or restarts) the "sword" animation of the hero's sprites.
 * The state of the hero should be SWORD.
 */
void HeroSprites::set_animation_sword(void) {

  int direction = tunic_sprite->get_current_direction();

  tunic_sprite->set_current_animation("sword");
  tunic_sprite->restart_animation();

  sword_sprite->set_current_animation("sword");
  sword_sprite->set_current_direction(direction);
  sword_sprite->restart_animation();
  sword_stars_sprite->stop_animation();

  if (equipment->has_shield()) {

    if (direction % 2 != 0) {
      shield_sprite->set_current_animation("sword");
      shield_sprite->set_current_direction(direction / 2);
      shield_sprite->restart_animation();
    }
    else {
      shield_sprite->stop_animation();
    }
  }
}

/**
 * Plays the sound corresponding to the current sword.
 */
void HeroSprites::play_sword_sound(void) {
  sword_sound->play();
}

/**
 * Starts (or restarts) the "sword tapping" animation of the hero's sprites.
 * The state of the hero should be SWORD_TAPPING.
 */
void HeroSprites::set_animation_sword_tapping(void) {

  int direction = tunic_sprite->get_current_direction();

  tunic_sprite->set_current_animation("sword_tapping");
  tunic_sprite->restart_animation();

  sword_sprite->set_current_animation("sword_tapping");
  sword_sprite->set_current_direction(direction);
  sword_sprite->restart_animation();
  sword_stars_sprite->stop_animation();

  if (equipment->has_shield()) {

    if (direction % 2 != 0) {
      shield_sprite->set_current_animation("sword_tapping");
      shield_sprite->set_current_direction(direction / 2);
      shield_sprite->restart_animation();
    }
    else {
      shield_sprite->stop_animation();
    }
  }
}

/**
 * Starts (or restarts) the "spin_attck" animation of the hero's sprites.
 * The state of the hero should be SPIN_ATTACK.
 */
void HeroSprites::set_animation_spin_attack(void) {

  tunic_sprite->set_current_animation("spin_attack");
  sword_sprite->set_current_animation("spin_attack");
  sword_stars_sprite->stop_animation();

  if (equipment->has_shield()) {
    shield_sprite->stop_animation(); // the shield is not visible during a spin attack
  }
}

/**
 * Starts the "grabbing" animation of the hero's sprites.
 * The hero's state should be GRABBING.
 */
void HeroSprites::set_animation_grabbing(void) {
  tunic_sprite->set_current_animation("grabbing");

  // the shield is not visible when the hero is grabbing an object
  if (equipment->has_shield()) {
    shield_sprite->stop_animation();
  }
}

/**
 * Starts the "pulling" animation of the hero's sprites.
 * The hero's state should be PULLING.
 */
void HeroSprites::set_animation_pulling(void) {
  tunic_sprite->set_current_animation("pulling");

  // the shield is not visible when the hero is pulling an object
  if (equipment->has_shield()) {
    shield_sprite->stop_animation();
  }
}

/**
 * Starts the "pushing" animation of the hero's sprites.
 * The hero's state should be PUSHING.
 */
void HeroSprites::set_animation_pushing(void) {
  tunic_sprite->set_current_animation("pushing");

  // the shield is not visible when the hero is pushing
  if (equipment->has_shield()) {
    shield_sprite->stop_animation();
  }
}

/**
 * Starts the "lifting" animation of the hero's sprites.
 * The hero's state should be LIFTING.
 */
void HeroSprites::set_animation_lifting(void) {
  tunic_sprite->set_current_animation("lifting");

  // the shield is not visible when the hero is lifting
  if (equipment->has_shield()) {
    shield_sprite->stop_animation();
  }
}

/**
 * Starts the "jumping" animation of the hero's sprites.
 * The hero's state should be JUMPING.
 */
void HeroSprites::set_animation_jumping(void) {
  tunic_sprite->set_current_animation("jumping");

  if (equipment->has_shield()) {
    shield_sprite->set_current_animation("stopped");
    shield_sprite->set_current_direction(get_animation_direction());
  }
}

/**
 * Starts the "hurt" animation of the hero's sprites.
 * The hero's state should be HURT.
 */
void HeroSprites::set_animation_hurt(void) {
  tunic_sprite->set_current_animation("hurt");

  // the shield is not visible when the hero is hurt
  if (equipment->has_shield()) {
    shield_sprite->stop_animation();
  }
}

/**
 * Starts the "plunging" animation of the hero's sprites.
 * The hero's state should be PLUNGING.
 */
void HeroSprites::set_animation_plunging(void) {
  tunic_sprite->set_current_animation("plunging");

  // the shield is not visible
  if (equipment->has_shield()) {
    shield_sprite->stop_animation();
  }
}

/**
 * Starts the "falling" animation of the hero's sprites.
 * The hero's state should be FALLING.
 */
void HeroSprites::set_animation_falling(void) {

  // show the animation
  tunic_sprite->set_current_animation("falling");

  // the shield and the sword are not visible
  if (equipment->has_shield()) {
    shield_sprite->stop_animation();
  }
  stop_displaying_sword();
}

/**
 * Starts the "boomerang" animation of the hero's sprites.
 * The hero's state should be USING_INVENTORY_ITEM.
 */
void HeroSprites::set_animation_boomerang(void) {
  tunic_sprite->set_current_animation("boomerang");

  if (equipment->has_shield()) {
    shield_sprite->set_current_animation("boomerang");
  }
}

/**
 * Starts the "bow" animation of the hero's sprites.
 * The hero's state should be USING_INVENTORY_ITEM.
 */
void HeroSprites::set_animation_bow(void) {

  tunic_sprite->set_current_animation("bow");

  // the shield is not visible
  if (equipment->has_shield()) {
    shield_sprite->stop_animation();
  }
  stop_displaying_sword();
}


/**
 * Starts the "brandish" animation of the hero's sprites.
 * The hero's state should be FREEZED.
 */
void HeroSprites::set_animation_brandish(void) {
  tunic_sprite->set_current_animation("brandish");
  tunic_sprite->set_current_direction(0);
}

/**
 * Starts the "victory" animation of the hero's sprites.
 * The hero's state should be VICTORY.
 */
void HeroSprites::set_animation_victory(void) {
  tunic_sprite->set_current_animation("victory");
  tunic_sprite->set_current_direction(0);
  sword_sprite->set_current_animation("victory");
  sword_sprite->set_current_direction(0);
  sword_stars_sprite->stop_animation();

  // the shield is not visible
  if (equipment->has_shield()) {
    shield_sprite->stop_animation();
  }
}

/**
 * Creates the ground sprite corresponding to the specified ground.
 */
void HeroSprites::create_ground(Ground ground) {
 
  ground_sprite = new Sprite(ground_sprite_ids[ground - 1]);
  if (hero->get_ground() != GROUND_SHALLOW_WATER) {
    ground_sprite->set_current_animation(walking ? "walking" : "stopped");
  }
  ground_sound = ResourceManager::get_sound(ground_sound_ids[ground - 1]);
}

/**
 * Deletes the ground sprite.
 */
void HeroSprites::destroy_ground(void) {
  delete ground_sprite;
  ground_sprite = NULL;
}

/**
 * Plays a sound for the ground displayed under the hero.
 */
void HeroSprites::play_ground_sound(void) {
  ground_sound->play();
}

