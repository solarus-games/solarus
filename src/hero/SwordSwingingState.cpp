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
#include "solarus/entities/Enemy.h"
#include "solarus/hero/FreeState.h"
#include "solarus/hero/HeroSprites.h"
#include "solarus/hero/SwordLoadingState.h"
#include "solarus/hero/SwordSwingingState.h"
#include "solarus/movements/StraightMovement.h"
#include "solarus/Equipment.h"
#include "solarus/Game.h"
#include "solarus/GameCommands.h"
#include <memory>

namespace Solarus {

/**
 * \brief Constructor.
 * \param hero The hero controlled by this state.
 */
Hero::SwordSwingingState::SwordSwingingState(Hero& hero):
  HeroState(hero, "sword swinging"),
  attacked(false),
  sword_finished(false) {

}

/**
 * \brief Starts this state.
 * \param previous_state the previous state
 */
void Hero::SwordSwingingState::start(const HeroState* previous_state) {

  HeroState::start(previous_state);

  get_sprites().play_sword_sound();
  get_sprites().set_animation_sword();
  get_equipment().notify_ability_used(Ability::SWORD);
}

/**
 * \brief Ends this state.
 * \param next_state the next state
 */
void Hero::SwordSwingingState::stop(const HeroState* next_state) {

  HeroState::stop(next_state);

  Hero& hero = get_hero();
  if (hero.get_movement() != nullptr) {
    // stop the movement of being pushed by an enemy after hitting him
    hero.clear_movement();
  }
}

/**
 * \brief Updates this state.
 */
void Hero::SwordSwingingState::update() {

  HeroState::update();

  // check the animation
  Hero& hero = get_hero();
  if (get_sprites().is_animation_finished()) {

    sword_finished = true;
    if (hero.get_movement() == nullptr) {

      // if the player is still pressing the sword key, start loading the sword
      if (get_commands().is_command_pressed(GameCommand::ATTACK)
          && !attacked) {
        hero.set_state(new SwordLoadingState(hero));
      }
      else {
        hero.set_state(new FreeState(hero));
      }
    }
    else {
      // the sword animation is finished, but the movement continues
      hero.get_hero_sprites().set_animation_stopped_normal();
    }
  }

  // check the movement if any
  if (hero.get_movement() != nullptr && hero.get_movement()->is_finished()) {
    hero.clear_movement();
    if (sword_finished) {
      hero.set_state(new FreeState(hero));
    }
  }
}

/**
 * \brief Returns whether the hero can swing his sword in this state.
 * \return true if the hero can swing his sword in this state
 */
bool Hero::SwordSwingingState::can_start_sword() const {
  return get_hero().get_movement() == nullptr;
}

/**
 * \brief Returns whether the hero can be hurt in this state.
 * \return true if the hero can be hurt in this state
 * \param attacker an attacker that is trying to hurt the hero
 * (or nullptr if the source of the attack is not an enemy)
 */
bool Hero::SwordSwingingState::can_be_hurt(Entity* /* attacker */) const {
  return true;
}

/**
 * \brief Returns whether the hero can pick a treasure in this state.
 * \param item The equipment item to obtain.
 * \return true if the hero can pick that treasure in this state.
 */
bool Hero::SwordSwingingState::can_pick_treasure(EquipmentItem& /* item */) const {
  return true;
}

/**
 * \copydoc HeroState::can_avoid_stream
 */
bool Hero::SwordSwingingState::can_avoid_stream(const Stream& /* stream */) const {
  return true;
}

/**
 * \copydoc HeroState::can_use_shield
 */
bool Hero::SwordSwingingState::can_use_shield() const {
  return false;
}

/**
 * \brief Returns whether crystals can be activated by the sword in this state.
 * \return true if crystals can be activated by the sword in this state
 */
bool Hero::SwordSwingingState::can_sword_hit_crystal() const {
  return true;
}

/**
 * \brief Tests whether the hero is cutting with his sword the specified detector
 * for which a collision was detected.
 * \param detector the detector to check
 * \return true if the sword is cutting this detector
 */
bool Hero::SwordSwingingState::is_cutting_with_sword(
    Detector& detector) {

  Hero& hero = get_hero();
  if (hero.get_movement() != nullptr) {
    return false;
  }

  // check the distance to the detector
  int distance = detector.is_obstacle_for(hero) ? 14 : 4;
  Point tested_point = hero.get_facing_point();

  switch (get_sprites().get_animation_direction()) {

    case 0: // right
      tested_point.x += distance;
      break;

    case 1: // up
      tested_point.y -= distance;
      break;

    case 2: // left
      tested_point.x -= distance;
      break;

    case 3: // down
      tested_point.y += distance;
      break;
  }

  return detector.overlaps(tested_point);
}

/**
 * \brief Returns whether a teletransporter is considered as an obstacle in this state.
 * \param teletransporter a teletransporter
 * \return true if the teletransporter is an obstacle in this state
 */
bool Hero::SwordSwingingState::is_teletransporter_obstacle(
    const Teletransporter& /* teletransporter */) const {

  // if the hero was pushed by an enemy, don't go on a teletransporter
  return get_hero().get_movement() != nullptr;
}

/**
 * \brief Notifies this state that the hero has just failed to change its
 * position because of obstacles.
 */
void Hero::SwordSwingingState::notify_obstacle_reached() {

  // the hero reached an obstacle while being pushed after hitting an enemy
  Hero& hero = get_hero();
  hero.clear_movement();

  if (sword_finished) {
    hero.set_state(new FreeState(hero));
  }
}

/**
 * \copydoc HeroState::notify_attacked_enemy
 */
void Hero::SwordSwingingState::notify_attacked_enemy(
    EnemyAttack attack,
    Enemy& victim,
    const Sprite* victim_sprite,
    EnemyReaction::Reaction& result,
    bool /* killed */) {

  if (result.type != EnemyReaction::ReactionType::IGNORED && attack == EnemyAttack::SWORD) {
    attacked = true;

    if (victim.get_push_hero_on_sword()) {

      Hero& hero = get_hero();
      double angle = victim.get_angle(hero, victim_sprite, nullptr);
      std::shared_ptr<StraightMovement> movement =
          std::make_shared<StraightMovement>(false, true);
      movement->set_max_distance(24);
      movement->set_speed(120);
      movement->set_angle(angle);
      hero.set_movement(movement);
    }
  }
}

}

