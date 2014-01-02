/*
 * Copyright (C) 2006-2014 Christopho, Solarus - http://www.solarus-games.org
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
#include "hero/ConveyorBeltState.h"
#include "hero/FreeState.h"
#include "hero/HeroSprites.h"
#include "entities/ConveyorBelt.h"
#include "movements/TargetMovement.h"
#include "movements/PathMovement.h"
#include "Game.h"
#include "Sprite.h"

namespace solarus {

/**
 * \brief Constructor.
 * \param hero The hero controlled by this state.
 * \param conveyor_belt The conveyor belt to take.
 */
Hero::ConveyorBeltState::ConveyorBeltState(
    Hero& hero, ConveyorBelt& conveyor_belt):
  State(hero, "conveyor belt"),
  conveyor_belt(conveyor_belt) {

}

/**
 * \brief Destructor.
 */
Hero::ConveyorBeltState::~ConveyorBeltState() {

}

/**
 * \brief Starts this state.
 * \param previous_state the previous state
 */
void Hero::ConveyorBeltState::start(const State* previous_state) {

  State::start(previous_state);

  get_sprites().set_animation_stopped_normal();

  // first, snap the hero to the center of the conveyor belt
  snapping = true;
  Hero& hero = get_hero();
  hero.set_movement(new TargetMovement(
      &conveyor_belt, 0, 0, hero.get_walking_speed() * 2 / 3, true));
}

/**
 * \brief Stops this state.
 * \param next_state the next state
 */
void Hero::ConveyorBeltState::stop(const State* next_state) {

  State::stop(next_state);

  get_hero().clear_movement();
}

/**
 * \brief Updates this state.
 */
void Hero::ConveyorBeltState::update() {

  State::update();

  if (is_suspended()) {
    return;
  }

  Hero& hero = get_hero();
  if (snapping && hero.get_movement()->is_finished()) {

    // the hero is now exactly placed on the conveyor belt: start the conveyor belt's movement
    snapping = false;
    std::string path = "  ";
    path[0] = path[1] = '0' + conveyor_belt.get_direction();
    hero.clear_movement();
    hero.set_movement(new PathMovement(path, 64, false, false, false));
  }
  else {

    // see if the conveyor belt's movement is finished
    if (hero.get_movement()->is_finished() || !hero.on_conveyor_belt) {

      hero.set_state(new FreeState(hero));
    }
    else {
      // update the sprites direction
      int keys_direction8 = get_commands().get_wanted_direction8();
      int movement_direction8 = conveyor_belt.get_direction();

      int animation_direction = get_sprites().get_animation_direction(keys_direction8, movement_direction8);
      if (animation_direction != get_sprites().get_animation_direction()
          && animation_direction != -1) {
        get_sprites().set_animation_direction(animation_direction);
      }
    }

    hero.on_conveyor_belt = false;
  }
}

/**
 * \brief Returns whether the hero ignores the effect of teletransporters in this state.
 * \return true if the hero ignores the effect of teletransporters in this state
 */
bool Hero::ConveyorBeltState::can_avoid_teletransporter() const {
  return true; // ignore the teletransporter until the conveyor belt is finished
}

/**
 * \brief Returns whether the hero ignores the effect of conveyor belts in this state.
 * \return true if the hero ignores the effect of conveyor belts in this state
 */
bool Hero::ConveyorBeltState::can_avoid_conveyor_belt() const {
  return true;
}

}

