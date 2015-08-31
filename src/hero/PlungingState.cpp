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
#include "solarus/hero/PlungingState.h"
#include "solarus/hero/FreeState.h"
#include "solarus/hero/SwimmingState.h"
#include "solarus/hero/BackToSolidGroundState.h"
#include "solarus/hero/HeroSprites.h"
#include "solarus/lowlevel/Sound.h"
#include "solarus/Game.h"
#include "solarus/Equipment.h"

namespace Solarus {

/**
 * \brief Constructor.
 * \param hero The hero controlled by this state.
 */
Hero::PlungingState::PlungingState(Hero& hero):
  BaseState(hero, "plunging") {

}

/**
 * \brief Starts this state.
 * \param previous_state the previous state
 */
void Hero::PlungingState::start(const State* previous_state) {

  State::start(previous_state);

  if (get_entity().get_ground_below() == Ground::DEEP_WATER) {
    get_sprites().set_animation("plunging_water");
  }
  else {
    get_sprites().set_animation("plunging_lava");
  }
  Sound::play("splash");
}

/**
 * \brief Updates this state.
 */
void Hero::PlungingState::update() {

  State::update();

  if (get_sprites().is_animation_finished()) {

    Hero& hero = get_entity();
    int drown = 0;
    if (hero.get_ground_below() == Ground::DEEP_WATER) {

      if (get_equipment().has_ability(Ability::SWIM)) {
        hero.set_state(new SwimmingState(hero));
      }
      else {
        drown = 1;
      }
    }
    else if (hero.get_ground_below() == Ground::LAVA) {
      drown = 4;
    }
    else {
      hero.set_state(new FreeState(hero));
    }

    if (drown > 0) {
      get_equipment().remove_life(drown);
      hero.set_state(new BackToSolidGroundState(hero, true, 300));
    }
  }
}

/**
 * \brief Returns whether the game over sequence can start in the current state.
 * \return true if the game over sequence can start in the current state
 */
bool Hero::PlungingState::can_start_gameover_sequence() const {
  return false;
}

/**
 * \brief Returns whether the hero is touching the ground in the current state.
 * \return true if the hero is touching the ground in the current state
 */
bool Hero::PlungingState::is_touching_ground() const {
  return false;
}

}

