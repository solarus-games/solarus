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
#ifndef ZSDX_SAVEGAME_H
#define ZSDX_SAVEGAME_H

#include "Common.h"

/**
 * This class handles the game data saved.
 */
class Savegame {

  /**
   * This structure contains the data saved (16 Ko of data are stored).
   * The system can save some strings, integers and boolean values.
   * See StringIndex and IntegerIndex for the meaning of each string and integer used.
   * The engine uses strings and integers. The booleans are only used the maps.
   */
  typedef struct SavedData {
    char strings[64][64];     /**< 64 NULL-terminated strings of 64 characters each (4 Ko) */
    uint32_t integers[2048];  /**< 2048 integers (8 Ko) */
    uint32_t booleans[1024];  /**< 32768 boolean values (4 Ko) */

  } SavedData;

  public:

  /**
   * Index of each string saved in the file.
   * Do not change these numbers, otherwise you might break
   * the existing savegames.
   * Values before 63 are used by the engine. The map only have a read-only access to them.
   * Values above 64 are available for the maps in reading and writing.
   */
  enum StringIndex {
    PLAYER_NAME                     = 0,
    STARTING_POINT                  = 1, /**< the player appears on this destination point on the starting map */

    /**
     * @name Joypad customizable controls.
     * Variables 1 to 9 indicate the SDL joypad action
     * associated to each game key: action, sword, item 1, item 2, pause,
     * right, up, left and down.
     * Examples: "button 1", "axis 1 +", "hat 1 left"
     * @{
     */
    JOYPAD_ACTION_KEY              = 10,
    JOYPAD_SWORD_KEY               = 11,
    JOYPAD_ITEM_1_KEY              = 12,
    JOYPAD_ITEM_2_KEY              = 13,
    JOYPAD_PAUSE_KEY               = 14,
    JOYPAD_RIGHT_KEY               = 15,
    JOYPAD_UP_KEY                  = 16,
    JOYPAD_LEFT_KEY                = 17,
    JOYPAD_DOWN_KEY                = 18,
    /**
     * @}
     */

    // values above 32 are available to the maps
  };

  /**
   * Index of each integer saved in the file.
   * Do not change these numbers, otherwise you might break
   * the existing savegames.
   * Values before 1023 are used by the engine. The map only have a read-only access to them.
   * Values above 1024 are available for the maps in reading and writing.
   */
  enum IntegerIndex {

    /**
     * @name Last game status
     * @{
     */
    STARTING_MAP                     = 0, /**< when starting the game, this map is loaded */
    // 1: unused
    PAUSE_LAST_SUBMENU               = 2, /**< last submenu shown in the pause menu */
    INVENTORY_LAST_ROW               = 3, /**< row of the last item selected in the inventory */
    INVENTORY_LAST_COLUMN            = 4, /**< column of the last item selected in the inventory */
    ITEM_SLOT_0                      = 5, /**< current object associated to the first item slot
					   * (X button by default), -1 means no item */
    ITEM_SLOT_1                      = 6, /**< current object associated to the second slot
					   * (V button by default), -1 means no item */
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
    MAX_RUPEES                       = 21, /**< maximum number of rupees (100, 300 or 999) */
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
    HERO_TUNIC                       = 31, /**< the tunic (0: green, 1: blue, 2: red) */
    HERO_SHIELD                      = 32, /**< the shield (0: no shield, 1 to 3: shields 1 to 3) */
    HERO_SWORD                       = 33, /**< the sword (0: no sword, 1 to 4: swords 1 to 4) */
    WORLD_MAP                        = 34, /**< has the player got the world map? (0: no, 1: yes) */
    /**
     * @}
     */

    /**
     * @name Keyboard customizable keys.
     * Variables 35 to 43 indicate the keyboard key
     * associated to each game key: action, sword, item 1, item 2, pause,
     * right, up, left and down.
     *
     * Each integer corresponds to a value of the SDLKey enumeration.
     * This makes the savegames SDL-dependent.
     * TODO: make our own enumeration to replace SDLKey.
     * Add a saved integer to define the type of key saved (SDL: 0, ZSDX: 1).
     * When loading the savegame, if this is SDL, replace by ZSDX and reset to default keys.
     * @{
     */
    KEYBOARD_ACTION_KEY              = 35,
    KEYBOARD_SWORD_KEY               = 36,
    KEYBOARD_ITEM_1_KEY              = 37,
    KEYBOARD_ITEM_2_KEY              = 38,
    KEYBOARD_PAUSE_KEY               = 39,
    KEYBOARD_RIGHT_KEY               = 40,
    KEYBOARD_UP_KEY                  = 41,
    KEYBOARD_LEFT_KEY                = 42,
    KEYBOARD_DOWN_KEY                = 43,
    /**
     * @}
     */

    /**
     * @name Items of the inventory
     * Variables 100 to 127 indicate whether the player has got each inventory item
     * (see enum InventoryItemId to know the item numbers).
     * Each value saved here corresponds to a slot in the inventory.
     * Zero indicates that the player does not have the item
     * of this slot.
     * Some slots can contains several items during the game:
     * the current variant of the item is then indicated by the value saved
     * (1 for the first variant, 2 for the second one, etc.).
     * @{
     */
    FIRST_INVENTORY_ITEM             = 100, /**< 0 if the player does not have item #0, 1 if he has
					     * its first variant, 2 if he has the second one, etc. */
    LAST_INVENTORY_ITEM              = 127, /**< same thing for item #27 */
    EXTENSION_INVENTORY_ITEM         = 199, /**< variables 128 to 199 are reserved for possible
					     * additional items in the future */
    /**
     * @}
     */

    /**
     * @name Dungeon items
     * For each dungeon, 10 variables are reserved:
     * - the map,
     * - the compass,
     * - the small keys,
     * - the big key,
     * - the boss key,
     * - whether the dungeon is finished
     * and 4 unused variables for future extension.
     * Up to 20 dungeons can be saved. Thus, 200 variables are used for the dungeon states (20*10).
     */
    FIRST_DUNGEON_MAP                = 200, /**< 1 if the player has the map of dungeon #1 */
    FIRST_DUNGEON_COMPASS            = 201, /**< 1 if the player has the map of dungeon #1 */
    FIRST_DUNGEON_BIG_KEY            = 202, /**< 1 if the player has the big key of dungeon #1 */
    FIRST_DUNGEON_BOSS_KEY           = 203, /**< 1 if the player has the boss key of dungeon #1 */
    FIRST_DUNGEON_SMALL_KEYS         = 204, /**< number of small keys found in dungeon #1 */
    FIRST_DUNGEON_FINISHED           = 205, /**< 1 if the player has finished dungeon #1 (unused for now) */
    FIRST_DUNGEON_UNUSED_1           = 206, /**< empty place for future new data in dungeon #1 */
    FIRST_DUNGEON_UNUSED_2           = 207, /**< empty place for future new data in dungeon #1 */
    FIRST_DUNGEON_UNUSED_3           = 208, /**< empty place for future new data in dungeon #1 */
    FIRST_DUNGEON_UNUSED_4           = 209, /**< empty place for future new data in dungeon #1 */
    // then, same thing for other dungeons
    LAST_DUNGEON_UNUSED_4            = 399, /**< empty place for future new data in dungeon #20 */

    // values above 1024 are available to the maps
  };

  private:

  bool empty;
  std::string file_name;
  SavedData saved_data;

  DungeonEquipment *dungeon_equipment;
  Equipment *equipment;

  void set_initial_values(void);

  public:

  // creation and destruction
  Savegame(const std::string &file_name);
  Savegame(Savegame *other);
  ~Savegame(void);

  // file state
  bool is_empty(void);
  void save(void);
  const std::string& get_file_name(void);

  // data
  const std::string get_string(int index);
  void set_string(int index, const std::string &value);

  uint32_t get_integer(int index);
  void set_integer(int index, uint32_t value);

  bool get_boolean(int index);
  void set_boolean(int index, bool value);

  // unsaved data
  Equipment *get_equipment(void);
  DungeonEquipment *get_dungeon_equipment(void);
};

#endif

