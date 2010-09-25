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
#include "lua/Scripts.h"
#include "lua/Script.h"
#include <lua.hpp>

/**
 * Constructor.
 */
Scripts::Scripts() {

}

/**
 * Destructor.
 */
Scripts::~Scripts() {

}

/**
 * @brief Adds a script to the list.
 * @param script the script to add
 */
void Scripts::add_script(Script &script) {
  scripts.push_back(&script);
}

/**
 * @brief Removes a script from the list.
 * @param script the script to remove
 */
void Scripts::remove_script(Script &script) {
  scripts.remove(&script);
}

// functions to call Lua from C++

/**
 * @brief Notifies the scripts that a dialog has just started to be displayed
 * in the dialog box.
 * @param message_id id of the first message in this dialog
 */
void Scripts::event_dialog_started(const MessageId &message_id) {

  for (std::list<Script*>::iterator it = scripts.begin(); it != scripts.end(); it++) {
    (*it)->notify_script("event_dialog_started", message_id);
  }
}

/**
 * @brief Notifies the scripts that the dialog box has just finished.
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
void Scripts::event_dialog_finished(const MessageId &first_message_id, int answer) {

  for (std::list<Script*>::iterator it = scripts.begin(); it != scripts.end(); it++) {
    (*it)->notify_script("event_dialog_finished", first_message_id, answer);
  }
}

/**
 * @brief Notifies the scripts that the camera moved by a call to camera_move() has reached its target.
 */
void Scripts::event_camera_reached_target() {

  for (std::list<Script*>::iterator it = scripts.begin(); it != scripts.end(); it++) {
    (*it)->notify_script("event_camera_reached_target");
  }
}

/**
 * @brief Notifies the scripts that the camera moved by a call to camera_restore() has reached the hero.
 */
void Scripts::event_camera_back() {

  for (std::list<Script*>::iterator it = scripts.begin(); it != scripts.end(); it++) {
    (*it)->notify_script("event_camera_back");
  }
}

/**
 * @brief Notifies the scripts that the player is obtaining a treasure.
 *
 * The treasure source does not matter: it can come from a chest,
 * a pickable item or a script.
 *
 * @param item_name name of the item obtained
 * @param variant variant of this item
 * @param savegame_variable the boolean variable where this treasure is saved
 * (or -1 if the treasure is not saved)
 */
void Scripts::event_treasure_obtaining(const std::string &item_name, int variant, int savegame_variable) {

  for (std::list<Script*>::iterator it = scripts.begin(); it != scripts.end(); it++) {
    (*it)->notify_script("event_treasure_obtaining", item_name, variant, savegame_variable);
  }
}

/**
 * @brief Notifies the scripts that the player has just finished obtaining a treasure.
 *
 * The treasure source does not matter: it can come from a chest,
 * a pickable item or a script.
 *
 * @param item_name name of the item obtained
 * @param variant variant of this item
 * @param savegame_variable the boolean variable where this treasure is saved
 * (or -1 if the treasure is not saved)
 */
void Scripts::event_treasure_obtained(const std::string &item_name, int variant, int savegame_variable) {

  for (std::list<Script*>::iterator it = scripts.begin(); it != scripts.end(); it++) {
    (*it)->notify_script("event_treasure_obtained", item_name, variant, savegame_variable);
  }
}

/**
 * @brief Notifies the scripts that the map has just been started.
 * @param destination_point_name name of the destination point where the hero is
 */
void Scripts::event_map_started(const std::string &destination_point_name) {
  
  for (std::list<Script*>::iterator it = scripts.begin(); it != scripts.end(); it++) {
    (*it)->notify_script("event_map_started",  destination_point_name);
  }
}

/**
 * @brief Notifies the scripts that the opening transition of the map has just finished.
 * @param destination_point_name name of the destination point where the hero is
 */
void Scripts::event_map_opening_transition_finished(const std::string &destination_point_name) {
  
  for (std::list<Script*>::iterator it = scripts.begin(); it != scripts.end(); it++) {
    (*it)->notify_script("event_map_opening_transition_finished", destination_point_name);
  }
}

/**
 * @brief Notifies the scripts that a switch has just been enabled.
 * @param switch_name name of the switch
 */
void Scripts::event_switch_enabled(const std::string &switch_name) {
  
  for (std::list<Script*>::iterator it = scripts.begin(); it != scripts.end(); it++) {
    (*it)->notify_script("event_switch_enabled", switch_name);
  }
}

/**
 * @brief Notifies the scripts that a switch has just been disabled.
 * @param switch_name name of the switch
 */
void Scripts::event_switch_disabled(const std::string &switch_name) {
  
  for (std::list<Script*>::iterator it = scripts.begin(); it != scripts.end(); it++) {
    (*it)->notify_script("event_switch_disabled", switch_name);
  }
}

/**
 * @brief Notifies the scripts that a switch has just been left by the entity that was on it.
 *
 * The fact that the switch is enabled or disabled does not matter here.
 *
 * @param switch_name name of the switch
 */
void Scripts::event_switch_left(const std::string &switch_name) {
  
  for (std::list<Script*>::iterator it = scripts.begin(); it != scripts.end(); it++) {
    (*it)->notify_script("event_switch_left", switch_name);
  }
}

/**
 * @brief Notifies the scripts that the victory sequence of the hero has just finished.
 */
void Scripts::event_hero_victory_sequence_finished() {
  
  for (std::list<Script*>::iterator it = scripts.begin(); it != scripts.end(); it++) {
    (*it)->notify_script("event_hero_victory_sequence_finished");
  }
}

/**
 * @brief Notifies the scripts that the hero is overlapping a sensor.
 * @param sensor_name name of the sensor
 */
void Scripts::event_hero_on_sensor(const std::string &sensor_name) {
  
  for (std::list<Script*>::iterator it = scripts.begin(); it != scripts.end(); it++) {
    (*it)->notify_script("event_hero_on_sensor", sensor_name);
  }
}

/**
 * @brief Notifies the scripts that the player has just pressed the action
 * key in front of an interactive entity.
 * @param entity_name name of the interactive entity
 */
void Scripts::event_hero_interaction(const std::string &entity_name) {
  
  for (std::list<Script*>::iterator it = scripts.begin(); it != scripts.end(); it++) {
    (*it)->notify_script("event_hero_interaction", entity_name);
  }
}

/**
 * @brief Notifies the scripts that the hero is using an inventory item
 * in front of an interactive entity.
 *
 * This event is called only for inventory items that want to use an interactive entity
 * (e.g. a key that is being used in front of a door).
 *
 * @param entity_name name of the interactive entity the hero is facing
 * @param item_name name of the inventory item that is being used
 * @param variant variant of this inventory item
 * @return true if a script has handled the event
 */
bool Scripts::event_hero_interaction_item(const std::string &entity_name, const std::string &item_name, int variant) {
  
  bool handled = false;
  for (std::list<Script*>::iterator it = scripts.begin(); it != scripts.end(); it++) {

    Script &script = *(*it);
    bool exists = script.notify_script("event_hero_interaction_item", entity_name, item_name, variant);
    bool interaction = lua_toboolean(script.context, 1);

    if (!handled) {
      handled = exists && interaction;
    }
  }

  return handled;
}

/**
 * @brief Notifies the scripts that the player has just pressed the action
 * key in front an NPC.
 * @param npc_name name of the NPC
 */
void Scripts::event_npc_dialog(const std::string &npc_name) {
   
  for (std::list<Script*>::iterator it = scripts.begin(); it != scripts.end(); it++) {
    (*it)->notify_script("event_npc_dialog", npc_name);
  }
}

/**
 * @brief Notifies the scripts that an NPC has just finished its movement.
 * @param npc_name name of the NPC
 */
void Scripts::event_npc_movement_finished(const std::string &npc_name) {
   
  for (std::list<Script*>::iterator it = scripts.begin(); it != scripts.end(); it++) {
    (*it)->notify_script("event_npc_movement_finished", npc_name);
  }
}

/**
 * @brief Notifies the scripts that the player has just open an empty chest.
 *
 * What happens next is controlled by your script if it handles this event.
 * The hero is in state FREEZE
 * so if you do something else than giving the player a treasure,
 * don't forget to call hero_unfreeze() once you have finished.
 * The script function does not have to return any value.
 *
 * @param chest_name name of the chest
 * @return true if a script has handled the event, i.e. if the
 * event_chest_empty exists in at least one script
 */
bool Scripts::event_chest_empty(const std::string &chest_name) {
   
  bool handled = false;
  for (std::list<Script*>::iterator it = scripts.begin(); it != scripts.end(); it++) {
    bool exists = (*it)->notify_script("event_chest_empty", chest_name);
    if (!handled) {
      handled = exists;
    }
  }

  return handled;
}

/**
 * @brief Notifies the scripts that the player has just bought an item in a shop.
 * @param shop_item_name name of the item bought
 */
void Scripts::event_shop_item_bought(const std::string &shop_item_name) {

  for (std::list<Script*>::iterator it = scripts.begin(); it != scripts.end(); it++) {
    (*it)->notify_script("event_shop_item_bought", shop_item_name);
  }
}

/**
 * @brief Notifies the scripts that an enemy has just been killed.
 * @param enemy_name name of the enemy
 */
void Scripts::event_enemy_dead(const std::string &enemy_name) {

  for (std::list<Script*>::iterator it = scripts.begin(); it != scripts.end(); it++) {
    (*it)->notify_script("event_enemy_dead", enemy_name);
  }
}

