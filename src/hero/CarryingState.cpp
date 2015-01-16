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
#include "solarus/hero/CarryingState.h"
#include "solarus/hero/FreeState.h"
#include "solarus/hero/HeroSprites.h"
#include "solarus/entities/CarriedItem.h"
#include "solarus/entities/MapEntities.h"
#include "solarus/lowlevel/Debug.h"
#include "solarus/Game.h"
#include "solarus/KeysEffect.h"
#include "solarus/Map.h"

namespace Solarus {

/**
 * \brief Constructor.
 * \param hero The hero controlled by this state.
 * \param carried_item The item to carry.
 */
Hero::CarryingState::CarryingState(
    Hero& hero, const std::shared_ptr<CarriedItem>& carried_item
):
  PlayerMovementState(hero, "carrying"),
  carried_item(carried_item) {

  Debug::check_assertion(carried_item != nullptr, "Missing carried item");
}

/**
 * \brief Starts this state.
 * \param previous_state the previous state
 */
void Hero::CarryingState::start(const State* previous_state) {

  PlayerMovementState::start(previous_state);

  if (is_current_state()) {
    get_sprites().set_lifted_item(carried_item);

    // action icon "throw"
    get_keys_effect().set_action_key_effect(KeysEffect::ACTION_KEY_THROW);
  }
}

/**
 * \brief Stops this state.
 * \param next_state the next state
 */
void Hero::CarryingState::stop(const State* next_state) {

  PlayerMovementState::stop(next_state);

  get_sprites().set_lifted_item(nullptr);
  get_keys_effect().set_action_key_effect(KeysEffect::ACTION_KEY_NONE);

  if (carried_item != nullptr) {

    switch (next_state->get_previous_carried_item_behavior()) {

    case CarriedItem::BEHAVIOR_THROW:
      throw_item();
      break;

    case CarriedItem::BEHAVIOR_DESTROY:
    case CarriedItem::BEHAVIOR_KEEP:
      carried_item = nullptr;
      break;

    default:
      Debug::die("Invalid carried item behavior");
    }
  }
}

/**
 * \brief Changes the map.
 * \param map the new map
 */
void Hero::CarryingState::set_map(Map& map) {

  PlayerMovementState::set_map(map);

  // the hero may go to another map while carrying an item
  if (carried_item != nullptr) {
    carried_item->set_map(map);
  }
}

/**
 * \brief Notifies this state that the layer has changed.
 */
void Hero::CarryingState::notify_layer_changed() {

  PlayerMovementState::notify_layer_changed();

  if (carried_item != nullptr) {
    carried_item->set_layer(get_hero().get_layer());
  }
}

/**
 * \brief Notifies this state that the game was just suspended or resumed.
 * \param suspended true if the game is suspended
 */
void Hero::CarryingState::set_suspended(bool suspended) {

  PlayerMovementState::set_suspended(suspended);

  if (carried_item != nullptr) {
    carried_item->set_suspended(suspended);
  }
}

/**
 * \brief Updates this state.
 */
void Hero::CarryingState::update() {

  PlayerMovementState::update();

  // The state may have just been changed for example by a jumper.
  if (is_current_state()) {
    carried_item->update();

    if (!is_suspended()) {

      if (carried_item->is_broken()) {
        carried_item = nullptr;
        Hero& hero = get_hero();
        hero.set_state(new FreeState(hero));
      }
    }
  }
}

/**
 * \brief Notifies this state that the action command was just pressed.
 */
void Hero::CarryingState::notify_action_command_pressed() {

  if (get_keys_effect().get_action_key_effect() == KeysEffect::ACTION_KEY_THROW) {
    throw_item();
    Hero& hero = get_hero();
    hero.set_state(new FreeState(hero));
  }
}

/**
 * \brief Throws the item carried.
 *
 * This function is called when the player presses the action key
 * or when another state becomes the current state.
 */
void Hero::CarryingState::throw_item() {

  carried_item->throw_item(get_sprites().get_animation_direction());
  get_entities().add_entity(carried_item);
  carried_item = nullptr;
}

/**
 * \brief Returns whether the hero can swing his sword in this state.
 * \return true if the hero can swing his sword in this state
 */
bool Hero::CarryingState::can_start_sword() const {
  return true;
}

/**
 * \copydoc Hero::State::can_use_shield
 */
bool Hero::CarryingState::can_use_shield() const {
  return false;
}

/**
 * \brief Returns whether the hero can take stairs in this state.
 * If false is returned, stairs have no effect (but they are obstacle for the hero).
 * \return true if the hero ignores the effect of stairs in this state
 */
bool Hero::CarryingState::can_take_stairs() const {
  return true;
}

/**
 * \brief Gives the sprites the animation stopped corresponding to this state.
 */
void Hero::CarryingState::set_animation_stopped() {
  get_sprites().set_animation_stopped_carrying();
}

/**
 * \brief Gives the sprites the animation walking corresponding to this state.
 */
void Hero::CarryingState::set_animation_walking() {
  get_sprites().set_animation_walking_carrying();
}

/**
 * \brief Returns the item currently carried by the hero in this state, if any.
 * \return the item carried by the hero, or nullptr
 */
std::shared_ptr<CarriedItem> Hero::CarryingState::get_carried_item() const {
  return carried_item;
}

/**
 * \copydoc Hero::State::get_previous_carried_item_behavior
 */
CarriedItem::Behavior Hero::CarryingState::get_previous_carried_item_behavior() const {

  return CarriedItem::BEHAVIOR_KEEP;
}

}

