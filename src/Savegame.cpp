/**
 * This module implements the class Savegame.
 */

#include "Savegame.h"

/**
 * Creates a savegame with a specified file name, existing or not.
 * @param file_name name of the savegame file (can be a new file)
 */
Savegame::Savegame(const char *file_name) {
  strncpy(this->file_name, file_name, 32);

  FILE *file = fopen(file_name, "r");

  if (file == NULL) {
    // this save slot is free
    empty = true;
    set_default_values();
  }
  else {
    // a save already exists, let's load it
    empty = false;
    fread(&saved_data, sizeof(SavedData), 1, file);
    fclose(file);
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
 * Loads the default values.
 */
void Savegame::set_default_values(void) {
  memset(&saved_data, 0x0000, sizeof(SavedData));

  set_reserved_integer(SAVEGAME_MAX_HEARTS, 3);
  set_reserved_integer(SAVEGAME_CURRENT_HEARTS, 12);

  set_reserved_integer(SAVEGAME_MAX_RUPEES, 99);
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
  fclose(file);

  empty = false;
}

/**
 * Returns an engine string value saved.
 * @param index index of the value to get, between 0 and 63
 * @return the string value saved at this index
 */
const char * Savegame::get_reserved_string(SavegameReservedStringIndex index) {
  return saved_data.reserved_strings[index];
}

/**
 * Sets the name of the player.
 * @param index index of the value to set, between 0 and 63
 * @param value the string value to store at this index
 */
void Savegame::set_reserved_string(SavegameReservedStringIndex index, const char *value) {
  strncpy(saved_data.reserved_strings[index], value, 63);
}

/**
 * Returns a engine integer value saved.
 * @param index index of the value to get, between 0 and 1023
 * @return the integer value saved at this index
 */
Uint32 Savegame::get_reserved_integer(SavegameReservedIntegerIndex index) {
  return saved_data.reserved_integers[index];
}

/**
 * Sets a engine integer value int the savegame.
 * @param index index of the value to set, between 0 and 1023
 * @param value the integer value to store at this index
 */
void Savegame::set_reserved_integer(SavegameReservedIntegerIndex index, Uint32 value) {
  saved_data.reserved_integers[index] = value;
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
