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
#include "solarus/hero/SpinAttackState.h"
#include "solarus/hero/SwordLoadingState.h"
#include "solarus/hero/SwordTappingState.h"
#include "solarus/lowlevel/QuestFiles.h"
#include "solarus/lowlevel/Geometry.h"
#include "solarus/lowlevel/Sound.h"
#include "solarus/lowlevel/System.h"
#include "solarus/Equipment.h"
#include "solarus/Game.h"
#include "solarus/GameCommands.h"
#include <sstream>
#include <string>

namespace Solarus {

/**
 * \brief Constructor.
 * \param hero The hero controlled by this state.
 */
Hero::SwordLoadingState::SwordLoadingState(Hero& hero):
  PlayerMovementState(hero, "sword loading"),
  sword_loaded_date(0),
  sword_loaded(false) {

}

/**
 * \brief Starts this state.
 * \param previous_state the previous state
 */
void Hero::SwordLoadingState::start(const State* previous_state) {

  PlayerMovementState::start(previous_state);

  sword_loaded = false;
  sword_loaded_date = System::now() + 1000;
}

/**
 * \brief Updates this state.
 */
void Hero::SwordLoadingState::update() {

  PlayerMovementState::update();

  if (is_suspended() || !is_current_state()) {
    return;
  }

  uint32_t now = System::now();

  // detect when the sword is loaded (i.e. ready for a spin attack)
  if (!sword_loaded && now >= sword_loaded_date) {
    play_load_sound();
    sword_loaded = true;
  }

  if (!get_commands().is_command_pressed(GameCommand::ATTACK)) {
    // the player has just released the sword key

    // stop loading the sword, go to the normal state or make a spin attack
    Hero& hero = get_hero();
    if (!sword_loaded) {
      // the sword was not loaded yet: go to the normal state
      hero.set_state(new FreeState(hero));
    }
    else {
      // the sword is loaded: release a spin attack
      hero.set_state(new SpinAttackState(hero));
    }
  }
}

/**
 * \brief Notifies this state that the game was just suspended or resumed.
 * \param suspended true if the game is suspended
 */
void Hero::SwordLoadingState::set_suspended(bool suspended) {

  PlayerMovementState::set_suspended(suspended);

  if (!suspended) {
    sword_loaded_date += System::now() - get_when_suspended();
  }
}

/**
 * \brief Notifies this state that the hero has just failed to change its
 * position because of obstacles.
 */
void Hero::SwordLoadingState::notify_obstacle_reached() {

  PlayerMovementState::notify_obstacle_reached();

  Hero& hero = get_hero();
  Detector* facing_entity = hero.get_facing_entity();

  if (hero.is_facing_point_on_obstacle()     // he is really facing an obstacle
      && get_wanted_movement_direction8() == get_sprites().get_animation_direction8()   // he is trying to move towards the obstacle
      && (facing_entity == nullptr || !facing_entity->is_sword_ignored())) {               // the obstacle allows him to tap with his sword

    hero.set_state(new SwordTappingState(hero));
  }
}

/**
 * \copydoc Hero::State::notify_attacked_enemy
 */
void Hero::SwordLoadingState::notify_attacked_enemy(
    EnemyAttack attack,
    Enemy& victim,
    const Sprite* victim_sprite,
    EnemyReaction::Reaction& result,
    bool killed) {

  if (result.type != EnemyReaction::ReactionType::IGNORED && attack == EnemyAttack::SWORD) {

    Hero& hero = get_hero();
    if (victim.get_push_hero_on_sword()) {
      // let SwordTappingState do the job so that no player movement interferes
      State* state = new SwordTappingState(hero);
      hero.set_state(state);
      state->notify_attacked_enemy(attack, victim, victim_sprite, result, killed);
    }
    else {
      // after an attack, stop loading the sword
      hero.set_state(new FreeState(hero));
    }
  }
}

/**
 * \brief Returns whether the animation direction is locked.
 * \return true if the animation direction is locked
 */
bool Hero::SwordLoadingState::is_direction_locked() const {
  return true;
}

/**
 * \brief Returns whether the hero can take stairs in this state.
 * If false is returned, stairs have no effect (but they are obstacle for the hero).
 * \return true if the hero ignores the effect of stairs in this state
 */
bool Hero::SwordLoadingState::can_take_stairs() const {
  return true;
}

/**
 * \brief Returns whether the hero can pick a treasure in this state.
 * \param item The equipment item to obtain.
 * \return true if the hero can pick that treasure in this state.
 */
bool Hero::SwordLoadingState::can_pick_treasure(EquipmentItem& /* item */) const {
  return true;
}

/**
 * \copydoc Hero::State::can_use_shield
 */
bool Hero::SwordLoadingState::can_use_shield() const {
  return false;
}

/**
 * \brief Gives the sprites the animation stopped corresponding to this state.
 */
void Hero::SwordLoadingState::set_animation_stopped() {
  get_sprites().set_animation_stopped_sword_loading();
}

/**
 * \brief Gives the sprites the animation walking corresponding to this state.
 */
void Hero::SwordLoadingState::set_animation_walking() {
  get_sprites().set_animation_walking_sword_loading();
}

/**
 * \brief Plays the sword loading sound.
 */
void Hero::SwordLoadingState::play_load_sound() {

  std::ostringstream oss;
  oss << "sword_spin_attack_load_" << get_equipment().get_ability(Ability::SWORD);
  std::string custom_sound_name = oss.str();
  if (Sound::exists(custom_sound_name)) {
    Sound::play(custom_sound_name); // this particular sword has a custom loading sound effect
  }
  else {
    Sound::play("sword_spin_attack_load");
  }
}

}

