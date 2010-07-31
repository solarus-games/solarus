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
#include "hero/StatePlayerMovement.h"
#include "movements/PlayerMovement.h"

/**
 * @brief Constructor.
 * @param hero the hero
 */
Hero::StatePlayerMovement::StatePlayerMovement(Hero *hero):
  State(hero) {

}

/**
 * @brief Destructor.
 */
Hero::StatePlayerMovement::~StatePlayerMovement(void) {

}

/**
 * @brief Returns the movement of the hero controlled by the player.
 * @return the movement
 */
PlayerMovement * Hero::StatePlayerMovement::get_player_movement(void) {
  return (PlayerMovement*) hero->get_movement();
}

/**
 * @brief Starts this state.
 *
 * This function is called automatically when this state becomes the active state of the hero.
 * The initializations should be done here rather than in the constructor.
 *
 * @param previous_state the previous state of NULL if this is the first state (for information)
 */
void Hero::StatePlayerMovement::start(State *previous_state) {

  if (previous_state == NULL || !previous_state->can_control_movement()) {
    // create the movement unless it already exists
    hero->set_movement(new PlayerMovement(hero->get_walking_speed()));
  }

  get_player_movement()->set_moving_enabled(true, true);
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
void Hero::StatePlayerMovement::stop(State *next_state) {
  
  if (!next_state->can_control_movement()) {
    // destroy the movement unless the next state intends to keep it
    hero->clear_movement();
  }
}

/**
 * @brief Notifies this state that a directional key was just pressed.
 * @param direction4 direction of the key (0 to 3)
 */
void Hero::StatePlayerMovement::directional_key_pressed(int direction4) {

  // notify the player's movement
  get_player_movement()->add_direction(direction);
}

/**
 * @brief Notifies this state that a directional key was just released.
 * @param direction4 direction of the key (0 to 3)
 */
void Hero::StatePlayerMovement::directional_key_released(int direction4) {

  // notify the movement
  get_player_movement()->remove_direction(direction);
}

/**
 * @brief Changes the map.
 *
 * This function is called when the hero is about to go to another map.
 */
void Hero::StatePlayerMovement::set_map(Map *map) {
}

/**
 * @brief Returns whether the player can control his movements in the current state.
 * @return true if the player can control his movements
 */
bool Hero::StatePlayerMovement::can_control_movement(void) {
  return true;
}

/**
 * @brief Returns whether the player can control his direction in the current state.
 * @return true if the player can control his direction
 */
bool Hero::StatePlayerMovement::can_control_direction(void) {
  return true;
}

/**
 * @brief Returns the direction of the hero's movement as defined by the controls applied by the player.
 *
 * If he is not moving, -1 is returned.
 * This direction may be different from the real movement direction because of obstacles.
 *
 * @return the hero's wanted direction between 0 and 359, or -1 if he is stopped
 */
int Hero::StatePlayerMovement::get_wanted_movement_direction(void) {
  return get_player_movement()->get_direction();
}

/**
 * @brief Notifies this state that the walking speed has changed.
 * 
 * If the hero can walk in this state, the state should modify its movement
 * to set the new speed.
 */
void Hero::StatePlayerMovement::notify_walking_speed_changed(int walking_speed) {
  get_player_movement()->set_moving_speed(walking_speed);
}

/////////////////////// TODO below are all remaining functions of State, implement only the necessary ones






/**
 * @brief Notifies this state that the layer has changed.
 */
void Hero::StatePlayerMovement::notify_layer_changed(void) {
}

/**
 * @brief Notifies this state that the movement has changed.
 *
 * This function is called when the hero's movement direction changes (for instance
 * because the player pressed or released a directional key, or the hero just reached an obstacle).
 * The animations and collisions should be updated according to the new movement.
 */
void Hero::StatePlayerMovement::notify_movement_changed(void) {
}

/**
 * @brief Notifies this state of the result of the current movement.
 *
 * This function is called at each cycle.
 *
 * @param tried_to_move true if the hero tried to change his position during this cycle
 * @param success true if the position has actually just changed
 */
void Hero::StatePlayerMovement::notify_movement_result(bool tried_to_move, bool success) {
}

/**
 * @brief Stops the movement of the player and lets the player restart it when he can.
 *
 * This function is typically called when the player loses temporarily the control
 * (e.g. because of a script or a teletransporter) whereas the movement remains the same.
 * Then the movement may want to move a few pixels more as soon as it is resumed.
 * This function removes such residual effects of the player's movement.
 * If the current movement is not controlled by the player, this function has no effect.
 */
void Hero::StatePlayerMovement::reset_movement(void) {
}

/**
 * @brief Returns whether the hero ignores the effect of deep water in this state.
 *
 * Returns false by default.
 *
 * @return true if the hero ignores the effect of deep water in the current state
 */
bool Hero::StatePlayerMovement::can_avoid_deep_water(void) {
  return false;
}

/**
 * @brief Returns whether the hero ignores the effect of holes in this state.
 *
 * Returns false by default.
 *
 * @return true if the hero ignores the effect of holes in the current state
 */
bool Hero::StatePlayerMovement::can_avoid_hole(void) {
  return false;
}

/**
 * @brief Returns whether the hero is touching the ground in the current state.
 *
 * Returns true by default.
 *
 * @return true if the hero is touching the ground in the current state
 */
bool Hero::StatePlayerMovement::is_touching_ground(void) {
  return true;
}

/**
 * @brief Notifies this state that the ground was just changed.
 */
void Hero::StatePlayerMovement::notify_ground_changed(void) {
}

/**
 * @brief Sets whether the movement allows to traverse obstacles.
 *
 * This function is used for debugging purposes only.
 *
 * @param stop_on_obstacles true to make the movement sensible to obstacles, false to ignore them
 */
void Hero::StatePlayerMovement::set_stop_on_obstacles(bool stop_on_obstacles) {
}

/**
 * @brief Returns whether this state ignores the collisions with the detectors and the ground.
 */
bool Hero::StatePlayerMovement::are_collisions_ignored(void) {
  return false;
}

/**
 * @brief Returns whether a deep water tile is considered as an obstacle in this state.
 *
 * Returns false by default.
 *
 * @return true if the deep water tiles are considered as obstacles in this state
 */
bool Hero::StatePlayerMovement::is_water_obstacle(void) {
  return false;
}

/**
 * @brief Returns whether a hole is considered as an obstacle in this state.
 *
 * Returns false by default.
 *
 * @return true if the holes are considered as obstacles in this state
 */
bool Hero::StatePlayerMovement::is_hole_obstacle(void) {
  return false;
}

/**
 * @brief Returns whether a ladder is considered as an obstacle in this state.
 *
 * Returns false by default.
 *
 * @return true if the ladders are considered as obstacles in this state
 */
bool Hero::StatePlayerMovement::is_ladder_obstacle(void) {
  return false;
}

/**
 * @brief Returns whether a teletransporter is considered as an obstacle in this state.
 *
 * Returns true by default.
 *
 * @param teletransporter a teletransporter
 * @return true if the teletransporter is an obstacle in this state
 */
bool Hero::StatePlayerMovement::is_teletransporter_obstacle(Teletransporter *teletransporter) {
  return true;
}

/**
 * @brief Returns whether the hero ignores the effect of teletransporters in this state.
 *
 * Returns false by default.
 *
 * @return true if the hero ignores the effect of teletransporters in this state
 */
bool Hero::StatePlayerMovement::can_avoid_teletransporter(void) {
  return false;
}

/**
 * @brief Returns whether the effect of teletransporters is delayed in this state.
 *
 * When overlapping a teletransporter, if this function returns true, the teletransporter
 * will not be activated immediately.
 * Returns false by default.
 */
bool Hero::StatePlayerMovement::is_teletransporter_delayed(void) {
  return false;
}

/**
 * @brief Returns whether a conveyor belt is considered as an obstacle in this state.
 *
 * Returns true by default.
 *
 * @param conveyor_belt a conveyor belt
 * @return true if the conveyor belt is an obstacle in this state
 */
bool Hero::StatePlayerMovement::is_conveyor_belt_obstacle(ConveyorBelt *conveyor_belt) {
  return true;
}

/**
 * @brief Returns whether the hero ignores the effect of conveyor belts in this state.
 *
 * Returns false by default.
 *
 * @return true if the hero ignores the effect of conveyor belts in this state
 */
bool Hero::StatePlayerMovement::can_avoid_conveyor_belt(void) {
  return false;
}

/**
 * @brief Returns whether the hero ignores the effect of stairs in this state.
 *
 * Returns false by default.
 *
 * @return true if the hero ignores the effect of stairs in this state
 */
bool Hero::StatePlayerMovement::can_avoid_stairs(void) {
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
bool Hero::StatePlayerMovement::is_sensor_obstacle(Sensor *sensor) {
  return false;
}

/**
 * @brief Returns whether the hero ignores the effect of sensors in this state.
 *
 * Returns false by default.
 *
 * @return true if the hero ignores the effect of sensors in this state
 */
bool Hero::StatePlayerMovement::can_avoid_sensor(void) {
  return false;
}

/**
 * @brief Returns whether a jump sensor is considered as an obstacle in this state.
 *
 * Returns false by default.
 *
 * @param jump_sensor a jump sensor
 * @return true if the jump sensor is an obstacle in this state
 */
bool Hero::StatePlayerMovement::is_jump_sensor_obstacle(JumpSensor *jump_sensor) {
  return false;
}

/**
 * @brief Returns whether the hero ignores the effect of explosions in this state.
 *
 * Returns false by default.
 *
 * @return true if the hero ignores the effect of explosions in this state
 */
bool Hero::StatePlayerMovement::can_avoid_explosion(void) {
  return false;
}

/**
 * @brief Returns whether crystal switches can be activated by the sword in this state.
 *
 * Returns false by default.
 *
 * @return true if crystal switches can be activated by the sword in this state
 */
bool Hero::StatePlayerMovement::can_sword_hit_crystal_switch(void) {
  return false;
}

/**
 * @brief Notifies this entity that it has just attacked an enemy
 *
 * This function is called even if this attack was not successful.
 *
 * @param attack the attack
 * @param victim the enemy just hurt
 * @param result indicates how the enemy has reacted to the attack (see Enemy.h)
 * @param killed indicates that the attack has just killed the enemy
 */
void Hero::StatePlayerMovement::notify_attacked_enemy(EnemyAttack attack, Enemy *victim, int result, bool killed) {
}

/**
 * @brief Returns the damage power of the sword for the current attack.
 *
 * Redefine the function if your state changes the power of the sword
 * (typically for a spin attack).
 *
 * @return the current damage factor of the sword
 */
int Hero::StatePlayerMovement::get_sword_damage_factor(void) {

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
bool Hero::StatePlayerMovement::can_be_hurt(void) {
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
bool Hero::StatePlayerMovement::is_free(void) {
  return false;
}

/**
 * @brief Returns whether the hero is grabbing or pulling an entity in this state.
 *
 * Returns false by default.
 *
 * @return true if the hero is grabbing or pulling an entity
 */
bool Hero::StatePlayerMovement::is_grabbing_or_pulling(void) {
  return false;
}

/**
 * @brief Returns whether the hero is grabbing and moving an entity in this state.
 *
 * If he is not grabbing any entity, false is returned.
 * Returns false by default.
 *
 *
 * @return true if the hero is grabbing and moving an entity
 */
bool Hero::StatePlayerMovement::is_moving_grabbed_entity(void) {
  return false;
}

/**
 * @brief Notifies the hero that the entity he is pushing or pulling in this state
 * cannot move anymore because of a collision.
 */
void Hero::StatePlayerMovement::notify_grabbed_entity_collision(void) {
}

/**
 * @brief Tests whether the hero is striking the specified detector with his sword.
 *
 * When the sword sprite collides with a detector,
 * this function can be called to determine whether the hero is
 * really striking this particular detector only.
 * This depends on the hero's state, his direction and his
 * distance to the detector.
 * This function assumes that there is already a collision
 * between the sword sprite and the detector's sprite.
 * This function should be called to check whether the
 * hero wants to cut a bush or some grass.
 * Returns false by default.
 *
 * @param detector the detector to check
 * @return true if the sword is striking this detector
 */
bool Hero::StatePlayerMovement::is_striking_with_sword(Detector *detector) {
  return false;
}

/**
 * @brief Returns whether the hero can swing his sword in this state.
 *
 * Returns false by default.
 *
 * @return true if the hero can swing his sword in this state
 */
bool Hero::StatePlayerMovement::can_start_sword(void) {
  return false;
}

#endif

