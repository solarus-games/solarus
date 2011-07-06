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
#include "hero/ForcedWalkingState.h"
#include "hero/HeroSprites.h"
#include "movements/PathMovement.h"

/**
 * @brief Constructor.
 * @param hero the hero controlled by this state
 * @param path the succession of basic moves
 * composing this movement (each character represents
 * a direction between '0' and '7'), as in PathMovement
 * @param loop true to make the movement return to the beginning
 * once finished
 * @param ignore_obstacles true to make the movement ignore obstacles
 */
Hero::ForcedWalkingState::ForcedWalkingState(Hero &hero, const std::string &path,
    bool loop, bool ignore_obstacles):

  State(hero) {

  this->movement = new PathMovement(path, hero.get_walking_speed(),
      loop, ignore_obstacles, false);
}

/**
 * @brief Destructor.
 */
Hero::ForcedWalkingState::~ForcedWalkingState() {
}

/**
 * @brief Starts this state.
 * @param previous_state the previous state
 */
void Hero::ForcedWalkingState::start(State *previous_state) {

  State::start(previous_state);

  // update the sprites
  HeroSprites &sprites = get_sprites();
  sprites.set_animation_walking_normal();

  // walk
  hero.set_movement(movement);
}

/**
 * @brief Stops this state.
 * @param next_state the next state
 */
void Hero::ForcedWalkingState::stop(State *next_state) {

  State::stop(next_state);

  hero.clear_movement();
}

/**
 * @brief Updates this state.
 */
void Hero::ForcedWalkingState::update() {

  State::update();

  HeroSprites &sprites = hero.get_sprites();
  sprites.set_animation_direction(movement->get_displayed_direction4());

  if (movement->is_finished()) {
    hero.start_state_from_ground();
  }
}

/**
 * @brief Returns whether the game over sequence can start in the current state.
 * @return true if the game over sequence can start in the current state
 */
bool Hero::ForcedWalkingState::can_start_gameover_sequence() {
  return false;
}

/**
 * @brief Returns whether the hero is touching the ground in the current state.
 * @return true if the hero is touching the ground in the current state
 */
bool Hero::ForcedWalkingState::is_touching_ground() {
  return false;
}

/**
 * @brief Returns whether the hero ignores the effect of deep water in this state.
 * @return true if the hero ignores the effect of deep water in the current state
 */
bool Hero::ForcedWalkingState::can_avoid_deep_water() {
  return true;
}

/**
 * @brief Returns whether the hero ignores the effect of holes in this state.
 * @return true if the hero ignores the effect of holes in the current state
 */
bool Hero::ForcedWalkingState::can_avoid_hole() {
  return true;
}

/**
 * @brief Returns whether the hero ignores the effect of lava in this state.
 * @return true if the hero ignores the effect of lava in the current state
 */
bool Hero::ForcedWalkingState::can_avoid_lava() {
  return true;
}

/**
 * @brief Returns whether the hero ignores the effect of prickles in this state.
 * @return true if the hero ignores the effect of prickles in the current state
 */
bool Hero::ForcedWalkingState::can_avoid_prickle() {
  return true;
}

/**
 * @brief Returns whether the hero ignores the effect of teletransporters in this state.
 * @return true if the hero ignores the effect of teletransporters in this state
 */
bool Hero::ForcedWalkingState::can_avoid_teletransporter() {
  return true;
}

/**
 * @brief Returns whether the hero ignores the effect of conveyor belts in this state.
 * @return true if the hero ignores the effect of conveyor belts in this state
 */
bool Hero::ForcedWalkingState::can_avoid_conveyor_belt() {
  return true;
}

/**
 * @brief Returns whether the hero ignores the effect of sensors in this state.
 * @return true if the hero ignores the effect of sensors in this state
 */
bool Hero::ForcedWalkingState::can_avoid_sensor() {
  return true;
}

/**
 * @brief Returns whether the hero ignores the effect of switches in this state.
 * @return true if the hero ignores the effect of switches in this state
 */
bool Hero::ForcedWalkingState::can_avoid_switch() {
  return true;
}

/**
 * @brief Returns whether the hero can be hurt in this state.
 * @return true if the hero can be hurt in this state
 */
bool Hero::ForcedWalkingState::can_be_hurt() {
  return false;
}

