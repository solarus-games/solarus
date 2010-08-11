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
#include "hero/StateGrabbing.h"
#include "hero/StateFree.h"
#include "hero/StatePushing.h"
#include "hero/StatePulling.h"
#include "hero/HeroSprites.h"
#include "Game.h"
#include "GameControls.h"

/**
 * @brief Constructor.
 * @param hero the hero controlled by this state
 */
Hero::StateGrabbing::StateGrabbing(Hero *hero):
  State(hero) {

}

/**
 * @brief Destructor.
 */
Hero::StateGrabbing::~StateGrabbing(void) {

}

/**
 * @brief Starts this state.
 * @param previous_state the previous state
 */
void Hero::StateGrabbing::start(State *previous_state) {

  State::start(previous_state);

  sprites->set_animation_grabbing();
}

/**
 * @brief Updates this state.
 */
void Hero::StateGrabbing::update(void) {

  // the hero is grabbing an obstacle: check the direction pressed
  GameControls *controls = game->get_controls();

  int wanted_direction8 = controls->get_wanted_direction8();
  int sprite_direction8 = sprites->get_animation_direction() * 2;

  // release the obstacle
  if (!controls->is_key_pressed(GameControls::ACTION)) {
    hero->set_state(new StateFree(hero));
  }

  // push the obstacle
  else if (wanted_direction8 == sprite_direction8) {
    hero->set_state(new StatePushing(hero));
  }

  // pull the obstacle
  else if (wanted_direction8 == (sprite_direction8 + 4) % 8) {
    hero->set_state(new StatePulling(hero));
  }
}

/**
 * @brief Returns whether the hero is grabbing or pulling an entity in this state.
 * @return true if the hero is grabbing or pulling an entity
 */
bool Hero::StateGrabbing::is_grabbing_or_pulling(void) {
  return true;
}

