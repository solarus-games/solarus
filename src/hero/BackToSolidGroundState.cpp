/*
 * Copyright (C) 2006-2016 Christopho, Solarus - http://www.solarus-games.org
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
#include "solarus/entities/Boomerang.h"
#include "solarus/entities/Entities.h"
#include "solarus/hero/BackToSolidGroundState.h"
#include "solarus/hero/FreeState.h"
#include "solarus/hero/HeroSprites.h"
#include "solarus/lowlevel/Sound.h"
#include "solarus/lowlevel/System.h"
#include "solarus/lua/LuaContext.h"
#include "solarus/lua/LuaTools.h"
#include "solarus/movements/TargetMovement.h"
#include "solarus/Equipment.h"
#include "solarus/Game.h"
#include "solarus/Map.h"
#include <lua.hpp>
#include <memory>

namespace Solarus {

/**
 * \brief Constructor.
 * \param hero the hero controlled by this state
 * \param use_specified_position true to get back to the place previously specified (if any),
 * false to get back to the last coordinates with solid ground
 * \param end_delay a delay to add at the end before returning control to the hero (default 0)
 * \param with_sound true to play a sound when returning to solid ground
 */
Hero::BackToSolidGroundState::BackToSolidGroundState(
    Hero& hero,
    bool use_specified_position,
    uint32_t end_delay,
    bool with_sound):
  HeroState(hero, "back to solid ground"),
  target_position(),
  end_delay(end_delay),
  end_date(0),
  with_sound(with_sound) {

  if (use_specified_position && !hero.get_target_solid_ground_callback().is_empty()) {
    // go back to a target position specified earlier
    this->target_position = hero.get_target_solid_ground_callback();
  }
  else {
    // just go back to the last solid ground location
    this->target_position = hero.make_solid_ground_callback(
        hero.get_last_solid_ground_coords(),
        hero.get_last_solid_ground_layer()
    );
  }
}

/**
 * \brief Starts this state.
 * \param previous_state the previous state
 */
void Hero::BackToSolidGroundState::start(const State* previous_state) {

  HeroState::start(previous_state);

  Point xy;
  int layer = 0;

  Hero& hero = get_entity();
  lua_State* l = hero.get_lua_context()->get_internal_state();

  // Call the Lua function to get the coordinates and layer.
  Debug::check_assertion(!target_position.is_empty(), "Missing solid ground callback");
  target_position.push();
  bool success = LuaTools::call_function(l, 0, 3, "Solid ground callback");
  if (!success) {
    // Fallback: use the last solid ground position.
    xy = hero.get_last_solid_ground_coords();
    layer = hero.get_last_solid_ground_layer();
  }
  else {
    // Normal case: use the result of the function.
    xy.x = LuaTools::check_int(l, -3);
    xy.y = LuaTools::check_int(l, -2);
    layer = LuaTools::check_int(l, -1);
    lua_pop(l, 3);
  }

  hero.set_movement(std::make_shared<TargetMovement>(
      nullptr, xy.x, xy.y, 144, true
  ));
  get_entities().set_entity_layer(hero, layer);

  const std::set<std::shared_ptr<Boomerang>>& boomerangs =
      get_entities().get_entities_by_type<Boomerang>();
  for (const std::shared_ptr<Boomerang>& boomerang : boomerangs) {
    boomerang->remove_from_map();
  }
}

/**
 * \brief Stops this state.
 * \param next_state the next state
 */
void Hero::BackToSolidGroundState::stop(const State* next_state) {

  HeroState::stop(next_state);

  get_entity().clear_movement();
}

/**
 * \brief Updates this state.
 */
void Hero::BackToSolidGroundState::update() {

  HeroState::update();

  if (is_suspended()) {
    return;
  }

  // the current movement is an instance of TargetMovement
  Hero& hero = get_entity();
  if (hero.get_movement()->is_finished()) {

    uint32_t now = System::now();
    if (end_date == 0) {
      end_date = now + end_delay;
      get_sprites().set_animation_stopped_normal();
      get_sprites().blink(2000);

      if (with_sound) {
        Sound::play("message_end");  // TODO rename this sound.
      }
    }

    if (now >= end_date) {

      if (get_equipment().get_life() <= 0 &&
          !get_game().is_showing_game_over()) {
        get_sprites().stop_blinking();
        get_game().start_game_over();
        return;
      }

      hero.start_state_from_ground();
    }
  }
}

void Hero::BackToSolidGroundState::set_suspended(bool suspended) {

  HeroState::set_suspended(suspended);

  if (!suspended && end_date != 0) {
    end_date += System::now() - get_when_suspended();
  }
}

/**
 * \brief Returns whether the game over sequence can start in the current state.
 * \return true if the game over sequence can start in the current state
 */
bool Hero::BackToSolidGroundState::can_start_gameover_sequence() const {
  return false;
}

/**
 * \brief Returns whether the hero is visible in the current state.
 * \return true if the hero is displayed in the current state
 */
bool Hero::BackToSolidGroundState::is_visible() const {
  return end_date != 0;
}

/**
 * \brief Returns whether the hero ignores the effect of deep water in this state.
 * \return true if the hero ignores the effect of deep water in the current state
 */
bool Hero::BackToSolidGroundState::can_avoid_deep_water() const {
  return true;
}

/**
 * \brief Returns whether the hero ignores the effect of holes in this state.
 * \return true if the hero ignores the effect of holes in the current state
 */
bool Hero::BackToSolidGroundState::can_avoid_hole() const {
  return true;
}

/**
 * \brief Returns whether the hero ignores the effect of ice in this state.
 * \return \c true if the hero ignores the effect of ice in the current state.
 */
bool Hero::BackToSolidGroundState::can_avoid_ice() const {
  return true;
}

/**
 * \brief Returns whether the hero ignores the effect of lava in this state.
 * \return true if the hero ignores the effect of lava in the current state
 */
bool Hero::BackToSolidGroundState::can_avoid_lava() const {
  return true;
}

/**
 * \brief Returns whether the hero ignores the effect of prickles in this state.
 * \return true if the hero ignores the effect of prickles in the current state
 */
bool Hero::BackToSolidGroundState::can_avoid_prickle() const {
  return true;
}

/**
 * \brief Returns whether the hero is touching the ground in the current state.
 * \return true if the hero is touching the ground in the current state
 */
bool Hero::BackToSolidGroundState::is_touching_ground() const {
  return false;
}

/**
 * \brief Returns whether this state ignores the collisions with the detectors and the ground.
 * \return true if the collisions are ignored
 */
bool Hero::BackToSolidGroundState::are_collisions_ignored() const {
  return true;
}

/**
 * \brief Returns whether the hero ignores the effect of teletransporters in this state.
 * \return true if the hero ignores the effect of teletransporters in this state
 */
bool Hero::BackToSolidGroundState::can_avoid_teletransporter() const {
  return true;
}

/**
 * \copydoc Entity::State::can_avoid_stream
 */
bool Hero::BackToSolidGroundState::can_avoid_stream(const Stream& /* stream */) const {
  return true;
}

/**
 * \brief Returns whether the hero ignores the effect of sensors in this state.
 * \return true if the hero ignores the effect of sensors in this state
 */
bool Hero::BackToSolidGroundState::can_avoid_sensor() const {
  return true;
}

/**
 * \brief Returns whether the hero ignores the effect of switches in this state.
 * \return true if the hero ignores the effect of switches in this state
 */
bool Hero::BackToSolidGroundState::can_avoid_switch() const {
  return true;
}

/**
 * \brief Returns whether the hero ignores the effect of explosions in this state.
 * \return true if the hero ignores the effect of explosions in this state
 */
bool Hero::BackToSolidGroundState::can_avoid_explosion() const {
  return true;
}

}

