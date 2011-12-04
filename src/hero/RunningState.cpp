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
#include "entities/Enemy.h"
#include "movements/StraightMovement.h"
#include "movements/JumpMovement.h"
#include "lowlevel/System.h"
#include "lowlevel/Sound.h"
#include "lowlevel/Geometry.h"
#include "Game.h"
#include "GameControls.h"
#include "Map.h"
#include "Equipment.h"

/**
 * @brief Constructor.
 * @param hero the hero controlled by this state
 */
Hero::RunningState::RunningState(Hero &hero):
  State(hero) {

}

/**
 * @brief Destructor.
 */
Hero::RunningState::~RunningState() {

}

/**
 * @brief Starts this state.
 * @param previous_state the previous state
 */
void Hero::RunningState::start(State *previous_state) {

  State::start(previous_state);

  get_sprites().set_animation_prepare_running();

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
    hero.clear_movement();
  }
}

/**
 * @brief Updates this state.
 */
void Hero::RunningState::update() {

  State::update();

  if (suspended) {
    return;
  }

  uint32_t now = System::now();

  if (!is_bouncing() && now >= next_sound_date) {
    Sound::play("running");
    next_sound_date = now + 170;
  }

  if (phase == 0) {
    
    if (now >= next_phase_date) {

      double angle = Geometry::degrees_to_radians(get_sprites().get_animation_direction() * 90);
      StraightMovement* movement = new StraightMovement(false, true);
      movement->set_max_distance(3000);
      movement->set_speed(300);
      movement->set_angle(angle);
      hero.clear_movement();
      hero.set_movement(movement);

      get_sprites().set_animation_running();
      phase++;
    }
    else if (!is_pressing_running_key()) {
      hero.set_state(new FreeState(hero));
    }
  }
  else if (hero.get_movement()->is_finished()) {
    hero.start_state_from_ground();
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
 * @brief Returns whether the hero is bouncing after he reached an obstacle during the run.
 * @return true if the hero is bouncing
 */
bool Hero::RunningState::is_bouncing() {
  return phase == 2;
}

/**
 * @brief Returns whether the hero is pressing the item key corresponding to the running shoes.
 * @return true if the hero is pressing the item key corresponding to the running shoes
 */
bool Hero::RunningState::is_pressing_running_key() {

  int slot = get_equipment().get_item_slot("pegasus_shoes"); // TODO make this quest-dependent
  if (slot == -1) {
    return false;
  }

  GameControls::GameKey key = (slot == 0) ? GameControls::ITEM_1 : GameControls::ITEM_2;
  return get_controls().is_key_pressed(key);
}

/**
 * @brief Notifies this state that a directional key was just pressed.
 * @param direction4 direction of the key (0 to 3)
 */
void Hero::RunningState::directional_key_pressed(int direction4) {

  if (!is_bouncing()
      && direction4 != get_sprites().get_animation_direction()) {
    hero.set_state(new FreeState(hero));
  }
}

/**
 * @brief Notifies this state that the hero has just failed to change its
 * position because of obstacles.
 */
void Hero::RunningState::notify_obstacle_reached() {

  State::notify_obstacle_reached();

  if (phase == 1) {
    int opposite_direction = (get_sprites().get_animation_direction8() + 4) % 8;
    hero.set_movement(new JumpMovement(opposite_direction, 32, 64, false));
    get_sprites().set_animation_hurt();
    Sound::play("explosion");
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
int Hero::RunningState::get_wanted_movement_direction8() {
  return get_sprites().get_animation_direction8();
}

/**
 * @brief Returns whether the hero can take stairs in this state.
 * If false is returned, stairs have no effect (but they are obstacle for the hero).
 * @return true if the hero ignores the effect of stairs in this state
 */
bool Hero::RunningState::can_take_stairs() {
  return !is_bouncing();
}
/**
 * @brief Returns whether can trigger a jumper in this state.
 * If false is returned, jumpers have no effect (but they are obstacle for the hero).
 * @return true if the hero can use jumpers in this state
 */
bool Hero::RunningState::can_take_jumper() {
  return !is_bouncing();
}

/**
 * @brief Returns whether the hero can be hurt by an attacker in this state.
 * @param attacker an attacker that is trying to hurt the hero
 * (or NULL if the source of the attack is not an enemy)
 * @return true if the hero can be hurt in this state
 */
bool Hero::RunningState::can_be_hurt(Enemy* attacker) {
  return phase == 0 ||
      (attacker != NULL && attacker->get_can_hurt_hero_running());
}

/**
 * @brief Returns whether the game over sequence can start in the current state.
 * @return true if the game over sequence can start in the current state
 */
bool Hero::RunningState::can_start_gameover_sequence() {
  return !is_bouncing();
}

/**
 * @brief Returns whether the hero is touching the ground in the current state.
 * @return true if the hero is touching the ground in the current state
 */
bool Hero::RunningState::is_touching_ground() {
  return !is_bouncing();
}

/**
 * @brief Returns whether the hero ignores the effect of deep water in this state.
 * @return true if the hero ignores the effect of deep water in the current state
 */
bool Hero::RunningState::can_avoid_deep_water() {
  return is_bouncing();
}

/**
 * @brief Returns whether the hero ignores the effect of holes in this state.
 * @return true if the hero ignores the effect of holes in the current state
 */
bool Hero::RunningState::can_avoid_hole() {
  return is_bouncing();
}

/**
 * @brief Returns whether the hero ignores the effect of lava in this state.
 * @return true if the hero ignores the effect of lava in the current state
 */
bool Hero::RunningState::can_avoid_lava() {
  return is_bouncing();
}

/**
 * @brief Returns whether the hero ignores the effect of prickles in this state.
 * @return true if the hero ignores the effect of prickles in the current state
 */
bool Hero::RunningState::can_avoid_prickle() {
  return is_bouncing();
}

/**
 * @brief Returns whether the hero ignores the effect of teletransporters in this state.
 * @return true if the hero ignores the effect of teletransporters in this state
 */
bool Hero::RunningState::can_avoid_teletransporter() {
  return is_bouncing();
}

/**
 * @brief Returns whether the hero ignores the effect of conveyor belts in this state.
 * @return true if the hero ignores the effect of conveyor belts in this state
 */
bool Hero::RunningState::can_avoid_conveyor_belt() {
  return is_bouncing();
}

/**
 * @brief Returns whether a sensor is considered as an obstacle in this state.
 * @param sensor a sensor
 * @return true if the sensor is an obstacle in this state
 */
bool Hero::RunningState::is_sensor_obstacle(Sensor &sensor) {
  return is_bouncing();
}

/**
 * @brief Tests whether the hero is cutting with his sword the specified detector
 * for which a collision was detected.
 * @param detector the detector to check
 * @return true if the sword is cutting this detector
 */
bool Hero::RunningState::is_cutting_with_sword(Detector &detector) {

  // check the distance to the detector
  int distance = 8;
  Rectangle tested_point = hero.get_facing_point();

  switch (get_sprites().get_animation_direction()) {

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

  return detector.overlaps(tested_point.get_x(), tested_point.get_y());
}

/**
 * @brief Returns the damage power of the sword for the current attack.
 * @return the current damage factor of the sword
 */
int Hero::RunningState::get_sword_damage_factor() {

  // the damage are multiplied by 2
  return State::get_sword_damage_factor() * 2;
}

