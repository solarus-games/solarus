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
  SAVEGAME_CURRENT_MAGIC                    = 12, /**< current level of magic (0 to 84) */
  SAVEGAME_CURRENT_BOMBS                    = 13, /**< current number of bombs */
  SAVEGAME_CURRENT_ARROWS                   = 14, /**< current number of arrows */
  SAVEGAME_CURRENT_PAINS_AU_CHOCOLAT        = 15, /**< current number of pains au chocolat */
  SAVEGAME_CURRENT_CROISSANTS               = 16, /**< current number of croissants */
  SAVEGAME_CURRENT_APPLES                   = 17, /**< current number of apples (0 to 10) */
  SAVEGAME_NB_FIRE_STONES                   = 18, /**< number of fire stones already found (0 to 3) */
  /**
   * @}
   */

  /**
   * @name Maximum values
   * @{
   */
  SAVEGAME_MAX_HEARTS                       = 20, /**< maximum number of hearts (counted in entire hearts) */
  SAVEGAME_MAX_RUPEES                       = 21, /**< maximum number of rupees (99, 199 or 999) */
  SAVEGAME_MAX_MAGIC                        = 22, /**< maximum level of magic (0, 42 or 84 points) */
  SAVEGAME_MAX_BOMBS                        = 23, /**< maximum number of bombs (0, 30 or 50) */
  SAVEGAME_MAX_ARROWS                       = 24, /**< maximum number of arrows (0, 30 or 50) */
  /**
   * @}
   */

  /**
   * @name Equipment and quest status
   * @{
   */
  SAVEGAME_PIECES_OF_HEART                  = 30, /**< current pieces of heart number (0 to 3) */
  SAVEGAME_LINK_TUNIC                       = 31, /**< Link's tunic (0: green, 1: blue, 2: red) */
  SAVEGAME_LINK_SHIELD                      = 32, /**< Link's shield (0: no shield, 1 to 3: shields 1 to 3) */
  SAVEGAME_LINK_SWORD                       = 33, /**< Link's sword (0: no sword, 1 to 4: swords 1 to 4) */
  SAVEGAME_WORLD_MAP                        = 34, /**< has Link got the world map (0: no, 1: yes) */
  /**
   * @}
   */

  /**
   * @name Pieces of heart
   * The variables 100 to 143 indicate whether the player has found each piece of heart.
   * There are 44 pieces of hearts in Mystery of Solarus DX.
   * @{
   */
  SAVEGAME_FIRST_PIECE_OF_HEART             = 100, /**< 1 if the player has the piece of heart #0, 0 otherwise */
  SAVEGAME_LAST_PIECE_OF_HEART              = 143, /**< 1 if the player has the piece of heart #43, 0 otherwise */
  SAVEGAME_EXTENSION_PIECE_OF_HEART         = 199, /**< variables 144 to 199 are reserved for possible
						     * additional pieces of heart in the future */

  /**
   * @}
   */

  /**
   * @name Items of the inventory
   * Variables 200 to 227 indicate whether the player has got each inventory item
   * (see enum InventoryItem::ItemId to know the item numbers).
   * Each value saved here corresponds to a slot in the inventory.
   * Zero indicates that the player does not have the item
   * of this slot.
   * Some slots can contains several items during the game:
   * the current variant of the item is then indicated by the value saved
   * (1 for the first variant, 2 for the second one, etc.).
   * @{
   */
  SAVEGAME_FIRST_INVENTORY_ITEM             = 200, /**< 0 if the player does not have item #0, 1 if he has
						    * its first variant, 2 if he has the second one, etc. */
  SAVEGAME_LAST_INVENTORY_ITEM              = 227, /**< same thing for item #27 */
  SAVEGAME_EXTENSION_INVENTORY_ITEM         = 299, /**< variables 228 to 299 are reserved for possible
						    * additional items in the future */
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
   * See SavegameReservedStringIndex and SavegameReservedIntegerIndex
   * for the meaning of each string and integer used.
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

  Equipment *equipment;

  void set_default_values(void);

 public:
  
  Savegame(const char *file_name);
  ~Savegame(void);

  bool is_empty(void);
  void save(void);
  const char *get_file_name(void);

  // engine data
  const char *get_reserved_string(int index);
  void set_reserved_string(int index, const char *string);

  Uint32 get_reserved_integer(int index);
  void set_reserved_integer(int index, Uint32 value);
  
  // custom project data
  Uint32 get_integer(int index);
  void set_integer(int index, Uint32 value);

  bool get_boolean(int index);
  void set_boolean(int index, bool value);

  // not saved data
  Equipment *get_equipment(void);
};

#endif
