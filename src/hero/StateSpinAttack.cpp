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
#include "hero/StateSpinAttack.h"
#include "hero/HeroSprites.h"
#include "Game.h"

/**
 * @brief Constructor.
 * @param hero the hero controlled by this state
 */
Hero::StateSpinAttack::StateSpinAttack(Hero *hero):
  State(hero) {

}

/**
 * @brief Destructor.
 */
Hero::StateSpinAttack::~StateSpinAttack(void) {

}

/**
 * @brief Starts this state.
 * @param previous_state the previous state
 */
void Hero::StateSpinAttack::start(State *previous_state) {

  State::start(previous_state);

  // play the sound
  game->play_sound("sword_spin_attack_release");

  // start the animation
  sprites->set_animation_spin_attack();
}

/**
 * @brief Updates this state.
 */
void Hero::StateSpinAttack::update(void) {

  if (sprites->is_animation_finished()) {
    hero->start_free();
  }
}

