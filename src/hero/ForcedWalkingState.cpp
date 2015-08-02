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
#include "solarus/hero/ForcedWalkingState.h"
#include "solarus/hero/HeroSprites.h"
#include "solarus/movements/PathMovement.h"

namespace Solarus {

/**
 * \brief Constructor.
 * \param hero the hero controlled by this state
 * \param path the succession of basic moves
 * composing this movement (each character represents
 * a direction between '0' and '7'), as in PathMovement
 * \param loop true to make the movement return to the beginning
 * once finished
 * \param ignore_obstacles true to make the movement ignore obstacles
 */
Hero::ForcedWalkingState::ForcedWalkingState(
    Hero& hero,
    const std::string& path,
    bool loop,
    bool ignore_obstacles):

  State(hero, "forced walking") {

  this->movement = std::make_shared<PathMovement>(
      path, hero.get_walking_speed(), loop, ignore_obstacles, false
  );
}

/**
 * \brief Starts this state.
 * \param previous_state the previous state
 */
void Hero::ForcedWalkingState::start(const State* previous_state) {

  State::start(previous_state);

  // update the sprites
  HeroSprites& sprites = get_sprites();
  sprites.set_animation_walking_normal();

  // walk
  get_hero().set_movement(movement);
}

/**
 * \brief Stops this state.
 * \param next_state the next state
 */
void Hero::ForcedWalkingState::stop(const State* next_state) {

  State::stop(next_state);

  get_hero().clear_movement();
}

/**
 * \brief Updates this state.
 */
void Hero::ForcedWalkingState::update() {

  State::update();

  get_sprites().set_animation_direction(movement->get_displayed_direction4());

  if (movement->is_finished()) {
    get_hero().start_state_from_ground();
  }
}

/**
 * \brief Returns whether the game over sequence can start in the current state.
 * \return true if the game over sequence can start in the current state
 */
bool Hero::ForcedWalkingState::can_start_gameover_sequence() const {
  return false;
}

/**
 * \brief Returns whether the hero is touching the ground in the current state.
 * \return true if the hero is touching the ground in the current state
 */
bool Hero::ForcedWalkingState::is_touching_ground() const {
  return false;
}

/**
 * \brief Returns whether the hero ignores the effect of deep water in this state.
 * \return true if the hero ignores the effect of deep water in the current state
 */
bool Hero::ForcedWalkingState::can_avoid_deep_water() const {
  return true;
}

/**
 * \brief Returns whether the hero ignores the effect of holes in this state.
 * \return true if the hero ignores the effect of holes in the current state
 */
bool Hero::ForcedWalkingState::can_avoid_hole() const {
  return true;
}

/**
 * \brief Returns whether the hero ignores the effect of ice in this state.
 * \return \c true if the hero ignores the effect of ice in the current state.
 */
bool Hero::ForcedWalkingState::can_avoid_ice() const {
  return true;
}

/**
 * \brief Returns whether the hero ignores the effect of lava in this state.
 * \return true if the hero ignores the effect of lava in the current state
 */
bool Hero::ForcedWalkingState::can_avoid_lava() const {
  return true;
}

/**
 * \brief Returns whether the hero ignores the effect of prickles in this state.
 * \return true if the hero ignores the effect of prickles in the current state
 */
bool Hero::ForcedWalkingState::can_avoid_prickle() const {
  return true;
}

/**
 * \brief Returns whether the hero ignores the effect of teletransporters in this state.
 * \return true if the hero ignores the effect of teletransporters in this state
 */
bool Hero::ForcedWalkingState::can_avoid_teletransporter() const {
  return true;
}

/**
 * \copydoc Hero::State::can_avoid_stream
 */
bool Hero::ForcedWalkingState::can_avoid_stream(const Stream& /* stream */) const {
  return true;
}

/**
 * \brief Returns whether the hero ignores the effect of sensors in this state.
 * \return true if the hero ignores the effect of sensors in this state
 */
bool Hero::ForcedWalkingState::can_avoid_sensor() const {
  return true;
}

/**
 * \brief Returns whether the hero ignores the effect of switches in this state.
 * \return true if the hero ignores the effect of switches in this state
 */
bool Hero::ForcedWalkingState::can_avoid_switch() const {
  return true;
}

/**
 * \brief Returns whether the hero can be hurt in this state
 * \param attacker an attacker that is trying to hurt the hero
 * (or nullptr if the source of the attack is not an enemy).
 * \return true if the hero can be hurt in this state
 */
bool Hero::ForcedWalkingState::can_be_hurt(MapEntity* /* attacker */) const {
  return false;
}

}

