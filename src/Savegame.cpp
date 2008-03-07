/**
 * This module implements the class Savegame.
 */

#include "Savegame.h"

/*
 * Saved data:
 *
 * - Reserved strings (for the game engine):
 *   0: player name
 *
 * - Reserved integers (for the game engine):
 *   0: starting map id
 *   1: starting entrance index
 *
 *
 *
 *
 */

/**
 * Creates a savegame with a specified file name, existing or not.
 * @param file_name name of the savegame file (can be a new file)
 */
Savegame::Savegame(const char *file_name) {
  memset(&saved_data, 0x0000, sizeof(SavedData));
  strncpy(this->file_name, file_name, 32);

  FILE *file = fopen(file_name, "r");

  if (file == NULL) {
    // this save slot is free
    empty = true;
  }
  else {
    // a save already exists, let's load it
    empty = false;
    fread(&saved_data, sizeof(SavedData), 1, file);
  }
}

/**
 * Destructor.
 */
Savegame::~Savegame(void) {

}

/**
 * Returns whether this is a new save.
 * @return true if there is no savegame file with this name yet
 */
bool Savegame::is_empty() {
  return empty;
}


/**
 * Saves the data into a file.
 */
void Savegame::save(void) {

  FILE *file = fopen(file_name, "w");

  if (file == NULL) {
    cerr << "Cannot write savegame file '" << file_name << "'" << endl;
    exit(1);
  }

  fwrite(&saved_data, sizeof(SavedData), 1, file);

}

/** Returns the name of the player. */
const char * Savegame::get_player_name(void) {
  return saved_data.reserved_strings[0];
}

/** Sets the name of the player. */
void Savegame::set_player_name(const char *player_name) {
  strncpy(saved_data.reserved_strings[0], player_name, 32);
}

/** Returns the id of the map to start when the savegame is loaded. */
MapId Savegame::get_starting_map(void) {
  return saved_data.reserved_integers[0];
}

/** Sets the map to start when the save game is loaded. */
void Savegame::set_starting_map(MapId mapId) {
  saved_data.reserved_integers[0] = mapId;
}

/** Returns the index of the entrance where the player starts. */
Uint32 Savegame::get_starting_entrance(void) {
  return saved_data.reserved_integers[1];
}

/** Sets the index of the entrance where the player starts. */
void Savegame::set_starting_entrance(Uint32 entrance_index) {
  saved_data.reserved_integers[1] = entrance_index;
}

/**
 * Returns a custom integer value saved.
 * @param index index of the value to get, between 0 and 1023
 * @return the integer value saved at this index
 */
Uint32 Savegame::get_integer(int index) {
  return saved_data.custom_integers[index];
}

/**
 * Sets a custom integer value int the savegame.
 * @param index index of the value to set, between 0 and 1023
 * @param value the integer value to store at this index
 */
void Savegame::set_integer(int index, Uint32 value) {
  saved_data.custom_integers[index] = value;
}

/**
 * Returns a boolean value saved.
 * @param index index of the value to get, between 0 and 32767
 * @return the boolean value saved at this index
 */
bool Savegame::get_boolean(int index) {

  Uint32 word = saved_data.custom_booleans[index / 32];

  //  cout << "getting value at index " << index << ": " << ((word >> (index % 32)) & 0x0001 != 0x0000) << endl;

  return (word >> (index % 32)) & 0x0001 != 0x0000;
}

/**
 * Sets a boolean value in the savegame.
 * @param index index of the value to set, between 0 and 32767
 * @param value the boolean value to store at this index
 */
void Savegame::set_boolean(int index, bool value) {

  Uint32 mask = (value ? 0x0001 : 0x0000) << (index % 32);
  saved_data.custom_booleans[index / 32] |= mask;

  //  cout << "setting value " << value << " at index " << index << endl;
}
