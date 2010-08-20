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
#include "hero/State.h"
#include "hero/SwordSwingingState.h"
#include "hero/InventoryItemState.h"
#include "hero/HeroSprites.h"
#include "entities/Hero.h"
#include "lowlevel/System.h"
#include "Game.h"
#include "Map.h"
#include "Equipment.h"
#include "InventoryItem.h"
#include "Sprite.h"
#include "KeysEffect.h"

/**
 * @brief Creates a state.
 *
 * This constructor can be called only from the subclasses.
 *
 * @param hero the hero to control with this state
 */
Hero::State::State(Hero *hero):
  game(hero->get_game()), map(hero->get_map()), hero(hero), sprites(hero->get_sprites()),
  suspended(false), when_suspended(0) {

}

/**
 * @brief Destructor.
 *
 * The state is destroyed once it is not the current state of the hero anymore.
 */
Hero::State::~State(void) {

}

/**
 * @brief Returns whether this state is the current state.
 * @return true if this state is the current state
 */
bool Hero::State::is_current_state(void) {
  return hero->state == this;
}

/**
 * @brief Starts this state.
 *
 * This function is called automatically when this state becomes the active state of the hero.
 * The initializations should be done here rather than in the constructor.
 *
 * @param previous_state the previous state or NULL if this is the first state (for information)
 */
void Hero::State::start(State *previous_state) {
  set_suspended(hero->is_suspended());
}

/**
 * @brief Ends this state.
 *
 * This function is called automatically when this state is not the active state anymore.
 * You should here close everything the start() function has opened.
 * The destructor will be called at the next cycle.
 *
 * @param next_state the next state (for information)
 */
void Hero::State::stop(State *next_state) {
}

/**
 * @brief Updates this state.
 *
 * This function is called repeatedly while this state is the active state.
 */
void Hero::State::update(void) {

}

/**
 * @brief Displays this state.
 *
 * This function displays the hero's sprites in its current state.
 * If your state needs to display additional elements, you can redefine this function.
 */
void Hero::State::display_on_map(void) {

  hero->get_sprites()->display_on_map();
}

/**
 * @brief Notifies this state that the game was just suspended or resumed.
 * @param suspended true if the game is suspended
 */
void Hero::State::set_suspended(bool suspended) {

  if (suspended != this->suspended) {

    this->suspended = suspended;

    // remember the date if the state is being suspended
    if (suspended) {
      when_suspended = System::now();
    }
  }
}

/**
 * @brief This function is called when a game key is pressed and the game is not suspended.
 * @param key the key pressed
 */
void Hero::State::key_pressed(GameControls::GameKey key) {

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
    directional_key_pressed(0);
    break;

  case GameControls::UP:
    directional_key_pressed(1);
    break;

  case GameControls::LEFT:
    directional_key_pressed(2);
    break;

  case GameControls::DOWN:
    directional_key_pressed(3);
    break;

    // use an inventory item
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
 * @brief This function is called when a key is released if the game is not suspended.
 * @param key the key released
 */
void Hero::State::key_released(GameControls::GameKey key) {

  switch (key) {

  case GameControls::ACTION:
    action_key_released();
    break;

  case GameControls::SWORD:
    sword_key_released();
    break;

  case GameControls::RIGHT:
    directional_key_released(0);
    break;

  case GameControls::UP:
    directional_key_released(1);
    break;

  case GameControls::LEFT:
    directional_key_released(2);
    break;

  case GameControls::DOWN:
    directional_key_released(3);
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
 * @brief Notifies this state that the action key was just pressed.
 */
void Hero::State::action_key_pressed(void) {
}

/**
 * @brief Notifies this state that the action key was just released.
 */
void Hero::State::action_key_released(void) {
}

/**
 * @brief Notifies this state that the sword key was just pressed.
 */
void Hero::State::sword_key_pressed(void) {

  KeysEffect *keys_effect = game->get_keys_effect();
  if (!hero->is_suspended()
      && keys_effect->get_sword_key_effect() == KeysEffect::SWORD_KEY_SWORD
      && can_start_sword()) {

    hero->set_state(new SwordSwingingState(hero));
  }
}

/**
 * @brief Notifies this state that the sword key was just released.
 */
void Hero::State::sword_key_released(void) {
}

/**
 * @brief Notifies this state that a directional key was just pressed.
 * @param direction4 direction of the key (0 to 3)
 */
void Hero::State::directional_key_pressed(int direction4) {
}

/**
 * @brief Notifies this state that a directional key was just released.
 * @param direction4 direction of the key (0 to 3)
 */
void Hero::State::directional_key_released(int direction4) {
}

/**
 * @brief Notifies this state that an item key was just pressed.
 * @param slot the slot activated (0 or 1)
 */
void Hero::State::item_key_pressed(int slot) {

  Equipment *equipment = game->get_equipment();
  InventoryItemId item_id = equipment->get_item_assigned(slot);

  if (InventoryItem::can_be_assigned(item_id)
      && equipment->has_inventory_item(item_id)
      && (item_id != hero->last_inventory_item_id || System::now() >= hero->can_use_inventory_item_date)
      && can_start_inventory_item()) {

    hero->set_state(new InventoryItemState(hero, item_id));
  }
}

/**
 * @brief Notifies this state that an item key was just released.
 * @param slot the slot (0 or 1)
 */
void Hero::State::item_key_released(int slot) {
}

/**
 * @brief Changes the map.
 *
 * This function is called when the hero is about to go to another map.
 *
 * @param map the new map
 */
void Hero::State::set_map(Map *map) {
  this->map = map;
  this->game = map->get_game();
}

/**
 * @brief Returns whether the game over sequence can start in the current state.
 * @return true if the game over sequence can start in the current state
 */
bool Hero::State::can_start_gameover_sequence(void) {
  return true;
}

/**
 * @brief Returns whether the hero is visible in the current state.
 * @return true if the hero is displayed in the current state
 */
bool Hero::State::is_hero_visible(void) {
  return true;
}

/**
 * @brief Returns whether the animation direction is locked.
 *
 * When this function returns false, which is the case most of the time,
 * it means that the animation direction is set to the movement direction.
 * When it returns true, it means that the animation direction is fixed
 * and do not depends on the movement direction anymore (this is the case
 * when the hero is loading his sword).
 *
 * @return true if the animation direction is locked
 */
bool Hero::State::is_direction_locked(void) {
  return false;
}

/**
 * @brief When a shadow is displayed separate from the tunic sprite,
 * returns the height where the tunic sprite should be displayed.
 * @return the height in pixels, or zero if there is no separate shadow in this state
 */
int Hero::State::get_height_above_shadow(void) {
  return 0;
}

/**
 * @brief Returns whether the player can control his movements in the current state.
 *
 * This function returns true if and only if the current movement of the hero is
 * an instance of PlayerMovement.
 *
 * @return true if the player can control his movements
 */
bool Hero::State::can_control_movement(void) {
  return false;
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
int Hero::State::get_wanted_movement_direction8(void) {
  return -1;
}

/**
 * @brief Notifies this state that the walking speed has changed.
 * 
 * If the hero can walk in this state, the state should modify its movement
 * to set the new speed.
 */
void Hero::State::notify_walking_speed_changed(void) {
}

/**
 * @brief Notifies this state that the layer has changed.
 */
void Hero::State::notify_layer_changed(void) {
}

/**
 * @brief Notifies this state that the movement has changed.
 *
 * This function is called when the hero's movement direction changes (for instance
 * because the player pressed or released a directional key, or the hero just reached an obstacle).
 * The animations and collisions should be updated according to the new movement.
 */
void Hero::State::notify_movement_changed(void) {
}

/**
 * @brief Notifies this state that the hero has just tried to change his position.
 *
 * This function is called only when the game is not suspended.
 *
 * @param success true if the position has actually just changed
 */
void Hero::State::notify_movement_tried(bool success) {
}

/**
 * @brief Returns whether the hero ignores the effect of deep water in this state.
 *
 * Returns false by default.
 *
 * @return true if the hero ignores the effect of deep water in the current state
 */
bool Hero::State::can_avoid_deep_water(void) {
  return false;
}

/**
 * @brief Returns whether the hero ignores the effect of holes in this state.
 *
 * Returns false by default.
 *
 * @return true if the hero ignores the effect of holes in the current state
 */
bool Hero::State::can_avoid_hole(void) {
  return false;
}

/**
 * @brief Returns whether the hero is touching the ground in the current state.
 *
 * Returns true by default.
 *
 * @return true if the hero is touching the ground in the current state
 */
bool Hero::State::is_touching_ground(void) {
  return true;
}

/**
 * @brief Notifies this state that the ground was just changed.
 */
void Hero::State::notify_ground_changed(void) {
}

/**
 * @brief Returns whether this state ignores the collisions with the detectors and the ground.
 * @return true if the collision are ignored
 */
bool Hero::State::are_collisions_ignored(void) {
  return false;
}

/**
 * @brief Returns whether a deep water tile is considered as an obstacle in this state.
 *
 * Returns false by default.
 *
 * @return true if the deep water tiles are considered as obstacles in this state
 */
bool Hero::State::is_water_obstacle(void) {
  return false;
}

/**
 * @brief Returns whether a hole is considered as an obstacle in this state.
 *
 * Returns false by default.
 *
 * @return true if the holes are considered as obstacles in this state
 */
bool Hero::State::is_hole_obstacle(void) {
  return false;
}

/**
 * @brief Returns whether a ladder is considered as an obstacle in this state.
 *
 * Returns false by default.
 *
 * @return true if the ladders are considered as obstacles in this state
 */
bool Hero::State::is_ladder_obstacle(void) {
  return false;
}

/**
 * @brief Returns whether a teletransporter is considered as an obstacle in this state.
 *
 * Returns false by default.
 *
 * @param teletransporter a teletransporter
 * @return true if the teletransporter is an obstacle in this state
 */
bool Hero::State::is_teletransporter_obstacle(Teletransporter *teletransporter) {
  return false;
}

/**
 * @brief Returns whether the hero ignores the effect of teletransporters in this state.
 *
 * Returns false by default.
 *
 * @return true if the hero ignores the effect of teletransporters in this state
 */
bool Hero::State::can_avoid_teletransporter(void) {
  return false;
}

/**
 * @brief Returns whether the effect of teletransporters is delayed in this state.
 *
 * When overlapping a teletransporter, if this function returns true, the teletransporter
 * will not be activated immediately. The state then has to activate it when it is ready.
 * Returns false by default.
 */
bool Hero::State::is_teletransporter_delayed(void) {
  return false;
}

/**
 * @brief Returns whether a conveyor belt is considered as an obstacle in this state.
 *
 * Returns false by default.
 *
 * @param conveyor_belt a conveyor belt
 * @return true if the conveyor belt is an obstacle in this state
 */
bool Hero::State::is_conveyor_belt_obstacle(ConveyorBelt *conveyor_belt) {
  return false;
}

/**
 * @brief Returns whether the hero ignores the effect of conveyor belts in this state.
 *
 * Returns false by default.
 *
 * @return true if the hero ignores the effect of conveyor belts in this state
 */
bool Hero::State::can_avoid_conveyor_belt(void) {
  return false;
}

/**
 * @brief Returns whether the hero can take stairs in this state.
 * If false is returned, stairs have no effect (but they are obstacle for the hero).
 *
 * Returns false by default.
 *
 * @return true if the hero can take stairs in this state
 */
bool Hero::State::can_take_stairs(void) {
  return false;
}

/**
 * @brief Returns whether can trigger a jump sensor in this state.
 * If false is returned, jump sensors have no effect (but they are obstacle for the hero).
 *
 * Returns false by default.
 *
 * @return true if the hero can use jump sensors in this state
 */
bool Hero::State::can_take_jump_sensor(void) {
  return false;
}

/**
 * @brief Returns whether a sensor is considered as an obstacle in this state.
 *
 * Returns false by default.
 *
 * @param sensor a sensor
 * @return true if the sensor is an obstacle in this state
 */
bool Hero::State::is_sensor_obstacle(Sensor *sensor) {
  return false;
}

/**
 * @brief Returns whether the hero ignores the effect of sensors in this state.
 *
 * Returns false by default.
 *
 * @return true if the hero ignores the effect of sensors in this state
 */
bool Hero::State::can_avoid_sensor(void) {
  return false;
}

/**
 * @brief Returns whether the hero ignores the effect of explosions in this state.
 *
 * Returns false by default.
 *
 * @return true if the hero ignores the effect of explosions in this state
 */
bool Hero::State::can_avoid_explosion(void) {
  return false;
}

/**
 * @brief Returns whether crystal switches can be activated by the sword in this state.
 *
 * Returns false by default.
 *
 * @return true if crystal switches can be activated by the sword in this state
 */
bool Hero::State::can_sword_hit_crystal_switch(void) {
  return false;
}

/**
 * @brief Notifies this state that the hero has just attacked an enemy
 *
 * This function is called even if this attack was not successful.
 *
 * @param attack the attack
 * @param victim the enemy just hurt
 * @param result indicates how the enemy has reacted to the attack (see Enemy.h)
 * @param killed indicates that the attack has just killed the enemy
 */
void Hero::State::notify_attacked_enemy(EnemyAttack attack, Enemy *victim, int result, bool killed) {
}

/**
 * @brief Returns the damage power of the sword for the current attack.
 *
 * Redefine the function if your state changes the power of the sword
 * (typically for a spin attack).
 *
 * @return the current damage factor of the sword
 */
int Hero::State::get_sword_damage_factor(void) {

  static const int sword_factors[] = {0, 1, 2, 4, 8};
  int sword = game->get_equipment()->get_sword();
  return sword_factors[sword];
}

/**
 * @brief Returns whether the hero can be hurt in this state.
 *
 * Returns false by default.
 *
 * @return true if the hero can be hurt in this state
 */
bool Hero::State::can_be_hurt(void) {
  return false;
}

/**
 * @brief Returns whether the hero can walk normally and interact with entities
 * in this state.
 *
 * Returns false by default.
 *
 * @return true if the hero can walk normally
 */
bool Hero::State::is_free(void) {
  return false;
}

/**
 * @brief Returns whether the hero is grabbing or pulling an entity in this state.
 *
 * Returns false by default.
 *
 * @return true if the hero is grabbing or pulling an entity
 */
bool Hero::State::is_grabbing_or_pulling(void) {
  return false;
}

/**
 * @brief Returns whether the hero is grabbing and moving an entity in this state.
 *
 * If he is not grabbing any entity, false is returned.
 * Returns false by default.
 *
 * @return true if the hero is grabbing and moving an entity
 */
bool Hero::State::is_moving_grabbed_entity(void) {
  return false;
}

/**
 * @brief Notifies the hero that the entity he is pushing or pulling in this state
 * cannot move anymore because of a collision.
 */
void Hero::State::notify_grabbed_entity_collision(void) {
}

/**
 * @brief Tests whether the hero is cutting with his sword the specified detector
 * for which a collision was detected.
 *
 * When the sword sprite collides with a detector,
 * this function can be called to determine whether the hero is
 * really cutting this particular detector precisely.
 * This depends on the hero's state, his direction and his
 * distance to the detector.
 * This function assumes that there is already a collision
 * between the sword sprite and the detector's sprite.
 * This function should be called to check whether the
 * hero wants to cut a bush or some grass.
 * Returns false by default.
 *
 * @param detector the detector to check
 * @return true if the sword is cutting this detector
 */
bool Hero::State::is_cutting_with_sword(Detector *detector) {
  return false;
}

/**
 * @brief Returns whether the hero can swing his sword in this state.
 *
 * Returns false by default.
 *
 * @return true if the hero can swing his sword in this state
 */
bool Hero::State::can_start_sword(void) {
  return false;
}

/**
 * @brief Returns whether the hero can use an inventory item in this state.
 *
 * Returns false by default.
 *
 * @return true if the hero can use an inventoy item in this state
 */
bool Hero::State::can_start_inventory_item(void) {
  return false;
}

/**
 * @brief Returns whether an item previously carried by the hero should be thrown when this state starts.
 * 
 * If false is returned, the item will be directly removed.
 * If true is returned, the item will be thrown in the direction the hero is looking towards.
 * Returns true by default.
 *
 * @return true if an item previously carried by the hero should be thrown when this state starts
 */
bool Hero::State::can_throw_item(void) {
  return true;
}

