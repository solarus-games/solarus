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
#include "hero/HeroSprites.h"
#include "entities/Detector.h"
#include "Game.h"
#include "KeysEffect.h"

/**
 * Constructor.
 * @param hero the hero controlled by this state
 */
Hero::StateFree::StateFree(Hero *hero):
  StatePlayerMovement(hero) {

}

/**
 * Destructor.
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

