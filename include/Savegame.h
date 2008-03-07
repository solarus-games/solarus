#ifndef ZSDX_SAVEGAME_H
#define ZSDX_SAVEGAME_H

#include "Common.h"

/**
 * This structure contains the data saved (16 Ko of data are stored).
 */
typedef struct SavedData {

  /**
   * Data reserved for the game engine (the C++ code).
   * See Savegame.cpp for the meaning of each string and integer used.
   */
  char reserved_strings[64][64];   // 64 strings of 64 characters each (4 Ko)
  Uint32 reserved_integers[1024];  // 1024 integers (4 Ko)

  /**
   * Custom data available to the maps of the project.
   * The C++ code does not use it.
   */
  Uint32 custom_integers[1024];    // 1024 integers (4 Ko)
  Uint32 custom_booleans[1024];    // 32768 boolean values (4 Ko)

} SavedData;

/**
 * This class handles the game data saved.
 */
class Savegame {

 private:
  bool empty;
  char file_name[32];
  SavedData saved_data;

 public:
  
  Savegame(const char *file_name);
  ~Savegame(void);

  bool is_empty(void);
  void save(void);

  // engine data
  const char *get_player_name(void);
  void set_player_name(const char *player_name);

  MapId get_starting_map(void);
  void set_starting_map(MapId mapId);

  Uint32 get_starting_entrance(void);
  void set_starting_entrance(Uint32 entrance_index);

  // custom project data
  Uint32 get_integer(int index);
  void set_integer(int index, Uint32 value);

  bool get_boolean(int index);
  void set_boolean(int index, bool value);
};

#endif
