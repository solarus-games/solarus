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
#include "hero/BackToSolidGroundState.h"
#include "hero/FreeState.h"
#include "hero/HeroSprites.h"
#include "entities/MapEntities.h"
#include "movements/TargetMovement.h"
#include "lowlevel/Rectangle.h"
#include "Map.h"

/**
 * @brief Constructor.
 * @param hero the hero controlled by this state
 * @param use_memorized_xy true to get back to the place previously memorized (if any),
 * false to get back to the last coordinates with solid ground
 */
Hero::BackToSolidGroundState::BackToSolidGroundState(Hero *hero, bool use_memorized_xy):
  State(hero) {

  if (use_memorized_xy && hero->target_solid_ground_coords.get_x() != -1) {
    // go back to a target point specified earlier
    this->target_xy.set_xy(hero->target_solid_ground_coords);
    this->target_layer = hero->target_solid_ground_layer;
  }
  else {
    // just go back to the last solid ground location
    this->target_xy.set_xy(hero->last_solid_ground_coords);
    this->target_layer = hero->last_solid_ground_layer;
  }

}

/**
 * @brief Destructor.
 */
Hero::BackToSolidGroundState::~BackToSolidGroundState() {

}

/**
 * @brief Starts this state.
 * @param previous_state the previous state
 */
void Hero::BackToSolidGroundState::start(State *previous_state) {

  State::start(previous_state);

  hero->set_movement(new TargetMovement(target_xy.get_x(), target_xy.get_y(), hero->get_walking_speed()));
  map->get_entities().set_entity_layer(hero, target_layer);
  map->get_entities().remove_boomerang();
}

/**
 * @brief Stops this state.
 * @param next_state the next state
 */
void Hero::BackToSolidGroundState::stop(State *next_state) {

  State::stop(next_state);

  hero->clear_movement();
}

/**
 * @brief Updates this state.
 */
void Hero::BackToSolidGroundState::update() {

  State::update();

  // the current movement is an instance of TargetMovement
  if (hero->get_movement()->is_finished()) {
    sprites->blink();
    hero->set_state(new FreeState(hero));
  }
}

/**
 * @brief Returns whether the game over sequence can start in the current state.
 * @return true if the game over sequence can start in the current state
 */
bool Hero::BackToSolidGroundState::can_start_gameover_sequence() {
  return false;
}

/**
 * @brief Returns whether the hero is visible in the current state.
 * @return true if the hero is displayed in the current state
 */
bool Hero::BackToSolidGroundState::is_hero_visible() {
  return false;
}

/**
 * @brief Returns whether the hero ignores the effect of deep water in this state.
 * @return true if the hero ignores the effect of deep water in the current state
 */
bool Hero::BackToSolidGroundState::can_avoid_deep_water() {
  return true;
}

/**
 * @brief Returns whether the hero ignores the effect of holes in this state.
 * @return true if the hero ignores the effect of holes in the current state
 */
bool Hero::BackToSolidGroundState::can_avoid_hole() {
  return true;
}

/**
 * @brief Returns whether the hero is touching the ground in the current state.
 * @return true if the hero is touching the ground in the current state
 */
bool Hero::BackToSolidGroundState::is_touching_ground() {
  return false;
}

/**
 * @brief Returns whether this state ignores the collisions with the detectors and the ground.
 * @return true if the collision are ignored
 */
bool Hero::BackToSolidGroundState::are_collisions_ignored() {
  return true;
}

/**
 * @brief Returns whether the hero ignores the effect of conveyor belts in this state.
 * @return true if the hero ignores the effect of conveyor belts in this state
 */
bool Hero::BackToSolidGroundState::can_avoid_conveyor_belt() {
  return true;
}

/**
 * @brief Returns whether the hero ignores the effect of sensors in this state.
 * @return true if the hero ignores the effect of sensors in this state
 */
bool Hero::BackToSolidGroundState::can_avoid_sensor() {
  return true;
}

/**
 * @brief Returns whether the hero ignores the effect of explosions in this state.
 * @return true if the hero ignores the effect of explosions in this state
 */
bool Hero::BackToSolidGroundState::can_avoid_explosion() {
  return true;
}

