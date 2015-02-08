/*
 * Copyright (C) 2006-2015 Christopho, Solarus - http://www.solarus-games.org
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
#include "solarus/hero/HurtState.h"
#include "solarus/hero/FreeState.h"
#include "solarus/hero/HeroSprites.h"
#include "solarus/lua/LuaContext.h"
#include "solarus/movements/StraightMovement.h"
#include "solarus/lowlevel/Sound.h"
#include "solarus/lowlevel/Geometry.h"
#include "solarus/lowlevel/System.h"
#include "solarus/Game.h"
#include "solarus/Equipment.h"
#include <algorithm>
#include <memory>

namespace Solarus {

/**
 * \brief Constructor.
 * \param hero The hero controlled by this state.
 * \param source_xy Coordinates of the thing (usually an enemy) that hurts
 * the hero, used to compute the trajectory of the hero.
 * If nullptr, the hero won't move.
 * \param damage Number of life poitns to remove
 * (this number may be reduced by the tunic or by hero:on_taking_damage()).
 */
Hero::HurtState::HurtState(
    Hero& hero,
    const Point* source_xy,
    int damage):
  State(hero, "hurt"),
  has_source(source_xy != nullptr),
  source_xy(has_source ? *source_xy : Point()),
  damage(damage),
  end_hurt_date(0) {

}

/**
 * \brief Starts this state.
 * \param previous_state The previous state.
 */
void Hero::HurtState::start(const State* previous_state) {

  State::start(previous_state);

  Equipment& equipment = get_equipment();

  Sound::play("hero_hurt");

  Hero& hero = get_hero();
  const uint32_t invincibility_duration = 2000;
  hero.set_invincible(true, invincibility_duration);
  get_sprites().set_animation_hurt();
  get_sprites().blink(invincibility_duration);

  if (has_source) {
    double angle = Geometry::get_angle(source_xy, hero.get_xy());
    std::shared_ptr<StraightMovement> movement =
        std::make_shared<StraightMovement>(false, true);
    movement->set_max_distance(24);
    movement->set_speed(120);
    movement->set_angle(angle);
    hero.set_movement(movement);
  }
  end_hurt_date = System::now() + 200;

  // See if the script customizes how the hero takes damages.
  bool handled = get_lua_context().hero_on_taking_damage(get_hero(), damage);

  if (!handled && damage != 0) {
    // No customized damage: perform the default calculation.
    // The level of the tunic reduces the damage,
    // but we remove at least 1 life point.
    int life_points = std::max(1, damage / (equipment.get_ability(Ability::TUNIC)));

    equipment.remove_life(life_points);
    if (equipment.has_ability(Ability::TUNIC)) {
      equipment.notify_ability_used(Ability::TUNIC);
    }
  }
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
  if ((hero.get_movement() != nullptr && hero.get_movement()->is_finished())
      || System::now() >= end_hurt_date) {
    // The movement may be finished, or the end date may be reached
    // when there is an obstacle or when there is no movement at all.

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
    const Teletransporter& /* teletransporter */) const {
  return true;
}

/**
 * \copydoc Hero::State::is_stream_obstacle
 */
bool Hero::HurtState::is_stream_obstacle(const Stream& /* stream */) const {
  return true;
}

/**
 * \brief Returns whether a sensor is considered as an obstacle in this state.
 * \param sensor a sensor
 * \return true if the sensor is an obstacle in this state
 */
bool Hero::HurtState::is_sensor_obstacle(const Sensor& /* sensor */) const {
  return true;
}

/**
 * \copydoc Hero::State::is_separator_obstacle
 */
bool Hero::HurtState::is_separator_obstacle(const Separator& /* separator */) const {
  return true;
}

/**
 * \brief Returns whether the hero can be hurt in this state.
 * \param attacker an attacker that is trying to hurt the hero
 * (or nullptr if the source of the attack is not an enemy)
 * \return true if the hero can be hurt in this state
 */
bool Hero::HurtState::can_be_hurt(MapEntity* /* attacker */) const {
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

