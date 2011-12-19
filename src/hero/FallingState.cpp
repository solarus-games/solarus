/*
 * Copyright (C) 2009-2011 Christopho, Solarus - http://www.solarus-engine.org
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
#include "hero/FallingState.h"
#include "hero/BackToSolidGroundState.h"
#include "hero/HeroSprites.h"
#include "entities/Teletransporter.h"
#include "lowlevel/Sound.h"
#include "Game.h"
#include "Equipment.h"

/**
 * @brief Constructor.
 * @param hero the hero controlled by this state
 */
Hero::FallingState::FallingState(Hero &hero):
  State(hero) {

}

/**
 * @brief Destructor.
 */
Hero::FallingState::~FallingState() {

}

/**
 * @brief Starts this state.
 * @param previous_state the previous state
 */
void Hero::FallingState::start(State *previous_state) {

  State::start(previous_state);

  hero.delayed_teletransporter = NULL;
  get_sprites().save_animation_direction();
  get_sprites().set_animation_falling();
  Sound::play("hero_falls");
}

/**
 * @brief Stops this state.
 * @param next_state the next state
 */
void Hero::FallingState::stop(State *next_state) {

  State::stop(next_state);

  get_sprites().set_animation_stopped_normal();
  get_sprites().restore_animation_direction();
}

/**
 * @brief Updates this state.
 */
void Hero::FallingState::update() {

  State::update();

  if (!suspended && get_sprites().is_animation_finished()) {

    // the hero has just finished falling
    Teletransporter *teletransporter = hero.get_delayed_teletransporter();
    if (teletransporter != NULL) {
      // special hole with a teletransporter
      teletransporter->transport_hero(hero);
    }
    else {
      // normal hole that hurts the hero
      get_equipment().remove_life(2);
      hero.set_state(new BackToSolidGroundState(hero, true));
    }
  }
}

/**
 * @brief Returns whether the hero ignores the effect of holes in this state.
 * @return true if the hero ignores the effect of holes in the current state
 */
bool Hero::FallingState::can_avoid_hole() {
  return true;
}

/**
 * @brief Returns whether the hero ignores the effect of lava in this state.
 * @return true if the hero ignores the effect of lava in the current state
 */
bool Hero::FallingState::can_avoid_lava() {
  return true;
}

/**
 * @brief Returns whether the hero ignores the effect of prickles in this state.
 * @return true if the hero ignores the effect of prickles in the current state
 */
bool Hero::FallingState::can_avoid_prickle() {
  return true;
}

/**
 * @brief Returns whether the game over sequence can start in the current state.
 * @return true if the game over sequence can start in the current state
 */
bool Hero::FallingState::can_start_gameover_sequence() {
  return false;
}

