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
#include "hero/HurtState.h"
#include "hero/FreeState.h"
#include "hero/HeroSprites.h"
#include "movements/StraightMovement.h"
#include "lowlevel/Sound.h"
#include "lowlevel/Geometry.h"
#include "lowlevel/System.h"
#include "Game.h"
#include "Equipment.h"

namespace solarus {

/**
 * \brief Constructor.
 * \param hero The hero controlled by this state.
 * \param source_xy Coordinates of the thing (usually an enemy) that hurts the hero,
 * used to compute the trajectory of the hero.
 * \param life_points Number of heart quarters to remove (this number may be reduced by the tunic).
 * \param magic_points Number of magic points to remove.
 */
Hero::HurtState::HurtState(
    Hero& hero,
    const Rectangle& source_xy,
    int life_points,
    int magic_points):
  State(hero, "hurt"),
  source_xy(source_xy),
  life_points(life_points),
  magic_points(magic_points),
  end_hurt_date(0) {

}

/**
 * \brief Destructor.
 */
Hero::HurtState::~HurtState() {

}

/**
 * \brief Starts this state.
 * \param previous_state the previous state
 */
void Hero::HurtState::start(const State* previous_state) {

  State::start(previous_state);

  Equipment& equipment = get_equipment();

  Sound::play("hero_hurt");

  if (life_points != 0) {
    // the level of the tunic reduces the damage, but we remove at least 1 life point
    life_points = std::max(1, life_points / (equipment.get_ability("tunic")));

    equipment.remove_life(life_points);
    if (equipment.has_ability("tunic")) {
      equipment.notify_ability_used("tunic");
    }
  }

  if (magic_points > 0 && equipment.get_magic() > 0) {
    equipment.remove_magic(magic_points);
    Sound::play("magic_bar");
  }
  get_sprites().set_animation_hurt();
  get_sprites().blink();

  Hero& hero = get_hero();
  double angle = Geometry::get_angle(source_xy.get_x(), source_xy.get_y(),
      hero.get_x(), hero.get_y());
  StraightMovement* movement = new StraightMovement(false, true);
  movement->set_max_distance(24);
  movement->set_speed(120);
  movement->set_angle(angle);
  hero.set_movement(movement);
  end_hurt_date = System::now() + 200;
}

/**
 * \brief Ends this state.
 * \param next_state the next state
 */
void Hero::HurtState::stop(const State* next_state) {

  State::stop(next_state);

  get_hero().clear_movement();
}

/**
 * \brief Updates this state.
 */
void Hero::HurtState::update() {

  State::update();

  Hero& hero = get_hero();
  if (hero.get_movement()->is_finished()
      || System::now() >= end_hurt_date) {
    // we have end_hurt_date because the movement may never finish if there is an obstacle

    hero.clear_movement();
    hero.start_state_from_ground();
  }
}

/**
 * \brief Notifies this state that the game was just suspended or resumed.
 * \param suspended true if the game is suspended
 */
void Hero::HurtState::set_suspended(bool suspended) {

  State::set_suspended(suspended);

  if (!suspended) {
    uint32_t diff = System::now() - get_when_suspended();
    end_hurt_date += diff;
  }
}

/**
 * \brief Returns whether the game over sequence can start in the current state.
 * \return true if the game over sequence can start in the current state
 */
bool Hero::HurtState::can_start_gameover_sequence() const {
  return false;
}

/**
 * \brief Returns whether the hero is touching the ground in the current state.
 * \return true if the hero is touching the ground in the current state
 */
bool Hero::HurtState::is_touching_ground() const {
  return false;
}

/**
 * \brief Returns whether a teletransporter is considered as an obstacle in this state.
 * \param teletransporter a teletransporter
 * \return true if the teletransporter is an obstacle in this state
 */
bool Hero::HurtState::is_teletransporter_obstacle(
    const Teletransporter& teletransporter) const {
  return true;
}

/**
 * \brief Returns whether a conveyor belt is considered as an obstacle in this state.
 * \param conveyor_belt a conveyor belt
 * \return true if the conveyor belt is an obstacle in this state
 */
bool Hero::HurtState::is_conveyor_belt_obstacle(
    const ConveyorBelt& conveyor_belt) const {
  return true;
}

/**
 * \brief Returns whether a sensor is considered as an obstacle in this state.
 * \param sensor a sensor
 * \return true if the sensor is an obstacle in this state
 */
bool Hero::HurtState::is_sensor_obstacle(const Sensor& sensor) const {
  return true;
}

/**
 * \copydoc Hero::State::is_separator_obstacle
 */
bool Hero::HurtState::is_separator_obstacle(const Separator& separator) const {
  return true;
}

/**
 * \brief Returns whether the hero can be hurt in this state.
 * \param attacker an attacker that is trying to hurt the hero
 * (or NULL if the source of the attack is not an enemy)
 * \return true if the hero can be hurt in this state
 */
bool Hero::HurtState::can_be_hurt(Enemy* attacker) const {
  return false;
}

/**
 * \brief Returns whether the hero ignores the effect of switches in this state.
 * \return true if the hero ignores the effect of switches in this state
 */
bool Hero::HurtState::can_avoid_switch() const {
  return true;
}

/**
 * \brief Returns whether the hero ignores the effect of ice in this state.
 * \return \c true if the hero ignores the effect of ice in the current state.
 */
bool Hero::HurtState::can_avoid_ice() const {
  return true;
}

}

