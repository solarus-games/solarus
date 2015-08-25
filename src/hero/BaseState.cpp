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
#include "solarus/hero/BaseState.h"
#include "solarus/hero/SwordSwingingState.h"
#include "solarus/KeysEffect.h"

namespace Solarus {

/**
 * \brief Constructor.
 * \param hero The hero controlled by this state.
 */
Hero::BaseState::BaseState(Hero& hero, const std::string& state_name):
  State(hero, state_name) {

}

/**
 * \copydoc Entity::State::notify_attack_command_pressed
 */
void Hero::BaseState::notify_attack_command_pressed() {
  Hero& hero = get_hero();

  if (!hero.is_suspended()
      && get_keys_effect().get_sword_key_effect() == KeysEffect::SWORD_KEY_SWORD
      && hero.can_start_sword()) {

    hero.set_state(new Hero::SwordSwingingState(hero));
  }
}

}

