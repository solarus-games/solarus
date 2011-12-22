/*
 * Copyright (C) 2009-2011 Christopho, Solarus - http://www.solarus-engine.org
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
#include "hero/PullingState.h"
#include "hero/GrabbingState.h"
#include "hero/FreeState.h"
#include "hero/HeroSprites.h"
#include "entities/Detector.h"
#include "movements/PathMovement.h"
#include "Game.h"
#include "GameControls.h"

/**
 * @brief Constructor.
 * @param hero the hero controlled by this state
 */
Hero::PullingState::PullingState(Hero &hero):
  State(hero) {

}

/**
 * @brief Destructor.
 */
Hero::PullingState::~PullingState() {

}

/**
 * @brief Starts this state.
 * @param previous_state the previous state
 */
void Hero::PullingState::start(State *previous_state) {

  State::start(previous_state);

  pulled_entity = NULL;
  get_sprites().set_animation_pulling();
}

/**
 * @brief Stops this state.
 */
void Hero::PullingState::stop(State *next_state) {

  State::stop(next_state);

  if (is_moving_grabbed_entity()) {
    hero.clear_movement();
  }
}

/**
 * @brief Updates this state.
 */
void Hero::PullingState::update() {

  State::update();

  if (!is_moving_grabbed_entity()) {

    int wanted_direction8 = get_controls().get_wanted_direction8();
    int opposite_direction8 = (get_sprites().get_animation_direction8() + 4) % 8;

    // stop pulling if the action key is released or if there is no more obstacle
    if (!get_controls().is_key_pressed(GameControls::ACTION)
        || !hero.is_facing_obstacle()) {
      hero.set_state(new FreeState(hero));
    }

    // stop pulling the obstacle if the player changes his direction
    else if (wanted_direction8 != opposite_direction8) {
      hero.set_state(new GrabbingState(hero));
    }

    // see if the obstacle is an entity that the hero can pull
    else {
 
      Detector *facing_entity = hero.get_facing_entity();
      if (facing_entity != NULL) {

        if (facing_entity->get_type() == BLOCK) { // TODO use dynamic binding
          hero.try_snap_to_facing_entity();
        }

        if (facing_entity->start_movement_by_hero()) {

          std::string path = "  ";
          path[0] = path[1] = '0' + opposite_direction8;

          hero.set_movement(new PathMovement(path, 40, false, false, false));
          pulled_entity = facing_entity;
        }
      }
    }
  }
}

/**
 * @brief Returns whether the hero is grabbing or pulling an entity in this state.
 * @return true if the hero is grabbing or pulling an entity
 */
bool Hero::PullingState::is_grabbing_or_pulling() {
  return true;
}

/**
 * @brief Returns whether the hero is grabbing and moving an entity in this state.
 * @return true if the hero is grabbing and moving an entity
 */
bool Hero::PullingState::is_moving_grabbed_entity() {
  return pulled_entity != NULL;
}

/**
 * @brief Notifies the hero that the entity he is pulling cannot move any more because of a collision.
 */
void Hero::PullingState::notify_grabbed_entity_collision() {

  stop_moving_pulled_entity();
}

/**
 * @brief Notifies this state that the movement if finished.
 */
void Hero::PullingState::notify_movement_finished() {

  if (is_moving_grabbed_entity()) {
    // the 16 pixels of the path are completed
    //std::cout << "stop moving block: 16 pixels were completed\n";
    stop_moving_pulled_entity();
  }
}

/**
 * @brief Notifies this state that the hero has just failed to change its
 * position because of obstacles.
 */
void Hero::PullingState::notify_obstacle_reached() {

  if (is_moving_grabbed_entity()) {
    // an obstacle is reached before the 16 pixels are completed
    //std::cout << "stop moving block: the hero has reached an obstacle\n";
    stop_moving_pulled_entity();
  }
}

/**
 * @brief Notifies this state that the hero has just changed its
 * position.
 */
void Hero::PullingState::notify_position_changed() {

  if (is_moving_grabbed_entity()) {
    // if the entity has made more than 8 pixels and is aligned on the grid,
    // we stop the movement

    PathMovement *movement = (PathMovement*) hero.get_movement();

    bool horizontal = get_sprites().get_animation_direction() % 2 == 0;
    bool has_reached_grid = movement->get_total_distance_covered() > 8
      && ((horizontal && pulled_entity->is_aligned_to_grid_x())
          || (!horizontal && pulled_entity->is_aligned_to_grid_y()));

    if (has_reached_grid) {
      //std::cout << "stop moving block: the entity has reached the grid\n";
      stop_moving_pulled_entity();
    }
  }
}

/**
 * @brief Makes the hero stop pulling the entity he is facing.
 *
 * This function is called while moving the entity, when the 
 * hero or the entity collides with an obstacle or when
 * the hero's movement is finished.
 */
void Hero::PullingState::stop_moving_pulled_entity() {

  if (pulled_entity != NULL) {
    pulled_entity->stop_movement_by_hero();

    // the hero may have moved one or several pixels too much
    // because he moved before the block, not knowing that the block would not follow him

    int direction4 = get_sprites().get_animation_direction();
    switch (direction4) {

      case 0:
        // east
        hero.set_x(pulled_entity->get_x() - 16);
        break;

      case 1:
        // north
        hero.set_y(pulled_entity->get_y() + 16);
        break;

      case 2:
        // west
        hero.set_x(pulled_entity->get_x() + 16);
        break;

      case 3:
        // south
        hero.set_y(pulled_entity->get_y() - 16);
        break;
    }
  }
  pulled_entity = NULL;

  hero.clear_movement();
  hero.set_state(new GrabbingState(hero));
}

/**
 * @brief Returns whether the hero can be hurt in this state.
 * @return true if the hero can be hurt in this state
 * @param attacker an attacker that is trying to hurt the hero
 * (or NULL if the source of the attack is not an enemy)
 */
bool Hero::PullingState::can_be_hurt(Enemy* attacker) {
  return !is_moving_grabbed_entity();
}

/**
 * @brief Returns whether a deep water tile is considered as an obstacle in this state.
 * @return true if the deep water tiles are considered as obstacles in this state
 */
bool Hero::PullingState::is_deep_water_obstacle() {
  return true;
}

/**
 * @brief Returns whether a hole is considered as an obstacle in this state.
 * @return true if the holes are considered as obstacles in this state
 */
bool Hero::PullingState::is_hole_obstacle() {
  return true;
}

/**
 * @brief Returns whether lava is considered as an obstacle in this state.
 * @return true if lava is considered as obstacles in this state
 */
bool Hero::PullingState::is_lava_obstacle() {
  return true;
}

/**
 * @brief Returns whether prickles are considered as an obstacle in this state.
 * @return true if prickles are considered as obstacles in this state
 */
bool Hero::PullingState::is_prickle_obstacle() {
  return true;
}

/**
 * @brief Returns whether a conveyor belt is considered as an obstacle in this state.
 * @param conveyor_belt a conveyor belt
 * @return true if the conveyor belt is an obstacle in this state
 */
bool Hero::PullingState::is_conveyor_belt_obstacle(ConveyorBelt& conveyor_belt) {
  return true;
}

