/*
 * Copyright (C) 2009 Christopho, Zelda Solarus - http://www.zelda-solarus.com
 * 
 * Zelda: Mystery of Solarus DX is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Zelda: Mystery of Solarus DX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "Savegame.h"
#include "Equipment.h"
#include "DungeonEquipment.h"

/**
 * Creates a savegame with a specified file name, existing or not.
 * @param file_name name of the savegame file (can be a new file)
 */
Savegame::Savegame(const std::string &file_name) {

  this->file_name = file_name;

  FILE *file = fopen(file_name.c_str(), "r");

  if (file == NULL) {
    // this save slot is free
    empty = true;
    set_initial_values();

    this->equipment = NULL;
    this->dungeon_equipment = NULL;
  }
  else {
    // a save already exists, let's load it
    empty = false;
    fread(&saved_data, sizeof(SavedData), 1, file);
    fclose(file);

    this->equipment = new Equipment(this);
    this->dungeon_equipment = new DungeonEquipment(this);
  }
}

/**
 * Creates a savegame by copying an existing one, even if
 * it is not saved in its current state.
 * @param other the savegame to copy
 */
Savegame::Savegame(Savegame *other) {

  this->empty = other->empty;
  this->file_name = other->file_name;
  this->saved_data = other->saved_data;

  this->equipment = new Equipment(this);
  this->dungeon_equipment = new DungeonEquipment(this);
}

/**
 * Destructor.
 */
Savegame::~Savegame(void) {
  delete equipment;
  delete dungeon_equipment;
}

/**
 * Returns whether this is a new save.
 * @return true if there is no savegame file with this name yet
 */
bool Savegame::is_empty() {
  return empty;
}

/**
 * Loads the initial values.
 */
void Savegame::set_initial_values(void) {

  // 0 is the initial value of most variables
  memset(&saved_data, 0x0000, sizeof(SavedData));

  // a few variable have other initial values
  set_string(STARTING_POINT, "_start_position");
  set_integer(MAX_HEARTS, 3);
  set_integer(CURRENT_HEARTS, 12);

  set_integer(MAX_RUPEES, 100);

  set_integer(ITEM_SLOT_0, (Uint32) -1);
  set_integer(ITEM_SLOT_1, (Uint32) -1);

  // default game controls
  set_integer(KEYBOARD_ACTION_KEY, SDLK_SPACE);
  set_integer(KEYBOARD_SWORD_KEY, SDLK_c);
  set_integer(KEYBOARD_ITEM_1_KEY, SDLK_x);
  set_integer(KEYBOARD_ITEM_2_KEY, SDLK_v);
  set_integer(KEYBOARD_PAUSE_KEY, SDLK_d);
  set_integer(KEYBOARD_RIGHT_KEY, SDLK_RIGHT);
  set_integer(KEYBOARD_UP_KEY, SDLK_UP);
  set_integer(KEYBOARD_LEFT_KEY, SDLK_LEFT);
  set_integer(KEYBOARD_DOWN_KEY, SDLK_DOWN);

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
 * Saves the data into a file.
 */
void Savegame::save(void) {

  FILE *file = fopen(file_name.c_str(), "w");

  if (file == NULL) {
    DIE("Cannot write savegame file '" << file_name << "'");
  }

  fwrite(&saved_data, sizeof(SavedData), 1, file);
  fclose(file);

  empty = false;
}

/**
 * Returns the name of the file where the data is saved.
 * @return the file name of this savegame
 */
const std::string& Savegame::get_file_name(void) {
  return file_name;
}

/**
 * Returns the player's equipment corresponding to this savegame.
 * @return the equipment
 */
Equipment * Savegame::get_equipment(void) {
  return equipment;
}

/**
 * Returns the player's dungeon equipment corresponding to this savegame.
 * @return the dungeon equipment
 */
DungeonEquipment * Savegame::get_dungeon_equipment(void) {
  return dungeon_equipment;
}

/**
 * Returns a string value saved.
 * @param index index of the value to get, between 0 and 63
 * (see enum StringIndex for their definition)
 * @return the string value saved at this index
 */
const std::string Savegame::get_string(int index) {
  return saved_data.strings[index];
}

/**
 * Sets a string value saved.
 * @param index index of the value to set, between 0 and 63
 * (see enum StringIndex for their definition)
 * @param value the string value to store at this index
 */
void Savegame::set_string(int index, const std::string &value) {

  strncpy(saved_data.strings[index], value.c_str(), 63);
}

/**
 * Returns a integer value saved.
 * Values between 0 and 1023 are used by the engine (the C++ code)
 * and values between 1024 and 2047 are available to the map scripts.
 * @param index index of the value to get, between 0 and 2047
 * (see enum IntegerIndex for their definition)
 * @return the integer value saved at this index
 */
Uint32 Savegame::get_integer(int index) {
  return saved_data.integers[index];
}

/**
 * Sets an integer value saved.
 * Values between 0 and 1023 are used by the engine (the C++ code)
 * and values between 1024 and 2047 are available to the map scripts.
 * @param index index of the value to set, between 0 and 2047
 * (see enum IntegerIndex for their definition)
 * @param value the integer value to store at this index
 */
void Savegame::set_integer(int index, Uint32 value) {
  saved_data.integers[index] = value;
}

/**
 * Returns a boolean value saved.
 * @param index index of the value to get, between 0 and 32767
 * @return the boolean value saved at this index
 */
bool Savegame::get_boolean(int index) {

  Uint32 word = saved_data.booleans[index / 32];

  //  std::cout << "getting value at index " << index << ": " << (((word >> (index % 32)) & 0x0001) != 0x0000) << std::endl;

  return ((word >> (index % 32)) & 0x0001) != 0x0000;
}

/**
 * Sets a boolean value in the savegame.
 * @param index index of the value to set, between 0 and 32767
 * @param value the boolean value to store at this index
 */
void Savegame::set_boolean(int index, bool value) {

  Uint32 mask = 0x0001 << (index % 32);
  saved_data.booleans[index / 32] &= ~mask;

  if (value) {
    saved_data.booleans[index / 32] |= mask;
  }

  //  std::cout << "setting value " << value << " at index " << index << std::endl;
}
