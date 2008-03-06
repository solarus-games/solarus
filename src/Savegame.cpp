/**
 * This module implements the class Savegame.
 */

#include "Savegame.h"
  
/**
 * Constructor.
 * Initializes all values at zero.
 * @param player_name name of the player (32 characters maxi)
 */
Savegame::Savegame(const char *player_name) {
  strncpy(this->player_name, player_name, 32);
  memset(this, 0x0000, sizeof(Savegame));
}

/**
 * Destructor.
 */
Savegame::~Savegame(void) {

}


/**
 * Loads the saved data from a file.
 */
void Savegame::load(char *file_name) {

  FILE *file = fopen(file_name, "r");

  if (file == NULL) {
    cerr << "Cannot read savegame file '" << file_name << "'" << endl;
    exit(1);
  }

  fread(this, sizeof(Savegame), 1, file);
}

/**
 * Saves the data into a file.
 */
void Savegame::save(char *file_name) {

  FILE *file = fopen(file_name, "w");

  if (file == NULL) {
    cerr << "Cannot write savegame file '" << file_name << "'" << endl;
    exit(1);
  }

  fwrite(this, sizeof(Savegame), 1, file);

}

/**
 * Returns the name of the player.
 * @return the name of the player
 */
const char * Savegame::get_player_name(void) {
  return player_name;
}

/**
 * Returns an integer value saved.
 * @param index index of the value to get, between 0 and 1023
 * @return the integer value saved at this index
 */
Uint32 Savegame::get_int(int index) {
  return int_values[index];
}

/**
 * Sets an integer value int the savegame.
 * @param index index of the value to set, between 0 and 1023
 * @param value the integer value to store at this index
 */
void Savegame::set_int(int index, Uint32 value) {
  int_values[index] = value;
}

/**
 * Returns a boolean value saved.
 * @param index index of the value to get, between 0 and 32767
 * @return the boolean value saved at this index
 */
bool Savegame::get_bool(int index) {

  Uint32 word = bool_values[index / 32];
  cout << "getting value at index " << index << ": " << ((word >> (index % 32)) & 0x0001 != 0x0000) << endl;

  return (word >> (index % 32)) & 0x0001 != 0x0000;
}

/**
 * Sets a boolean value in the savegame.
 * @param index index of the value to set, between 0 and 32767
 * @param value the boolean value to store at this index
 */
void Savegame::set_bool(int index, bool value) {

  Uint32 mask = (value ? 0x0001 : 0x0000) << (index % 32);
  bool_values[index / 32] |= mask;

  cout << "setting value " << value << " at index " << index << endl;
}
