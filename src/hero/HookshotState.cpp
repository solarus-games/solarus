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
#include "hero/HookshotState.h"
#include "hero/HeroSprites.h"
#include "entities/MapEntities.h"
#include "entities/Hookshot.h"

/**
 * @brief Constructor.
 * @param hero the hero controlled by this state
 */
Hero::HookshotState::HookshotState(Hero &hero):
  State(hero),
  hookshot(NULL) {

}

/**
 * @brief Destructor.
 */
Hero::HookshotState::~HookshotState() {
}

/**
 * @brief Starts this state.
 * @param previous_state the previous state
 */
void Hero::HookshotState::start(State *previous_state) {

  State::start(previous_state);

  get_sprites().set_animation_hookshot();
  hookshot = new Hookshot(hero);
  get_entities().add_entity(hookshot);
}

/**
 * @brief Ends this state.
 * @param next_state the next state (for information)
 */
void Hero::HookshotState::stop(State *next_state) {

  State::stop(next_state);

  if (!hookshot->is_being_removed()) {
    // the hookshot state was stopped by something other than the hookshot (e.g. an enemy)
    hookshot->remove_from_map();
    hero.clear_movement();
  }
}

/**
 * @brief Returns whether the hero is touching the ground in the current state.
 * @return true if the hero is touching the ground in the current state
 */
bool Hero::HookshotState::is_touching_ground() {
  return false;
}

/**
 * @brief Returns whether the hero ignores the effect of deep water in this state.
 * @return true if the hero ignores the effect of deep water in the current state
 */
bool Hero::HookshotState::can_avoid_deep_water() {
  return true;
}

/**
 * @brief Returns whether the hero ignores the effect of holes in this state.
 * @return true if the hero ignores the effect of holes in the current state
 */
bool Hero::HookshotState::can_avoid_hole() {
  return true;
}

/**
 * @brief Returns whether the hero ignores the effect of lava in this state.
 * @return true if the hero ignores the effect of lava in the current state
 */
bool Hero::HookshotState::can_avoid_lava() {
  return true;
}

/**
 * @brief Returns whether the hero ignores the effect of prickles in this state.
 * @return true if the hero ignores the effect of prickles in the current state
 */
bool Hero::HookshotState::can_avoid_prickle() {
  return true;
}

/**
 * @brief Returns whether the hero ignores the effect of teletransporters in this state.
 * @return true if the hero ignores the effect of teletransporters in this state
 */
bool Hero::HookshotState::can_avoid_teletransporter() {
  return true;
}

/**
 * @brief Returns whether the hero ignores the effect of conveyor belts in this state.
 * @return true if the hero ignores the effect of conveyor belts in this state
 */
bool Hero::HookshotState::can_avoid_conveyor_belt() {
  return true;
}

/**
 * @brief Returns whether some stairs are considered as obstacle in this state.
 * @param stairs some stairs
 * @return true if the stairs are obstacle in this state
 */
bool Hero::HookshotState::is_stairs_obstacle(Stairs& stairs) {
  // allow to fly over stairs covered by water
  return hero.get_ground() != GROUND_DEEP_WATER;
}

/**
 * @brief Returns whether a sensor is considered as an obstacle in this state.
 * @param sensor a sensor
 * @return true if the sensor is an obstacle in this state
 */
bool Hero::HookshotState::is_sensor_obstacle(Sensor& sensor) {
  return true;
}

/**
 * @brief Returns whether a jump sensor is considered as an obstacle in this state.
 * @param jump_sensor a jump sensor
 * @return true if the sensor is an obstacle in this state
 */
bool Hero::HookshotState::is_jump_sensor_obstacle(JumpSensor& jump_sensor) {
  return false;
}

/**
 * @brief Returns whether the hero ignores the effect of switches in this state.
 * @return true if the hero ignores the effect of switches in this state
 */
bool Hero::HookshotState::can_avoid_switch() {
  return true;
}

/**
 * @brief Returns whether the hero can be hurt in this state.
 * @return true if the hero can be hurt in this state
 */
bool Hero::HookshotState::can_be_hurt() {
  return true;
}

