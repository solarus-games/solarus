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
#include "hero/FreeState.h"
#include "hero/GrabbingState.h"
#include "hero/PushingState.h"
#include "hero/HeroSprites.h"
#include "entities/Detector.h"
#include "movements/PlayerMovement.h"
#include "lowlevel/System.h"
#include "Game.h"
#include "KeysEffect.h"

/**
 * @brief Constructor.
 * @param hero the hero controlled by this state
 */
Hero::FreeState::FreeState(Hero &hero):
  PlayerMovementState(hero) {

}

/**
 * @brief Destructor.
 */
Hero::FreeState::~FreeState() {
}

/**
 * @brief Starts this state.
 * @param previous_state the previous state
 */
void Hero::FreeState::start(State *previous_state) {

  PlayerMovementState::start(previous_state);

  pushing_direction4 = -1;
  start_pushing_date = 0;
}

/**
 * @brief Stops this state.
 * @param next_state the next state
 */
void Hero::FreeState::stop(State *next_state) {

  PlayerMovementState::stop(next_state);

  get_keys_effect().set_action_key_effect(KeysEffect::ACTION_KEY_NONE);
}

/**
 * @brief Updates this state.
 */
void Hero::FreeState::update() {

  PlayerMovementState::update();

  if (!suspended
      && pushing_direction4 != -1					// the hero is trying to push
      && get_wanted_movement_direction8() != pushing_direction4 * 2) {	// but his movement direction has changed

    pushing_direction4 = -1; // stop trying to push
  }
}

/**
 * @brief Notifies this state that the game was just suspended or resumed.
 * @param suspended true if the game is suspended
 */
void Hero::FreeState::set_suspended(bool suspended) {

  PlayerMovementState::set_suspended(suspended);

  if (!suspended) {
    start_pushing_date += System::now() - when_suspended;
  }
}

/**
 * @brief Notifies this state that the action key was just pressed.
 */
void Hero::FreeState::action_key_pressed() {

  if (get_keys_effect().is_action_key_acting_on_facing_entity()) {

    // action on the facing entity
    hero.get_facing_entity()->action_key_pressed();
  }
  else if (hero.is_facing_point_on_obstacle()) {

    // grab an obstacle
    hero.set_state(new GrabbingState(hero));
  }
}

/**
 * @brief Notifies this state that the hero has just tried to change his position.
 * @param success true if the position has actually just changed
 */
void Hero::FreeState::notify_movement_tried(bool success) {

  PlayerMovementState::notify_movement_tried(success);

  if (!success // the hero has just tried to move unsuccessfuly
      &&  hero.is_facing_point_on_obstacle()) { // he is really facing an obstacle

    uint32_t now = System::now();
    if (pushing_direction4 == -1) { // we start counting to trigger animation "pushing"
      start_pushing_date = now + 800; // start animation "pushing" after 800 ms
      pushing_direction4 = hero.get_animation_direction();
    }
    else if (now >= start_pushing_date) {
      hero.set_state(new PushingState(hero));
    }
  }
}

/**
 * @brief Returns whether the hero can walk normally and interact with entities
 * in this state.
 * @return true
 */
bool Hero::FreeState::is_free() {
  return true;
}

/**
 * @brief Returns whether the hero can swing his sword in this state.
 * @return true if the hero can swing his sword in this state
 */
bool Hero::FreeState::can_start_sword() {
  return true;
}

/**
 * @brief Returns whether the hero can use an inventory item in this state.
 * @return true if the hero can use an inventoy item in this state
 */
bool Hero::FreeState::can_start_inventory_item() {
  return hero.get_ground() != GROUND_HOLE;
}

/**
 * @brief Returns whether the hero can take stairs in this state.
 * If false is returned, stairs have no effect (but they are obstacle for the hero).
 * @return true if the hero ignores the effect of stairs in this state
 */
bool Hero::FreeState::can_take_stairs() {
  return true;
}

/**
 * @brief Returns whether can trigger a jumper in this state.
 * If false is returned, jumpers have no effect (but they are obstacle for the hero).
 * @return true if the hero can use jumpers in this state
 */
bool Hero::FreeState::can_take_jumper() {
  return true;
}

/**
 * @brief Returns the action to do with an item previously carried by the hero when this state starts.
 * @param carried_item the item carried in the previous state
 * @return the action to do with a previous carried item when this state starts
 */
CarriedItem::Behavior Hero::FreeState::get_previous_carried_item_behavior(CarriedItem& carried_item) {
  return CarriedItem::BEHAVIOR_DESTROY;
}

/**
 * Gives the sprites the animation stopped corresponding to this state.
 */
void Hero::FreeState::set_animation_stopped() {
  get_sprites().set_animation_stopped_normal();
}

/**
 * Gives the sprites the animation walking corresponding to this state.
 */
void Hero::FreeState::set_animation_walking() {
  get_sprites().set_animation_walking_normal();
}

