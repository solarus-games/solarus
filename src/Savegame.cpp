/*
 * Copyright (C) 2006-2012 Christopho, Solarus - http://www.solarus-games.org
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
#include "Savegame.h"
#include "SavegameConverterV1.h"
#include "lua/LuaContext.h"
#include "lowlevel/FileTools.h"
#include "lowlevel/InputEvent.h"
#include "lowlevel/IniFile.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"

const int Savegame::SAVEGAME_VERSION = 2;

const std::string Savegame::KEY_SAVEGAME_VERSION = "_version";
const std::string Savegame::KEY_STARTING_MAP = "_starting_map";
const std::string Savegame::KEY_STARTING_POINT = "_starting_point";
const std::string Savegame::KEY_KEYBOARD_ACTION = "_keyboard_action";
const std::string Savegame::KEY_KEYBOARD_SWORD = "_keyboard_sword";
const std::string Savegame::KEY_KEYBOARD_ITEM_1 = "_keyboard_item_1";
const std::string Savegame::KEY_KEYBOARD_ITEM_2 = "_keyboard_item_2";
const std::string Savegame::KEY_KEYBOARD_PAUSE = "_keyboard_pause";
const std::string Savegame::KEY_KEYBOARD_RIGHT = "_keyboard_right";
const std::string Savegame::KEY_KEYBOARD_UP = "_keyboard_up";
const std::string Savegame::KEY_KEYBOARD_LEFT = "_keyboard_left";
const std::string Savegame::KEY_KEYBOARD_DOWN = "_keyboard_down";
const std::string Savegame::KEY_JOYPAD_ACTION = "_joypad_action";
const std::string Savegame::KEY_JOYPAD_SWORD = "_joypad_sword";
const std::string Savegame::KEY_JOYPAD_ITEM_1 = "_joypad_item_1";
const std::string Savegame::KEY_JOYPAD_ITEM_2 = "_joypad_item_2";
const std::string Savegame::KEY_JOYPAD_PAUSE = "_joypad_pause";
const std::string Savegame::KEY_JOYPAD_RIGHT = "_joypad_right";
const std::string Savegame::KEY_JOYPAD_UP = "_joypad_up_key";
const std::string Savegame::KEY_JOYPAD_LEFT = "_joypad_left_key";
const std::string Savegame::KEY_JOYPAD_DOWN = "_joypad_down_key";
const std::string Savegame::KEY_EQUIPMENT_INITIALIZED = "_equipment_initialized";
const std::string Savegame::KEY_CURRENT_LIFE = "_current_life";
const std::string Savegame::KEY_CURRENT_MONEY = "_current_money";
const std::string Savegame::KEY_CURRENT_MAGIC = "_max_money";
const std::string Savegame::KEY_MAX_LIFE = "_max_life";
const std::string Savegame::KEY_MAX_MONEY = "_max_money";
const std::string Savegame::KEY_MAX_MAGIC = "_max_magic";
const std::string Savegame::KEY_ABILITY_TUNIC = "_ability_tunic";
const std::string Savegame::KEY_ABILITY_SWORD = "_ability_sword";
const std::string Savegame::KEY_ABILITY_SHIELD = "_ability_shield";
const std::string Savegame::KEY_ABILITY_LIFT = "_ability_lift";
const std::string Savegame::KEY_ABILITY_SWIM = "_ability_swim";
const std::string Savegame::KEY_ABILITY_SWORD_KNOWLEDGE = "_ability_sword_knowledge";
const std::string Savegame::KEY_ABILITY_DETECT_WEAK_WALLS = "_ability_detect_weak_walls";
const std::string Savegame::KEY_ABILITY_SEE_OUTSIDE_WORLD_MINIMAP = "_ability_see_outside_world_minimap";
const std::string Savegame::KEY_ABILITY_GET_BACK_FROM_DEATH = "_ability_get_back_from_death";
const std::string Savegame::KEY_ABILITY_RUN = "_ability_run";

/**
 * @brief Creates a savegame with a specified file name, existing or not.
 * @param file_name Name of the savegame file (can be a new file),
 * relative to the savegames directory, with its extension.
 */
Savegame::Savegame(const std::string& file_name):
  ExportableToLua(),
  file_name(file_name),
  equipment(*this),
  game(NULL) {

  if (!FileTools::data_file_exists(file_name)) {
    // This save does not exist yet.
    empty = true;
    set_initial_values();
  }
  else {
    // A save already exists, let's load it.
    empty = false;
    load();
  }
}

/**
 * @brief Destructor.
 */
Savegame::~Savegame() {
}

/**
 * @brief Returns whether this is a new save.
 * @return true if there is no savegame file with this name yet
 */
bool Savegame::is_empty() {
  return empty;
}

/**
 * @brief Loads the initial values.
 */
void Savegame::set_initial_values() {

  // Set the savegame format version.
  set_integer(KEY_SAVEGAME_VERSION, SAVEGAME_VERSION);

  // Set the initial controls.
  set_default_keyboard_controls();
  set_default_joypad_controls();

  // Set some other values from the quest file.
  IniFile ini("quest.dat", IniFile::READ);
  ini.set_group("initial");
  const std::string& starting_map_id = ini.get_string_value("starting_map", "");
  const std::string& starting_destination_name = ini.get_string_value("starting_point", "");
  int max_life = ini.get_integer_value("max_life", 1);

  Debug::check_assertion(!starting_map_id.empty(),
      "No starting map defined in quest.dat. Please set the value starting_map to the id of the initial map of your quest.");
  Debug::check_assertion(!starting_destination_name.empty(),
      "No starting point defined in quest.dat. Please set the value starting_point to the name of the "
      "destination point where the hero should be placed on the initial map.");

  set_string(KEY_STARTING_MAP, starting_map_id);
  set_string(KEY_STARTING_POINT, starting_destination_name);
  set_integer(KEY_MAX_LIFE, max_life);
  set_integer(KEY_CURRENT_LIFE, max_life);
}

/**
 * @brief Sets default values for the keyboard game controls.
 */
void Savegame::set_default_keyboard_controls() {

#ifndef PANDORA
  set_integer(KEY_KEYBOARD_ACTION, InputEvent::KEY_SPACE);
  set_integer(KEY_KEYBOARD_SWORD, InputEvent::KEY_c);
  set_integer(KEY_KEYBOARD_ITEM_1, InputEvent::KEY_x);
  set_integer(KEY_KEYBOARD_ITEM_2, InputEvent::KEY_v);
  set_integer(KEY_KEYBOARD_PAUSE, InputEvent::KEY_d);
  set_integer(KEY_KEYBOARD_RIGHT, InputEvent::KEY_RIGHT);
  set_integer(KEY_KEYBOARD_UP, InputEvent::KEY_UP);
  set_integer(KEY_KEYBOARD_LEFT, InputEvent::KEY_LEFT);
  set_integer(KEY_KEYBOARD_DOWN, InputEvent::KEY_DOWN);
#else
  set_integer(KEY_KEYBOARD_ACTION, SDLK_PAGEDOWN);
  set_integer(KEY_KEYBOARD_SWORD, SDLK_HOME);
  set_integer(KEY_KEYBOARD_ITEM_1, SDLK_PAGEUP);
  set_integer(KEY_KEYBOARD_ITEM_2, SDLK_END);
  set_integer(KEY_KEYBOARD_PAUSE, SDLK_LALT);
  set_integer(KEY_KEYBOARD_RIGHT, InputEvent::KEY_RIGHT);
  set_integer(KEY_KEYBOARD_UP, InputEvent::KEY_UP);
  set_integer(KEY_KEYBOARD_LEFT, InputEvent::KEY_LEFT);
  set_integer(KEY_KEYBOARD_DOWN, InputEvent::KEY_DOWN);
#endif
}

/**
 * @brief Sets default values for the joypad game controls.
 */
void Savegame::set_default_joypad_controls() {

  set_string(KEY_JOYPAD_ACTION, "button 0");
  set_string(KEY_JOYPAD_SWORD, "button 1");
  set_string(KEY_JOYPAD_ITEM_1, "button 2");
  set_string(KEY_JOYPAD_ITEM_2, "button 3");
  set_string(KEY_JOYPAD_PAUSE, "button 4");
  set_string(KEY_JOYPAD_RIGHT, "axis 0 +");
  set_string(KEY_JOYPAD_UP, "axis 1 -");
  set_string(KEY_JOYPAD_LEFT, "axis 0 -");
  set_string(KEY_JOYPAD_DOWN, "axis 1 +");
}

/**
 * @brief Reads the data from the savegame file.
 */
void Savegame::load() {

  // TODO try to parse as Lua, if fail try to convert from version 1 format
  SavegameConverterV1 converter(file_name);
  converter.convert_to_v2(*this);

  // TODO remove (temporary debugging code)
  save();
}

/**
 * @brief Saves the data into a file.
 */
void Savegame::save() {

  std::ostringstream oss;
  std::map<std::string, SavedValue>::iterator it;
  for (it = saved_values.begin(); it != saved_values.end(); it++) {
    const std::string& key = it->first;
    oss << key << " = ";
    const SavedValue& value = it->second;
    if (value.type == SavedValue::VALUE_BOOLEAN) {
      oss << (value.int_data ? "true" : "false");
    }
    else if (value.type == SavedValue::VALUE_INTEGER) {
      oss << value.int_data;
    }
    else {  // String.
      oss << "\"" << value.string_data << "\"";
    }
    oss << "\n";
  }

  const std::string& text = oss.str();
  // TODO remove ".alpha" once it's stable
  FileTools::data_file_save_buffer(file_name + ".alpha", text.c_str(), text.size());
  empty = false;
}

/**
 * @brief Returns the name of the file where the data is saved.
 * @return the file name of this savegame
 */
const std::string& Savegame::get_file_name() {
  return file_name;
}

/**
 * @brief Returns the player's equipment corresponding to this savegame.
 * @return the equipment
 */
Equipment& Savegame::get_equipment() {
  return equipment;
}

/**
 * @brief If this savegame is currently running in a game, return that game.
 * @return A game or NULL.
 */
Game* Savegame::get_game() {
  return game;
}

/**
 * @brief Sets the game that is running this savegame.
 * @param game A game or NULL.
 */
void Savegame::set_game(Game* game) {

  this->game = game;
  if (game != NULL) {
    // Notify the equipment.
    equipment.set_game(*game);
  }
}

/**
 * @brief Returns whether a saved value is a string.
 * @param key Name of the value to get.
 * @return true if this value exists and is a string.
 */
bool Savegame::is_string(const std::string& key) {

  Debug::check_assertion(LuaContext::is_valid_lua_identifier(key), StringConcat() <<
      "Savegame variable '" << key << "' is not a valid key");

  bool result = false;
  if (saved_values.count(key) > 0) {
    const SavedValue& value = saved_values[key];
    result = (value.type == SavedValue::VALUE_STRING);
  }
  return result;
}

/**
 * @brief Returns a string value saved.
 * @param key Name of the value to get.
 * @return The string value associated with this key or an empty string.
 */
const std::string& Savegame::get_string(const std::string& key) {

  Debug::check_assertion(LuaContext::is_valid_lua_identifier(key), StringConcat() <<
      "Savegame variable '" << key << "' is not a valid key");

  if (saved_values.count(key) > 0) {
    const SavedValue& value = saved_values[key];
    Debug::check_assertion(value.type == SavedValue::VALUE_STRING, StringConcat() <<
        "Value '" << key << "' is not a string");
    return value.string_data;
  }

  static const std::string empty_string = "";
  return empty_string;
}

/**
 * @brief Sets a string value saved.
 * @param key Name of the value to set.
 * @return The string value to associate with this key.
 */
void Savegame::set_string(const std::string& key, const std::string& value) {

  Debug::check_assertion(LuaContext::is_valid_lua_identifier(key), StringConcat() <<
      "Savegame variable '" << key << "' is not a valid key");

  saved_values[key].type = SavedValue::VALUE_STRING;
  saved_values[key].string_data = value;
}

/**
 * @brief Returns whether a saved value is an integer.
 * @param key Name of the value to get.
 * @return true if this value exists and is an integer.
 */
bool Savegame::is_integer(const std::string& key) {

  Debug::check_assertion(LuaContext::is_valid_lua_identifier(key), StringConcat() <<
      "Savegame variable '" << key << "' is not a valid key");

  bool result = false;
  if (saved_values.count(key) > 0) {
    const SavedValue& value = saved_values[key];
    result = (value.type == SavedValue::VALUE_INTEGER);
  }
  return result;
}

/**
 * @brief Returns a integer value saved.
 * @param key Name of the value to get.
 * @return The integer value associated with this key or 0.
 */
int Savegame::get_integer(const std::string& key) {

  Debug::check_assertion(LuaContext::is_valid_lua_identifier(key), StringConcat() <<
      "Savegame variable '" << key << "' is not a valid key");

  int result = 0;
  if (saved_values.count(key) > 0) {
    const SavedValue& value = saved_values[key];
    Debug::check_assertion(value.type == SavedValue::VALUE_INTEGER, StringConcat() <<
        "Value '" << key << "' is not an integer");
    result = value.int_data;
  }
  return result;
}

/**
 * @brief Sets an integer value saved.
 * @param key Name of the value to set.
 * @return The integer value to associate with this key.
 */
void Savegame::set_integer(const std::string& key, int value) {

  Debug::check_assertion(LuaContext::is_valid_lua_identifier(key), StringConcat() <<
      "Savegame variable '" << key << "' is not a valid key");

  saved_values[key].type = SavedValue::VALUE_INTEGER;
  saved_values[key].int_data = value;
}

/**
 * @brief Returns whether a saved value is a boolean.
 * @param key Name of the value to get.
 * @return true if this value exists and is a boolean.
 */
bool Savegame::is_boolean(const std::string& key) {

  Debug::check_assertion(LuaContext::is_valid_lua_identifier(key), StringConcat() <<
      "Savegame variable '" << key << "' is not a valid key");

  bool result = false;
  if (saved_values.count(key) > 0) {
    const SavedValue& value = saved_values[key];
    result = (value.type == SavedValue::VALUE_BOOLEAN);
  }
  return result;
}

/**
 * @brief Returns a boolean value saved.
 * @param key Name of the value to get.
 * @return The boolean value associated with this key or false.
 */
bool Savegame::get_boolean(const std::string& key) {

  Debug::check_assertion(LuaContext::is_valid_lua_identifier(key), StringConcat() <<
      "Savegame variable '" << key << "' is not a valid key");

  bool result = false;
  if (saved_values.count(key) > 0) {
    const SavedValue& value = saved_values[key];
    Debug::check_assertion(value.type == SavedValue::VALUE_BOOLEAN, StringConcat() <<
        "Value '" << key << "' is not a boolean");
    result = value.int_data != 0;
  }
  return result;
}

/**
 * @brief Sets a boolean value saved.
 * @param key Name of the value to set.
 * @return The boolean value to associate with this key.
 */
void Savegame::set_boolean(const std::string& key, bool value) {

  Debug::check_assertion(LuaContext::is_valid_lua_identifier(key), StringConcat() <<
      "Savegame variable '" << key << "' is not a valid key");

  saved_values[key].type = SavedValue::VALUE_BOOLEAN;
  saved_values[key].int_data = value;
}

/**
 * @brief Returns the name identifying this type in Lua.
 * @return The name identifying this type in Lua.
 */
const std::string& Savegame::get_lua_type_name() const {
  return LuaContext::game_module_name;
}

