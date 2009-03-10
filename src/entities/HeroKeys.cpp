#include "entities/Hero.h"
#include "entities/Detector.h"
#include "movements/PlayerMovement.h"
#include "ZSDX.h"
#include "Game.h"
#include "KeysEffect.h"

/**
 * This function is called by the controls when a game key is pressed
 * if the game is not suspended.
 * @param key the key pressed
 */
void Hero::key_pressed(Controls::GameKey key) {

  switch (key) {

    // action key
  case Controls::ACTION:
    action_key_pressed();
    break;

    // sword key
  case Controls::SWORD:
    sword_key_pressed();
    break;

    // move the hero
  case Controls::RIGHT:
    arrow_pressed(0);
    break;

  case Controls::UP:
    arrow_pressed(1);
    break;

  case Controls::LEFT:
    arrow_pressed(2);
    break;

  case Controls::DOWN:
    arrow_pressed(3);
    break;

  default:
    break;
  }
}

/**
 * This function is called when a key is released
 * if the game is not suspended.
 * @param key the key released
 */
void Hero::key_released(Controls::GameKey key) {

  /*
   * When the action key or the sword key are released,
   * we do nothing here: the update() functions will detect that
   * the key is not pressed anymore. Thus, the key will be
   * detected even if it is released while the game is suspended.
   */

  switch (key) {

  case Controls::RIGHT:
    arrow_released(0);
    break;

  case Controls::UP:
    arrow_released(1);
    break;

  case Controls::LEFT:
    arrow_released(2);
    break;

  case Controls::DOWN:
    arrow_released(3);
    break;

  default:
    break;
  }
}

/**
 * This function is called when the action key is pressed.
 * Depending on its effect, an action may be performed.
 */
void Hero::action_key_pressed(void) {

  KeysEffect *keys_effect = zsdx->game->get_keys_effect();

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
 * This function is called when the sword key is pressed.
 * Depending on its effect, an action may be performed.
 */
void Hero::sword_key_pressed(void) {

  if (can_start_sword()) {
    start_sword();
  }
}

/**
 * This function is called when an arrow key is pressed.
 * @param direction of the arrow pressed (0 to 3)
 */
void Hero::arrow_pressed(int direction) {

  // notify the movement
  get_normal_movement()->add_direction(direction);

  // grabbing or pulling
  if (state == GRABBING) {

    int opposite_direction = (get_animation_direction() + 2) % 4;
    if (direction == get_animation_direction()) {
      start_pushing();
    }
    else if (direction == opposite_direction) {
      start_pulling();
    }
  }
}

/**
 * This function is called when an arrow key is released.
 * @param direction of the arrow released (0 to 3)
 */
void Hero::arrow_released(int direction) {

  // notify the movement
  get_normal_movement()->remove_direction(direction);
}
