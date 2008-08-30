#ifndef ZSDX_SAVEGAME_H
#define ZSDX_SAVEGAME_H

#include "Common.h"

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

 public:

  /**
   * Index of each reserved string saved in the file.
   * Do not change these numbers, otherwise you might break
   * the existing savegames.
   */
  enum ReservedStringIndex {
    PLAYER_NAME                 = 0,
  };

  /**
   * Index of each reserved integer saved in the file.
   * Do not change these numbers, otherwise you might break
   * the existing savegames.
   */
  enum ReservedIntegerIndex {

    /**
     * @name Last game status
     * @{
     */
    STARTING_MAP                     = 0, /**< when starting the game, this map is loaded */
    STARTING_ENTRANCE                = 1, /**< the player appears on this entrance on the starting map */
    PAUSE_LAST_SCREEN                = 2, /**< last screen shown in the pause menu */
    INVENTORY_LAST_ROW               = 3, /**< row of the last item selected in the inventory */
    INVENTORY_LAST_COLUMN            = 4, /**< column of the last item selected in the inventory */
    ITEM_SLOT_0                      = 5, /**< current object associated to the first item slot
					   * (X button by default), -1 means no item */
    ITEM_SLOT_1                      = 6, /**< current object associated to the second slot
					   * (V button by default) */
    /**
     * @}
     */

    /**
     * @name Current value of rupees, hearts and others
     * @{
     */
    CURRENT_HEARTS                   = 10, /**< current number of hearts filled (counted in 1/4 of hearts) */
    CURRENT_RUPEES                   = 11, /**< current number of rupees */
    CURRENT_MAGIC                    = 12, /**< current level of magic (0 to 84) */
    CURRENT_BOMBS                    = 13, /**< current number of bombs */
    CURRENT_ARROWS                   = 14, /**< current number of arrows */
    CURRENT_PAINS_AU_CHOCOLAT        = 15, /**< current number of pains au chocolat (0 to 10) */
    CURRENT_CROISSANTS               = 16, /**< current number of croissants (0 to 10) */
    CURRENT_APPLES                   = 17, /**< current number of apples (0 to 10) */
    NB_FIRE_STONES                   = 18, /**< number of fire stones already found (0 to 3) */
    /**
     * @}
     */

    /**
     * @name Maximum values
     * @{
     */
    MAX_HEARTS                       = 20, /**< maximum number of hearts (counted in entire hearts) */
    MAX_RUPEES                       = 21, /**< maximum number of rupees (99, 199 or 999) */
    MAX_MAGIC                        = 22, /**< maximum level of magic (0, 42 or 84 points) */
    MAX_BOMBS                        = 23, /**< maximum number of bombs (0, 10, 30 or 99) */
    MAX_ARROWS                       = 24, /**< maximum number of arrows (0, 10, 30 or 99) */
    /**
     * @}
     */

    /**
     * @name Equipment and quest status
     * @{
     */
    PIECES_OF_HEART                  = 30, /**< current pieces of heart number (0 to 3) */
    LINK_TUNIC                       = 31, /**< Link's tunic (0: green, 1: blue, 2: red) */
    LINK_SHIELD                      = 32, /**< Link's shield (0: no shield, 1 to 3: shields 1 to 3) */
    LINK_SWORD                       = 33, /**< Link's sword (0: no sword, 1 to 4: swords 1 to 4) */
    WORLD_MAP                        = 34, /**< has Link got the world map (0: no, 1: yes) */
    /**
     * @}
     */

    /**
     * @name Pieces of heart
     * TODO useless?
     * The variables 100 to 143 indicate whether the player has found each piece of heart.
     * There are 44 pieces of hearts in Mystery of Solarus DX.
     * @{
     */
    FIRST_PIECE_OF_HEART             = 100, /**< 1 if the player has the piece of heart #0, 0 otherwise */
    LAST_PIECE_OF_HEART              = 143, /**< 1 if the player has the piece of heart #43, 0 otherwise */
    EXTENSION_PIECE_OF_HEART         = 199, /**< variables 144 to 199 are reserved for possible
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
    FIRST_INVENTORY_ITEM             = 200, /**< 0 if the player does not have item #0, 1 if he has
					     * its first variant, 2 if he has the second one, etc. */
    LAST_INVENTORY_ITEM              = 227, /**< same thing for item #27 */
    EXTENSION_INVENTORY_ITEM         = 299, /**< variables 228 to 299 are reserved for possible
					     * additional items in the future */
    /**
     * @}
     */

    // TODO dungeon items: 7 variables per dungeon (5 used, 2 free) and 14 potential dungeons: 98 values
  };

 private:

  bool empty;
  char file_name[32];
  SavedData saved_data;

  Equipment *equipment;

  void set_initial_values(void);

 public:
  
  // creation and destruction
  Savegame(const char *file_name);
  ~Savegame(void);

  // file state
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

  // unsaved data
  Equipment *get_equipment(void);
};

#endif
