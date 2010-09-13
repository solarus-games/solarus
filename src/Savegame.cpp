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
#include "Savegame.h"
#include "Equipment.h"
#include "lowlevel/FileTools.h"
#include "lowlevel/InputEvent.h"
#include "lowlevel/IniFile.h"

/**
 * @brief Creates a savegame with a specified file name, existing or not.
 * @param file_name name of the savegame file (can be a new file), relative to the savegames directory
 */
Savegame::Savegame(const std::string &file_name) {

  this->file_name = file_name;
  this->equipment = new Equipment(this);

  if (!FileTools::data_file_exists(file_name.c_str())) {
    // this save slot is free
    empty = true;
    set_initial_values();
  }
  else {
    // a save already exists, let's load it
    empty = false;

    size_t size;
    char *buffer;

    FileTools::data_file_open_buffer(file_name, &buffer, &size);
    if (size != sizeof(SavedData)) {
      DIE("Cannot read savegame file '" << file_name << "': invalid file size");
    }
    memcpy(&saved_data, buffer, sizeof(SavedData));
    FileTools::data_file_close_buffer(buffer);

    // check that the savegame is compatible with the current savegame system
    if (get_integer(SAVEGAME_COMPATIBILITY_FORMAT) != CURRENT_COMPATIBILITY_FORMAT) {

      // obsolete savegame file: create a new savegame instead
      empty = true;
      set_initial_values();
    }
    else {

      // the savegame file is okay
      check_game_controls();
    }
  }
}

/**
 * @brief Creates a savegame by copying an existing one, even if
 * it is not saved in its current state.
 * @param other the savegame to copy
 */
Savegame::Savegame(Savegame *other) {

  this->empty = other->empty;
  this->file_name = other->file_name;
  this->saved_data = other->saved_data;

  this->equipment = new Equipment(this);
}

/**
 * @brief Destructor.
 */
Savegame::~Savegame(void) {
  delete equipment;
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
void Savegame::set_initial_values(void) {

  // 0 is the initial value of most variables
  memset(&saved_data, 0x0000, sizeof(SavedData));

  // set the compatibility version
  set_integer(SAVEGAME_COMPATIBILITY_FORMAT, CURRENT_COMPATIBILITY_FORMAT);

  // set the initial controls
  set_default_keyboard_controls();
  set_default_joypad_controls();

  // set some other values from the quest file
  IniFile ini("quest.dat", IniFile::READ);
  ini.set_group("initial");
  int starting_map_id = ini.get_integer_value("starting_map", -1);
  const std::string &starting_destination_point_name = ini.get_string_value("starting_point", "");
  int max_life = ini.get_integer_value("max_life", 1);

  if (starting_map_id == -1) {
    DIE("No starting map defined in quest.dat. Please set the value starting_map to the id of the initial map of your quest.");
  }
  if (starting_destination_point_name == "") {
    DIE("No starting point defined in quest.dat. Please set the value starting_point to the name of the "
	"destination point where the hero should be placed on the initial map.");
  }

  // the equipment may give some initial items
  equipment->set_initial_items();

  set_integer(STARTING_MAP, starting_map_id);
  set_string(STARTING_POINT, starting_destination_point_name);
  set_integer(MAX_LIFE, max_life);
  set_integer(CURRENT_LIFE, max_life);
}

/**
 * @brief Sets default values for the keyboard game controls.
 */
void Savegame::set_default_keyboard_controls(void) {

  set_integer(KEYBOARD_ENUM_VERSION, InputEvent::KEYBOARD_ENUM_VERSION);

  set_integer(KEYBOARD_ACTION_KEY, InputEvent::KEY_SPACE);
  set_integer(KEYBOARD_SWORD_KEY, InputEvent::KEY_c);
  set_integer(KEYBOARD_ITEM_1_KEY, InputEvent::KEY_x);
  set_integer(KEYBOARD_ITEM_2_KEY, InputEvent::KEY_v);
  set_integer(KEYBOARD_PAUSE_KEY, InputEvent::KEY_d);
  set_integer(KEYBOARD_RIGHT_KEY, InputEvent::KEY_RIGHT);
  set_integer(KEYBOARD_UP_KEY, InputEvent::KEY_UP);
  set_integer(KEYBOARD_LEFT_KEY, InputEvent::KEY_LEFT);
  set_integer(KEYBOARD_DOWN_KEY, InputEvent::KEY_DOWN);
}

/**
 * @brief Sets default values for the joypad game controls.
 */
void Savegame::set_default_joypad_controls(void) {

  set_string(JOYPAD_ACTION_KEY, "button 0");
  set_string(JOYPAD_SWORD_KEY, "button 1");
  set_string(JOYPAD_ITEM_1_KEY, "button 2");
  set_string(JOYPAD_ITEM_2_KEY, "button 3");
  set_string(JOYPAD_PAUSE_KEY, "button 4");
  set_string(JOYPAD_RIGHT_KEY, "axis 0 +");
  set_string(JOYPAD_UP_KEY, "axis 1 -");
  set_string(JOYPAD_LEFT_KEY, "axis 0 -");
  set_string(JOYPAD_DOWN_KEY, "axis 1 +");
}

/**
 * @brief Ensures the keyboard mapping saved is valid with respect to the current version of
 * the enumeration InputEvent::KeyboardKey.
 *
 * If the bindings saved corresponds to an old version of this enumeration, it is obsolete and
 * we reset it to the default values.
 */
void Savegame::check_game_controls(void) {

  if (get_integer(KEYBOARD_ENUM_VERSION) != (uint16_t) InputEvent::KEYBOARD_ENUM_VERSION) {
    /* The enumeration has changed, probably because this savegame was created with an old version of the game.
     * Thus, the keys saved are not valid anymore and we reset them to the default values.
     */
    set_default_keyboard_controls();
  }
}

/**
 * @brief Saves the data into a file.
 */
void Savegame::save(void) {

  FileTools::data_file_save_buffer(file_name, (char*) &saved_data, sizeof(SavedData));
  empty = false;
}

/**
 * @brief Returns the name of the file where the data is saved.
 * @return the file name of this savegame
 */
const std::string& Savegame::get_file_name(void) {
  return file_name;
}

/**
 * @brief Returns the player's equipment corresponding to this savegame.
 * @return the equipment
 */
Equipment * Savegame::get_equipment(void) {
  return equipment;
}

/**
 * @brief Returns a string value saved.
 * @param index index of the value to get, between 0 and 63
 * (see enum StringIndex for their definition)
 * @return the string value saved at this index
 */
const std::string Savegame::get_string(int index) {
  return saved_data.strings[index];
}

/**
 * @brief Sets a string value saved.
 * @param index index of the value to set, between 0 and 63
 * (see enum StringIndex for their definition)
 * @param value the string value to store at this index
 */
void Savegame::set_string(int index, const std::string &value) {
  strncpy(saved_data.strings[index], value.c_str(), 63);
}

/**
 * @brief Returns a integer value saved.
 *
 * Values between 0 and 1023 are used by the engine (the C++ code)
 * and values between 1024 and 2047 are available to the map scripts.
 * @param index index of the value to get, between 0 and 2047
 * (see enum IntegerIndex for their definition)
 * @return the integer value saved at this index
 */
uint32_t Savegame::get_integer(int index) {
  return saved_data.integers[index];
}

/**
 * @brief Sets an integer value saved.
 *
 * Values between 0 and 1023 are used by the engine (the C++ code)
 * and values between 1024 and 2047 are available to the map scripts.
 * @param index index of the value to set, between 0 and 2047
 * (see enum IntegerIndex for their definition)
 * @param value the integer value to store at this index
 */
void Savegame::set_integer(int index, uint32_t value) {
  saved_data.integers[index] = value;
}

/**
 * @brief Returns a boolean value saved.
 * @param index index of the value to get, between 0 and 32767
 * @return the boolean value saved at this index
 */
bool Savegame::get_boolean(int index) {

  uint32_t word = saved_data.booleans[index / 32];
  return ((word >> (index % 32)) & 0x0001) != 0x0000;
}

/**
 * @brief Sets a boolean value in the savegame.
 * @param index index of the value to set, between 0 and 32767
 * @param value the boolean value to store at this index
 */
void Savegame::set_boolean(int index, bool value) {

  uint32_t mask = 0x0001 << (index % 32);
  saved_data.booleans[index / 32] &= ~mask;

  if (value) {
    saved_data.booleans[index / 32] |= mask;
  }
}

