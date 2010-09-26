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
#include "hero/JumpingState.h"
#include "hero/FreeState.h"
#include "hero/HeroSprites.h"
#include "entities/MapEntities.h"
#include "movements/JumpMovement.h"
#include "lowlevel/Sound.h"
#include "Game.h"
#include "Map.h"

/**
 * @brief Constructor.
 * @param hero the hero controlled by this state
 * @param direction8 direction of the jump (0 to 7)
 * @param length length of the jump in pixels
 * @param ignore_obstacles true to make the movement ignore obstacles
 * @param with_sound true to play the "jump" sound
 * @param movement_delay delay between each one-pixel move in the jump movement (0: default)
 * @param layer_after_jump the layer to set when the jump is finished
 * (or LAYER_NB to keep the same layer)
 */
Hero::JumpingState::JumpingState(Hero *hero, int direction8, int length,
    bool ignore_obstacles, bool with_sound, uint32_t movement_delay, Layer layer_after_jump):
  State(hero) {

    this->movement = new JumpMovement(direction8, length, ignore_obstacles);
    this->direction8 = direction8;
    this->with_sound = with_sound;

    if (layer_after_jump == LAYER_NB) {
      layer_after_jump = hero->get_layer();
    }

    this->layer_after_jump = layer_after_jump;
}

/**
 * @brief Destructor.
 */
Hero::JumpingState::~JumpingState() {

}

/**
 * @brief Starts this state.
 * @param previous_state the previous state
 */
void Hero::JumpingState::start(State *previous_state) {

  State::start(previous_state);

  // update the sprites
  sprites->set_animation_direction8(direction8);
  sprites->set_animation_jumping();

  // jump
  hero->set_movement(movement);

  if (with_sound) {
    Sound::play("jump");
  }
  this->layer_after_jump = layer_after_jump;
}

/**
 * @brief Stops this state.
 * @param next_state the next state
 */
void Hero::JumpingState::stop(State *next_state) {

  State::stop(next_state);

  hero->clear_movement();
}

/**
 * @brief Updates this state.
 */
void Hero::JumpingState::update() {

  State::update();

  if (movement->is_finished()) {
    map->get_entities()->set_entity_layer(hero, layer_after_jump);

    if (hero->get_ground() == GROUND_DEEP_WATER) {
      hero->start_deep_water();
    }
    else {
      hero->set_state(new FreeState(hero));
    }
  }
}

/**
 * @brief Returns the direction of the hero's movement as defined by the controls applied by the player
 * and the movements allowed is the current state.
 *
 * If he is not moving, -1 is returned.
 * This direction may be different from the real movement direction because of obstacles.
 *
 * @return the hero's wanted direction between 0 and 7, or -1 if he is stopped
 */
int Hero::JumpingState::get_wanted_movement_direction8() {
  return sprites->get_animation_direction8();
}

/**
 * @brief Returns whether the game over sequence can start in the current state.
 * @return true if the game over sequence can start in the current state
 */
bool Hero::JumpingState::can_start_gameover_sequence() {
  return false;
}

/**
 * @brief When a shadow is displayed separate from the tunic sprite,
 * returns the height where the tunic sprite should be displayed.
 * @return the height in pixels, or zero if there is no separate shadow in this state
 */
int Hero::JumpingState::get_height_above_shadow() {
  return movement->get_jump_height();
}

/**
 * @brief Returns whether the hero is touching the ground in the current state.
 * @return true if the hero is touching the ground in the current state
 */
bool Hero::JumpingState::is_touching_ground() {
  return false;
}

/**
 * @brief Returns whether the hero ignores the effect of deep water in this state.
 * @return true if the hero ignores the effect of deep water in the current state
 */
bool Hero::JumpingState::can_avoid_deep_water() {
  return true;
}

/**
 * @brief Returns whether the hero ignores the effect of holes in this state.
 * @return true if the hero ignores the effect of holes in the current state
 */
bool Hero::JumpingState::can_avoid_hole() {
  return true;
}

/**
 * @brief Returns whether the hero ignores the effect of teletransporters in this state.
 * @return true if the hero ignores the effect of teletransporters in this state
 */
bool Hero::JumpingState::can_avoid_teletransporter() {
  return true;
}

/**
 * @brief Returns whether the hero ignores the effect of conveyor belts in this state.
 * @return true if the hero ignores the effect of conveyor belts in this state
 */
bool Hero::JumpingState::can_avoid_conveyor_belt() {
  return true;
}

/**
 * @brief Returns whether a sensor is considered as an obstacle in this state.
 * @param sensor a sensor
 * @return true if the sensor is an obstacle in this state
 */
bool Hero::JumpingState::is_sensor_obstacle(Sensor *sensor) {

  // we allow small jumps (e.g. falling from a raised crystal switch block)
  // but not jumping with the feather
  return movement->get_length() > 8;
}

/**
 * @brief Returns whether the hero ignores the effect of sensors in this state.
 * @return true if the hero ignores the effect of sensors in this state
 */
bool Hero::JumpingState::can_avoid_sensor() {
  return true; // however, the sensor is an obstacle if the jump length is important
}

/**
 * @brief Returns whether the hero can be hurt in this state.
 * @return true if the hero can be hurt in this state
 */
bool Hero::JumpingState::can_be_hurt() {
  return false;
}

