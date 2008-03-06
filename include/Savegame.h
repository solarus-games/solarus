#ifndef ZSDX_SAVEGAME_H
#define ZSDX_SAVEGAME_H

#include "Common.h"

typedef struct SavedData {

  /**
   * Name of the player.
   */
  char player_name[32];

  /**
   * Data reserved for the game engine.
   * See Savegame.cpp for the meaning of each integer.
   */
  Uint32 reserved_data[1024];

  /**
   * Custom data available to the maps of the project.
   */
  Uint32 int_values[1024]; // 1024 integers
  Uint32 bool_values[2048]; // 65536 boolean values

} SavedData;

/**
 * This class contains the game data saved.
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

  // player name
  const char *get_player_name(void);
  void set_player_name(const char *player_name);

  // engine data
  MapId get_starting_map(void);
  void set_starting_map(MapId mapId);

  Uint32 get_starting_entrance(void);
  void set_starting_entrance(Uint32 entrance_index);

  // custom project data
  Uint32 get_int(int index);
  void set_int(int index, Uint32 value);

  bool get_bool(int index);
  void set_bool(int index, bool value);
};

#endif
