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
#include "hero/SpinAttackState.h"
#include "hero/FreeState.h"
#include "hero/HeroSprites.h"
#include "lowlevel/Sound.h"
#include "lowlevel/FileTools.h"
#include "Game.h"
#include <sstream>

/**
 * @brief Constructor.
 * @param hero the hero controlled by this state
 */
Hero::SpinAttackState::SpinAttackState(Hero &hero):
  State(hero) {

}

/**
 * @brief Destructor.
 */
Hero::SpinAttackState::~SpinAttackState() {

}

/**
 * @brief Starts this state.
 * @param previous_state the previous state
 */
void Hero::SpinAttackState::start(State *previous_state) {

  State::start(previous_state);

  // play the sound
  play_spin_attack_sound();

  // start the animation
  get_sprites().set_animation_spin_attack();
}

/**
 * @brief Updates this state.
 */
void Hero::SpinAttackState::update() {

  if (get_sprites().is_animation_finished()) {
    hero.set_state(new FreeState(hero));
  }
}

/**
 * @brief Returns whether crystal switches can be activated by the sword in this state.
 * @return true if crystal switches can be activated by the sword in this state
 */
bool Hero::SpinAttackState::can_sword_hit_crystal_switch() {
  return true;
}

/**
 * @brief Returns whether the hero can be hurt in this state.
 * @return true if the hero can be hurt in this state
 */
bool Hero::SpinAttackState::can_be_hurt() {
  return true;
}

/**
 * @brief Tests whether the hero is cutting with his sword the specified detector
 * for which a collision was detected.
 * @param detector the detector to check
 * @return true if the sword is cutting this detector
 */
bool Hero::SpinAttackState::is_cutting_with_sword(Detector &detector) {

  // during a spin attack, any sprite collision can cut things
  return true;
}

/**
 * @brief Returns the damage power of the sword for the current attack.
 * @return the current damage factor of the sword
 */
int Hero::SpinAttackState::get_sword_damage_factor() {

  // the damage are multiplied by 2
  return State::get_sword_damage_factor() * 2;
}

/**
 * @brief Plays the sword loading sound.
 */
void Hero::SpinAttackState::play_spin_attack_sound() {

  std::ostringstream oss;
  oss << "sword_spin_attack_release_" << get_equipment().get_ability("sword");
  std::string custom_sound_name = oss.str();
  if (Sound::exists(custom_sound_name)) {
    Sound::play(custom_sound_name); // this particular sword has a spin attack sound effect
  }
  else {
    Sound::play("sword_spin_attack_release");
  }
}

