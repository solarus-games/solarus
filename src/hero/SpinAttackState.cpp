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
#include "entities/Enemy.h"
#include "movements/RectilinearMovement.h"
#include "lowlevel/Sound.h"
#include "lowlevel/FileTools.h"
#include "lowlevel/Geometry.h"
#include "Game.h"
#include <sstream>

/**
 * @brief Constructor.
 * @param hero the hero controlled by this state
 */
Hero::SpinAttackState::SpinAttackState(Hero& hero):
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
void Hero::SpinAttackState::start(State* previous_state) {

  State::start(previous_state);

  // play the sound
  play_spin_attack_sound();

  // start the animation
  get_sprites().set_animation_spin_attack();
}

/**
 * @brief Ends this state.
 * @param next_state the next state
 */
void Hero::SpinAttackState::stop(State* next_state) {

  State::stop(next_state);

  if (hero.get_movement() != NULL) {
    // stop the movement of being pushed by an enemy after hitting him
    hero.clear_movement();
  }
}

/**
 * @brief Updates this state.
 */
void Hero::SpinAttackState::update() {

  // check the animation
  if (get_sprites().is_animation_finished()) {
    hero.set_state(new FreeState(hero));
  }

  // check the movement if any
  if (hero.get_movement() != NULL && hero.get_movement()->is_finished()) {
    hero.clear_movement();
  }
}

/**
 * @brief Returns whether crystals can be activated by the sword in this state.
 * @return true if crystals can be activated by the sword in this state
 */
bool Hero::SpinAttackState::can_sword_hit_crystal() {
  return true;
}

/**
 * @brief Returns whether the hero can be hurt in this state.
 * @return true if the hero can be hurt in this state
 * @param attacker an attacker that is trying to hurt the hero
 * (or NULL if the source of the attack is not an enemy)
 */
bool Hero::SpinAttackState::can_be_hurt(Enemy* attacker) {
  return true;
}

/**
 * @brief Tests whether the hero is cutting with his sword the specified detector
 * for which a collision was detected.
 * @param detector the detector to check
 * @return true if the sword is cutting this detector
 */
bool Hero::SpinAttackState::is_cutting_with_sword(Detector& detector) {

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

/**
 * @brief Returns whether a teletransporter is considered as an obstacle in this state.
 * @param teletransporter a teletransporter
 * @return true if the teletransporter is an obstacle in this state
 */
bool Hero::SpinAttackState::is_teletransporter_obstacle(Teletransporter& teletransporter) {

  // if the hero was pushed by an enemy, don't go on a teletransporter
  return hero.get_movement() != NULL;
}

/**
 * @brief Notifies this state that the hero has just failed to change its
 * position because of obstacles.
 *
 * This function is called only when the game is not suspended.
 */
void Hero::SpinAttackState::notify_obstacle_reached() {

  // the hero reached an obstacle while being pushed after hitting an enemy
  hero.clear_movement();
}

/**
 * @brief Notifies this state that the hero has just attacked an enemy.
 * @param attack the attack
 * @param victim the enemy just hurt
 * @param result indicates how the enemy has reacted to the attack (see Enemy.h)
 * @param killed indicates that the attack has just killed the enemy
 */
void Hero::SpinAttackState::notify_attacked_enemy(EnemyAttack attack, Enemy& victim,
    EnemyReaction::Reaction& result, bool killed) {

  if (result.type != EnemyReaction::IGNORED && attack == ATTACK_SWORD) {

    if (victim.get_push_hero_on_sword()) {

      double angle = Geometry::get_angle(victim.get_x(), victim.get_y(),
          hero.get_x(), hero.get_y());
      RectilinearMovement* movement = new RectilinearMovement(false, true);
      movement->set_max_distance(24);
      movement->set_speed(120);
      movement->set_angle(angle);
      hero.set_movement(movement);
    }
  }
}

