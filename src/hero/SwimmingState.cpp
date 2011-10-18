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
#include "hero/SwimmingState.h"
#include "hero/FreeState.h"
#include "hero/HeroSprites.h"
#include "lowlevel/Sound.h"
#include "lowlevel/System.h"
#include "Equipment.h"
#include "KeysEffect.h"

/**
 * @brief Constructor.
 * @param hero the hero controlled by this state
 */
Hero::SwimmingState::SwimmingState(Hero& hero):
  PlayerMovementState(hero),
  fast_swimming(false),
  end_fast_swim_date(0) {

}

/**
 * @brief Destructor.
 */
Hero::SwimmingState::~SwimmingState() {

}

/**
 * @brief Starts this state.
 * @param previous_state the previous state
 */
void Hero::SwimmingState::start(State* previous_state) {

  PlayerMovementState::start(previous_state);

  get_equipment().notify_ability_used("swim");
  hero.set_walking_speed(get_slow_swimming_speed());
  get_keys_effect().set_action_key_effect(KeysEffect::ACTION_KEY_SWIM);
}

/**
 * @brief Stops this state.
 * @param next_state the next state
 */
void Hero::SwimmingState::stop(State* next_state) {

  PlayerMovementState::stop(next_state);

  get_keys_effect().set_action_key_effect(KeysEffect::ACTION_KEY_NONE);
}

/**
 * @brief Updates this state.
 */
void Hero::SwimmingState::update() {

  PlayerMovementState::update();

  if (suspended) {
    return;
  }

  if (hero.get_ground() != GROUND_DEEP_WATER) {
    hero.set_state(new FreeState(hero));
  }
  else if (fast_swimming && System::now() >= end_fast_swim_date) {
    fast_swimming = false;
    hero.set_walking_speed(get_slow_swimming_speed());

    if (get_wanted_movement_direction8() != -1) {
      set_animation_walking();
    }
    else {
      set_animation_stopped();
    }
  }
}

/**
 * @brief Notifies this state that the game was just suspended or resumed.
 * @param suspended true if the game is suspended
 */
void Hero::SwimmingState::set_suspended(bool suspended) {

  PlayerMovementState::set_suspended(suspended);

  if (!suspended && fast_swimming) {
    end_fast_swim_date += System::now() - when_suspended;
  }
}

/**
 * Gives the sprites the animation stopped corresponding to this state.
 */
void Hero::SwimmingState::set_animation_stopped() {

  if (!fast_swimming) {
    get_sprites().set_animation_stopped_swimming();
  }
}

/**
 * Gives the sprites the animation walking corresponding to this state.
 */
void Hero::SwimmingState::set_animation_walking() {

  if (!fast_swimming) {
    get_sprites().set_animation_swimming_slow();
  }
}

/**
 * @brief Notifies this state that the action key was just pressed.
 */
void Hero::SwimmingState::action_key_pressed() {
  try_swim_faster();
}
/**
 * @brief Notifies this state that the sword key was just pressed.
 */
void Hero::SwimmingState::sword_key_pressed() {
  try_swim_faster();
}

/**
 * @brief Makes the hero swim faster for a while if possible.
 */
void Hero::SwimmingState::try_swim_faster() {

  if (!fast_swimming) {
    fast_swimming = true;
    hero.set_walking_speed(get_fast_swimming_speed());
    get_sprites().set_animation_swimming_fast();
    Sound::play("swim");
    end_fast_swim_date = System::now() + 600;
  }
}

/**
 * @brief Returns the normal swimming speed.
 * @return the swimming speed in pixels per second
 */
int Hero::SwimmingState::get_slow_swimming_speed() {
  return hero.get_normal_walking_speed() / 2;
}

/**
 * @brief Returns the faster swimming speed.
 * @return the faster swimming speed in pixels per second
 */
int Hero::SwimmingState::get_fast_swimming_speed() {
  return hero.get_normal_walking_speed();
}

/**
 * @brief Returns whether some stairs are considered as obstacle in this state.
 * @param stairs some stairs
 * @return true if the stairs are obstacle in this state
 */
bool Hero::SwimmingState::is_stairs_obstacle(Stairs& stairs) {
  return false;
}

/**
 * @brief Returns whether can trigger a jumper in this state.
 * If false is returned, jumpers have no effect (but they are obstacle for the hero).
 * @return true if the hero can use jumpers in this state
 */
bool Hero::SwimmingState::can_take_jumper() {
  return true;
}

