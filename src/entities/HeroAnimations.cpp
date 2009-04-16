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
#include "entities/Hero.h"
#include "entities/CarriedItem.h"
#include "Sprite.h"
#include "Equipment.h"
#include "Map.h"
#include "ZSDX.h"
#include "Game.h"

/**
 * String constants corresponding to the sprites of the tunics.
 */
const SpriteAnimationSetId Hero::tunic_sprite_ids[] = {
  "hero/tunic0", // green tunic
  "hero/tunic1", // blue tunic
  "hero/tunic2", // red tunic
};

/**
 * String constants corresponding to the sprites of the swords.
 */
const SpriteAnimationSetId Hero::sword_sprite_ids[] = {
  "hero/sword1",
  "hero/sword2",
  "hero/sword3",
  "hero/sword4",
};

/**
 * String constants corresponding to the sprites of the stars of the swords.
 */
const SpriteAnimationSetId Hero::sword_stars_sprite_ids[] = {
  "hero/sword_stars1",
  "hero/sword_stars1",
  "hero/sword_stars2",
  "hero/sword_stars2",
};

/**
 * String constants corresponding to the sprites of the shields.
 */
const SpriteAnimationSetId Hero::shield_sprite_ids[] = {
  "hero/shield1",
  "hero/shield2",
  "hero/shield3",
};

/**
 * String constants corresponding to the sprites of the ground displayed under the hero.
 */
const SpriteAnimationSetId Hero::ground_sprite_ids[] = {
  "hero/ground1",
  "hero/ground2",
};

/**
 * String constants corresponding to the sounds of the swords.
 */
const SoundId Hero::sword_sound_ids[] = {
  "sword1",
  "sword2",
  "sword3",
  "sword4",
};

/**
 * String constants corresponding to the sounds of the ground under the hero.
 */
const SoundId Hero::ground_sound_ids[] = {
  "walk_on_grass",
  "walk_on_water",
};

/**
 * Returns whether the hero is currently visible.
 * @return true if the hero is currently visible
 */
bool Hero::is_visible(void) {
  return !zsdx->game->is_showing_gameover() && state != RETURNING_TO_SOLID_GROUND;
}

/**
 * Returns whether the sword is currently displayed on the screen.
 * @return true if the sword is currently displayed on the screen
 */
bool Hero::is_sword_visible(void) {
  return is_visible() && equipment->has_sword() && sword_sprite != NULL && sword_sprite->is_animation_started();
}

/**
 * Returns whether the stars of the sword are currently displayed on the screen.
 * @return true if the stars of the sword are currently displayed on the screen
 */
bool Hero::is_sword_stars_visible(void) {
  return equipment->has_sword() && sword_stars_sprite != NULL && sword_stars_sprite->is_animation_started();
}

/**
 * Returns whether the shield is currently displayed on the screen.
 * @return true if the shield is currently displayed on the screen
 */
bool Hero::is_shield_visible(void) {
  return equipment->has_shield() && shield_sprite != NULL && shield_sprite->is_animation_started();
}

/**
 * Returns whether the shadow is currently displayed, separated from the tunic sprite.
 * @return true if the shadow is currently displayed, separated from the tunic sprite
 */
bool Hero::is_shadow_visible(void) {
  return get_state() == JUMPING;
}

/**
 * Stops displaying the sword (if any).
 */
void Hero::stop_displaying_sword(void) {

  if (is_sword_visible()) {
    sword_sprite->stop_animation();
    sword_stars_sprite->stop_animation();
  }
}

/**
 * Makes the hero blink for a while.
 */
void Hero::blink(void) {
  tunic_sprite->set_blinking(50);

  if (equipment->has_shield()) {
    shield_sprite->set_blinking(50);
  }
  if (equipment->has_sword()) {
    sword_sprite->set_blinking(50);
  }

  end_blink_date = SDL_GetTicks() + 2000;
}

/**
 * Stops making the hero's sprites blink.
 */
void Hero::stop_blinking() {

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
 * Returns the direction of the hero's sprites.
 * It is different from the movement direction.
 * @return the direction of the sprites (0 to 3)
 */
int Hero::get_animation_direction(void) {
  return tunic_sprite->get_current_direction();
}

/**
 * Changes the direction of the hero's sprites.
 * It is different from the movement direction.
 * @param direction the direction to set (0 to 3)
 */
void Hero::set_animation_direction(int direction) {

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
 * Saves the current direction of the the sprite.
 * Call restore_animation_direction() to restore the direction saved here.
 */
void Hero::save_animation_direction(void) {
  this->animation_direction_saved = get_animation_direction();
}

/**
 * Restores the direction of the hero's sprite saved by the last
 * save_animation_direction() call.
 */
void Hero::restore_animation_direction(void) {
  set_animation_direction(animation_direction_saved);
}

/**
 * Updates the animation of the hero's sprites if necessary.
 */
void Hero::update_sprites(void) {

  // update the frames
  tunic_sprite->update();

  if (is_sword_visible()) {
    sword_sprite->update();
    sword_sprite->set_current_frame(tunic_sprite->get_current_frame());
    map->check_collision_with_detectors(this, sword_sprite);
  }

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

  if (state == CARRYING && walking) {
    lifted_item->get_sprite()->set_current_frame(tunic_sprite->get_current_frame() % 3);
  }

  if (is_ground_visible()) {
    ground_sprite->update();
  }

  // blinking
  if (tunic_sprite->is_blinking() && SDL_GetTicks() >= end_blink_date) {
    stop_blinking();
  }
}

/**
 * Restarts the animation of the hero's sprites.
 * This function is called when the sprites have to
 * get back to their first frame.
 */
void Hero::restart_animation(void) {
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

  if (is_ground_visible()) {
    ground_sprite->restart_animation();
  }
}

/**
 * Starts the "stopped" animation of the hero's sprites.
 */
void Hero::set_animation_stopped(void) {

  int direction = tunic_sprite->get_current_direction();

  switch (get_state()) {

  case SWORD_LOADING:

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

  case CARRYING:
    tunic_sprite->set_current_animation("carrying_stopped");
    lifted_item->set_animation_stopped();
    break;

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

  if (is_ground_visible() && ground != GROUND_SHALLOW_WATER) {
    ground_sprite->set_current_animation("stopped");
  }

  walking = false;
}

/**
 * Starts the "walking" animation of the hero's sprites.
 */
void Hero::set_animation_walking(void) {

  int direction = tunic_sprite->get_current_direction();

  switch (get_state()) {

  case FREE:

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

  case SWORD_LOADING:

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

  case CARRYING:
    tunic_sprite->set_current_animation("carrying_walking");
    lifted_item->set_animation_walking();
    break;

  default:
    break;
  }

  if (is_ground_visible() && ground != GROUND_SHALLOW_WATER) {
    ground_sprite->set_current_animation("walking");
  }

  walking = true;
}

/**
 * Starts (or restarts) the "sword" animation of the hero's sprites.
 * The state of the hero should be SWORD.
 */
void Hero::set_animation_sword(void) {

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
 * Starts the "grabbing" animation of the hero's sprites.
 * The hero's state should be GRABBING.
 */
void Hero::set_animation_grabbing(void) {
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
void Hero::set_animation_pulling(void) {
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
void Hero::set_animation_pushing(void) {
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
void Hero::set_animation_lifting(void) {
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
void Hero::set_animation_jumping(void) {
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
void Hero::set_animation_hurt(void) {
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
void Hero::set_animation_plunging(void) {
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
void Hero::set_animation_falling(void) {

  // show the animation
  save_animation_direction();
  tunic_sprite->set_current_direction(0);
  tunic_sprite->set_current_animation("falling");

  // the shield and the sword are not visible
  if (equipment->has_shield()) {
    shield_sprite->stop_animation();
  }
  stop_displaying_sword();
}
