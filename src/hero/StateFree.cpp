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
#include "hero/StateFree.h"
#include "hero/StateGrabbing.h"
#include "hero/StatePushing.h"
#include "hero/HeroSprites.h"
#include "movements/PlayerMovement.h"
#include "entities/Detector.h"
#include "lowlevel/System.h"
#include "Game.h"
#include "KeysEffect.h"

/**
 * @brief Constructor.
 * @param hero the hero controlled by this state
 */
Hero::StateFree::StateFree(Hero *hero):
  StatePlayerMovement(hero) {

}

/**
 * @brief Destructor.
 */
Hero::StateFree::~StateFree(void) {

}

/**
 * @brief Starts this state.
 *
 * This function is called automatically when this state becomes the active state of the hero.
 *
 * @param previous_state the previous state of NULL if this is the first state (for information)
 */
void Hero::StateFree::start(State *previous_state) {

  StatePlayerMovement::start(previous_state);
  set_animation_stopped();

  pushing_direction_mask = 0xFFFF;
}

/**
 * @brief Returns whether the hero can walk normally and interact with entities
 * in this state.
 * @return true
 */
bool Hero::StateFree::is_free(void) {
  return true;
}

/**
 * @brief Notifies this state that the action key was just pressed.
 */
void Hero::StateFree::action_key_pressed(void) {

  KeysEffect *keys_effect = game->get_keys_effect();

  if (keys_effect->is_action_key_acting_on_facing_entity()) {

    // action on the facing entity
    hero->get_facing_entity()->action_key_pressed();
  }
  else if (hero->is_facing_point_on_obstacle()) {

    // grab an obstacle
    hero->set_state(new StateGrabbing(hero));
  }
}

/**
 * Gives the sprites the animation stopped corresponding to this state.
 */
void Hero::StateFree::set_animation_stopped(void) {
  hero->get_sprites()->set_animation_stopped_normal();
}

/**
 * Gives the sprites the animation walking corresponding to this state.
 */
void Hero::StateFree::set_animation_walking(void) {
  hero->get_sprites()->set_animation_walking_normal();
}

/**
 * @brief Returns whether the hero can swing his sword in this state.
 * @return true if the hero can swing his sword in this state
 */
bool Hero::StateFree::can_start_sword(void) {
  return true;
}

/**
 * @brief Notifies this state of the result of the current movement.
 * @param tried_to_move true if the hero tried to change his position during this cycle
 * @param success true if the position has actually just changed
 */
void Hero::StateFree::notify_movement_result(bool tried_to_move, bool success) {

  StatePlayerMovement::notify_movement_result(tried_to_move, success);

  // get the direction of the player
  uint16_t direction_mask = get_player_movement()->get_direction_mask();

  if (tried_to_move) { // the hero tried to move

    if (!success) { // the hero is facing an obstacle

      if (hero->get_wanted_movement_direction8() == hero->get_animation_direction() * 2) {
	// see when we can start animation "pushing"

	uint32_t now = System::now();
	if (pushing_direction_mask == 0xFFFF) { // we start counting to trigger animation "pushing"
	  start_pushing_date = now + 800; // start animation "pushing" after 800 ms
	  pushing_direction_mask = direction_mask;
	}
	else if (now >= start_pushing_date) {
	  hero->set_state(new StatePushing(hero));
	}
      }
    }
    else {
      // the hero has just moved successfuly: stop trying to push
      pushing_direction_mask = 0xFFFF;
    }
  }
  else if (pushing_direction_mask != 0xFFFF && // the hero is about to push
      direction_mask != pushing_direction_mask) {

    // stop trying to push if the player changes his direction
    pushing_direction_mask = 0xFFFF;
  }
}

