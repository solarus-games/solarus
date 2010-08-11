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
#include "hero/StateSwordSwinging.h"
#include "hero/StateSwordLoading.h"
#include "hero/HeroSprites.h"
#include "Game.h"
#include "GameControls.h"

/**
 * @brief Constructor.
 * @param hero the hero controlled by this state
 */
Hero::StateSwordSwinging::StateSwordSwinging(Hero *hero):
  State(hero) {

}

/**
 * @brief Destructor.
 */
Hero::StateSwordSwinging::~StateSwordSwinging(void) {

}

/**
 * @brief Starts this state.
 * @param previous_state the previous state of NULL if this is the first state (for information)
 */
void Hero::StateSwordSwinging::start(State *previous_state) {

  sprites->play_sword_sound();
  sprites->set_animation_sword();
}

/**
 * @brief Updates this state.
 */
void Hero::StateSwordSwinging::update(void) {

  if (sprites->is_animation_finished()) {

    // if the player is still pressing the sword key, start loading the sword
    if (game->get_controls()->is_key_pressed(GameControls::SWORD)) {
      hero->set_state(new StateSwordLoading(hero));
    }
    else {
      hero->start_free();
    }
  }
}

/**
 * @brief Returns whether the hero can swing his sword in this state.
 * @return true if the hero can swing his sword in this state
 */
bool Hero::StateSwordSwinging::can_start_sword(void) {
  return true;
}

/**
 * @brief Returns whether the hero can be hurt in this state.
 * @return true if the hero can be hurt in this state
 */
bool Hero::StateSwordSwinging::can_be_hurt(void) {
  return true;
}

