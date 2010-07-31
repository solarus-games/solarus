#include "hero/State.h"

/**
 * @brief Creates a state.
 *
 * This constructor can be called only from the subclasses.
 */
Hero::State::State(Hero *hero):
  hero(hero), next_state(NULL) {

}

/**
 * @brief Destructor.
 *
 * The state is destroyed once it is not the current state of the hero anymore.
 */
Hero::State::~State(void) {

}

/**
 * @brief Returns the next state to set.
 *
 * This function is called by the hero when he needs to move to a next state.
 *
 * @return the next state to set, or NULL to stay in the current state.
 */
Hero::State * Hero::State::get_next_state(void) {
  return next_state;
}

/**
 * @brief Sets a next state to go to.
 *
 * Use this function to change the state from a subclass of State.
 * This current state stops and the next state starts immediately.
 *
 * @param next_state the next state
 */
void Hero::State::set_next_state(State *next_state) {
  this->next_state = next_state;
  hero->start_next_state(); // the hero will call get_next_state() to know the new state
}

/**
 * @brief Starts this state.
 *
 * This function is called automatically when this state becomes the active state of the hero.
 * The initializations should be done here rather than in the constructor.
 */
void Hero::State::start(void) {
}

/**
 * @brief Ends this state.
 *
 * This function is called automatically when this state is not the active state anymore.
 * You should here close everything the start() function has opened.
 * The destructor will be called at the next cycle.
 */
void Hero::State::stop(void) {
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
}

/**
 * @brief Notifies this state that the game was just suspended or resumed.
 * @param suspended true if the game is suspended
 */
void Hero::State::set_suspended(bool suspended) {
}

/**
 * @brief Notifies this state that the action key was just pressed.
 */
void Hero::State::action_key_pressed(void) {
}

/**
 * @brief Notifies this state that the sword key was just pressed.
 */
void Hero::State::sword_key_pressed(void) {
}

/**
 * @brief Notifies this state that a directional key was just pressed.
 * @param direction4 direction of the key (0 to 3)
 */
void Hero::State::directional_key_pressed(int direction4) {
}

/**
 * @brief Notifies this state that an item key was just pressed.
 * @param slot the slot activated (0 or 1)
 */
void Hero::State::item_key_pressed(int slot) {
}

/**
 * @brief Notifies this state that a directional key was just released.
 * @param direction4 direction of the key (0 to 3)
 */
void Hero::State::directional_key_released(int direction4) {
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
 */
void Hero::State::set_map(Map *map) {
}

/**
 * @brief Returns whether the game over sequence can start in the current state.
 * @return true if the game over sequence can start in the current state
 */
bool Hero::State::can_start_gameover_sequence(void) {
  return true;
}

/**
 * @brief Notifies this state that the equipment of the player has just changed.
 */
void Hero::State::notify_equipment_changed(void) {
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
 * @return true if the player can control his movements
 */
bool Hero::State::can_control_movement(void) {
  return false;
}

/**
 * @brief Returns whether the player can control his direction in the current state.
 * @return true if the player can control his direction
 */
bool Hero::State::can_control_direction(void) {
  return false;
}

/**
 * @brief Returns the direction of the hero's movement as defined by the controls applied by the player.
 *
 * If he is not moving, -1 is returned.
 * This direction may be different from the real movement direction because of obstacles.
 *
 * @return the hero's wanted direction between 0 and 359, or -1 if he is stopped
 */
int Hero::State::get_wanted_movement_direction(void) {
  return 0;
}

/**
 * @brief Notifies this state that the walking speed has changed.
 * 
 * If the hero can walk in this state, the state should modify its movement
 * to set the new speed.
 */
void Hero::State::notify_walking_speed_changed(int walking_speed) {
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
 * @brief Notifies this state of the result of the current movement.
 *
 * This function is called at each cycle.
 *
 * @param tried_to_move true if the hero tried to change his position during this cycle
 * @param success true if the position has actually just changed
 */
void Hero::State::notify_movement_result(bool tried_to_move, bool success) {
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
void Hero::State::reset_movement(void) {
}

/**
 * @brief Returns whether the hero ignores the effect of deep water in this state.
 * @return true if the hero ignores the effect of deep water in the current state
 */
bool Hero::State::can_avoid_deep_water(void) {
  return false;
}

/**
 * @brief Returns whether the hero ignores the effect of holes in this state.
 * @return true if the hero ignores the effect of holes in the current state
 */
bool Hero::State::can_avoid_hole(void) {
  return false;
}

/**
 * @brief Returns whether the hero is touching the ground in the current state.
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
 * @brief Sets whether the movement allows to traverse obstacles.
 *
 * This function is used for debugging purposes only.
 *
 * @param stop_on_obstacles true to make the movement sensible to obstacles, false to ignore them
 */
void Hero::State::set_stop_on_obstacles(bool stop_on_obstacles) {
}

/**
 * @bried Returns whether this state ignores the collisions with the detectors and the ground.
 */
bool Hero::State::are_collisions_ignored(void) {
  return false;
}

/**
 * @brief Returns whether a deep water tile is considered as an obstacle in this state.
 * @return true if the deep water tiles are considered as obstacles in this state
 */
bool Hero::State::is_water_obstacle(void) {
  return false;
}

/**
 * @brief Returns whether a hole is considered as an obstacle in this state.
 * @return true if the holes are considered as obstacles in this state
 */
bool Hero::State::is_hole_obstacle(void) {
  return false;
}

/**
 * @brief Returns whether a ladder is considered as an obstacle in this state.
 * @return true if the ladders are considered as obstacles in this state
 */
bool Hero::State::is_ladder_obstacle(void) {
  return false;
}

/**
 * @brief Returns whether a teletransporter is considered as an obstacle in this state.
 * @param teletransporter a teletransporter
 * @return true if the teletransporter is an obstacle in this state
 */
bool Hero::State::is_teletransporter_obstacle(Teletransporter *teletransporter) {
  return false;
}

/**
 * @brief Returns whether the hero ignores the effect of teletransporters in this state.
 * @return true if the hero ignores the effect of teletransporters in this state
 */
bool Hero::State::can_avoid_teletransporter(void) {
  return false;
}

/**
 * @brief Returns whether the effect of teletransporters is delayed in this state.
 *
 * When overlapping a teletransporter, if this function returns true, the teletransporter
 * will not be activated immediately.
 */
bool Hero::State::is_teletransporter_delayed(void) {
  return false;
}

/**
 * @brief Returns whether a conveyor belt is considered as an obstacle in this state.
 * @param conveyor_belt a conveyor belt
 * @return true if the conveyor belt is an obstacle in this state
 */
bool Hero::State::is_conveyor_belt_obstacle(ConveyorBelt *conveyor_belt) {
  return false;
}

/**
 * @brief Returns whether the hero ignores the effect of conveyor belts in this state.
 * @return true if the hero ignores the effect of conveyor belts in this state
 */
bool Hero::State::can_avoid_conveyor_belt(void) {
  return false;
}

/**
 * @brief Returns whether a stairs entity is considered as an obstacle in this state.
 * @param stairs a stairs entity
 * @return true if these stairs are an obstacle in this state
 */
bool Hero::State::is_stairs_obstacle(Stairs *stairs) {
  return false;
}

/**
 * @brief Returns whether the hero ignores the effect of stairs in this state.
 * @return true if the hero ignores the effect of stairs in this state
 */
bool Hero::State::can_avoid_stairs(void) {
  return false;
}

/**
 * @brief Returns whether a sensor is considered as an obstacle in this state.
 * @param sensor a sensor
 * @return true if the sensor is an obstacle in this state
 */
bool Hero::State::is_sensor_obstacle(Sensor *sensor) {
  return false;
}

/**
 * @brief Returns whether the hero ignores the effect of sensors in this state.
 * @return true if the hero ignores the effect of sensors in this state
 */
bool Hero::State::can_avoid_sensor(void) {
  return false;
}

/**
 * @brief Returns whether a raised crystal switch block is considered as an obstacle in this state.
 * @param raised_block a crystal switch block raised
 * @return true if the raised block is an obstacle in this state
 */
bool Hero::State::is_raised_block_obstacle(CrystalSwitchBlock *raised_block) {
  return false;
}

/**
 * @brief Returns whether a jump sensor is considered as an obstacle in this state.
 * @param jump_sensor a jump sensor
 * @return true if the jump sensor is an obstacle in this state
 */
bool Hero::State::is_jump_sensor_obstacle(JumpSensor *jump_sensor) {
  return false;
}

/**
 * @brief Returns whether the hero ignores the effect of explosions in this state.
 * @return true if the hero ignores the effect of explosions in this state
 */
bool Hero::State::can_avoid_explosion(void) {
  return false;
}

/**
 * @brief Returns whether crystal switches can be activated by the hero in this state.
 * @return true if crystal switches can be activated by the hero in this state
 */
bool Hero::State::can_activate_crystal_switch(void) {
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
void Hero::State::notify_attacked_enemy(EnemyAttack attack, Enemy *victim, int result, bool killed) {
}

/**
 * @brief Returns the damage power of the sword for the current attack.
 *
 * The value returned takes into account the power of the current sword
 * and the fact that a spin attack is more powerful than other attacks.
 *
 * @return the current damage factor of the sword
 */
int Hero::State::get_sword_damage_factor(void) {
  return 0;
}

/**
 * @brief Returns whether the hero can be hurt in this state.
 * @return true if the hero can be hurt in this state
 */
bool Hero::State::can_be_hurt(void) {
  return false;
}

/**
 * @brief Hurts the hero.
 * @param source the entity that hurts the hero (usually an enemy)
 * @param life_points number of heart quarters to remove (this number may be reduced by the tunic)
 * @param magic_points number of magic points to remove
 */
void Hero::State::hurt(MapEntity *source, int life_points, int magic_points) {
}

/**
 * @brief Returns whether the hero can walk normally and interact with entities
 * in this state.
 * @return true if the hero can walk normally
 */
bool Hero::State::is_free(void) {
  return false;
}

/**
 * @brief Returns whether the hero is grabbing or pulling an entity in this state.
 * @return true if the hero is grabbing or pulling an entity
 */
bool Hero::State::is_grabbing_or_pulling(void) {
  return false;
}

/**
 * @brief Returns whether the hero is grabbing and moving an entity in this state.
 *
 * If he is not grabbing any entity, false is returned.
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
 *
 * @param detector the detector to check
 * @return true if the sword is striking this detector
 */
bool Hero::State::is_striking_with_sword(Detector *detector) {
  return false;
}

