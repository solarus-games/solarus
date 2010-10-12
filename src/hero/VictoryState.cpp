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
#include "hero/VictoryState.h"
#include "hero/HeroSprites.h"
#include "lowlevel/System.h"
#include "lowlevel/Sound.h"
#include "lua/MapScript.h"
#include "Game.h"
#include "Map.h"

/**
 * @brief Constructor.
 * @param hero the hero controlled by this state
 */
Hero::VictoryState::VictoryState(Hero &hero):
  State(hero), end_victory_date(0) {

}

/**
 * @brief Destructor.
 */
Hero::VictoryState::~VictoryState() {

}

/**
 * @brief Starts this state.
 * @param previous_state the previous state
 */
void Hero::VictoryState::start(State *previous_state) {

  State::start(previous_state);

  get_sprites().set_animation_victory();
  Sound::play("victory");

  // compute the date when the victory state is considered as finished,
  // but the game may be currently suspended
  uint32_t start_victory_date = suspended ? when_suspended : System::now();
  end_victory_date = start_victory_date + 1500;
}

/**
 * @brief Updates this state.
 */
void Hero::VictoryState::update() {

  State::update();

  if (System::now() >= end_victory_date) {
    get_map_script().event_hero_victory_sequence_finished();
  }
}

/**
 * @brief Notifies this state that the game was just suspended or resumed.
 * @param suspended true if the game is suspended
 */
void Hero::VictoryState::set_suspended(bool suspended) {

  State::set_suspended(suspended);

  if (!suspended) {
    end_victory_date += System::now() - when_suspended;
  }
}

