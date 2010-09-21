/*
 * Copyright (C) 2009 Christopho, Solarus - http://www.solarus-engine.org
 * 
 * Solarus is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Solarus is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef SOLARUS_SAVEGAME_H
#define SOLARUS_SAVEGAME_H

#include "Common.h"

/**
 * @brief Manages the game data saved.
 *
 * This class provides read and write access to the saved data.
 */
class Savegame {

  private:

    /**
     * @brief Contains the data saved.
     *
     * This structure contains the data saved (16 Ko of data are stored).
     * The system can save some strings, integers and boolean values.
     * See StringIndex and IntegerIndex for the meaning of each string and integer used.
     * The engine uses strings and integers. The booleans are only used the maps.
     */
    typedef struct SavedData {
      char strings[64][64];		/**< 64 NULL-terminated strings of 64 bytes each (4 Ko) */
      uint32_t integers[2048];		/**< 2048 integers (8 Ko) */
      uint32_t booleans[1024];		/**< 32768 boolean values (4 Ko) */

    } SavedData;

  public:

    /**
     * @brief Index of each string saved in the file.
     *
     * Do not change these numbers, otherwise you might break
     * the existing savegames.
     * Values before 63 are used by the engine. The map only have a read-only access to them.
     * Values above 64 are available for the maps in reading and writing.
     */
    enum StringIndex {
      PLAYER_NAME			= 0,
      STARTING_POINT			= 1,		/**< the player appears on this destination point on the starting map */

      ITEM_SLOT_0			= 2,		/**< current item associated to the first item slot
							 * (X button by default), an empty string means no item */
      ITEM_SLOT_1			= 3,		/**< current item associated to the second slot
							 * (V button by default), an empty string means no item */

      /**
       * @name Joypad customizable controls.
       * Variables 1 to 9 indicate the joypad event
       * of each game key: action, sword, item 1, item 2, pause,
       * right, up, left and down.
       * Examples: "button 1", "axis 1 +", "hat 1 left"
       * @{
       */
      JOYPAD_ACTION_KEY			= 10,
      JOYPAD_SWORD_KEY			= 11,
      JOYPAD_ITEM_1_KEY			= 12,
      JOYPAD_ITEM_2_KEY			= 13,
      JOYPAD_PAUSE_KEY			= 14,
      JOYPAD_RIGHT_KEY			= 15,
      JOYPAD_UP_KEY			= 16,
      JOYPAD_LEFT_KEY			= 17,
      JOYPAD_DOWN_KEY			= 18
      /**
       * @}
       */

      // values above 32 are available to the maps
    };

    /**
     * @brief Index of each integer saved in the file.
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
      STARTING_MAP			= 0,		/**< when starting the game, this map is loaded */
      PAUSE_LAST_SUBMENU		= 1,		/**< last submenu shown in the pause menu */
      INVENTORY_LAST_ITEM_INDEX		= 2,		/**< index of the last item selected in the inventory */
      /**
       * @}
       */

      /**
       * @name Current value of rupees, hearts and others
       * @{
       */
      CURRENT_LIFE			= 10,		/**< current level of life */
      CURRENT_MONEY			= 11,		/**< current amount of money */
      CURRENT_MAGIC			= 12,		/**< current level of magic */
      /**
       * @}
       */

      /**
       * @name Maximum values
       * @{
       */
      MAX_LIFE				= 20,		/**< maximum level of life */
      MAX_MONEY				= 21,		/**< maximum amount of money */
      MAX_MAGIC				= 22,		/**< maximum level of magic */
      /**
       * @}
       */

      /**
       * @name Keyboard customizable keys.
       * Variables 35 to 49 indicate the keyboard key
       * associated to each game key: action, sword, item 1, item 2, pause,
       * right, up, left and down.
       * Each integer corresponds to a value of the Input::KeyboardKey enumeration.
       * @{
       */
      KEYBOARD_ACTION_KEY		= 35,
      KEYBOARD_SWORD_KEY		= 36,
      KEYBOARD_ITEM_1_KEY		= 37,
      KEYBOARD_ITEM_2_KEY		= 38,
      KEYBOARD_PAUSE_KEY		= 39,
      KEYBOARD_RIGHT_KEY		= 40,
      KEYBOARD_UP_KEY			= 41,
      KEYBOARD_LEFT_KEY			= 42,
      KEYBOARD_DOWN_KEY			= 43,
      KEYBOARD_ENUM_VERSION		= 49,		/**< Indicates the version of the Input::KeyboardKey enumeration this savegame was created with.
							 * If the version saved is different from Input::KEYBOARD_ENUM_VERSION, then
							 * we know it is obsolete and the customization is reset to some default values. */
      /**
       * @}
       */

      /**
       * @name General abilities.
       * @{
       */
      ABILITY_TUNIC				= 50,
      ABILITY_SWORD				= 51,
      ABILITY_SHIELD				= 52,
      ABILITY_LIFT				= 53,
      ABILITY_SWIM				= 54,
      ABILITY_SEE_IN_DARK			= 55,
      ABILITY_DETECT_WEAK_WALLS			= 56,
      ABILITY_SEE_OUTSIDE_WORLD_MINIMAP		= 57,
      /**
       * @}
       */

      /**
       * @name Dungeon-specific abilities and information.
       * Up to 40 dungeons can be saved.
       */
      DUNGEON_1_FINISHED			= 200,
      DUNGEON_1_ABILITY_SEE_MINIMAP_ROOMS	= 201,
      DUNGEON_1_ABILITY_SEE_MINIMAP_ELEMENTS	= 202,
      DUNGEON_1_ABILITY_OPEN_BIG_LOCKS		= 203,
      DUNGEON_1_ABILITY_OPEN_BOSS_LOCK		= 204,
      DUNGEON_1_SMALL_KEYS			= 205,		/**< small key counter in dungeon #1 */
      DUNGEON_1_UNUSED_1			= 206,		/**< empty place for future new data in dungeon #1 */
      DUNGEON_1_UNUSED_2			= 207,		/**< empty place for future new data in dungeon #1 */
      DUNGEON_1_UNUSED_3			= 208,		/**< empty place for future new data in dungeon #1 */
      DUNGEON_1_UNUSED_4			= 209,		/**< empty place for future new data in dungeon #1 */
      // then, same thing for other dungeons
      DUNGEON_40_UNUSED_4			= 599,		/**< empty place for future new data in dungeon #40 */


      SAVEGAME_COMPATIBILITY_FORMAT		= 1023		/**< Indicates the version of the savegame format this savegame was created with.
								 * If the version saved is different from COMPATIBILITY_FORMAT, then
								 * we know it is obsolete (not compatible anymore) and the savegame is destroyed. */

      // values above 1024 are available to the maps
    };

    static const unsigned int CURRENT_COMPATIBILITY_FORMAT = 1;	/**< version of the savegame format currently recognized by the engine
								 * (only savegames created with this format can be loaded,
								 * the old ones are obsolete and are destroyed) */

  private:

    bool empty;
    std::string file_name;
    SavedData saved_data;

    Equipment *equipment;

    void set_initial_values();
    void set_default_keyboard_controls();
    void set_default_joypad_controls();
    void check_game_controls();

  public:

    // creation and destruction
    Savegame(const std::string &file_name);
    Savegame(Savegame *other);
    ~Savegame();

    // file state
    bool is_empty();
    void save();
    const std::string& get_file_name();

    // data
    const std::string get_string(int index);
    void set_string(int index, const std::string &value);

    uint32_t get_integer(int index);
    void set_integer(int index, uint32_t value);

    bool get_boolean(int index);
    void set_boolean(int index, bool value);

    // unsaved data
    Equipment *get_equipment();
};

#endif

