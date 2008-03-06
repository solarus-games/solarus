#ifndef ZSDX_SAVEGAME_H
#define ZSDX_SAVEGAME_H

#include "Common.h"

/**
 * This class contains the game data saved.
 */
class Savegame {

 private:

  char player_name[32];
  Uint32 int_values[1024];
  Uint32 bool_values[1024];

 public:
  
  Savegame(const char *player_name);
  ~Savegame(void);

  void load(char *file_name);
  void save(char *file_name);

  const char *get_player_name(void);

  Uint32 get_int(int index);
  void set_int(int index, Uint32 value);

  bool get_bool(int index);
  void set_bool(int index, bool value);
};

#endif
