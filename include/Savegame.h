#ifndef ZSDX_SAVEGAME_H
#define ZSDX_SAVEGAME_H

#include "Common.h"

/**
 * Index of each reserved string saved in the file.
 * Do not change these numbers, otherwise you might break
 * the existing savegames.
 */
enum SavegameReservedStringIndex {
  SAVEGAME_PLAYER_NAME                 = 0,
};

/**
 * Index of each reserved integer saved in the file.
 * Do not change these numbers, otherwise you might break
 * the existing savegames.
 */
enum SavegameReservedIntegerIndex {

  /**
   * @name Last game status
   * @{
   */
  SAVEGAME_STARTING_MAP                     = 0, /**< when starting the game, this map is loaded */
  SAVEGAME_STARTING_ENTRANCE                = 1, /**< the player appears on this entrance on the starting map */
  SAVEGAME_PAUSE_LAST_SCREEN                = 2, /**< last screen shown in the pause menu */
  SAVEGAME_INVENTORY_LAST_ROW               = 3, /**< row of the last item selected in the inventory */
  SAVEGAME_INVENTORY_LAST_COLUMN            = 4, /**< column of the last item selected in the inventory */
  SAVEGAME_ITEM_X                           = 5, /**< current object associated to the X button */
  SAVEGAME_ITEM_V                           = 6, /**< current object associated to the Y button */
  /**
   * @}
   */

  /**
   * @name Current value of rupees, hearts and others
   * @{
   */
  SAVEGAME_CURRENT_HEARTS                   = 10, /**< current number of hearts filled (counted in 1/4 of hearts) */
  SAVEGAME_CURRENT_RUPEES                   = 11, /**< current number of rupees */
  SAVEGAME_CURRENT_MAGIC                    = 12, /**< current level of magic (0 to 28) */
  SAVEGAME_CURRENT_BOMBS                    = 13, /**< current number of bombs */
  SAVEGAME_CURRENT_ARROWS                   = 14, /**< current number of arrows */
  SAVEGAME_CURRENT_PAINS_AU_CHOCOLAT        = 15, /**< current number of pains au chocolat */
  SAVEGAME_CURRENT_CROISSANTS               = 16, /**< current number of croissants*/
  /**
   * @}
   */

  /**
   * @name Maximum values
   * @{
   */
  SAVEGAME_MAX_HEARTS                       = 20, /**< maximum number of hearts (counted in entire hearts) */
  SAVEGAME_MAX_RUPEES                       = 21, /**< maximum number of rupees */
  SAVEGAME_MAX_MAGIC                        = 22, /**< maximum level of magic (0: no magic, 1: 14 points, 2: 28 points) */
  SAVEGAME_MAX_BOMBS                        = 23, /**< maximum number of bombs */
  SAVEGAME_MAX_ARROWS                       = 24, /**< maximum number of arrows */
  /**
   * @}
   */

  /**
   * @name Equipment and quest status
   * @{
   */
  SAVEGAME_HEART_FRAGMENTS                  = 30, /**< number of heart fragments obtained (0 to 28) */
  SAVEGAME_LINK_TUNIC                       = 31, /**< Link's tunic (0: green, 1: blue, 2: red) */
  SAVEGAME_LINK_SHIELD                      = 32, /**< Link's shield (0: no shield, 1 to 3: shields 1 to 3) */
  SAVEGAME_LINK_SWORD                       = 33, /**< Link's sword (0: no sword, 1 to 4: swords 1 to 4) */
  /**
   * @}
   */
};

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
  const char *get_reserved_string(SavegameReservedStringIndex index);
  void set_reserved_string(SavegameReservedStringIndex index, const char *string);

  Uint32 get_reserved_integer(SavegameReservedIntegerIndex index);
  void set_reserved_integer(SavegameReservedIntegerIndex index, Uint32 value);
  
  // custom project data
  Uint32 get_integer(int index);
  void set_integer(int index, Uint32 value);

  bool get_boolean(int index);
  void set_boolean(int index, bool value);
};

#endif
