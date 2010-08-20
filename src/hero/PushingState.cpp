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
Hero::PushingState::PushingState(Hero *hero):
  State(hero) {

}

/**
 * @brief Destructor.
 */
Hero::PushingState::~PushingState(void) {

}

/**
 * @brief Starts this state.
 * @param previous_state the previous state
 */
void Hero::PushingState::start(State *previous_state) {

  State::start(previous_state);

  pushed_entity = NULL;
  pushing_direction4 = sprites->get_animation_direction();
  sprites->set_animation_pushing();
}

/**
 * @brief Stops this state.
 */
void Hero::PushingState::stop(State *next_state) {

  State::stop(next_state);

  if (is_moving_grabbed_entity()) {
    hero->clear_movement();
  }
}

/**
 * @brief Updates this state.
 */
void Hero::PushingState::update(void) {

  State::update();

  if (is_moving_grabbed_entity()) { // the hero is pushing an entity and is currently moving it (typically a block)

    // detect when the hero movement is finished
    // because the hero has covered 16 pixels, has reached an obstacle or has aligned the entity on the grid

    PathMovement *movement = (PathMovement*) hero->get_movement();

    bool horizontal = pushing_direction4 % 2 == 0;
    bool has_reached_grid = movement->get_total_distance_covered() > 8
      && ((horizontal && hero->is_x_aligned_to_grid()) || (!horizontal && hero->is_y_aligned_to_grid()));

    if (movement->is_finished() || has_reached_grid) {
      stop_moving_pushed_entity();
    }
  }
  else { // the hero is pushing a fixed obstacle

    GameControls *controls = game->get_controls();

    // stop pushing if there is no more obstacle
    if (!hero->is_facing_obstacle()) {
      hero->set_state(new FreeState(hero));
    }

    // stop pushing if the player changes his direction
    else if (controls->get_wanted_direction8() != pushing_direction4 * 2) {

      if (controls->is_key_pressed(GameControls::ACTION)) {
	hero->set_state(new GrabbingState(hero));
      }
      else {
	hero->set_state(new FreeState(hero));
      }
    }

    // see if the obstacle pushed is an entity that the hero can move
    else {
 
      Detector *facing_entity = hero->get_facing_entity();
      if (facing_entity != NULL) { // the obstacle pushed is an entity

	if (facing_entity->get_type() == BLOCK) { // it can be moved by the hero (TODO dynamic linking)
	  hero->try_snap_to_facing_entity();
	}

	if (facing_entity->moved_by_hero()) {

	  std::string path = "  ";
	  path[0] = path[1] = '0' + pushing_direction4 * 2;

	  hero->set_movement(new PathMovement(path, 8, false, false, false));
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
bool Hero::PushingState::can_avoid_conveyor_belt(void) {
  return true;
}

/**
 * @brief Returns whether the hero can swing his sword in this state.
 * @return true if the hero can swing his sword in this state
 */
bool Hero::PushingState::can_start_sword(void) {
  return !is_moving_grabbed_entity();
}

/**
 * @brief Returns whether the hero is grabbing and moving an entity in this state.
 * @return true if the hero is grabbing and moving an entity
 */
bool Hero::PushingState::is_moving_grabbed_entity(void) {
  return pushed_entity != NULL;
}

/**
 * @brief Notifies the hero that the entity he is pushing cannot move any more because of a collision.
 */
void Hero::PushingState::notify_grabbed_entity_collision(void) {

  // the hero has moved one pixel too much
  // because he moved before the block, not knowing that the block would not follow him

  static const Rectangle opposite_dxy[] = {
    Rectangle(-1, 0),
    Rectangle( 0, 1),
    Rectangle( 1, 0),
    Rectangle( 0,-1)
  };

  // go back one pixel into the opposite direction
  Rectangle bounding_box = hero->get_bounding_box();
  bounding_box.add_xy(opposite_dxy[pushing_direction4]);
  hero->set_bounding_box(bounding_box);

  stop_moving_pushed_entity();
}

/**
 * @brief Makes the hero stop pushing the entity he is facing.
 *
 * This function is called while moving the entity, when the 
 * hero or the entity collides with an obstacle or when
 * the hero's movement is finished.
 */
void Hero::PushingState::stop_moving_pushed_entity(void) {

  pushed_entity = NULL;
  hero->clear_movement();

  GameControls *controls = game->get_controls();
  if (!controls->is_key_pressed(GameControls::ACTION)) {
    // the hero was pushing an entity without grabbing it

    // stop the animation pushing if his direction changed
    if (controls->get_wanted_direction8() != pushing_direction4 * 2) {
      hero->set_state(new FreeState(hero));
    }
  }
  else {
    hero->set_state(new GrabbingState(hero));
  }
}

/**
 * @brief Returns whether the hero can be hurt in this state.
 * @return true if the hero can be hurt in this state
 */
bool Hero::PushingState::can_be_hurt(void) {
  return !is_moving_grabbed_entity();
}

