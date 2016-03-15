/*
 * Copyright (C) 2006-2016 Christopho, Solarus - http://www.solarus-games.org
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
#include "solarus/entities/Entities.h"
#include "solarus/entities/Hookshot.h"
#include "solarus/hero/BackToSolidGroundState.h"
#include "solarus/hero/FreeState.h"
#include "solarus/hero/HeroSprites.h"
#include "solarus/hero/HookshotState.h"
#include "solarus/lowlevel/Sound.h"
#include "solarus/Map.h"

namespace Solarus {

/**
 * \brief Constructor.
 * \param hero The hero controlled by this state.
 */
Hero::HookshotState::HookshotState(Hero& hero):
  HeroState(hero, "hookshot"),
  hookshot(nullptr) {

}

/**
 * \brief Starts this state.
 * \param previous_state the previous state
 */
void Hero::HookshotState::start(const State* previous_state) {

  HeroState::start(previous_state);

  get_sprites().set_animation("hookshot");
  hookshot = std::make_shared<Hookshot>(get_entity());
  get_entities().add_entity(hookshot);
}

/**
 * \brief Ends this state.
 * \param next_state the next state (for information)
 */
void Hero::HookshotState::stop(const State* next_state) {

  HeroState::stop(next_state);

  if (!hookshot->is_being_removed()) {
    // the hookshot state was stopped by something other than the hookshot (e.g. an enemy)
    hookshot->remove_from_map();
    get_entity().clear_movement();
  }
}

/**
 * \brief Returns whether the hero is touching the ground in the current state.
 * \return true if the hero is touching the ground in the current state
 */
bool Hero::HookshotState::is_touching_ground() const {
  return false;
}

/**
 * \brief Returns whether the hero ignores the effect of deep water in this state.
 * \return true if the hero ignores the effect of deep water in the current state
 */
bool Hero::HookshotState::can_avoid_deep_water() const {
  return true;
}

/**
 * \brief Returns whether the hero ignores the effect of holes in this state.
 * \return true if the hero ignores the effect of holes in the current state
 */
bool Hero::HookshotState::can_avoid_hole() const {
  return true;
}

/**
 * \brief Returns whether the hero ignores the effect of ice in this state.
 * \return \c true if the hero ignores the effect of ice in the current state.
 */
bool Hero::HookshotState::can_avoid_ice() const {
  return true;
}

/**
 * \brief Returns whether the hero ignores the effect of lava in this state.
 * \return true if the hero ignores the effect of lava in the current state
 */
bool Hero::HookshotState::can_avoid_lava() const {
  return true;
}

/**
 * \brief Returns whether the hero ignores the effect of prickles in this state.
 * \return true if the hero ignores the effect of prickles in the current state
 */
bool Hero::HookshotState::can_avoid_prickle() const {
  return true;
}

/**
 * \brief Returns whether the hero ignores the effect of teletransporters in this state.
 * \return true if the hero ignores the effect of teletransporters in this state
 */
bool Hero::HookshotState::can_avoid_teletransporter() const {
  return true;
}

/**
 * \copydoc Entity::State::can_avoid_stream
 */
bool Hero::HookshotState::can_avoid_stream(const Stream& /* stream */) const {
  return true;
}

/**
 * \brief Returns whether some stairs are considered as obstacle in this state.
 * \param stairs some stairs
 * \return true if the stairs are obstacle in this state
 */
bool Hero::HookshotState::is_stairs_obstacle(const Stairs& /* stairs */) const {

  // allow to fly over stairs covered by water
  return get_entity().get_ground_below() != Ground::DEEP_WATER;
}

/**
 * \brief Returns whether a sensor is considered as an obstacle in this state.
 * \param sensor a sensor
 * \return true if the sensor is an obstacle in this state
 */
bool Hero::HookshotState::is_sensor_obstacle(const Sensor& /* sensor */) const {
  return false;
}

/**
 * \copydoc Entity::State::is_jumper_obstacle
 */
bool Hero::HookshotState::is_jumper_obstacle(
    const Jumper& /* jumper */, const Rectangle& /* candidate_position */) const {
  return false;
}

/**
 * \brief Returns whether the hero ignores the effect of switches in this state.
 * \return true if the hero ignores the effect of switches in this state
 */
bool Hero::HookshotState::can_avoid_switch() const {
  return true;
}

/**
 * \brief Returns whether the hero can be hurt in this state.
 * \param attacker an attacker that is trying to hurt the hero
 * (or nullptr if the source of the attack is not an enemy)
 * \return true if the hero can be hurt in this state
 */
bool Hero::HookshotState::can_be_hurt(Entity* /* attacker */) const {
  return false;
}

/**
 * \brief Returns whether the hero can pick a treasure in this state.
 * \param item The equipment item to obtain.
 * \return true if the hero can pick that treasure in this state.
 */
bool Hero::HookshotState::can_pick_treasure(EquipmentItem& /* item */) const {
  return true;
}

/**
 * \brief Notifies this state that the hero has just failed to change its
 * position because of obstacles.
 */
void Hero::HookshotState::notify_obstacle_reached() {

  // the movement of the hero has finished normally or an early obstacle was
  // reached (e.g. an NPC who moved after the hookshot passed)
  finish_movement();
}

/**
 * \brief Returns control to the hero after its hookshot movement.
 *
 * This function is called when the hero has finished the hookshot movement.
 * It checks the validity of the destination position.
 */
void Hero::HookshotState::finish_movement() {

  Hero& hero = get_entity();
  const Rectangle& hero_position = hero.get_bounding_box();
  int layer = hero.get_layer();
  Map& map = get_map();
  Entities& entities = get_entities();

  if (layer == 0 || !map.has_empty_ground(layer, hero_position)) {
    // the hero is totally on the same layer: no problem
    hero.start_state_from_ground();
  }
  else {
    // a part of the hero is on empty tiles: this is often illegal, especially
    // if there are jumpers; allow this only if tiles on
    // the lower layer are not obstacles, and go to this layer
    --layer;
    if (!map.test_collision_with_obstacles(layer, hero_position, hero)) {
      Sound::play("hero_lands");
      entities.set_entity_layer(hero, layer);
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

}

