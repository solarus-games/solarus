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
#include "hero/PushingState.h"
#include "hero/FreeState.h"
#include "hero/GrabbingState.h"
#include "hero/HeroSprites.h"
#include "entities/Detector.h"
#include "movements/PathMovement.h"
#include "lowlevel/Rectangle.h"
#include "Game.h"
#include "GameControls.h"

/**
 * @brief Constructor.
 * @param hero the hero controlled by this state
 */
Hero::PushingState::PushingState(Hero &hero):
  State(hero),
  pushed_entity(NULL) {

}

/**
 * @brief Destructor.
 */
Hero::PushingState::~PushingState() {

}

/**
 * @brief Starts this state.
 * @param previous_state the previous state
 */
void Hero::PushingState::start(State *previous_state) {

  State::start(previous_state);

  pushing_direction4 = get_sprites().get_animation_direction();
  get_sprites().set_animation_pushing();
}

/**
 * @brief Stops this state.
 */
void Hero::PushingState::stop(State *next_state) {

  State::stop(next_state);

  if (is_moving_grabbed_entity()) {
    hero.clear_movement();
  }
}

/**
 * @brief Updates this state.
 */
void Hero::PushingState::update() {

  State::update();

  if (!is_moving_grabbed_entity()) { // the hero is pushing a fixed obstacle

    // stop pushing if there is no more obstacle
    if (!hero.is_facing_obstacle()) {
      hero.set_state(new FreeState(hero));
    }

    // stop pushing if the player changes his direction
    else if (get_controls().get_wanted_direction8() != pushing_direction4 * 2) {

      if (get_controls().is_key_pressed(GameControls::ACTION)) {
        hero.set_state(new GrabbingState(hero));
      }
      else {
        hero.set_state(new FreeState(hero));
      }
    }

    // see if the obstacle pushed is an entity that the hero can move
    else {
 
      Detector *facing_entity = hero.get_facing_entity();
      if (facing_entity != NULL) { // the obstacle pushed is an entity

        if (facing_entity->get_type() == BLOCK) { // it can be moved by the hero (TODO dynamic binding)
          hero.try_snap_to_facing_entity();
        }

        if (facing_entity->start_movement_by_hero()) {

          std::string path = "  ";
          path[0] = path[1] = '0' + pushing_direction4 * 2;

          hero.set_movement(new PathMovement(path, 40, false, false, false));
          pushed_entity = facing_entity;
        }
      }
    }
  }
}

/**
 * @brief Returns whether the hero ignores the effect of conveyor belts in this state.
 * @return true if the hero ignores the effect of conveyor belts in this state
 */
bool Hero::PushingState::can_avoid_conveyor_belt() {
  return true;
}

/**
 * @brief Returns whether the hero can swing his sword in this state.
 * @return true if the hero can swing his sword in this state
 */
bool Hero::PushingState::can_start_sword() {
  return !is_moving_grabbed_entity();
}

/**
 * @brief Returns whether the hero is grabbing and moving an entity in this state.
 * @return true if the hero is grabbing and moving an entity
 */
bool Hero::PushingState::is_moving_grabbed_entity() {
  return pushed_entity != NULL;
}

/**
 * @brief Notifies the hero that the entity he is pushing cannot move any more because of a collision.
 */
void Hero::PushingState::notify_grabbed_entity_collision() {

  //std::cout << "stop moving block: the block has reached an obstacle\n";
  stop_moving_pushed_entity();
}

/**
 * @brief Notifies this state that the movement if finished.
 */
void Hero::PushingState::notify_movement_finished() {

  if (is_moving_grabbed_entity()) {
    // the 16 pixels of the path are completed
    //std::cout << "stop moving block: 16 pixels were completed\n";
    stop_moving_pushed_entity();
  }
}

/**
 * @brief Notifies this state that the hero has just failed to change its
 * position because of obstacles.
 */
void Hero::PushingState::notify_obstacle_reached() {

  if (is_moving_grabbed_entity()) {
    // an obstacle is reached before the 16 pixels are completed
    //std::cout << "stop moving block: the hero has reached an obstacle\n";
    stop_moving_pushed_entity();
  }
}

/**
 * @brief Notifies this state that the hero has just changed its
 * position.
 */
void Hero::PushingState::notify_position_changed() {

  if (is_moving_grabbed_entity()) {
    // if the entity has made more than 8 pixels and is aligned on the grid,
    // we stop the movement

    PathMovement *movement = (PathMovement*) hero.get_movement();

    bool horizontal = pushing_direction4 % 2 == 0;
    bool has_reached_grid = movement->get_total_distance_covered() > 8
      && ((horizontal && pushed_entity->is_aligned_to_grid_x())
          || (!horizontal && pushed_entity->is_aligned_to_grid_y()));

    if (has_reached_grid) {
      //std::cout << "stop moving block: the entity has reached the grid\n";
      stop_moving_pushed_entity();
    }
  }
}

/**
 * @brief Makes the hero stop pushing the entity he is facing.
 */
void Hero::PushingState::stop_moving_pushed_entity() {

  if (pushed_entity != NULL) {
    pushed_entity->stop_movement_by_hero();

    // the hero may have moved one or several pixels too much
    // because he moved before the block, not knowing that the block would not follow him

    switch (pushing_direction4) {

      case 0:
        // east
        hero.set_x(pushed_entity->get_x() - 16);
        break;

      case 1:
        // north
        hero.set_y(pushed_entity->get_y() + 16);
        break;

      case 2:
        // west
        hero.set_x(pushed_entity->get_x() + 16);
        break;

      case 3:
        // south
        hero.set_y(pushed_entity->get_y() - 16);
        break;
    }
    pushed_entity = NULL;
  }

  hero.clear_movement();

  if (!get_controls().is_key_pressed(GameControls::ACTION)) {
    // the hero was pushing an entity without grabbing it

    // stop the animation pushing if his direction changed
    if (get_controls().get_wanted_direction8() != pushing_direction4 * 2) {
      hero.set_state(new FreeState(hero));
    }
  }
  else {
    hero.set_state(new GrabbingState(hero));
  }
}

/**
 * @brief Returns whether the hero can be hurt in this state.
 * @param attacker an attacker that is trying to hurt the hero
 * (or NULL if the source of the attack is not an enemy)
 * @return true if the hero can be hurt in this state
 */
bool Hero::PushingState::can_be_hurt(Enemy* attacker) {
  return !is_moving_grabbed_entity();
}

/**
 * @brief Returns whether a deep water tile is considered as an obstacle in this state.
 * @return true if the deep water tiles are considered as obstacles in this state
 */
bool Hero::PushingState::is_deep_water_obstacle() {
  return true;
}

/**
 * @brief Returns whether a hole is considered as an obstacle in this state.
 * @return true if the holes are considered as obstacles in this state
 */
bool Hero::PushingState::is_hole_obstacle() {
  return true;
}

/**
 * @brief Returns whether lava is considered as an obstacle in this state.
 * @return true if lava is considered as obstacles in this state
 */
bool Hero::PushingState::is_lava_obstacle() {
  return true;
}

/**
 * @brief Returns whether prickles are considered as an obstacle in this state.
 * @return true if prickles are considered as obstacles in this state
 */
bool Hero::PushingState::is_prickle_obstacle() {
  return true;
}

