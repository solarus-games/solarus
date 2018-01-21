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
#include "solarus/core/CommandsEffects.h"
#include "solarus/core/Game.h"
#include "solarus/hero/FrozenState.h"
#include "solarus/hero/HeroSprites.h"

namespace Solarus {

/**
 * \brief Constructor.
 * \param hero The hero controlled by this state.
 */
Hero::FrozenState::FrozenState(Hero& hero):
  HeroState(hero, "frozen") {

}

/**
 * \brief Starts this state.
 * \param previous_state the previous state
 */
void Hero::FrozenState::start(const State* previous_state) {

  HeroState::start(previous_state);

  get_sprites().set_animation_stopped_normal();
  get_commands_effects().set_action_key_effect(CommandsEffects::ACTION_KEY_NONE);
}

}

