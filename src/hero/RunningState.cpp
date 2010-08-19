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
#include "movements/StraightMovement.h"
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

  hero->set_movement(new StraightMovement(10, sprites->get_animation_direction() * 90, 300));
  sprites->set_animation_walking_normal();

  phase = 0;
  next_phase_date = System::now() + 300;
}

/**
 * @brief Stops this state.
 */
void Hero::RunningState::stop(State *next_state) {

  State::stop(next_state);

  hero->clear_movement();
}

/**
 * @brief Updates this state.
 */
void Hero::RunningState::update(void) {

  uint32_t now = System::now();
  if (phase == 0) {
    
    if (now >= next_phase_date) {
      hero->clear_movement();
      hero->set_movement(new StraightMovement(30, sprites->get_animation_direction() * 90, 10000));
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
    next_phase_date += System::now() - when_suspended;
  }
}

/**
 * @brief Returns whether the hero is pressing the item key corresponding to the running shoes.
 * @return true if the hero is pressing the item key corresponding to the running shoes
 */
bool Hero::RunningState::is_pressing_running_key(void) {

  Equipment *equipment = game->get_equipment();
  int slot = equipment->get_item_slot(INVENTORY_SPEED_SHOES);
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

  if (direction4 != sprites->get_animation_direction()) {
    hero->set_state(new FreeState(hero));
  }
}

/**
 * @brief Notifies this state that the hero has just tried to change his position.
 * @param success true if the position has actually just changed
 */
void Hero::RunningState::notify_movement_tried(bool success) {

  State::notify_movement_tried(success);

  if (!success) {
    hero->set_state(new FreeState(hero));
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
 * @brief Returns whether can trigger a jump sensor in this state.
 * If false is returned, jump sensors have no effect (but they are obstacle for the hero).
 * @param jump_sensor a jump sensor
 * @return true if the hero can use jump sensors in this state
 */
bool Hero::RunningState::can_take_jump_sensor(void) {
  return true;
}

/**
 * @brief Returns whether the hero can be hurt in this state.
 * @return true if the hero can be hurt in this state
 */
bool Hero::RunningState::can_be_hurt(void) {
  return true;
}

