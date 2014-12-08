/*
 * Copyright (C) 2006-2014 Christopho, Solarus - http://www.solarus-games.org
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
#include "solarus/hero/State.h"
#include "solarus/hero/SwordSwingingState.h"
#include "solarus/hero/HeroSprites.h"
#include "solarus/entities/Hero.h"
#include "solarus/entities/Jumper.h"
#include "solarus/entities/Stairs.h"
#include "solarus/lowlevel/System.h"
#include "solarus/lowlevel/Debug.h"
#include "solarus/lua/LuaContext.h"
#include "solarus/Game.h"
#include "solarus/Map.h"
#include "solarus/Equipment.h"
#include "solarus/EquipmentItem.h"
#include "solarus/Sprite.h"
#include "solarus/KeysEffect.h"

namespace Solarus {

/**
 * \brief Creates a state.
 *
 * This constructor can be called only from the subclasses.
 *
 * \param hero The hero to control with this state.
 * \param state_name A name describing this state.
 */
Hero::State::State(Hero& hero, const std::string& state_name):
  hero(hero),
  suspended(false),
  when_suspended(0),
  map(&hero.get_map()),
  name(state_name),
  stopping(false) {

}

/**
 * \brief Destructor.
 *
 * The state is destroyed once it is not the current state of the hero anymore.
 */
Hero::State::~State() {
}

/**
 * \brief Returns a name describing this state.
 * \return A name describing this state.
 */
const std::string& Hero::State::get_name() const {
  return name;
}

/**
 * \brief Returns whether this state is the current state.
 * \return \c true if this state is the current state.
 */
bool Hero::State::is_current_state() const {
  return hero.state.get() == this && !hero.state->is_stopping();
}

/**
 * \brief Returns whether this state is being stopped.
 * \return \c true if this state is being stopped.
 */
bool Hero::State::is_stopping() const {
  return stopping;
}

/**
 * \brief Returns the hero.
 * \return The hero.
 */
Hero& Hero::State::get_hero() {
  return hero;
}

/**
 * \brief Returns the hero.
 * \return The hero.
 */
const Hero& Hero::State::get_hero() const {
  return hero;
}

/**
 * \brief Returns the hero's sprites.
 * \return the sprites
 */
HeroSprites& Hero::State::get_sprites() {
  return hero.get_hero_sprites();
}

/**
 * \brief Returns the hero's sprites.
 * \return the sprites
 */
const HeroSprites& Hero::State::get_sprites() const {
  return hero.get_hero_sprites();
}

/**
 * \brief Returns the current map.
 * \return the map
 */
Map& Hero::State::get_map() {
  return *map;
}

/**
 * \brief Returns the entities of the current map.
 * \return the entities
 */
MapEntities& Hero::State::get_entities() {
  return map->get_entities();
}

/**
 * \brief Returns the shared Lua context.
 * \return The Lua context where all scripts are run.
 */
LuaContext& Hero::State::get_lua_context() {
  return map->get_game().get_lua_context();
}

/**
 * \brief Returns the current game.
 * \return the game
 */
Game& Hero::State::get_game() {
  return map->get_game();
}

/**
 * \brief Returns the current game.
 * \return the game
 */
const Game& Hero::State::get_game() const {
  return map->get_game();
}

/**
 * \brief Returns the current equipment.
 * \return the equipment
 */
Equipment& Hero::State::get_equipment() {
  return get_game().get_equipment();
}

/**
 * \brief Returns the current equipment.
 * \return the equipment
 */
const Equipment& Hero::State::get_equipment() const {
  return get_game().get_equipment();
}

/**
 * \brief Returns the keys effect manager.
 * \return the keys effect
 */
KeysEffect& Hero::State::get_keys_effect() {
  return get_game().get_keys_effect();
}

/**
 * \brief Returns the game commands.
 * \return The commands.
 */
GameCommands& Hero::State::get_commands() {
  return get_game().get_commands();
}

/**
 * \brief Returns the game commands.
 * \return The commands.
 */
const GameCommands& Hero::State::get_commands() const {
  return get_game().get_commands();
}

/**
 * \brief Starts this state.
 *
 * This function is called automatically when this state becomes the active
 * state of the hero.
 * The initializations should be done here rather than in the constructor.
 *
 * \param previous_state The previous state or nullptr if this is the first state
 * (for information).
 */
void Hero::State::start(const State* /* previous_state */) {

  set_suspended(hero.is_suspended());

  // Notify Lua.
  if (hero.is_on_map()) {
    get_lua_context().hero_on_state_changed(hero, get_name());
  }
}

/**
 * \brief Ends this state.
 *
 * This function is called automatically when this state is not the active
 * state anymore.
 * You should here close everything the start() function has opened.
 * The destructor will be called at the next cycle.
 *
 * \param next_state The next state (for information).
 */
void Hero::State::stop(const State* /* next_state */) {

  Debug::check_assertion(!is_stopping(),
      std::string("This state is already stopping: ") + get_name());

  this->stopping = true;
}

/**
 * \brief Updates this state.
 *
 * This function is called repeatedly while this state is the active state.
 */
void Hero::State::update() {

}

/**
 * \brief Draws this state.
 *
 * This function draws the hero's sprites in its current state.
 * If your state needs to draw additional elements, you can redefine this function.
 */
void Hero::State::draw_on_map() {

  get_sprites().draw_on_map();
}

/**
 * \brief Returns whether this state is suspended.
 * \return \c true if this state is suspended.
 */
bool Hero::State::is_suspended() const {
  return suspended;
}

/**
 * \brief Notifies this state that the game was just suspended or resumed.
 * \param suspended true if the game is suspended
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
 * \brief Returns the date when this state was suspended.
 * \return The date when this state was suspended.
 */
uint32_t Hero::State::get_when_suspended() const {
  return when_suspended;
}

/**
 * \brief This function is called when a game command is pressed and the game
 * is not suspended.
 * \param command The command pressed.
 */
void Hero::State::notify_command_pressed(GameCommand command) {

  switch (command) {

    // action key
  case GameCommand::ACTION:
    notify_action_command_pressed();
    break;

    // sword key
  case GameCommand::ATTACK:
    notify_attack_command_pressed();
    break;

    // move the hero
  case GameCommand::RIGHT:
    notify_direction_command_pressed(0);
    break;

  case GameCommand::UP:
    notify_direction_command_pressed(1);
    break;

  case GameCommand::LEFT:
    notify_direction_command_pressed(2);
    break;

  case GameCommand::DOWN:
    notify_direction_command_pressed(3);
    break;

    // use an equipment item
  case GameCommand::ITEM_1:
    notify_item_command_pressed(1);
    break;

  case GameCommand::ITEM_2:
    notify_item_command_pressed(2);
    break;

  default:
    break;
  }
}

/**
 * \brief This function is called when a command is released if the game is
 * not suspended.
 * \param command The command released.
 */
void Hero::State::notify_command_released(GameCommand command) {

  switch (command) {

  case GameCommand::ACTION:
    notify_action_command_released();
    break;

  case GameCommand::ATTACK:
    notify_attack_command_released();
    break;

  case GameCommand::RIGHT:
    notify_direction_command_released(0);
    break;

  case GameCommand::UP:
    notify_direction_command_released(1);
    break;

  case GameCommand::LEFT:
    notify_direction_command_released(2);
    break;

  case GameCommand::DOWN:
    notify_direction_command_released(3);
    break;

  case GameCommand::ITEM_1:
    notify_item_command_released(0);
    break;

  case GameCommand::ITEM_2:
    notify_item_command_released(1);
    break;

  default:
    break;
  }
}

/**
 * \brief Notifies this state that the action command was just pressed.
 */
void Hero::State::notify_action_command_pressed() {
}

/**
 * \brief Notifies this state that the action command was just released.
 */
void Hero::State::notify_action_command_released() {
}

/**
 * \brief Notifies this state that the attack command was just pressed.
 */
void Hero::State::notify_attack_command_pressed() {

  if (!hero.is_suspended()
      && get_keys_effect().get_sword_key_effect() == KeysEffect::SWORD_KEY_SWORD
      && hero.can_start_sword()) {

    hero.set_state(new SwordSwingingState(hero));
  }
}

/**
 * \brief Notifies this state that the attack command was just released.
 */
void Hero::State::notify_attack_command_released() {
}

/**
 * \brief Notifies this state that a directional command was just pressed.
 * \param direction4 direction of the command (0 to 3)
 */
void Hero::State::notify_direction_command_pressed(int /* direction4 */) {
}

/**
 * \brief Notifies this state that a directional command was just released.
 * \param direction4 direction of the command (0 to 3)
 */
void Hero::State::notify_direction_command_released(int /* direction4 */) {
}

/**
 * \brief Notifies this state that an item command was just pressed.
 * \param slot The slot activated (1 or 2).
 */
void Hero::State::notify_item_command_pressed(int slot) {

  EquipmentItem* item = get_equipment().get_item_assigned(slot);

  if (item != nullptr && hero.can_start_item(*item)) {
    hero.start_item(*item);
  }
}

/**
 * \brief Notifies this state that an item command was just released.
 * \param slot the slot (1 or 2)
 */
void Hero::State::notify_item_command_released(int /* slot */) {
}

/**
 * \brief Changes the map.
 *
 * This function is called when the hero is about to go to another map.
 *
 * \param map the new map
 */
void Hero::State::set_map(Map& map) {
  this->map = &map;
}

/**
 * \brief Returns whether the game over sequence can start in the current state.
 * \return true if the game over sequence can start in the current state
 */
bool Hero::State::can_start_gameover_sequence() const {
  return true;
}

/**
 * \brief Returns whether the hero is visible in the current state.
 * \return true if the hero is displayed in the current state
 */
bool Hero::State::is_hero_visible() const {
  return true;
}

/**
 * \brief Returns whether the animation direction is locked.
 *
 * When this function returns false, which is the case most of the time,
 * it means that the animation direction is set to the movement direction.
 * When it returns true, it means that the animation direction is fixed
 * and do not depends on the movement direction anymore (this is the case
 * when the hero is loading his sword).
 *
 * \return true if the animation direction is locked
 */
bool Hero::State::is_direction_locked() const {
  return false;
}

/**
 * \brief Returns whether the player can control his movements in the current state.
 *
 * This function returns true if and only if the current movement of the hero is
 * an instance of PlayerMovement.
 *
 * \return true if the player can control his movements
 */
bool Hero::State::can_control_movement() const {
  return false;
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
int Hero::State::get_wanted_movement_direction8() const {
  return -1;
}

/**
 * \brief Notifies this state that the walking speed has changed.
 *
 * If the hero can walk in this state, the state should modify its movement
 * to set the new speed.
 */
void Hero::State::notify_walking_speed_changed() {
}

/**
 * \brief Notifies this state that the layer has changed.
 */
void Hero::State::notify_layer_changed() {
}

/**
 * \brief Notifies this state that the movement has changed.
 *
 * This function is called when the hero's movement direction changes (for instance
 * because the player pressed or released a directional key, or the hero just reached an obstacle).
 * The animations and collisions should be updated according to the new movement.
 */
void Hero::State::notify_movement_changed() {
}

/**
 * \brief Notifies this state that the movement if finished.
 */
void Hero::State::notify_movement_finished() {
}

/**
 * \brief Notifies this state that the hero has just failed to change its
 * position because of obstacles.
 */
void Hero::State::notify_obstacle_reached() {
}

/**
 * \brief Notifies this state that the hero has just changed its
 * position.
 */
void Hero::State::notify_position_changed() {
}

/**
 * \brief Returns whether the hero ignores the effect of deep water in this state.
 *
 * Returns false by default.
 *
 * \return true if the hero ignores the effect of deep water in the current state
 */
bool Hero::State::can_avoid_deep_water() const {
  return false;
}

/**
 * \brief Returns whether the hero ignores the effect of holes in this state.
 *
 * Returns false by default.
 *
 * \return true if the hero ignores the effect of holes in the current state
 */
bool Hero::State::can_avoid_hole() const {
  return false;
}

/**
 * \brief Returns whether the hero ignores the effect of ice in this state.
 *
 * Returns false by default.
 *
 * \return \c true if the hero ignores the effect of ice in the current state.
 */
bool Hero::State::can_avoid_ice() const {
  return false;
}

/**
 * \brief Returns whether the hero ignores the effect of lava in this state.
 *
 * Returns false by default.
 *
 * \return true if the hero ignores the effect of lava in the current state
 */
bool Hero::State::can_avoid_lava() const {
  return false;
}

/**
 * \brief Returns whether the hero ignores the effect of prickles in this state.
 *
 * Returns false by default.
 *
 * \return true if the hero ignores the effect of prickles in the current state
 */
bool Hero::State::can_avoid_prickle() const {
  return false;
}

/**
 * \brief Returns whether the hero is touching the ground in the current state.
 *
 * Returns true by default.
 *
 * \return true if the hero is touching the ground in the current state
 */
bool Hero::State::is_touching_ground() const {
  return true;
}

/**
 * \brief Returns whether the hero's current position can be considered
 * as a place to come back after a bad ground (hole, deep water, etc).
 *
 * Returns is_touching_ground() by default.
 *
 * \return true if the hero can come back here
 */
bool Hero::State::can_come_from_bad_ground() const {
  return is_touching_ground();
}

/**
 * \brief Notifies this state that the ground was just changed.
 */
void Hero::State::notify_ground_changed() {
}

/**
 * \brief Returns whether this state ignores the collisions with the detectors and the ground.
 * \return true if the collisions are ignored
 */
bool Hero::State::are_collisions_ignored() const {
  return false;
}

/**
 * \brief Returns whether shallow water is considered as an obstacle in this state.
 *
 * Returns false by default.
 *
 * \return true if shallow water is considered as an obstacle in this state
 */
bool Hero::State::is_shallow_water_obstacle() const {
  return false;
}

/**
 * \brief Returns whether deep water tile is considered as an obstacle in this state.
 *
 * Returns false by default.
 *
 * \return true if deep water is considered as an obstacle in this state
 */
bool Hero::State::is_deep_water_obstacle() const {
  return false;
}

/**
 * \brief Returns whether a hole is considered as an obstacle in this state.
 *
 * Returns false by default.
 *
 * \return true if the holes are considered as obstacles in this state
 */
bool Hero::State::is_hole_obstacle() const {
  return false;
}

/**
 * \brief Returns whether lava is considered as an obstacle in this state.
 *
 * Returns false by default.
 *
 * \return true if lava is considered as obstacles in this state
 */
bool Hero::State::is_lava_obstacle() const {
  return false;
}

/**
 * \brief Returns whether prickles are considered as an obstacle in this state.
 *
 * Returns false by default.
 *
 * \return true if prickles are considered as obstacles in this state
 */
bool Hero::State::is_prickle_obstacle() const {
  return false;
}

/**
 * \brief Returns whether a ladder is considered as an obstacle in this state.
 *
 * Returns false by default.
 *
 * \return true if the ladders are considered as obstacles in this state
 */
bool Hero::State::is_ladder_obstacle() const {
  return false;
}

/**
 * \brief Returns whether a teletransporter is considered as an obstacle in this state.
 *
 * Returns false by default.
 *
 * \param teletransporter a teletransporter
 * \return true if the teletransporter is an obstacle in this state
 */
bool Hero::State::is_teletransporter_obstacle(
    const Teletransporter& /* teletransporter */) const {
  return false;
}

/**
 * \brief Returns whether the hero ignores the effect of teletransporters in this state.
 *
 * Returns false by default.
 *
 * \return true if the hero ignores the effect of teletransporters in this state
 */
bool Hero::State::can_avoid_teletransporter() const {
  return false;
}

/**
 * \brief Returns whether the effect of teletransporters is delayed in this state.
 *
 * When overlapping a teletransporter, if this function returns true, the teletransporter
 * will not be activated immediately. The state then has to activate it when it is ready.
 * Returns false by default.
 *
 * \return true if the effect of teletransporters is delayed in this state
 */
bool Hero::State::is_teletransporter_delayed() const {
  return false;
}

/**
 * \brief Returns whether a stream is considered as an obstacle in this state.
 * \param stream A stream.
 * \return \c true if the stream is an obstacle in this state.
 */
bool Hero::State::is_stream_obstacle(
    const Stream& /* stream */) const {
  return false;
}

/**
 * \brief Returns whether the hero ignores the effect of a stream in this state.
 * \param stream A stream.
 * \return \c true if the hero ignores the effect of the stream in this state.
 */
bool Hero::State::can_avoid_stream(const Stream& /* stream */) const {
  return false;
}

/**
 * \brief Returns whether this state can continue when taking a stream.
 *
 * This function only matters if the stream is applied in this state, that is,
 * when can_avoid_stream() is \c false.
 *
 * \param stream A stream.
 * \return \c true if this state can continue, \c false if the hero should
 * get back to FreeState.
 */
bool Hero::State::can_persist_on_stream(const Stream& /* stream */) const {
  return true;
}

/**
 * \brief Returns whether the hero can take stairs in this state.
 * If false is returned, stairs have no effect (but they are obstacle for the hero).
 *
 * Returns false by default.
 *
 * \return true if the hero can take stairs in this state
 */
bool Hero::State::can_take_stairs() const {
  return false;
}

/**
 * \brief Returns whether some stairs are considered as obstacle in this state.
 * \param stairs Some stairs.
 * \return \c true if the stairs are obstacle in this state.
 */
bool Hero::State::is_stairs_obstacle(const Stairs& stairs) const {

  // The hero may overlap stairs in rare cases,
  // for example if he arrived by swimming over them
  // and thus did not activate them.
  // This is allowed and can be used to leave water pools for example.
  if (hero.overlaps(stairs)) {
    return false;
  }

  return true;
}

/**
 * \brief Returns whether a sensor is considered as an obstacle in this state.
 *
 * Returns false by default.
 *
 * \param sensor a sensor
 * \return true if the sensor is an obstacle in this state
 */
bool Hero::State::is_sensor_obstacle(const Sensor& /* sensor */) const {
  return false;
}

/**
 * \brief Returns whether a jumper is considered as an obstacle in this state
 * for the hero at the specified position.
 * \param jumper A jumper.
 * \param candidate_position Position of the hero to test.
 * \return \c true if the jumper is an obstacle in this state with this
 * hero position.
 */
bool Hero::State::is_jumper_obstacle(
    const Jumper& jumper, const Rectangle& candidate_position) const {

  if (jumper.overlaps_jumping_region(hero.get_bounding_box(), false)) {
    // The hero already overlaps the active part of the jumper.
    // This is authorized if he arrived from another direction
    // and thus did not activate it.
    // This can be used to leave water pools for example.
    return false;
  }

  if (!jumper.overlaps_jumping_region(candidate_position, false)) {
    // The candidate position is in the inactive region: always accept that.
    return false;
  }

  if (!can_take_jumper()) {
    // If jumpers cannot be used in this state, consider their active region
    // as obstacles and their inactive region as traversable.
    // The active region should be an obstacle.
    return true;
  }

  // At this point, we know that the jumper can be activated.

  const bool hero_in_jump_position =
      jumper.is_in_jump_position(hero, hero.get_bounding_box(), false);
  const bool candidate_in_jump_position =
      jumper.is_in_jump_position(hero, candidate_position, false);

  if (candidate_in_jump_position) {
    // Wants to move to a valid jump position: accept.
    return false;
  }

  if (hero_in_jump_position) {
    // If the hero is already correctly placed (ready to jump),
    // make the jumper obstacle so that the player has to move in the
    // jumper's direction during a small delay before jumping.
    // This also prevents the hero to get inside the jumper's active region.
    return true;
  }

  const bool candidate_in_extended_jump_position =
      jumper.is_in_jump_position(hero, candidate_position, true);

  if (candidate_in_extended_jump_position) {
    // Wants to get inside the active region from an end of the jumper:
    // don't accept this.
    return true;
  }

  if (!jumper.is_jump_diagonal() &&
      hero.is_moving_towards(jumper.get_direction() / 2)) {
    // Special case: make the jumper traversable so
    // that the smooth movement can slide to it.
    return false;
  }

  if (!jumper.is_jump_diagonal() &&
      get_name() == "swimming" &&  // TODO use inheritance instead
      hero.is_moving_towards(((jumper.get_direction() / 2) + 2) % 4)
  ) {
    // Other special case: trying to enter the jumper the reverse way while
    // swimming: we accept this to allow the hero to leave water pools.
    // TODO I'm not sure this behavior is really a good idea.
    // This may change in a future version.
    return false;
  }

  return true;
}

/**
 * \brief Returns whether a separator is considered as an obstacle in this
 * state.
 *
 * Returns false by default.
 *
 * \param separator A separator.
 * \return \c true if the separator is an obstacle in this state.
 */
bool Hero::State::is_separator_obstacle(const Separator& /* separator */) const {
  return false;
}

/**
 * \brief Returns whether the hero ignores the effect of sensors in this state.
 *
 * Returns false by default.
 *
 * \return true if the hero ignores the effect of sensors in this state
 */
bool Hero::State::can_avoid_sensor() const {
  return false;
}

/**
 * \brief Returns whether the hero ignores the effect of switches in this state.
 *
 * Returns false by default.
 *
 * \return true if the hero ignores the effect of switches in this state
 */
bool Hero::State::can_avoid_switch() const {
  return false;
}

/**
 * \brief Returns whether crystals can be activated by the sword in this state.
 *
 * Returns false by default.
 *
 * \return true if crystals can be activated by the sword in this state
 */
bool Hero::State::can_sword_hit_crystal() const {
  return false;
}

/**
 * \brief Returns whether the hero ignores the effect of explosions in this state.
 *
 * Returns false by default.
 *
 * \return true if the hero ignores the effect of explosions in this state
 */
bool Hero::State::can_avoid_explosion() const {
  return false;
}

/**
 * \brief Returns whether the hero can trigger a jumper in this state.
 *
 * If false is returned, jumpers have no effect (but they are obstacle for
 * the hero).
 *
 * Returns false by default.
 *
 * \return \c true if the hero can use jumpers in this state.
 */
bool Hero::State::can_take_jumper() const {
  return false;
}

/**
 * \brief Notifies this state that the hero is activating a jumper.
 *
 * Does nothing by default.
 * Redefine this function if you want to perform the jump in your state.
 *
 * \param jumper The jumper activated.
 */
void Hero::State::notify_jumper_activated(Jumper& /* jumper */) {
}

/**
 * \brief Notifies this state that the hero has just attacked an enemy.
 *
 * This function is called even if this attack was not successful.
 *
 * \param attack The attack.
 * \param victim The enemy just hurt.
 * \param victim_sprite The enemy's sprite that was touched or nullptr.
 * \param result How the enemy has reacted to the attack.
 * \param killed Whether the attack has just killed the enemy.
 */
void Hero::State::notify_attacked_enemy(
    EnemyAttack /* attack */,
    Enemy& /* victim */,
    const Sprite* /* victim_sprite */,
    EnemyReaction::Reaction& /* result */,
    bool /* killed */) {
}

/**
 * \brief Returns the damage power of the sword for the current attack.
 *
 * Redefine the function if your state changes the power of the sword
 * (typically for a spin attack).
 *
 * \return the current damage factor of the sword
 */
int Hero::State::get_sword_damage_factor() const {

  return get_equipment().get_ability(Ability::SWORD);
}

/**
 * \brief Returns whether the hero can be hurt in this state.
 *
 * Returns false by default.
 *
 * \param attacker an attacker that is trying to hurt the hero
 * (or nullptr if the source of the attack is not an enemy)
 * \return true if the hero can be hurt in this state
 */
bool Hero::State::can_be_hurt(MapEntity* /* attacker */) const {
  return false;
}

/**
 * \brief Returns whether the hero can walk normally and interact with entities
 * in this state.
 *
 * Returns false by default.
 *
 * \return true if the hero can walk normally
 */
bool Hero::State::is_free() const {
  return false;
}

/**
 * \brief Returns whether the hero is using an equipment item in this state.
 *
 * Returns false by default.
 *
 * \return true if the hero is using an equipment item.
 */
bool Hero::State::is_using_item() const {
  return false;
}

/**
 * \brief When the hero is using an equipment item, returns that item.
 * \return The current equipment item.
 */
EquipmentItemUsage& Hero::State::get_item_being_used() {

  Debug::die("No item is being used in this state");
  throw;
}

/**
 * \brief Returns whether the hero is brandishing a treasure in this state.
 *
 * Returns false by default.
 *
 * \return \c true if the hero is brandishing a treasure.
 */
bool Hero::State::is_brandishing_treasure() const {
  return false;
}

/**
 * \brief Returns whether the hero is grabbing or pulling an entity in this state.
 *
 * Returns false by default.
 *
 * \return true if the hero is grabbing or pulling an entity
 */
bool Hero::State::is_grabbing_or_pulling() const {
  return false;
}

/**
 * \brief Returns whether the hero is grabbing and moving an entity in this state.
 *
 * If he is not grabbing any entity, false is returned.
 * Returns false by default.
 *
 * \return true if the hero is grabbing and moving an entity
 */
bool Hero::State::is_moving_grabbed_entity() const {
  return false;
}

/**
 * \brief Notifies the hero that the entity he is pushing or pulling in this state
 * cannot move anymore because of a collision.
 */
void Hero::State::notify_grabbed_entity_collision() {
}

/**
 * \brief Tests whether the hero is cutting with his sword the specified detector
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
 * \param detector the detector to check
 * \return true if the sword is cutting this detector
 */
bool Hero::State::is_cutting_with_sword(Detector& /* detector */) {
  return false;
}

/**
 * \brief Returns whether the hero can swing his sword in this state.
 *
 * Returns false by default.
 *
 * \return true if the hero can swing his sword in this state
 */
bool Hero::State::can_start_sword() const {
  return false;
}

/**
 * \brief Returns whether the hero can pick a treasure in this state.
 *
 * Returns false by default.
 *
 * \param item The equipment item to obtain.
 * \return true if the hero can pick that treasure in this state.
 */
bool Hero::State::can_pick_treasure(EquipmentItem& /* item */) const {
  return false;
}

/**
 * \brief Returns whether attack can be stopped with a shield in this state.
 *
 * Returns \c true by default.
 *
 * \return \c true if the shield is active is this state.
 */
bool Hero::State::can_use_shield() const {
  return true;
}

/**
 * \brief Returns whether the hero can use an equipment item in this state.
 *
 * Returns false by default.
 *
 * \param item The equipment item to check.
 * \return true if the hero can use an equipment item in this state.
 */
bool Hero::State::can_start_item(EquipmentItem& /* item */) const {
  return false;
}

/**
 * \brief Returns whether the hero is currently carrying an item in this state.
 *
 * This function returns true if get_carried_item() is not nullptr.
 * Redefine get_carried_item() if the hero is able to carry an item in this state.
 *
 * \return true if the hero is currently carrying an item in this state
 */
bool Hero::State::is_carrying_item() const {
  return get_carried_item() != nullptr;
}

/**
 * \brief Returns the item currently carried by the hero in this state, if any.
 *
 * Redefine this function to make the hero able to carry an item in this state.
 *
 * \return the item carried by the hero, or nullptr
 */
std::shared_ptr<CarriedItem> Hero::State::get_carried_item() const {
  return nullptr;
}

/**
 * \brief Returns the action to do with an item previously carried by the hero when this state starts.
 *
 * Returns CarriedItem::BEHAVIOR_THROW by default.
 *
 * \return the action to do with a previous carried item when this state starts
 */
CarriedItem::Behavior Hero::State::get_previous_carried_item_behavior() const {
  return CarriedItem::BEHAVIOR_THROW;
}

}

