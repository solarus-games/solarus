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
#include "hero/FreeState.h"
#include "hero/BackToSolidGroundState.h"
#include "hero/HeroSprites.h"
#include "entities/MapEntities.h"
#include "entities/Hookshot.h"
#include "lowlevel/Sound.h"
#include "Map.h"

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

  get_sprites().set_animation("hookshot", "hookshot");
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
 * @brief Returns whether a jumper is considered as an obstacle in this state.
 * @param jumper a jumper
 * @return true if the sensor is an obstacle in this state
 */
bool Hero::HookshotState::is_jumper_obstacle(Jumper& jumper) {
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
 * @param attacker an attacker that is trying to hurt the hero
 * (or NULL if the source of the attack is not an enemy)
 * @return true if the hero can be hurt in this state
 */
bool Hero::HookshotState::can_be_hurt(Enemy* attacker) {
  return false;
}

/**
 * @brief Notifies this state that the hero has just failed to change its
 * position because of obstacles.
 */
void Hero::HookshotState::notify_obstacle_reached() {

  // the movement of the hero has finished normally or an early obstacle was
  // reached (e.g. an NPC who moved after the hookshot passed)
  finish_movement();
}

/**
 * @brief Returns control to the hero after its hookshot movement.
 *
 * This function is called when the hero has finished the hookshot movement.
 * It checks the validity of the destination position.
 */
void Hero::HookshotState::finish_movement() {

  const Rectangle& hero_position = hero.get_bounding_box();
  Layer layer = hero.get_layer();
  Map& map = get_map();
  MapEntities& entities = get_entities();

  if (layer == LAYER_LOW || !map.has_empty_tiles(layer, hero_position)) {
    // the hero is totally on the same layer: no problem
    hero.start_state_from_ground();
  }
  else {
    // a part of the hero is on empty tiles: this is often illegal, especially
    // if there are jumpers; allow this only if tiles on
    // the lower layer are not obstacles, and go to this layer
    layer = Layer(layer - 1);
    if (!map.test_collision_with_obstacles(layer, hero_position, hero)) {
      Sound::play("hero_lands");
      entities.set_entity_layer(&hero, layer);
      hero.start_state_from_ground();
    }
    else {
      // illegal position: get back to the start point
      // TODO: get back to the closest valid point from the destination instead
      Sound::play("hero_hurt");
      hero.set_state(new BackToSolidGroundState(hero, false, 0, true));
    }
  }
}

