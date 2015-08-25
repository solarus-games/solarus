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
#include "solarus/hero/PlayerMovementState.h"
#include "solarus/hero/HeroSprites.h"
#include "solarus/entities/Jumper.h"
#include "solarus/entities/Stream.h"
#include "solarus/entities/StreamAction.h"
#include "solarus/movements/PlayerMovement.h"
#include "solarus/lowlevel/Debug.h"
#include "solarus/lowlevel/System.h"

namespace Solarus {

/**
 * \brief Constructor.
 * \param hero The hero controlled by this state.
 * \param state_name A name describing this state.
 */
Hero::PlayerMovementState::PlayerMovementState(
    Hero& hero, const std::string& state_name
):
  BaseState(hero, state_name),
  player_movement(),
  current_jumper(),
  jumper_start_date(0) {
}

/**
 * \brief Destructor.
 */
Hero::PlayerMovementState::~PlayerMovementState() {
}

/**
 * \brief Returns the movement created by this state.
 *
 * This movement is applied to the hero until the end of this state or until
 * someone sets another movement.
 *
 * \return The player movement.
 */
const std::shared_ptr<PlayerMovement>& Hero::PlayerMovementState::get_player_movement() {
  return player_movement;
}

/**
 * \brief Returns the movement of the hero controlled by the player.
 *
 * This movement is applied to the hero until the end of this state or until
 * someone sets another movement.
 *
 * \return The player movement.
 */
std::shared_ptr<const PlayerMovement> Hero::PlayerMovementState::get_player_movement() const {
  return player_movement;
}

/**
 * \brief Starts this state.
 *
 * This function is called automatically when this state becomes the active
 * state of the hero.
 *
 * \param previous_state The previous state (for information).
 */
void Hero::PlayerMovementState::start(const State* previous_state) {

  State::start(previous_state);

  player_movement = std::make_shared<PlayerMovement>(
      get_hero().get_walking_speed()
  );
  get_hero().set_movement(player_movement);

  if (is_current_state()) { // yes, the state may have already changed
    get_player_movement()->compute_movement();
    if (is_current_state()) {
      if (get_wanted_movement_direction8() != -1) {
        set_animation_walking();
      }
      else {
        set_animation_stopped();
      }
    }
  }
}

/**
 * \brief Ends this state.
 *
 * This function is called automatically when this state is not the active state anymore.
 * You should here close everything the start() function has opened.
 * The destructor will be called at the next cycle.
 *
 * \param next_state The next state (for information).
 */
void Hero::PlayerMovementState::stop(const State* next_state) {

  State::stop(next_state);

  get_hero().clear_movement();
  get_sprites().set_animation_stopped_normal();
  cancel_jumper();
  player_movement = nullptr;
}

/**
 * \brief Changes the map.
 *
 * This function is called when the hero is about to go to another map.
 *
 * \param map the new map
 */
void Hero::PlayerMovementState::set_map(Map &map) {

  State::set_map(map);
  set_animation_stopped();
}

/**
 * \brief Updates this state.
 *
 * This function may change the state of the hero,
 * so when you call it from a subclass, call is_current_state() then.
 */
void Hero::PlayerMovementState::update() {

  State::update();

  if (!is_suspended()) {

    if (current_jumper != nullptr) {

      const int jump_direction8 = current_jumper->get_direction();
      if (!current_jumper->is_enabled()
          || current_jumper->is_being_removed()
          || !current_jumper->is_in_jump_position(get_hero(), get_hero().get_bounding_box(), false)) {

        // Cancel the jumper preparation.
        current_jumper = nullptr;
        jumper_start_date = 0;
      }
      else if (System::now() >= jumper_start_date) {
        // Time to make the jump and everything is okay.
        get_hero().start_jumping(
            jump_direction8, current_jumper->get_jump_length(), true, true);
      }
    }
  }
}

/**
 * \brief Notifies this state that the game was just suspended or resumed.
 * \param suspended \c true if the game is suspended.
 */
void Hero::PlayerMovementState::set_suspended(bool suspended) {

  State::set_suspended(suspended);

  if (!suspended) {
    if (jumper_start_date != 0) {
      jumper_start_date += System::now() - get_when_suspended();
    }
  }
}

/**
 * Gives the sprites the animation stopped corresponding to this state.
 */
void Hero::PlayerMovementState::set_animation_stopped() {
}

/**
 * Gives the sprites the animation walking corresponding to this state.
 */
void Hero::PlayerMovementState::set_animation_walking() {
}

/**
 * \brief Returns whether the player can control his movements in the current state.
 * \return true if the player can control his movements
 */
bool Hero::PlayerMovementState::can_control_movement() const {

  // The player has control, unless a script has set another movement.
  return get_hero().get_movement().get() == get_player_movement().get();
}

/**
 * \brief Returns the direction of the hero's movement as defined by the controls applied by the player
 * and the movements allowed is the current state.
 *
 * If he is not moving, -1 is returned.
 * This direction may be different from the real movement direction because of obstacles.
 *
 * \return the hero's wanted direction between 0 and 7, or -1 if he is stopped
 */
int Hero::PlayerMovementState::get_wanted_movement_direction8() const {
  return get_player_movement()->get_wanted_direction8();
}

/**
 * \brief Notifies this state that the walking speed has changed.
 * 
 * If the hero can walk in this state, the state should modify its movement
 * to set the new speed.
 */
void Hero::PlayerMovementState::notify_walking_speed_changed() {

  if (get_player_movement() != nullptr) {
    get_player_movement()->set_moving_speed(get_hero().get_walking_speed());
  }
}

/**
 * \brief Notifies this state that the movement has changed.
 *
 * This function is called when the hero's movement direction changes (for instance
 * because the player pressed or released a directional key, or the hero just reached an obstacle).
 * The animations and collisions should be updated according to the new movement.
 */
void Hero::PlayerMovementState::notify_movement_changed() {

  // the movement has changed: update the animation of the sprites

  bool movement_walking = get_wanted_movement_direction8() != -1;
  bool sprites_walking = get_sprites().is_walking();

  if (movement_walking && !sprites_walking) {
    set_animation_walking();
  }
  else if (!movement_walking && sprites_walking) {
    set_animation_stopped();
  }
}

/**
 * \brief Notifies this state that the hero has just changed its
 * position.
 */
void Hero::PlayerMovementState::notify_position_changed() {

  // Stop the preparation to a jump if any.
  cancel_jumper();
}

/**
 * \brief Notifies this state that the layer has changed.
 */
void Hero::PlayerMovementState::notify_layer_changed() {
  get_hero().update_movement();
}

/**
 * \brief Returns whether the hero can be hurt in this state.
 * \return true if the hero can be hurt in this state
 * \param attacker an attacker that is trying to hurt the hero
 * (or nullptr if the source of the attack is not an enemy)
 */
bool Hero::PlayerMovementState::can_be_hurt(Entity* /* attacker */) const {
  return true;
}

/**
 * \brief Returns whether the hero can pick a treasure in this state.
 * \param item The equipment item to obtain.
 * \return true if the hero can pick that treasure in this state.
 */
bool Hero::PlayerMovementState::can_pick_treasure(EquipmentItem& /* item */) const {
  return true;
}

/**
 * \brief Returns whether can trigger a jumper in this state.
 *
 * If false is returned, jumpers have no effect (but they are obstacle for
 * the hero).
 *
 * \return \c true if the hero can use jumpers in this state.
 */
bool Hero::PlayerMovementState::can_take_jumper() const {
  return true;
}

/**
 * \brief Notifies this state that the hero is activating a jumper.
 * \param jumper The jumper activated.
 */
void Hero::PlayerMovementState::notify_jumper_activated(Jumper& jumper) {

  if (&jumper == current_jumper.get()) {
    // We already know.
    return;
  }

  // Add a small delay before jumping.
  current_jumper = std::static_pointer_cast<Jumper>(jumper.shared_from_this());
  jumper_start_date = System::now() + 200;
}

/**
 * \brief Cancels the jump preparation that was ongoing if any.
 */
void Hero::PlayerMovementState::cancel_jumper() {

  if (current_jumper != nullptr) {
    current_jumper = nullptr;
    jumper_start_date = 0;
  }
}

}

