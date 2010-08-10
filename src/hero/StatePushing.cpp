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
#include "hero/StatePushing.h"
#include "hero/StateGrabbing.h"
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
Hero::StatePushing::StatePushing(Hero *hero):
  StatePlayerMovement(hero) {

}

/**
 * @brief Destructor.
 */
Hero::StatePushing::~StatePushing(void) {

}

/**
 * @brief Starts this state.
 */
void Hero::StatePushing::start(State *previous_state) {

  StatePlayerMovement::start(previous_state);

  sprites->set_animation_pushing();
  pushing_direction = sprites->get_animation_direction();
  grabbed_entity = NULL;
}

/**
 * @brief Stops this state.
 */
void Hero::StatePushing::stop(State *next_state) {

  StatePlayerMovement::stop(next_state);

  if (is_moving_grabbed_entity()) {
    hero->clear_movement();
  }
}

/**
 * @brief Returns whether the player can control his movements in the current state.
 * @return true if the player can control his movements
 */
bool Hero::StatePushing::can_control_movement(void) {
  return false;
}

/**
 * @brief Returns whether the animation direction is locked.
 * @return true if the animation direction is locked
 */
bool Hero::StatePushing::is_direction_locked(void) {
  return true;
}

/**
 * @brief Returns whether the hero ignores the effect of conveyor belts in this state.
 * @return true if the hero ignores the effect of conveyor belts in this state
 */
bool Hero::StatePushing::can_avoid_conveyor_belt(void) {
  return true;
}

/**
 * @brief Returns whether the hero can swing his sword in this state.
 * @return true if the hero can swing his sword in this state
 */
bool Hero::StatePushing::can_start_sword(void) {
  return true;
}

/**
 * @brief Notifies this state of the result of the current movement.
 * @param tried_to_move true if the hero tried to change his position during this cycle
 * @param success true if the position has actually just changed
 */
void Hero::StatePushing::notify_movement_result(bool tried_to_move, bool success) {

  StatePlayerMovement::notify_movement_result(tried_to_move, success);

  if (is_moving_grabbed_entity()) {
    // detect when the hero movement is finished
    // because the hero has covered 16 pixels, has reached an obstacle or has aligned the entity on the grid

    PathMovement *movement = (PathMovement*) hero->get_movement();

    bool horizontal = hero->get_animation_direction() % 2 == 0;
    bool has_reached_grid = movement->get_total_distance_covered() > 8
      && ((horizontal && hero->is_x_aligned_to_grid()) || (!horizontal && hero->is_y_aligned_to_grid()));

    if (movement->is_finished() || has_reached_grid) {
      stop_moving_grabbed_entity();
    }
  }
  else {

    // stop pushing if there is no more obstacle
    if (!hero->is_facing_obstacle()) {
      hero->start_free();
    }

    // stop pushing if the player changes his direction
    else if (get_wanted_movement_direction() != sprites->get_animation_direction() * 90) {

      if (game->get_controls()->is_key_pressed(GameControls::ACTION)) {
        hero->set_state(new StateGrabbing(hero));
      }
      else {
	hero->start_free();
      }
    }

    // see if the obstacle is an entity that the hero can push
    else {
 
      Detector *facing_entity = hero->get_facing_entity();
      if (facing_entity != NULL && grabbed_entity == NULL) {

	if (facing_entity->get_type() == BLOCK) {
	  hero->try_snap_to_facing_entity();
	}

	if (facing_entity->moved_by_hero()) {

	  std::string path = "  ";
	  int direction = sprites->get_animation_direction();
	  path[0] = path[1] = '0' + direction * 2;

	  hero->set_movement(new PathMovement(path, 8, false, true, false));
	  grabbed_entity = facing_entity;
	}
      }
    }
  }
}

/**
 * @brief Returns whether the hero is grabbing and moving an entity in this state.
 * @return true if the hero is grabbing and moving an entity
 */
bool Hero::StatePushing::is_moving_grabbed_entity(void) {
  return grabbed_entity != NULL;
}

/**
 * @brief Notifies the hero that the entity he is pushing cannot move any more because of a collision.
 */
void Hero::StatePushing::notify_grabbed_entity_collision(void) {

  // the hero has moved one pixel too much
  // because he moved before the block, not knowing that the block would not follow him

  static const Rectangle opposite_dxy[] = {
    Rectangle(-1, 0),
    Rectangle( 0, 1),
    Rectangle( 1, 0),
    Rectangle( 0,-1)
  };

  // go back one pixel into the opposite direction
  int direction = sprites->get_animation_direction();
  Rectangle bounding_box = hero->get_bounding_box();
  bounding_box.add_xy(opposite_dxy[direction]);
  hero->set_xy(bounding_box);

  stop_moving_grabbed_entity();
}

/**
 * @brief Makes the hero stop pushing the entity he is grabbing.
 *
 * This function is called while moving the entity, when the 
 * hero or the entity collides with an obstacle or when
 * the hero's movement is finished.
 */
void Hero::StatePushing::stop_moving_grabbed_entity(void) {

  if (is_moving_grabbed_entity()) {
    hero->clear_movement();
  }

  GameControls *controls = game->get_controls();
  if (!controls->is_key_pressed(GameControls::ACTION)) {

    // the hero was pushing an entity without grabbing it
    grabbed_entity = NULL;

    // stop the animation pushing if his direction changed
    if (get_wanted_movement_direction() != sprites->get_animation_direction() * 90) {
      hero->start_free();
    }
  }
  else {
    hero->set_state(new StateGrabbing(hero));
  }
}

