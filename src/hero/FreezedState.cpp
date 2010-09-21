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
#include "hero/FreezedState.h"
#include "hero/HeroSprites.h"
#include "Game.h"
#include "KeysEffect.h"

/**
 * @brief Constructor.
 * @param hero the hero controlled by this state
 */
Hero::FreezedState::FreezedState(Hero *hero):
  State(hero) {

}

/**
 * @brief Destructor.
 */
Hero::FreezedState::~FreezedState() {

}

/**
 * @brief Starts this state.
 * @param previous_state the previous state
 */
void Hero::FreezedState::start(State *previous_state) {

  State::start(previous_state);

  sprites->set_animation_stopped_normal();
  game->get_keys_effect()->set_action_key_effect(KeysEffect::ACTION_KEY_NONE);
}

