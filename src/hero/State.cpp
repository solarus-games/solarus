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
#include "lowlevel/Debug.h"
#include "Game.h"
#include "Map.h"
#include "Equipment.h"
#include "ItemProperties.h"
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
Hero::State::State(Hero &hero):

  map(&hero.get_map()),
  hero(hero),
  suspended(false),
  when_suspended(0) {

}

/**
 * @brief Destructor.
 *
 * The state is destroyed once it is not the current state of the hero anymore.
 */
Hero::State::~State() {
}

/**
 * @brief Returns whether this state is the current state.
 * @return true if this state is the current state
 */
bool Hero::State::is_current_state() {
  return hero.state == this;
}

/**
 * @brief Returns the hero's sprites.
 * @return the sprites
 */
HeroSprites& Hero::State::get_sprites() {
  return hero.get_sprites();
}

/**
 * @brief Returns the current map.
 * @return the map
 */
Map& Hero::State::get_map() {
  return *map;
}

/**
 * @brief Returns the entities of the current map.
 * @return the entities
 */
MapEntities& Hero::State::get_entities() {
  return map->get_entities();
}

/**
 * @brief Returns the script of the current map.
 * @return the current script
 */
MapScript& Hero::State::get_map_script() {
  return map->get_script();
}

/**
 * @brief Returns the current game.
 * @return the game
 */
Game& Hero::State::get_game() {
  return map->get_game();
}

/**
 * @brief Returns the current equipment.
 * @return the equipment
 */
Equipment& Hero::State::get_equipment() {
  return get_game().get_equipment();
}

/**
 * @brief Returns the keys effect manager.
 * @return the keys effect
 */
KeysEffect& Hero::State::get_keys_effect() {
  return get_game().get_keys_effect();
}

/**
 * @brief Returns the game controls.
 * @return the controls
 */
GameControls& Hero::State::get_controls() {
  return get_game().get_controls();
}

/**
 * @brief Returns the dialog box manager.
 * @return the dialog box
 */
DialogBox& Hero::State::get_dialog_box() {
  return get_game().get_dialog_box();
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
  set_suspended(hero.is_suspended());
  hero.set_facing_entity(NULL);
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
void Hero::State::update() {

}

/**
 * @brief Displays this state.
 *
 * This function displays the hero's sprites in its current state.
 * If your state needs to display additional elements, you can redefine this function.
 */
void Hero::State::display_on_map() {

  get_sprites().display_on_map();
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
void Hero::State::action_key_pressed() {
}

/**
 * @brief Notifies this state that the action key was just released.
 */
void Hero::State::action_key_released() {
}

/**
 * @brief Notifies this state that the sword key was just pressed.
 */
void Hero::State::sword_key_pressed() {

  if (!hero.is_suspended()
      && get_keys_effect().get_sword_key_effect() == KeysEffect::SWORD_KEY_SWORD
      && can_start_sword()) {

    hero.set_state(new SwordSwingingState(hero));
  }
}

/**
 * @brief Notifies this state that the sword key was just released.
 */
void Hero::State::sword_key_released() {
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

  const std::string item_name = get_equipment().get_item_assigned(slot);

  if (item_name.size() > 0
      && get_equipment().get_item_properties(item_name).can_be_assigned()
      && get_equipment().has_item(item_name)
      && can_start_inventory_item()) {

    hero.set_state(new InventoryItemState(hero, item_name));
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
void Hero::State::set_map(Map &map) {
  this->map = &map;
}

/**
 * @brief Returns whether the game over sequence can start in the current state.
 * @return true if the game over sequence can start in the current state
 */
bool Hero::State::can_start_gameover_sequence() {
  return true;
}

/**
 * @brief Returns whether the hero is visible in the current state.
 * @return true if the hero is displayed in the current state
 */
bool Hero::State::is_hero_visible() {
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
bool Hero::State::is_direction_locked() {
  return false;
}

/**
 * @brief Returns whether the player can control his movements in the current state.
 *
 * This function returns true if and only if the current movement of the hero is
 * an instance of PlayerMovement.
 *
 * @return true if the player can control his movements
 */
bool Hero::State::can_control_movement() {
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
int Hero::State::get_wanted_movement_direction8() {
  return -1;
}

/**
 * @brief Notifies this state that the walking speed has changed.
 * 
 * If the hero can walk in this state, the state should modify its movement
 * to set the new speed.
 */
void Hero::State::notify_walking_speed_changed() {
}

/**
 * @brief Notifies this state that the layer has changed.
 */
void Hero::State::notify_layer_changed() {
}

/**
 * @brief Notifies this state that the movement has changed.
 *
 * This function is called when the hero's movement direction changes (for instance
 * because the player pressed or released a directional key, or the hero just reached an obstacle).
 * The animations and collisions should be updated according to the new movement.
 */
void Hero::State::notify_movement_changed() {
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
bool Hero::State::can_avoid_deep_water() {
  return false;
}

/**
 * @brief Returns whether the hero ignores the effect of holes in this state.
 *
 * Returns false by default.
 *
 * @return true if the hero ignores the effect of holes in the current state
 */
bool Hero::State::can_avoid_hole() {
  return false;
}

/**
 * @brief Returns whether the hero ignores the effect of lava in this state.
 *
 * Returns false by default.
 *
 * @return true if the hero ignores the effect of lava in the current state
 */
bool Hero::State::can_avoid_lava() {
  return false;
}

/**
 * @brief Returns whether the hero ignores the effect of prickles in this state.
 *
 * Returns false by default.
 *
 * @return true if the hero ignores the effect of prickles in the current state
 */
bool Hero::State::can_avoid_prickle() {
  return false;
}

/**
 * @brief Returns whether the hero is touching the ground in the current state.
 *
 * Returns true by default.
 *
 * @return true if the hero is touching the ground in the current state
 */
bool Hero::State::is_touching_ground() {
  return true;
}

/**
 * @brief Returns whether the hero's current position can be considered
 * as a place to come back after a bad ground (hole, deep water, etc).
 *
 * Returns is_touching_ground() by default.
 *
 * @return true if the hero can come back here
 */
bool Hero::State::can_come_from_bad_ground() {
  return is_touching_ground();
}

/**
 * @brief Notifies this state that the ground was just changed.
 */
void Hero::State::notify_ground_changed() {
}

/**
 * @brief Returns whether this state ignores the collisions with the detectors and the ground.
 * @return true if the collision are ignored
 */
bool Hero::State::are_collisions_ignored() {
  return false;
}

/**
 * @brief Returns whether a deep water tile is considered as an obstacle in this state.
 *
 * Returns false by default.
 *
 * @return true if the deep water tiles are considered as obstacles in this state
 */
bool Hero::State::is_deep_water_obstacle() {
  return false;
}

/**
 * @brief Returns whether a hole is considered as an obstacle in this state.
 *
 * Returns false by default.
 *
 * @return true if the holes are considered as obstacles in this state
 */
bool Hero::State::is_hole_obstacle() {
  return false;
}

/**
 * @brief Returns whether lava is considered as an obstacle in this state.
 *
 * Returns false by default.
 *
 * @return true if lava is considered as obstacles in this state
 */
bool Hero::State::is_lava_obstacle() {
  return false;
}

/**
 * @brief Returns whether prickles are considered as an obstacle in this state.
 *
 * Returns false by default.
 *
 * @return true if prickles are considered as obstacles in this state
 */
bool Hero::State::is_prickle_obstacle() {
  return false;
}

/**
 * @brief Returns whether a ladder is considered as an obstacle in this state.
 *
 * Returns false by default.
 *
 * @return true if the ladders are considered as obstacles in this state
 */
bool Hero::State::is_ladder_obstacle() {
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
bool Hero::State::is_teletransporter_obstacle(Teletransporter& teletransporter) {
  return false;
}

/**
 * @brief Returns whether the hero ignores the effect of teletransporters in this state.
 *
 * Returns false by default.
 *
 * @return true if the hero ignores the effect of teletransporters in this state
 */
bool Hero::State::can_avoid_teletransporter() {
  return false;
}

/**
 * @brief Returns whether the effect of teletransporters is delayed in this state.
 *
 * When overlapping a teletransporter, if this function returns true, the teletransporter
 * will not be activated immediately. The state then has to activate it when it is ready.
 * Returns false by default.
 *
 * @return true if the effect of teletransporters is delayed in this state
 */
bool Hero::State::is_teletransporter_delayed() {
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
bool Hero::State::is_conveyor_belt_obstacle(ConveyorBelt& conveyor_belt) {
  return false;
}

/**
 * @brief Returns whether the hero ignores the effect of conveyor belts in this state.
 *
 * Returns false by default.
 *
 * @return true if the hero ignores the effect of conveyor belts in this state
 */
bool Hero::State::can_avoid_conveyor_belt() {
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
bool Hero::State::can_take_stairs() {
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
bool Hero::State::can_take_jump_sensor() {
  return false;
}

/**
 * @brief Returns whether some stairs are considered as obstacle in this state.
 *
 * Returns true by default.
 *
 * @param stairs some stairs
 * @return true if the stairs are obstacle in this state
 */
bool Hero::State::is_stairs_obstacle(Stairs& stairs) {
  return true;
}

/**
 * @brief Returns whether a sensor is considered as an obstacle in this state.
 *
 * Returns false by default.
 *
 * @param sensor a sensor
 * @return true if the sensor is an obstacle in this state
 */
bool Hero::State::is_sensor_obstacle(Sensor& sensor) {
  return false;
}

/**
 * @brief Returns whether the hero ignores the effect of sensors in this state.
 *
 * Returns false by default.
 *
 * @return true if the hero ignores the effect of sensors in this state
 */
bool Hero::State::can_avoid_sensor() {
  return false;
}

/**
 * @brief Returns whether the hero ignores the effect of switches in this state.
 *
 * Returns false by default.
 *
 * @return true if the hero ignores the effect of switches in this state
 */
bool Hero::State::can_avoid_switch() {
  return false;
}

/**
 * @brief Returns whether crystal switches can be activated by the sword in this state.
 *
 * Returns false by default.
 *
 * @return true if crystal switches can be activated by the sword in this state
 */
bool Hero::State::can_sword_hit_crystal_switch() {
  return false;
}

/**
 * @brief Returns whether the hero ignores the effect of explosions in this state.
 *
 * Returns false by default.
 *
 * @return true if the hero ignores the effect of explosions in this state
 */
bool Hero::State::can_avoid_explosion() {
  return false;
}

/**
 * @brief Notifies this state that the hero has just attacked an enemy
 *
 * This function is called even if this attack was not successful.
 *
 * @param attack the attack
 * @param victim the enemy just hurt
 * @param result indicates how the enemy has reacted to the attack
 * @param killed indicates that the attack has just killed the enemy
 */
void Hero::State::notify_attacked_enemy(EnemyAttack attack, Enemy& victim,
    EnemyReaction::Reaction& result, bool killed) {
}

/**
 * @brief Returns the damage power of the sword for the current attack.
 *
 * Redefine the function if your state changes the power of the sword
 * (typically for a spin attack).
 *
 * @return the current damage factor of the sword
 */
int Hero::State::get_sword_damage_factor() {

  static const int sword_factors[] = {0, 1, 2, 4, 8};
  int sword = get_equipment().get_ability("sword");
  return sword_factors[sword];
}

/**
 * @brief Returns whether the hero can be hurt in this state.
 *
 * Returns false by default.
 *
 * @return true if the hero can be hurt in this state
 */
bool Hero::State::can_be_hurt() {
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
bool Hero::State::is_free() {
  return false;
}

/**
 * @brief Returns whether the hero is using an inventory item in this state.
 *
 * Returns false by default.
 *
 * @return true if the hero is using an inventory item
 */
bool Hero::State::is_using_inventory_item() {
  return false;
}

/**
 * @brief When the hero is using an inventory item, returns the inventory item.
 * @return the current inventory item
 */
InventoryItem& Hero::State::get_current_inventory_item() {

  Debug::die("No inventory item in this state");
  throw;
}

/**
 * @brief Returns whether the hero is grabbing or pulling an entity in this state.
 *
 * Returns false by default.
 *
 * @return true if the hero is grabbing or pulling an entity
 */
bool Hero::State::is_grabbing_or_pulling() {
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
bool Hero::State::is_moving_grabbed_entity() {
  return false;
}

/**
 * @brief Notifies the hero that the entity he is pushing or pulling in this state
 * cannot move anymore because of a collision.
 */
void Hero::State::notify_grabbed_entity_collision() {
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
bool Hero::State::is_cutting_with_sword(Detector &detector) {
  return false;
}

/**
 * @brief Returns whether the hero can swing his sword in this state.
 *
 * Returns false by default.
 *
 * @return true if the hero can swing his sword in this state
 */
bool Hero::State::can_start_sword() {
  return false;
}

/**
 * @brief Returns whether the hero can use an inventory item in this state.
 *
 * Returns false by default.
 *
 * @return true if the hero can use an inventoy item in this state
 */
bool Hero::State::can_start_inventory_item() {
  return false;
}

/**
 * @brief Returns whether the hero is currently carrying an item in this state.
 *
 * This function returns true if get_carried_item() is not NULL.
 * Redefine get_carried_item() if the hero is able to carry an item in this state.
 *
 * @return true if the hero is currently carrying an item in this state
 */
bool Hero::State::is_carrying_item() {
  return get_carried_item() != NULL;
}

/**
 * @brief Returns the item currently carried by the hero in this state, if any.
 *
 * Redefine this function to make the hero able to carry an item in this state.
 *
 * @return the item carried by the hero, or NULL
 */
CarriedItem* Hero::State::get_carried_item() {
  return NULL;
}

/**
 * @brief Returns the action to do with an item previously carried by the hero when this state starts.
 * 
 * Returns CarriedItem::BEHAVIOR_THROW by default.
 *
 * @param carried_item the item carried in the previous state
 * @return the action to do with a previous carried item when this state starts
 */
CarriedItem::Behavior Hero::State::get_previous_carried_item_behavior(CarriedItem& carried_item) {
  return CarriedItem::BEHAVIOR_THROW;
}

