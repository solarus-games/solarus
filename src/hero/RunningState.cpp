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
#include "hero/RunningState.h"
#include "hero/FreeState.h"
#include "hero/HeroSprites.h"
#include "entities/Detector.h"
#include "movements/StraightMovement.h"
#include "movements/JumpMovement.h"
#include "lowlevel/System.h"
#include "Game.h"
#include "GameControls.h"
#include "Map.h"
#include "Equipment.h"

/**
 * @brief Constructor.
 * @param hero the hero controlled by this state
 */
Hero::RunningState::RunningState(Hero *hero):
  State(hero) {

}

/**
 * @brief Destructor.
 */
Hero::RunningState::~RunningState(void) {

}

/**
 * @brief Starts this state.
 * @param previous_state the previous state
 */
void Hero::RunningState::start(State *previous_state) {

  State::start(previous_state);

  sprites->set_animation_prepare_running();

  phase = 0;

  uint32_t now = System::now();
  next_phase_date = now + 500;
  next_sound_date = now + 300;
}

/**
 * @brief Stops this state.
 */
void Hero::RunningState::stop(State *next_state) {

  State::stop(next_state);

  if (phase != 0) {
    hero->clear_movement();
  }
}

/**
 * @brief Updates this state.
 */
void Hero::RunningState::update(void) {

  State::update();

  if (suspended) {
    return;
  }

  uint32_t now = System::now();

  if (!is_bouncing() && now >= next_sound_date) {
    game->play_sound("running");
    next_sound_date = now + 170;
  }

  if (phase == 0) {
    
    if (now >= next_phase_date) {
      hero->set_movement(new StraightMovement(30, sprites->get_animation_direction() * 90, 10000));
      sprites->set_animation_running();
      phase++;
    }
    else if (!is_pressing_running_key()) {
      hero->set_state(new FreeState(hero));
    }
  }
  else if (hero->get_movement()->is_finished()) {
    hero->set_state(new FreeState(hero));
  }
}

/**
 * @brief Notifies this state that the game was just suspended or resumed.
 * @param suspended true if the game is suspended
 */
void Hero::RunningState::set_suspended(bool suspended) {

  State::set_suspended(suspended);

  if (!suspended) {
    uint32_t diff = System::now() - when_suspended;
    next_phase_date += diff;
    next_sound_date += diff;
  }
}

/**
 * @brief Returns whether the hero is boucing after he reached an obstacle during the run.
 * @return true if the hero is bouncing
 */
bool Hero::RunningState::is_bouncing(void) {
  return phase == 2;
}

/**
 * @brief Returns whether the hero is pressing the item key corresponding to the running shoes.
 * @return true if the hero is pressing the item key corresponding to the running shoes
 */
bool Hero::RunningState::is_pressing_running_key(void) {

  Equipment *equipment = game->get_equipment();
  int slot = equipment->get_item_slot("speed_shoes");
  if (slot == -1) {
    return false;
  }

  GameControls::GameKey key = (slot == 0) ? GameControls::ITEM_1 : GameControls::ITEM_2;
  return game->get_controls()->is_key_pressed(key);
}

/**
 * @brief Notifies this state that a directional key was just pressed.
 * @param direction4 direction of the key (0 to 3)
 */
void Hero::RunningState::directional_key_pressed(int direction4) {

  if (!is_bouncing()
      && direction4 != sprites->get_animation_direction()) {
    hero->set_state(new FreeState(hero));
  }
}

/**
 * @brief Notifies this state that the hero has just tried to change his position.
 * @param success true if the position has actually just changed
 */
void Hero::RunningState::notify_movement_tried(bool success) {

  State::notify_movement_tried(success);

  if (!success && phase == 1) {
    int opposite_direction = (sprites->get_animation_direction8() + 4) % 8;
    hero->set_movement(new JumpMovement(opposite_direction, 32, false, 15));
    sprites->set_animation_hurt();
    game->play_sound("explosion");
    phase++;
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
int Hero::RunningState::get_wanted_movement_direction8(void) {
  return sprites->get_animation_direction8();
}

/**
 * @brief Returns whether the hero can take stairs in this state.
 * If false is returned, stairs have no effect (but they are obstacle for the hero).
 * @return true if the hero ignores the effect of stairs in this state
 */
bool Hero::RunningState::can_take_stairs(void) {
  return !is_bouncing();
}
/**
 * @brief Returns whether can trigger a jump sensor in this state.
 * If false is returned, jump sensors have no effect (but they are obstacle for the hero).
 * @return true if the hero can use jump sensors in this state
 */
bool Hero::RunningState::can_take_jump_sensor(void) {
  return !is_bouncing();
}

/**
 * @brief Returns whether the hero can be hurt in this state.
 * @return true if the hero can be hurt in this state
 */
bool Hero::RunningState::can_be_hurt(void) {
  return !is_bouncing();
}

/**
 * @brief Returns whether the game over sequence can start in the current state.
 * @return true if the game over sequence can start in the current state
 */
bool Hero::RunningState::can_start_gameover_sequence(void) {
  return !is_bouncing();
}

/**
 * @brief When a shadow is displayed separate from the tunic sprite,
 * returns the height where the tunic sprite should be displayed.
 * @return the height in pixels, or zero if there is no separate shadow in this state
 */
int Hero::RunningState::get_height_above_shadow(void) {

  if (is_bouncing()) {
    return ((JumpMovement*) hero->get_movement())->get_jump_height();
  }

  return 0;
}

/**
 * @brief Returns whether the hero is touching the ground in the current state.
 * @return true if the hero is touching the ground in the current state
 */
bool Hero::RunningState::is_touching_ground(void) {
  return !is_bouncing();
}

/**
 * @brief Returns whether the hero ignores the effect of deep water in this state.
 * @return true if the hero ignores the effect of deep water in the current state
 */
bool Hero::RunningState::can_avoid_deep_water(void) {
  return is_bouncing();
}

/**
 * @brief Returns whether the hero ignores the effect of holes in this state.
 * @return true if the hero ignores the effect of holes in the current state
 */
bool Hero::RunningState::can_avoid_hole(void) {
  return is_bouncing();
}

/**
 * @brief Returns whether the hero ignores the effect of teletransporters in this state.
 * @return true if the hero ignores the effect of teletransporters in this state
 */
bool Hero::RunningState::can_avoid_teletransporter(void) {
  return is_bouncing();
}

/**
 * @brief Returns whether the hero ignores the effect of conveyor belts in this state.
 * @return true if the hero ignores the effect of conveyor belts in this state
 */
bool Hero::RunningState::can_avoid_conveyor_belt(void) {
  return is_bouncing();
}

/**
 * @brief Returns whether a sensor is considered as an obstacle in this state.
 * @param sensor a sensor
 * @return true if the sensor is an obstacle in this state
 */
bool Hero::RunningState::is_sensor_obstacle(Sensor *sensor) {
  return is_bouncing();
}

/**
 * @brief Tests whether the hero is cutting with his sword the specified detector
 * for which a collision was detected.
 * @param detector the detector to check
 * @return true if the sword is cutting this detector
 */
bool Hero::RunningState::is_cutting_with_sword(Detector *detector) {

  // check the distance to the detector
  int distance = 8;
  Rectangle tested_point = hero->get_facing_point();

  switch (sprites->get_animation_direction()) {

    case 0: // right
      tested_point.add_x(distance);
      break;

    case 1: // up
      tested_point.add_y(-distance);
      break;

    case 2: // left
      tested_point.add_x(-distance);
      break;

    case 3: // down
      tested_point.add_y(distance);
      break;
  }

  return detector->get_bounding_box().contains(tested_point.get_x(), tested_point.get_y());
}

/**
 * @brief Returns the damage power of the sword for the current attack.
 * @return the current damage factor of the sword
 */
int Hero::RunningState::get_sword_damage_factor(void) {

  // the damage are multiplied by 2
  return State::get_sword_damage_factor() * 2;
}

