/*
 * Copyright (C) 2009-2011 Christopho, Solarus - http://www.solarus-engine.org
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
#include "lua/MapScript.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"
#include "Map.h"
#include "Game.h"
#include "Treasure.h"
#include <sstream>
#include <iomanip>
#include <lua.hpp>

/**
 * @brief Creates a map script.
 * @param map the map
 */
MapScript::MapScript(Map& map):
  Script(MAIN_API | GAME_API | MAP_API),
  map(map) {

}

/**
 * @brief Desctructor.
 */
MapScript::~MapScript() {

}

/**
 * @brief Returns the game that runs this script.
 * @return the game
 */
Game& MapScript::get_game() {
  return map.get_game();
}

/**
 * @brief Returns the map controlled by this script.
 * @return the map
 */
Map& MapScript::get_map() {
  return map;
}

/**
 * @brief Loads the script and starts it.
 *
 * This function is called when the map starts.
 *
 * @param destination_point_name name of the destination point where the hero is
 */
void MapScript::start(const std::string& destination_point_name) {

  // compute the file name, depending on the id of the map
  int id = (int) map.get_id();
  std::ostringstream oss;
  oss << "maps/map" << std::setfill('0') << std::setw(4) << id;

  // load the script
  load(oss.str());

  // notify the script
  event_map_started(destination_point_name);
}

/**
 * @brief Updates the script.
 */
void MapScript::update() {

  Script::update();

  if (is_loaded() && !get_game().is_suspended()) {
    event_update();
  }
}

/**
 * @brief This function is called when the game is being suspended or resumed.
 * @param suspended true if the game is suspended, false if it is resumed
 */
void MapScript::set_suspended(bool suspended) {

  Script::set_suspended(suspended);

  if (is_loaded()) {
    event_set_suspended(suspended);
  }
}

/**
 * @brief Notifies the script that it can update itself.
 *
 * This function is called at each cycle of the main loop,
 * so if you define it in your script, take care of the performances.
 */
void MapScript::event_update() {

  notify_script("event_update");
}

/**
 * @brief Notifies the script that the game is being suspended or resumed.
 * @param suspended true if the game becomes suspended, false if it is resumed.
 */
void MapScript::event_set_suspended(bool suspended) {

  notify_script("event_suspended", "b", suspended ? 1 : 0);
}

/**
 * @brief Notifies the script that the sequence started by a call to camera_move()
 * has reached the target.
 */
void MapScript::notify_camera_reached_target() {

  lua_settop(l, 0);

  lua_getfield(l, LUA_REGISTRYINDEX, "sol.camera_delay_before");
  int delay_before = lua_tointeger(l, -1);
  lua_pop(l, 1);

  // set a timer to execute the function
  lua_pushcfunction(l, camera_execute_function);
  add_timer(l, delay_before, false);
}

/**
 * @brief Executes the function of a camera movement.
 * @param l the Lua context that is calling this function
 */
int MapScript::camera_execute_function(lua_State* l) {

  // execute the function
  lua_getfield(l, LUA_REGISTRYINDEX, "sol.camera_function");
  if (lua_pcall(l, 0, 0, 0) != 0) {
    Debug::print(StringConcat() << "Error in the function of the camera: "
        << lua_tostring(l, -1));
    lua_pop(l, 1);
  }

  // set a second timer to restore the camera
  lua_getfield(l, LUA_REGISTRYINDEX, "sol.camera_delay_after");
  int delay_after = lua_tointeger(l, -1);
  lua_pop(l, 1);
  lua_pushcfunction(l, camera_restore);
  add_timer(l, delay_after, false);

  return 0;
}

/**
 * @brief Moves the camera back to the hero.
 * @param l the Lua context that is calling this function
 */
int MapScript::camera_restore(lua_State* l) {

  lua_getfield(l, LUA_REGISTRYINDEX, "sol.cpp_object");
  Script* script = (Script*) lua_touserdata(l, -1);

  script->get_game().get_current_map().restore_camera();

  return 0;
}

/**
 * @brief Notifies the script that the sequence started by a call to camera_move()
 * has reached the hero.
 */
void MapScript::notify_camera_back() {

  notify_script("event_camera_back");
}

/**
 * @brief Notifies the script that the player is obtaining a treasure.
 *
 * The treasure source does not matter: it can come from a chest,
 * a pickable item or a script.
 *
 * @param treasure the treasure obtained
 */
void MapScript::event_treasure_obtaining(const Treasure &treasure) {

  notify_script("event_treasure_obtaining", "sii",
      treasure.get_item_name().c_str(),
      treasure.get_variant(),
      treasure.get_savegame_variable());
}

/**
 * @brief Notifies the script that the player has just finished obtaining a treasure.
 *
 * The treasure source does not matter: it can come from a chest,
 * a pickable item or a script.
 *
 * @param treasure the treasure obtained
 */
void MapScript::event_treasure_obtained(const Treasure &treasure) {

  notify_script("event_treasure_obtained", "sii",
      treasure.get_item_name().c_str(),
      treasure.get_variant(),
      treasure.get_savegame_variable());
}

/**
 * @brief Notifies the script that the map has just been started.
 * @param destination_point_name name of the destination point where the hero is
 */
void MapScript::event_map_started(const std::string &destination_point_name) {

  notify_script("event_map_started",  "s", destination_point_name.c_str());
}

/**
 * @brief Notifies the script that the opening transition of the map has just finished.
 * @param destination_point_name name of the destination point where the hero is
 */
void MapScript::event_map_opening_transition_finished(const std::string &destination_point_name) {

  notify_script("event_map_opening_transition_finished", "s", destination_point_name.c_str());
}

/**
 * @brief Notifies the script that a switch has just been activated.
 * @param switch_name name of the switch
 */
void MapScript::event_switch_activated(const std::string &switch_name) {

  notify_script("event_switch_activated", "s", switch_name.c_str());
}

/**
 * @brief Notifies the script that a switch has just been inactivated.
 * @param switch_name name of the switch
 */
void MapScript::event_switch_inactivated(const std::string &switch_name) {

  notify_script("event_switch_inactivated", "s", switch_name.c_str());
}

/**
 * @brief Notifies the script that a switch has just been left by the entity that was on it.
 *
 * The fact that the switch is enabled or disabled does not matter here.
 *
 * @param switch_name name of the switch
 */
void MapScript::event_switch_left(const std::string &switch_name) {

  notify_script("event_switch_left", "s", switch_name.c_str());
}

/**
 * @brief Notifies the script that the victory sequence of the hero has just finished.
 */
void MapScript::event_hero_victory_sequence_finished() {

  notify_script("event_hero_victory_sequence_finished");
}

/**
 * @brief Notifies the script that the hero is overlapping a sensor.
 * @param sensor_name name of the sensor
 */
void MapScript::event_hero_on_sensor(const std::string& sensor_name) {

  notify_script("event_hero_on_sensor", "s", sensor_name.c_str());
}

/**
 * @brief Notifies the script that the hero is still overlapping a sensor.
 * @param sensor_name name of the sensor
 */
void MapScript::event_hero_still_on_sensor(const std::string& sensor_name) {

  notify_script("event_hero_still_on_sensor", "s", sensor_name.c_str());
}

/**
 * @brief Notifies the script that the hero has just finished interacting with an
 * NPC by pressing the action key, for an interaction that was handled via a script
 * (possibly this script or an item's script).
 * @param npc_name name of the NPC in interaction
 */
void MapScript::event_npc_interaction_finished(const std::string& npc_name) {

  notify_script("event_npc_interaction_finished", "s", npc_name.c_str());
}

/**
 * @brief Notifies the script that the hero has just finished interacting with an
 * NPC by pressing an item's key, for an interaction that was handled via a script
 * (possibly this script or an item's script).
 * @param npc_name name of the NPC in interaction
 * @param item_name name of the item that was used
 * @param variant variant of that item
 */
void MapScript::event_npc_interaction_item_finished(const std::string& npc_name,
    const std::string& item_name, int variant) {

  notify_script("event_npc_interaction_item_finished", "ssi", npc_name.c_str(),
      item_name.c_str(), variant);
}

/**
 * @brief Notifies the script that an NPC has just finished its movement.
 * @param npc_name name of the NPC
 */
void MapScript::event_npc_movement_finished(const std::string& npc_name) {

  notify_script("event_npc_movement_finished", "s", npc_name.c_str());
}

/**
 * @brief Notifies the script that an explosion just occured on a sensor.
 * @param sensor_name name of the sensor
 */
void MapScript::event_sensor_collision_explosion(const std::string& sensor_name) {

  notify_script("event_sensor_collision_explosion", "s", sensor_name.c_str());
}

/**
 * @brief Notifies the script that the player has just open an empty chest.
 *
 * What happens next is controlled by your script if it handles this event.
 * The hero is in state "freezed",
 * so if you do something else than giving the player a treasure,
 * don't forget to call hero_unfreeze() once you have finished.
 * The script function does not have to return any value.
 *
 * @param chest_name name of the chest
 * @return true if a script has handled the event, i.e. if the
 * event_chest_empty exists in the script
 */
bool MapScript::event_chest_empty(const std::string &chest_name) {

  bool exists = notify_script("event_chest_empty", "s", chest_name.c_str());
  return exists;
}

/**
 * @brief Notifies the script that the player wants to buy an item in a shop.
 * @param shop_item_name name of the item bought
 * @return true if the player is allowed to buy the item, false otherwise
 */
bool MapScript::event_shop_item_buying(const std::string& shop_item_name) {

  int can_buy = true; // if the function does not exist, allow buying the item
  notify_script("event_shop_item_buying", "s b", shop_item_name.c_str(), &can_buy);
  return can_buy != 0;
}

/**
 * @brief Notifies the script that the player has just bought an item in a shop.
 * @param shop_item_name name of the item bought
 */
void MapScript::event_shop_item_bought(const std::string& shop_item_name) {

  notify_script("event_shop_item_bought", "s", shop_item_name.c_str());
}

/**
 * @brief Notifies the script that a door has just been open.
 * @param door_name name of the door
 */
void MapScript::event_door_open(const std::string& door_name) {

  notify_script("event_door_open", "s", door_name.c_str());
}

/**
 * @brief Notifies the script that a door has just been closed.
 * @param door_name name of the door
 */
void MapScript::event_door_closed(const std::string& door_name) {

  notify_script("event_door_closed", "s", door_name.c_str());
}

/**
 * @brief Notifies the script that a block has just been pushed
 * or pulled by the hero.
 * @param block_name name of the block
 */
void MapScript::event_block_moved(const std::string& block_name) {

  notify_script("event_block_moved", "s", block_name.c_str());
}

/**
 * @brief Notifies the script that an enemy is being killed.
 * @param enemy_name name of the enemy
 */
void MapScript::event_enemy_dying(const std::string& enemy_name) {

  notify_script("event_enemy_dying", "s", enemy_name.c_str());
}

/**
 * @brief Notifies the script that an enemy has just been killed.
 * @param enemy_name name of the enemy
 */
void MapScript::event_enemy_dead(const std::string& enemy_name) {

  notify_script("event_enemy_dead", "s", enemy_name.c_str());
}

