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
#include "hero/LiftingState.h"
#include "hero/CarryingState.h"
#include "hero/HeroSprites.h"
#include "entities/MapEntities.h"
#include "entities/CarriedItem.h"
#include "Game.h"
#include "Map.h"
#include "KeysEffect.h"

/**
 * @brief Constructor.
 * @param hero the hero controlled by this state
 * @param item_to_lift the item to lift
 */
Hero::LiftingState::LiftingState(Hero &hero, DestructibleItem &item_to_lift):
  State(hero), item_to_lift(item_to_lift), lifted_item(NULL) {

}

/**
 * @brief Destructor.
 */
Hero::LiftingState::~LiftingState() {

  delete lifted_item;
}

/**
 * @brief Starts this state.
 * @param previous_state the previous state
 */
void Hero::LiftingState::start(State *previous_state) {

  State::start(previous_state);

  // create the entity that will actually be lifted
  lifted_item = new CarriedItem(hero, item_to_lift);
  lifted_item->set_map(get_map());

  get_keys_effect().set_action_key_effect(KeysEffect::ACTION_KEY_THROW);
  get_sprites().set_animation_lifting();
  get_sprites().set_lifted_item(lifted_item);
  hero.set_facing_entity(NULL);

  get_equipment().notify_ability_used("lift");
}

/**
 * @brief Ends this state.
 * @param next_state the next state
 */
void Hero::LiftingState::stop(State *next_state) {

  State::stop(next_state);

  get_sprites().set_lifted_item(NULL);

  if (lifted_item != NULL) {

    // the lifted item is still managed by this state
    if (next_state->can_throw_item()) {
      throw_item();
    }
    else {
      delete lifted_item;
      lifted_item = NULL;
    }
    get_keys_effect().set_action_key_effect(KeysEffect::ACTION_KEY_NONE);
  }
}

/**
 * @brief Updates this state.
 */
void Hero::LiftingState::update() {

  State::update();

  lifted_item->update();

  if (!suspended && !lifted_item->is_being_lifted()) { // the item has finished being lifted

    CarriedItem *carried_item = lifted_item;
    lifted_item = NULL; // we do not take care of the carried item from this state anymore
    hero.set_state(new CarryingState(hero, carried_item));
  }
}

/**
 * @brief Notifies this state that the game was just suspended or resumed.
 * @param suspended true if the game is suspended
 */
void Hero::LiftingState::set_suspended(bool suspended) {

  State::set_suspended(suspended);

  if (lifted_item != NULL) {
    lifted_item->set_suspended(suspended);
  }
}

/**
 * @brief Returns whether the hero can be hurt in this state.
 * @return true if the hero can be hurt in this state
 */
bool Hero::LiftingState::can_be_hurt() {
  return true;
}

/**
 * @brief Throws the item that is being lifted.
 *
 * This function is called when this state is interrupted by a new state,
 * e.g. when the hero is hurt while lifting an item.
 */
void Hero::LiftingState::throw_item() {

  lifted_item->throw_item(get_sprites().get_animation_direction());
  get_entities().add_entity(lifted_item);
  lifted_item = NULL;
}

