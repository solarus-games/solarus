/*
 * Copyright (C) 2006-2012 Christopho, Solarus - http://www.solarus-games.org
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
#include "Equipment.h"
#include "lua/ExportableToLua.h"

struct lua_State;

/**
 * @brief Manages the game data saved.
 *
 * This class provides read and write access to the saved data.
 */
class Savegame: public ExportableToLua {

  public:

    static const int SAVEGAME_VERSION;

    // Keys to built-in values saved.
    static const std::string KEY_SAVEGAME_VERSION;
    static const std::string KEY_STARTING_MAP;
    static const std::string KEY_STARTING_POINT;
    static const std::string KEY_KEYBOARD_ACTION;
    static const std::string KEY_KEYBOARD_SWORD;
    static const std::string KEY_KEYBOARD_ITEM_1;
    static const std::string KEY_KEYBOARD_ITEM_2;
    static const std::string KEY_KEYBOARD_PAUSE;
    static const std::string KEY_KEYBOARD_RIGHT;
    static const std::string KEY_KEYBOARD_UP;
    static const std::string KEY_KEYBOARD_LEFT;
    static const std::string KEY_KEYBOARD_DOWN;
    static const std::string KEY_JOYPAD_ACTION;
    static const std::string KEY_JOYPAD_SWORD;
    static const std::string KEY_JOYPAD_ITEM_1;
    static const std::string KEY_JOYPAD_ITEM_2;
    static const std::string KEY_JOYPAD_PAUSE;
    static const std::string KEY_JOYPAD_RIGHT;
    static const std::string KEY_JOYPAD_UP;
    static const std::string KEY_JOYPAD_LEFT;
    static const std::string KEY_JOYPAD_DOWN;
    static const std::string KEY_EQUIPMENT_INITIALIZED;
    static const std::string KEY_CURRENT_LIFE;
    static const std::string KEY_CURRENT_MONEY;
    static const std::string KEY_CURRENT_MAGIC;
    static const std::string KEY_MAX_LIFE;
    static const std::string KEY_MAX_MONEY;
    static const std::string KEY_MAX_MAGIC;
    static const std::string KEY_ABILITY_TUNIC;
    static const std::string KEY_ABILITY_SWORD;
    static const std::string KEY_ABILITY_SHIELD;
    static const std::string KEY_ABILITY_LIFT;
    static const std::string KEY_ABILITY_SWIM;
    static const std::string KEY_ABILITY_SWORD_KNOWLEDGE;
    static const std::string KEY_ABILITY_DETECT_WEAK_WALLS;
    static const std::string KEY_ABILITY_SEE_OUTSIDE_WORLD_MINIMAP;
    static const std::string KEY_ABILITY_GET_BACK_FROM_DEATH;
    static const std::string KEY_ABILITY_RUN;

    // creation and destruction
    Savegame(const std::string& file_name);
    ~Savegame();

    // file state
    bool is_empty();
    void save();
    const std::string& get_file_name();

    // data
    bool is_string(const std::string& key);
    const std::string& get_string(const std::string& key);
    void set_string(const std::string& key, const std::string& value);

    bool is_integer(const std::string& key);
    int get_integer(const std::string& key);
    void set_integer(const std::string& key, int value);

    bool is_boolean(const std::string& key);
    bool get_boolean(const std::string& key);
    void set_boolean(const std::string& key, bool value);

    // unsaved data
    Equipment& get_equipment();
    Game* get_game();
    void set_game(Game* game);

    virtual const std::string& get_lua_type_name() const;

  private:

    struct SavedValue {

      enum {
        VALUE_STRING,
        VALUE_INTEGER,
        VALUE_BOOLEAN
      } type;

      std::string string_data;
      int int_data;  // Also used for boolean
    };

    std::map<std::string, SavedValue> saved_values;

    bool empty;
    std::string file_name;
    Equipment equipment;
    Game* game;             /**< NULL if this savegame is not currently running */

    void load();
    static int l_newindex(lua_State* l);

    void set_initial_values();
    void set_default_keyboard_controls();
    void set_default_joypad_controls();
};

#endif

