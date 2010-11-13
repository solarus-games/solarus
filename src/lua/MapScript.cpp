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
#include "lua/MapScript.h"
#include "Map.h"
#include <sstream>
#include <iomanip>

/**
 * @brief Creates a map script.
 * @param map the map
 */
MapScript::MapScript(Map &map):
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
void MapScript::start(const std::string &destination_point_name) {

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

  if (is_loaded()) {
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

  notify_script("event_suspended", "b", suspended);
}

/**
 * @brief Notifies the script that a dialog has just started to be displayed
 * in the dialog box.
 * @param message_id id of the first message in this dialog
 */
void MapScript::event_dialog_started(const MessageId &message_id) {

  notify_script("event_dialog_started", "s", message_id.c_str());
}

/**
 * @brief Notifies the script that the dialog box has just finished.
 *
 * This function is called when the last message of a dialog is finished.
 * The dialog box has just been closed but the game is still suspended.
 * Note that this event is not called if the dialog was skipped.
 *
 * @param first_message_id id of the first message in the dialog
 * that has just finished
 * @param answer the answer selected by the player: 0 for the first one,
 * 1 for the second one, -1 if there was no question
 */
void MapScript::event_dialog_finished(const MessageId &first_message_id, int answer) {

  notify_script("event_dialog_finished", "si", first_message_id.c_str(), answer);
}

/**
 * @brief Notifies the script that the camera moved by a call to camera_move() has reached its target.
 */
void MapScript::event_camera_reached_target() {

  notify_script("event_camera_reached_target");
}

/**
 * @brief Notifies the script that the camera moved by a call to camera_restore() has reached the hero.
 */
void MapScript::event_camera_back() {

  notify_script("event_camera_back");
}

/**
 * @brief Notifies the script that the player is obtaining a treasure.
 *
 * The treasure source does not matter: it can come from a chest,
 * a pickable item or a script.
 *
 * @param item_name name of the item obtained
 * @param variant variant of this item
 * @param savegame_variable the boolean variable where this treasure is saved
 * (or -1 if the treasure is not saved)
 */
void MapScript::event_treasure_obtaining(const std::string &item_name, int variant, int savegame_variable) {

  notify_script("event_treasure_obtaining", "sii", item_name.c_str(), variant, savegame_variable);
}

/**
 * @brief Notifies the script that the player has just finished obtaining a treasure.
 *
 * The treasure source does not matter: it can come from a chest,
 * a pickable item or a script.
 *
 * @param item_name name of the item obtained
 * @param variant variant of this item
 * @param savegame_variable the boolean variable where this treasure is saved
 * (or -1 if the treasure is not saved)
 */
void MapScript::event_treasure_obtained(const std::string &item_name, int variant, int savegame_variable) {

  notify_script("event_treasure_obtained", "sii", item_name.c_str(), variant, savegame_variable);
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
 * @brief Notifies the script that a switch has just been enabled.
 * @param switch_name name of the switch
 */
void MapScript::event_switch_enabled(const std::string &switch_name) {

  notify_script("event_switch_enabled", "s", switch_name.c_str());
}

/**
 * @brief Notifies the script that a switch has just been disabled.
 * @param switch_name name of the switch
 */
void MapScript::event_switch_disabled(const std::string &switch_name) {

  notify_script("event_switch_disabled", "s", switch_name.c_str());
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
void MapScript::event_hero_on_sensor(const std::string &sensor_name) {

  notify_script("event_hero_on_sensor", "s", sensor_name.c_str());
}

/**
 * @brief Notifies the script that the player has just pressed the action
 * key in front of an interactive entity.
 * @param entity_name name of the interactive entity
 */
void MapScript::event_hero_interaction(const std::string &entity_name) {

  notify_script("event_hero_interaction", "s", entity_name.c_str());
}

/**
 * @brief Notifies the script that the hero is using an inventory item
 * in front of an interactive entity.
 *
 * This event is called only for inventory items that want to use an interactive entity
 * (e.g. a key that is being used in front of a door).
 *
 * @param entity_name name of the interactive entity the hero is facing
 * @param item_name name of the inventory item that is being used
 * @param variant variant of this inventory item
 * @return true if the script has handled the event,
 * i.e. if the function event_hero_interaction_item exists in the script and returned true
 */
bool MapScript::event_hero_interaction_item(const std::string &entity_name, const std::string &item_name, int variant) {

  bool interaction = false;
  notify_script("event_hero_interaction_item", "sss b", entity_name.c_str(), item_name.c_str(), variant, &interaction);

  return interaction;
}

/**
 * @brief Notifies the script that the player has just pressed the action
 * key in front an NPC.
 * @param npc_name name of the NPC
 */
void MapScript::event_npc_dialog(const std::string &npc_name) {

  notify_script("event_npc_dialog", "s", npc_name.c_str());
}

/**
 * @brief Notifies the script that an NPC has just finished its movement.
 * @param npc_name name of the NPC
 */
void MapScript::event_npc_movement_finished(const std::string &npc_name) {

  notify_script("event_npc_movement_finished", "s", npc_name.c_str());
}

/**
 * @brief Notifies the script that the player has just open an empty chest.
 *
 * What happens next is controlled by your script if it handles this event.
 * The hero is in state FREEZE,
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
 * @brief Notifies the script that the player has just bought an item in a shop.
 * @param shop_item_name name of the item bought
 */
void MapScript::event_shop_item_bought(const std::string &shop_item_name) {

  notify_script("event_shop_item_bought", "s", shop_item_name.c_str());
}

/**
 * @brief Notifies the script that an enemy has just been killed.
 * @param enemy_name name of the enemy
 */
void MapScript::event_enemy_dead(const std::string &enemy_name) {

  notify_script("event_enemy_dead", "s", enemy_name.c_str());
}

