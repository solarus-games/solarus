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
#include "hero/RunningState.h"
#include "hero/FreeState.h"
#include "hero/HeroSprites.h"
#include "movements/StraightMovement.h"
#include "lowlevel/System.h"
#include "Map.h"

/**
 * @brief Constructor.
 * @param hero the hero controlled by this state
 */
Hero::RunningState::RunningState(Hero *hero):
  State(hero) {

}

/**
 * @brief Destructor.
 */
Hero::RunningState::~RunningState(void) {

}

/**
 * @brief Starts this state.
 * @param previous_state the previous state
 */
void Hero::RunningState::start(State *previous_state) {

  State::start(previous_state);

  hero->set_movement(new StraightMovement(10, sprites->get_animation_direction() * 90, 300));
  sprites->set_animation_walking_normal();

  phase = 0;
  next_phase_date = System::now() + 300;
}

/**
 * @brief Stops this state.
 */
void Hero::RunningState::stop(State *next_state) {

  State::stop(next_state);

  hero->clear_movement();
}

/**
 * @brief Updates this state.
 */
void Hero::RunningState::update(void) {

  uint32_t now = System::now();
  if (phase == 0) {
    
    if (now >= next_phase_date) {
      hero->clear_movement();
      hero->set_movement(new StraightMovement(30, sprites->get_animation_direction() * 90, 2000));
      phase++;
    }
  }
  else if (hero->get_movement()->is_finished()) {
    hero->set_state(new FreeState(hero));
  }
}

/**
 * @brief Notifies this state that the game was just suspended or resumed.
 * @param suspended true if the game is suspended
 */
void Hero::RunningState::set_suspended(bool suspended) {

  State::set_suspended(suspended);

  if (!suspended) {
    next_phase_date += System::now() - when_suspended;
  }
}

/**
 * @brief Notifies this state that the hero has just tried to change his position.
 * @param success true if the position has actually just changed
 */
void Hero::RunningState::notify_movement_tried(bool success) {

  State::notify_movement_tried(success);

  if (!success) {
    hero->set_state(new FreeState(hero));
  }
}

