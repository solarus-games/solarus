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
#include "hero/SwordSwingingState.h"
#include "hero/SwordLoadingState.h"
#include "hero/HeroSprites.h"
#include "entities/Detector.h"
#include "Game.h"
#include "GameControls.h"

/**
 * @brief Constructor.
 * @param hero the hero controlled by this state
 */
Hero::SwordSwingingState::SwordSwingingState(Hero *hero):
  State(hero) {

}

/**
 * @brief Destructor.
 */
Hero::SwordSwingingState::~SwordSwingingState(void) {

}

/**
 * @brief Starts this state.
 * @param previous_state the previous state
 */
void Hero::SwordSwingingState::start(State *previous_state) {

  sprites->play_sword_sound();
  sprites->set_animation_sword();
}

/**
 * @brief Updates this state.
 */
void Hero::SwordSwingingState::update(void) {

  if (sprites->is_animation_finished()) {

    // if the player is still pressing the sword key, start loading the sword
    if (game->get_controls()->is_key_pressed(GameControls::SWORD)) {
      hero->set_state(new SwordLoadingState(hero));
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
bool Hero::SwordSwingingState::can_start_sword(void) {
  return true;
}

/**
 * @brief Returns whether the hero can be hurt in this state.
 * @return true if the hero can be hurt in this state
 */
bool Hero::SwordSwingingState::can_be_hurt(void) {
  return true;
}

/**
 * @brief Returns whether crystal switches can be activated by the sword in this state.
 * @return true if crystal switches can be activated by the sword in this state
 */
bool Hero::SwordSwingingState::can_sword_hit_crystal_switch(void) {
  return true;
}

/**
 * @brief Tests whether the hero is cutting with his sword the specified detector
 * for which a collision was detected.
 * @param detector the detector to check
 * @return true if the sword is cutting this detector
 */
bool Hero::SwordSwingingState::is_cutting_with_sword(Detector *detector) {

  // check the distance to the detector
  int distance = detector->is_obstacle_for(hero) ? 14 : 4;
  Rectangle tested_point = hero->get_facing_point();

  switch (sprites->get_animation_direction()) {

    case 0: // right
      tested_point.add_x(distance);
      break;

    case 1: // up
      tested_point.add_y(-distance);
      break;

    case 2: // left
      tested_point.add_x(-distance);
      break;

    case 3: // down
      tested_point.add_y(distance);
      break;
  }

  return detector->get_bounding_box().contains(tested_point.get_x(), tested_point.get_y());
}

