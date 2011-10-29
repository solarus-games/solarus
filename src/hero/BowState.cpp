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
#include "hero/BowState.h"
#include "hero/FreeState.h"
#include "hero/HeroSprites.h"
#include "entities/MapEntities.h"
#include "entities/Arrow.h"
#include "lowlevel/Sound.h"

/**
 * @brief Constructor.
 * @param hero the hero controlled by this state
 */
Hero::BowState::BowState(Hero &hero):
  State(hero) {
}

/**
 * @brief Destructor.
 */
Hero::BowState::~BowState() {
}

/**
 * @brief Starts this state.
 * @param previous_state the previous state
 */
void Hero::BowState::start(State *previous_state) {

  State::start(previous_state);
  get_sprites().set_animation("bow", "");
}

/**
 * @brief Updates this state.
 */
void Hero::BowState::update() {

  State::update();

  if (get_sprites().is_animation_finished()) {
    Sound::play("bow");
    get_entities().add_entity(new Arrow(hero));
    hero.set_state(new FreeState(hero));
  }
}

