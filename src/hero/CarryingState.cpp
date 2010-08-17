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
#include "hero/CarryingState.h"
#include "hero/FreeState.h"
#include "hero/HeroSprites.h"
#include "entities/CarriedItem.h"
#include "entities/MapEntities.h"
#include "Game.h"
#include "KeysEffect.h"
#include "Map.h"

/**
 * @brief Constructor.
 * @param hero the hero controlled by this state
 * @param carried_item the item to carry
 */
Hero::CarryingState::CarryingState(Hero *hero, CarriedItem *carried_item):
  PlayerMovementState(hero), carried_item(carried_item) {

}

/**
 * @brief Destructor.
 */
Hero::CarryingState::~CarryingState(void) {

  delete carried_item;
}

/**
 * @brief Starts this state
 * @param previous_state the previous state
 */
void Hero::CarryingState::start(State *previous_state) {

  PlayerMovementState::start(previous_state);

  sprites->set_lifted_item(carried_item);

  // action icon "throw"
  KeysEffect *keys_effect = game->get_keys_effect();
  keys_effect->set_action_key_effect(KeysEffect::ACTION_KEY_THROW);
}

/**
 * @brief Stops this state.
 * @param next_state the next state
 */
void Hero::CarryingState::stop(State *next_state) {

  PlayerMovementState::stop(next_state);

  sprites->set_lifted_item(NULL);
  game->get_keys_effect()->set_action_key_effect(KeysEffect::ACTION_KEY_NONE);

  if (carried_item != NULL && next_state->can_throw_item()) {
    throw_item();
  }
  else {
    delete carried_item;
    carried_item = NULL;
  }
}

/**
 * @brief Changes the map.
 * @param map the new map
 */
void Hero::CarryingState::set_map(Map *map) {

  PlayerMovementState::set_map(map);

  // the hero may go to another map while carrying an item
  carried_item->set_map(map);
}

/**
 * @brief Notifies this state that the game was just suspended or resumed.
 * @param suspended true if the game is suspended
 */
void Hero::CarryingState::set_suspended(bool suspended) {

  PlayerMovementState::set_suspended(suspended);
  carried_item->set_suspended(suspended);
}

/**
 * @brief Updates this state.
 */
void Hero::CarryingState::update(void) {

  PlayerMovementState::update();
  carried_item->update();

  if (!suspended) {

    if (carried_item->is_broken()) {
      delete carried_item;
      carried_item = NULL;
      hero->set_state(new FreeState(hero));
    }
  }
}

/**
 * @brief Notifies this state that the action key was just pressed.
 */
void Hero::CarryingState::action_key_pressed(void) {

  KeysEffect *keys_effect = game->get_keys_effect();

  if (keys_effect->get_action_key_effect() == KeysEffect::ACTION_KEY_THROW) {
    throw_item();
    hero->set_state(new FreeState(hero));
  }
}

/**
 * @brief Throws the item carried.
 *
 * This function is called when the player presses the action key
 * or when another state becomes the current state.
 */
void Hero::CarryingState::throw_item(void) {

  carried_item->throw_item(sprites->get_animation_direction());
  map->get_entities()->add_entity(carried_item);
  carried_item = NULL;
}

/**
 * @brief Returns whether the hero can swing his sword in this state.
 * @return true if the hero can swing his sword in this state
 */
bool Hero::CarryingState::can_start_sword(void) {
  return true;
}

/**
 * @brief Returns whether can trigger a jump sensor in this state.
 * If false is returned, jump sensors have no effect (but they are obstacle for the hero).
 * @param jump_sensor a jump sensor
 * @return true if the hero can use jump sensors in this state
 */
bool Hero::CarryingState::can_take_jump_sensor(void) {
  return true;
}

/**
 * Gives the sprites the animation stopped corresponding to this state.
 */
void Hero::CarryingState::set_animation_stopped(void) {
  sprites->set_animation_stopped_carrying();
}

/**
 * Gives the sprites the animation walking corresponding to this state.
 */
void Hero::CarryingState::set_animation_walking(void) {
  sprites->set_animation_walking_carrying();
}

