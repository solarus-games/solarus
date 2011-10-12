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
#include "hero/FreeState.h"
#include "hero/HeroSprites.h"
#include "entities/Enemy.h"
#include "movements/TemporalMovement.h"
#include "lowlevel/Geometry.h"
#include "Game.h"
#include "GameControls.h"

/**
 * @brief Constructor.
 * @param hero the hero controlled by this state
 */
Hero::SwordSwingingState::SwordSwingingState(Hero& hero):
  State(hero),
  attacked(false),
  sword_finished(false) {

}

/**
 * @brief Destructor.
 */
Hero::SwordSwingingState::~SwordSwingingState() {

}

/**
 * @brief Starts this state.
 * @param previous_state the previous state
 */
void Hero::SwordSwingingState::start(State* previous_state) {

  State::start(previous_state);

  get_sprites().play_sword_sound();
  get_sprites().set_animation_sword();
  get_equipment().notify_ability_used("sword");
}

/**
 * @brief Ends this state.
 * @param next_state the next state
 */
void Hero::SwordSwingingState::stop(State* next_state) {

  State::stop(next_state);

  if (hero.get_movement() != NULL) {
    // stop the movement of being pushed by an enemy after hitting him
    hero.clear_movement();
  }
}

/**
 * @brief Updates this state.
 */
void Hero::SwordSwingingState::update() {

  State::update();

  // check the animation
  if (get_sprites().is_animation_finished()) {

    sword_finished = true;
    if (hero.get_movement() == NULL) {

      // if the player is still pressing the sword key, start loading the sword
      if (get_controls().is_key_pressed(GameControls::SWORD)
          && !attacked) {
        hero.set_state(new SwordLoadingState(hero));
      }
      else {
        hero.set_state(new FreeState(hero));
      }
    }
    else {
      // the sword animation is finished, but the movement continues
      hero.get_sprites().set_animation_stopped_normal();
    }
  }

  // check the movement if any
  if (hero.get_movement() != NULL && hero.get_movement()->is_finished()) {
    hero.clear_movement();
    if (sword_finished) {

      // if the player is still pressing the sword key, start loading the sword
      if (get_controls().is_key_pressed(GameControls::SWORD)) {
        hero.set_state(new SwordLoadingState(hero));
      }
      else {
        hero.set_state(new FreeState(hero));
      }
    }
  }
}

/**
 * @brief Returns whether the hero can swing his sword in this state.
 * @return true if the hero can swing his sword in this state
 */
bool Hero::SwordSwingingState::can_start_sword() {
  return hero.get_movement() == NULL;
}

/**
 * @brief Returns whether the hero can be hurt in this state.
 * @return true if the hero can be hurt in this state
 * @param attacker an attacker that is trying to hurt the hero
 * (or NULL if the source of the attack is not an enemy)
 */
bool Hero::SwordSwingingState::can_be_hurt(Enemy* attacker) {
  return true;
}

/**
 * @brief Returns whether crystal switches can be activated by the sword in this state.
 * @return true if crystal switches can be activated by the sword in this state
 */
bool Hero::SwordSwingingState::can_sword_hit_crystal_switch() {
  return true;
}

/**
 * @brief Tests whether the hero is cutting with his sword the specified detector
 * for which a collision was detected.
 * @param detector the detector to check
 * @return true if the sword is cutting this detector
 */
bool Hero::SwordSwingingState::is_cutting_with_sword(Detector &detector) {

  if (hero.get_movement() != NULL) {
    return false;
  }

  // check the distance to the detector
  int distance = detector.is_obstacle_for(hero) ? 14 : 4;
  Rectangle tested_point = hero.get_facing_point();

  switch (get_sprites().get_animation_direction()) {

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

  return detector.overlaps(tested_point.get_x(), tested_point.get_y());
}

/**
 * @brief Returns whether a teletransporter is considered as an obstacle in this state.
 * @param teletransporter a teletransporter
 * @return true if the teletransporter is an obstacle in this state
 */
bool Hero::SwordSwingingState::is_teletransporter_obstacle(Teletransporter& teletransporter) {

  // if the hero was pushed by an enemy, don't go on a teletransporter
  return hero.get_movement() != NULL;
}

/**
 * @brief Notifies this state that the hero has just attacked an enemy.
 * @param attack the attack
 * @param victim the enemy just hurt
 * @param result indicates how the enemy has reacted to the attack (see Enemy.h)
 * @param killed indicates that the attack has just killed the enemy
 */
void Hero::SwordSwingingState::notify_attacked_enemy(EnemyAttack attack, Enemy& victim,
    EnemyReaction::Reaction& result, bool killed) {

  if (result.type != EnemyReaction::IGNORED && attack == ATTACK_SWORD) {

    attacked = true;

    if (victim.get_push_hero_on_sword()) {

      double angle = Geometry::get_angle(victim.get_x(), victim.get_y(),
          hero.get_x(), hero.get_y());
      hero.set_movement(new TemporalMovement(120, angle, 200));
    }
  }
}

