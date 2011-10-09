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
#include "hero/GrabbingState.h"
#include "hero/FreeState.h"
#include "hero/PushingState.h"
#include "hero/PullingState.h"
#include "hero/HeroSprites.h"
#include "Game.h"
#include "GameControls.h"

/**
 * @brief Constructor.
 * @param hero the hero controlled by this state
 */
Hero::GrabbingState::GrabbingState(Hero &hero):
  State(hero) {

}

/**
 * @brief Destructor.
 */
Hero::GrabbingState::~GrabbingState() {

}

/**
 * @brief Starts this state.
 * @param previous_state the previous state
 */
void Hero::GrabbingState::start(State *previous_state) {

  State::start(previous_state);

  get_sprites().set_animation_grabbing();
}

/**
 * @brief Updates this state.
 */
void Hero::GrabbingState::update() {

  // the hero is grabbing an obstacle: check the direction pressed

  int wanted_direction8 = get_controls().get_wanted_direction8();
  int sprite_direction8 = get_sprites().get_animation_direction8();

  // release the obstacle
  if (!get_controls().is_key_pressed(GameControls::ACTION)) {
    hero.set_state(new FreeState(hero));
  }

  // push the obstacle
  else if (wanted_direction8 == sprite_direction8) {
    hero.set_state(new PushingState(hero));
  }

  // pull the obstacle
  else if (wanted_direction8 == (sprite_direction8 + 4) % 8) {
    hero.set_state(new PullingState(hero));
  }
}

/**
 * @brief Returns whether the hero is grabbing or pulling an entity in this state.
 * @return true if the hero is grabbing or pulling an entity
 */
bool Hero::GrabbingState::is_grabbing_or_pulling() {
  return true;
}

/**
 * @brief Returns whether the hero can be hurt in this state.
 * @param enemy an attacker that is trying to hurt the hero
 * (or NULL if the source of the attack is not an enemy)
 * @return true if the hero can be hurt in this state
 */
bool Hero::GrabbingState::can_be_hurt(Enemy* attacker) {
  return true;
}

