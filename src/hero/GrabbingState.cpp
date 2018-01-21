/*
 * Copyright (C) 2006-2018 Christopho, Solarus - http://www.solarus-games.org
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
#include "solarus/core/Equipment.h"
#include "solarus/core/Game.h"
#include "solarus/core/GameCommands.h"
#include "solarus/hero/GrabbingState.h"
#include "solarus/hero/HeroSprites.h"

namespace Solarus {

/**
 * \brief Constructor.
 * \param hero The hero controlled by this state.
 */
Hero::GrabbingState::GrabbingState(Hero& hero):
  HeroState(hero, "grabbing") {

}

/**
 * \brief Starts this state.
 * \param previous_state the previous state
 */
void Hero::GrabbingState::start(const State* previous_state) {

  HeroState::start(previous_state);

  get_sprites().set_animation_grabbing();
}

/**
 * \brief Updates this state.
 */
void Hero::GrabbingState::update() {

  if (is_suspended()) {
    return;
  }

  // The hero is grabbing an obstacle: check the direction pressed.

  int wanted_direction8 = get_commands().get_wanted_direction8();
  int sprite_direction8 = get_sprites().get_animation_direction8();

  // Release the obstacle.
  Hero& hero = get_entity();
  if (!get_commands().is_command_pressed(GameCommand::ACTION)) {
    hero.start_free();
    return;
  }

  // Push the obstacle.
  if (wanted_direction8 == sprite_direction8) {
    if (hero.can_push()) {
      hero.start_pushing();
    }
    return;
  }

  // Pull the obstacle.
  if (wanted_direction8 == (sprite_direction8 + 4) % 8) {
    if (hero.can_pull()) {
      hero.start_pulling();
    }
    return;
  }
}

/**
 * \brief Returns whether the hero is grabbing or pulling an entity in this state.
 * \return true if the hero is grabbing or pulling an entity
 */
bool Hero::GrabbingState::is_grabbing_or_pulling() const {
  return true;
}

/**
 * \brief Returns whether the hero can be hurt in this state.
 * \param attacker an attacker that is trying to hurt the hero
 * (or nullptr if the source of the attack is not an enemy)
 * \return true if the hero can be hurt in this state
 */
bool Hero::GrabbingState::can_be_hurt(Entity* /* attacker */) const {
  return true;
}

/**
 * \brief Returns whether the hero can pick a treasure in this state.
 * \param item The equipment item to obtain.
 * \return true if the hero can pick that treasure in this state.
 */
bool Hero::GrabbingState::can_pick_treasure(EquipmentItem& /* item */) const {
  return true;
}

}

