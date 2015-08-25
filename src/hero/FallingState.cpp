/*
 * Copyright (C) 2006-2015 Christopho, Solarus - http://www.solarus-games.org
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
#include "solarus/hero/FallingState.h"
#include "solarus/hero/BackToSolidGroundState.h"
#include "solarus/hero/HeroSprites.h"
#include "solarus/entities/Teletransporter.h"
#include "solarus/lowlevel/Sound.h"
#include "solarus/Game.h"
#include "solarus/Equipment.h"

namespace Solarus {

/**
 * \brief Constructor.
 * \param hero The hero controlled by this state.
 */
Hero::FallingState::FallingState(Hero& hero):
  BaseState(hero, "falling") {

}

/**
 * \brief Starts this state.
 * \param previous_state the previous state
 */
void Hero::FallingState::start(const State* previous_state) {

  State::start(previous_state);

  get_hero().delayed_teletransporter = nullptr;
  get_sprites().save_animation_direction();
  get_sprites().set_animation_falling();
  Sound::play("hero_falls");
}

/**
 * \brief Stops this state.
 * \param next_state the next state
 */
void Hero::FallingState::stop(const State* next_state) {

  State::stop(next_state);

  get_sprites().set_animation_stopped_normal();
  get_sprites().restore_animation_direction();
}

/**
 * \brief Updates this state.
 */
void Hero::FallingState::update() {

  State::update();

  Hero& hero = get_hero();
  if (!is_suspended() && get_sprites().is_animation_finished()) {

    // the hero has just finished falling
    Teletransporter* teletransporter = hero.get_delayed_teletransporter();
    if (teletransporter != nullptr) {
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
 * \brief Returns whether the hero ignores the effect of holes in this state.
 * \return true if the hero ignores the effect of holes in the current state
 */
bool Hero::FallingState::can_avoid_hole() const {
  return true;
}

/**
 * \brief Returns whether the hero ignores the effect of lava in this state.
 * \return true if the hero ignores the effect of lava in the current state
 */
bool Hero::FallingState::can_avoid_lava() const {
  return true;
}

/**
 * \brief Returns whether the hero ignores the effect of prickles in this state.
 * \return true if the hero ignores the effect of prickles in the current state
 */
bool Hero::FallingState::can_avoid_prickle() const {
  return true;
}

/**
 * \brief Returns whether the game over sequence can start in the current state.
 * \return true if the game over sequence can start in the current state
 */
bool Hero::FallingState::can_start_gameover_sequence() const {
  return false;
}

}

