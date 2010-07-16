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
#include "entities/Hero.h"
#include "entities/Detector.h"
#include "movements/PlayerMovement.h"
#include "Game.h"
#include "GameControls.h"
#include "KeysEffect.h"
#include "InventoryItemId.h"
#include "Equipment.h"

/**
 * @brief This function is called when a game key is pressed
 * if the game is not suspended.
 * @param key the key pressed
 */
void Hero::key_pressed(GameControls::GameKey key) {

  switch (key) {

    // action key
  case GameControls::ACTION:
    action_key_pressed();
    break;

    // sword key
  case GameControls::SWORD:
    sword_key_pressed();
    break;

    // move the hero
  case GameControls::RIGHT:
    arrow_pressed(0);
    break;

  case GameControls::UP:
    arrow_pressed(1);
    break;

  case GameControls::LEFT:
    arrow_pressed(2);
    break;

  case GameControls::DOWN:
    arrow_pressed(3);
    break;

  case GameControls::ITEM_1:
    item_key_pressed(0);
    break;

  case GameControls::ITEM_2:
    item_key_pressed(1);
    break;

  default:
    break;
  }
}

/**
 * @brief This function is called when a key is released
 * if the game is not suspended.
 * @param key the key released
 */
void Hero::key_released(GameControls::GameKey key) {

  /*
   * When the action key or the sword key are released,
   * we do nothing here: the update() functions will detect that
   * the key is not pressed anymore. Thus, the key will be
   * detected even if it is released while the game is suspended.
   */

  switch (key) {

  case GameControls::RIGHT:
    arrow_released(0);
    break;

  case GameControls::UP:
    arrow_released(1);
    break;

  case GameControls::LEFT:
    arrow_released(2);
    break;

  case GameControls::DOWN:
    arrow_released(3);
    break;

  case GameControls::ITEM_1:
    item_key_released(0);
    break;

  case GameControls::ITEM_2:
    item_key_released(1);
    break;

  default:
    break;
  }
}

/**
 * @brief This function is called when the action key is pressed.
 *
 * Depending on its effect, an action may be performed.
 */
void Hero::action_key_pressed(void) {

  // this line causes a bug: the hero can interact with an entity with the wrong sprite direction
  //  set_animation_direction_from_movement(); 

  KeysEffect *keys_effect = game->get_keys_effect();

  switch (keys_effect->get_action_key_effect()) {

  case KeysEffect::ACTION_KEY_THROW:
    // throw the object carried
    start_throwing();
    break;

  case KeysEffect::ACTION_KEY_GRAB:
    // grab an entity
    start_grabbing();
    break;

  default:

    if (keys_effect->is_action_key_acting_on_facing_entity()) {
      // action on the facing entity
      facing_entity->action_key_pressed();
    }
    else if (is_facing_obstacle() && get_state() == FREE) {
      // grab an obstacle
      start_grabbing();
    }
    break;
  }
}

/**
 * @brief This function is called when the sword key is pressed.
 *
 * Depending on its effect, an action may be performed.
 */
void Hero::sword_key_pressed(void) {

  if (can_start_sword()) {
    start_sword();
  }
}

/**
 * @brief This function is called when an item key is pressed.
 *
 * Depending on the item assigned to this key, an
 * action may be performed.
 */
void Hero::item_key_pressed(int slot) {

  InventoryItemId item_id = equipment->get_item_assigned(slot);

  if (can_start_inventory_item(item_id)) {
    start_inventory_item(item_id);
  }
}

/**
 * @brief This function is called when an item key is relased.
 *
 * Depending on the item assigned to this key, an
 * action may be performed.
 */
void Hero::item_key_released(int slot) {

  if (is_running() && inventory_item_phase == 0) {
    stop_running();
  }
}

/**
 * @brief This function is called when an directional key is pressed.
 * @param direction of the arrow pressed (0 to 3)
 */
void Hero::arrow_pressed(int direction) {

  // notify the movement
  get_normal_movement()->add_direction(direction);

  if (is_running()) {
    stop_running();
  }
}

/**
 * @brief This function is called when a directional key is released.
 * @param direction of the arrow released (0 to 3)
 */
void Hero::arrow_released(int direction) {

  // notify the movement
  get_normal_movement()->remove_direction(direction);
}

