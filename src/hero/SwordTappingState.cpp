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
#include "solarus/hero/SwordTappingState.h"
#include "solarus/hero/SwordLoadingState.h"
#include "solarus/hero/FreeState.h"
#include "solarus/hero/HeroSprites.h"
#include "solarus/entities/Enemy.h"
#include "solarus/lowlevel/System.h"
#include "solarus/lowlevel/Sound.h"
#include "solarus/movements/StraightMovement.h"
#include "solarus/lowlevel/Geometry.h"
#include "solarus/Game.h"
#include "solarus/GameCommands.h"
#include "solarus/Map.h"
#include <memory>
#include <string>

namespace Solarus {

/**
 * \brief Constructor.
 * \param hero The hero controlled by this state.
 */
Hero::SwordTappingState::SwordTappingState(Hero& hero):
  BaseState(hero, "sword tapping"),
  next_sound_date(0) {

}

/**
 * \brief Starts this state.
 * \param previous_state the previous state
 */
void Hero::SwordTappingState::start(const State* previous_state) {

  State::start(previous_state);

  get_sprites().set_animation_sword_tapping();
  next_sound_date = System::now() + 100;
}

/**
 * \brief Ends this state.
 * \param next_state the next state
 */
void Hero::SwordTappingState::stop(const State* next_state) {

  State::stop(next_state);

  Hero& hero = get_entity();
  if (hero.get_movement() != nullptr) {
    // stop the movement of being pushed by an enemy after hitting him
    hero.clear_movement();
  }
}

/**
 * \brief Updates this state.
 */
void Hero::SwordTappingState::update() {

  State::update();

  if (is_suspended()) {
    return;
  }

  Hero& hero = get_entity();
  if (hero.get_movement() == nullptr) {
    // the hero is not being pushed after hitting an enemy

    const Point& facing_point = hero.get_facing_point();

    if (!get_commands().is_command_pressed(GameCommand::ATTACK)
        || get_commands().get_wanted_direction8() != get_sprites().get_animation_direction8()
        || !get_map().test_collision_with_obstacles(hero.get_layer(), facing_point, hero)) {
      // the sword key has been released, the player has moved or the obstacle is gone

      if (get_sprites().get_current_frame() >= 5) {
        // when the animation is ok, stop tapping the wall, go back to loading the sword
        hero.set_state(new SwordLoadingState(hero));
      }
    }
    else {

      // play the sound every 100 ms
      uint32_t now = System::now();
      if (get_sprites().get_current_frame() == 3 && now >= next_sound_date) {

        Detector* facing_entity = hero.get_facing_entity();
        std::string sound_id;
        if (facing_entity != nullptr) {
          sound_id = facing_entity->get_sword_tapping_sound();
        }
        else {
          sound_id = "sword_tapping";
        }
        Sound::play(sound_id);
        next_sound_date = now + 100;
      }
    }
  }
  else if (hero.get_movement()->is_finished()) {
    // the hero was pushed by an enemy
    hero.set_state(new FreeState(hero));
  }
}

/**
 * \brief Notifies this state that the game was just suspended or resumed.
 * \param suspended true if the game is suspended
 */
void Hero::SwordTappingState::set_suspended(bool suspended) {

  State::set_suspended(suspended);

  if (!suspended) {
    next_sound_date += System::now() - get_when_suspended();
  }
}

/**
 * \brief Returns whether crystals can be activated by the sword in this state.
 * \return true if crystals can be activated by the sword in this state
 */
bool Hero::SwordTappingState::can_sword_hit_crystal() const {
  return true;
}

/**
 * \brief Returns whether the hero can pick a treasure in this state.
 * \param item The equipment item to obtain.
 * \return true if the hero can pick that treasure in this state.
 */
bool Hero::SwordTappingState::can_pick_treasure(EquipmentItem& /* item */) const {
  return true;
}

/**
 * \copydoc Entity::State::can_use_shield
 */
bool Hero::SwordTappingState::can_use_shield() const {
  return false;
}

/**
 * \brief Tests whether the hero is cutting with his sword the specified detector
 * for which a collision was detected.
 * \param detector the detector to check
 * \return true if the sword is cutting this detector
 */
bool Hero::SwordTappingState::is_cutting_with_sword(Detector& detector) {

  Hero& hero = get_entity();
  return detector.is_obstacle_for(hero)         // only obstacle entities can be cut
    && hero.get_facing_entity() == &detector    // only one entity at a time
    && get_sprites().get_current_frame() >= 3;  // wait until the animation shows an appropriate frame
}

/**
 * \brief Returns whether a teletransporter is considered as an obstacle in this state.
 * \param teletransporter a teletransporter
 * \return true if the teletransporter is an obstacle in this state
 */
bool Hero::SwordTappingState::is_teletransporter_obstacle(
    const Teletransporter& /* teletransporter */) const {

  // if the hero was pushed by an enemy, don't go on a teletransporter
  return get_entity().get_movement() != nullptr;
}

/**
 * \brief Notifies this state that the hero has just failed to change its
 * position because of obstacles.
 */
void Hero::SwordTappingState::notify_obstacle_reached() {

  // the hero reached an obstacle while being pushed after hitting an enemy
  Hero& hero = get_entity();
  hero.clear_movement();
  hero.set_state(new FreeState(hero));
}

/**
 * \copydoc Entity::State::notify_attacked_enemy
 */
void Hero::SwordTappingState::notify_attacked_enemy(
    EnemyAttack attack,
    Enemy& victim,
    const Sprite* victim_sprite,
    EnemyReaction::Reaction& result,
    bool /* killed */) {

  if (result.type != EnemyReaction::ReactionType::IGNORED && attack == EnemyAttack::SWORD) {

    if (victim.get_push_hero_on_sword()) {

      Hero& hero = get_entity();
      double angle = victim.get_angle(hero, victim_sprite, nullptr);
      std::shared_ptr<StraightMovement> movement =
          std::make_shared<StraightMovement>(false, true);
      movement->set_max_distance(24);
      movement->set_speed(120);
      movement->set_angle(angle);
      hero.set_movement(movement);
      get_sprites().set_animation_walking_normal();
    }
  }
}

}

