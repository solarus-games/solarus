/*
 * Copyright (C) 2006-2018 Christopho, Solarus - http://www.solarus-games.org
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
#include "solarus/core/Game.h"
#include "solarus/core/GameCommands.h"
#include "solarus/hero/FreeState.h"
#include "solarus/hero/GrabbingState.h"
#include "solarus/hero/HeroSprites.h"
#include "solarus/hero/PushingState.h"
#include "solarus/movements/PathMovement.h"
#include <string>

namespace Solarus {

/**
 * \brief Constructor.
 * \param hero The hero controlled by this state.
 */
Hero::PushingState::PushingState(Hero& hero):
  HeroState(hero, "pushing"),
  pushing_direction4(0),
  pushed_entity(nullptr),
  pushing_movement(nullptr) {

}

/**
 * \brief Starts this state.
 * \param previous_state the previous state
 */
void Hero::PushingState::start(const State* previous_state) {

  HeroState::start(previous_state);

  pushing_direction4 = get_sprites().get_animation_direction();
  get_sprites().set_animation_pushing();
}

/**
 * \brief Stops this state.
 */
void Hero::PushingState::stop(const State* next_state) {

  HeroState::stop(next_state);

  if (is_moving_grabbed_entity()) {
    get_entity().clear_movement();
    pushed_entity->update();
    stop_moving_pushed_entity();
  }
}

/**
 * \brief Updates this state.
 */
void Hero::PushingState::update() {

  HeroState::update();

  Hero& hero = get_entity();
  if (!is_moving_grabbed_entity()) { // the hero is pushing a fixed obstacle

    // stop pushing if there is no more obstacle
    if (!hero.is_facing_obstacle()) {
      hero.set_state(new FreeState(hero));
    }

    // stop pushing if the player changes his direction
    else if (get_commands().get_wanted_direction8() != pushing_direction4 * 2) {

      if (get_commands().is_command_pressed(GameCommand::ACTION) &&
          hero.can_grab()
      ) {
        hero.start_grabbing();
      }
      else {
        hero.set_state(new FreeState(hero));
      }
    }

    // see if the obstacle pushed is an entity that the hero can move
    else {

      Entity* facing_entity = hero.get_facing_entity();
      if (facing_entity != nullptr) { // the obstacle pushed is an entity

        if (facing_entity->get_type() == EntityType::BLOCK) { // it can be moved by the hero (TODO dynamic binding)
          hero.try_snap_to_facing_entity();
        }

        if (facing_entity->start_movement_by_hero()) {

          std::string path = "  ";
          path[0] = path[1] = '0' + pushing_direction4 * 2;

          pushing_movement = std::make_shared<PathMovement>(
              path, 40, false, false, false
          );
          hero.set_movement(pushing_movement);
          pushed_entity = facing_entity;
          pushed_entity->notify_moving_by(hero);
        }
      }
    }
  }
}

/**
 * \copydoc Entity::State::can_avoid_stream
 */
bool Hero::PushingState::can_avoid_stream(const Stream& /* stream */) const {
  return true;
}

/**
 * \brief Returns whether the hero can swing his sword in this state.
 * \return true if the hero can swing his sword in this state
 */
bool Hero::PushingState::can_start_sword() const {
  return !is_moving_grabbed_entity();
}

/**
 * \brief Returns whether the hero is grabbing and moving an entity in this state.
 * \return true if the hero is grabbing and moving an entity
 */
bool Hero::PushingState::is_moving_grabbed_entity() const {
  return pushed_entity != nullptr;
}

/**
 * \brief Notifies the hero that the entity he is pushing cannot move any more because of a collision.
 */
void Hero::PushingState::notify_grabbed_entity_collision() {

  stop_moving_pushed_entity();
}

/**
 * \brief Notifies this state that the movement if finished.
 */
void Hero::PushingState::notify_movement_finished() {

  if (is_moving_grabbed_entity()) {
    // the 16 pixels of the path are completed
    pushed_entity->update();
    stop_moving_pushed_entity();
  }
}

/**
 * \brief Notifies this state that the hero has just failed to change its
 * position because of obstacles.
 */
void Hero::PushingState::notify_obstacle_reached() {

  if (is_moving_grabbed_entity()) {
    // an obstacle is reached before the 16 pixels are completed
    pushed_entity->update();
    stop_moving_pushed_entity();
  }
}

/**
 * \brief Notifies this state that the hero has just changed its
 * position.
 */
void Hero::PushingState::notify_position_changed() {

  if (is_moving_grabbed_entity()) {
    // if the entity has made more than 8 pixels and is aligned on the grid,
    // we stop the movement

    bool horizontal = pushing_direction4 % 2 == 0;
    bool has_reached_grid = pushing_movement->get_total_distance_covered() > 8
      && ((horizontal && pushed_entity->is_aligned_to_grid_x())
          || (!horizontal && pushed_entity->is_aligned_to_grid_y()));

    if (has_reached_grid) {
      stop_moving_pushed_entity();
    }
  }
}

/**
 * \brief Makes the hero stop pushing the entity he is facing.
 */
void Hero::PushingState::stop_moving_pushed_entity() {

  Hero& hero = get_entity();
  if (pushed_entity != nullptr) {
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

    hero.clear_movement();
    pushing_movement = nullptr;
    Entity* entity_just_moved = pushed_entity;
    pushed_entity = nullptr;
    entity_just_moved->notify_moved_by(hero);
  }

  if (!is_current_state()) {
    // Another state is already starting (for example TreasureState).
    // In this case, don't overwrite it.
    return;
  }

  if (get_commands().is_command_pressed(GameCommand::ACTION) &&
      hero.can_grab()
  ) {
    // The hero was pushing an entity and grabbing it.
    hero.start_grabbing();
  }
  else {
    // The hero was pushing an entity without grabbing it.

    // Stop the animation pushing if his direction changed.
    if (get_commands().get_wanted_direction8() != pushing_direction4 * 2) {
      hero.set_state(new FreeState(hero));
    }
  }
}

/**
 * \brief Returns whether the hero can be hurt in this state.
 * \param attacker an attacker that is trying to hurt the hero
 * (or nullptr if the source of the attack is not an enemy)
 * \return true if the hero can be hurt in this state
 */
bool Hero::PushingState::can_be_hurt(Entity* /* attacker */) const {
  return !is_moving_grabbed_entity();
}

/**
 * \brief Returns whether the hero can pick a treasure in this state.
 * \param item The equipment item to obtain.
 * \return true if the hero can pick that treasure in this state.
 */
bool Hero::PushingState::can_pick_treasure(EquipmentItem& /* item */) const {
  return true;
}

/**
 * \brief Returns whether shallow water is considered as an obstacle in this state.
 * \return true if shallow water is considered as an obstacle in this state
 */
bool Hero::PushingState::is_shallow_water_obstacle() const {
  return true;
}

/**
 * \brief Returns whether deep water is considered as an obstacle in this state.
 * \return true if deep water is considered as an obstacle in this state
 */
bool Hero::PushingState::is_deep_water_obstacle() const {
  return true;
}

/**
 * \brief Returns whether a hole is considered as an obstacle in this state.
 * \return true if the holes are considered as obstacles in this state
 */
bool Hero::PushingState::is_hole_obstacle() const {
  return true;
}

/**
 * \brief Returns whether lava is considered as an obstacle in this state.
 * \return true if lava is considered as obstacles in this state
 */
bool Hero::PushingState::is_lava_obstacle() const {
  return true;
}

/**
 * \brief Returns whether prickles are considered as an obstacle in this state.
 * \return true if prickles are considered as obstacles in this state
 */
bool Hero::PushingState::is_prickle_obstacle() const {
  return true;
}

}

