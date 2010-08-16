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
#include "hero/StateLifting.h"
#include "hero/StateCarrying.h"
#include "hero/HeroSprites.h"
#include "entities/CarriedItem.h"
#include "Game.h"
#include "KeysEffect.h"

/**
 * @brief Constructor.
 * @param hero the hero controlled by this state
 * @param item_to_lift the item to lift
 */
Hero::StateLifting::StateLifting(Hero *hero, DestructibleItem *item_to_lift):
  State(hero), item_to_lift(item_to_lift) {

}

/**
 * @brief Destructor.
 */
Hero::StateLifting::~StateLifting(void) {

}

/**
 * @brief Starts this state.
 * @param previous_state the previous state
 */
void Hero::StateLifting::start(State *previous_state) {

  State::start(previous_state);

  // create the entity that will actually be lifted
  lifted_item = new CarriedItem(hero, item_to_lift);
  lifted_item->set_map(map);

  game->get_keys_effect()->set_action_key_effect(KeysEffect::ACTION_KEY_THROW);
  sprites->set_animation_lifting();
  sprites->set_lifted_item(lifted_item);
  hero->set_facing_entity(NULL);
}

/**
 * @brief Ends this state.
 * @param next_state the next state
 */
void Hero::StateLifting::stop(State *next_state) {

  State::stop(next_state);

  sprites->set_lifted_item(NULL);

  if (lifted_item != NULL) {

    // the lifted item is still managed by this state
    delete lifted_item;
    game->get_keys_effect()->set_action_key_effect(KeysEffect::ACTION_KEY_NONE);
  }
}

/**
 * @brief Updates this state.
 */
void Hero::StateLifting::update(void) {

  State::update();

  lifted_item->update();

  if (!lifted_item->is_being_lifted()) { // the item has finished being lifted
    hero->set_state(new StateCarrying(hero, lifted_item));
    lifted_item = NULL; // we do not take care of the carried item from this state anymore
  }
}

