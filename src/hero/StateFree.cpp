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
#include "hero/StateFree.h"

/**
 * Constructor.
 * @param hero the hero controlled by this state
 */
Hero::StateFree::StateFree(Hero *hero):
  StatePlayerMovement(hero) {

}

/**
 * Destructor.
 */
Hero::StateFree::~StateFree(void) {

}

/**
 * @brief Notifies this state that the action key was just pressed.
 */
void Hero::StateFree::action_key_pressed(void) {

  KeysEffect *keys_effect = game->get_keys_effect();

  if (keys_effect->get_action_key_effect() == KeysEffect::ACTION_KEY_GRAB) {

    // grab an entity
    hero->set_state(new StateGrabbing(hero));
  }
  else if (keys_effect->is_action_key_acting_on_facing_entity()) {

    // action on the facing entity
    facing_entity->action_key_pressed();
  }
  else if (is_facing_point_on_obstacle()) {

    // grab an obstacle
    hero->set_state(new StateGrabbing(hero));
  }
}

