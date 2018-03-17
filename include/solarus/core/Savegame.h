/*
 * Copyright (C) 2006-2018 Christopho, Solarus - http://www.solarus-games.org
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

#include "solarus/core/Common.h"
#include "solarus/core/Equipment.h"
#include "solarus/lua/ExportableToLua.h"
#include <map>
#include <string>

struct lua_State;

namespace Solarus {

class LuaContext;
class MainLoop;

/**
 * \brief Manages the game data saved.
 *
 * This class provides read and write access to the saved data.
 */
class SOLARUS_API Savegame: public ExportableToLua {

  public:

    static const int SAVEGAME_VERSION;  /**< Version number of the savegame file format. */

    // Keys to built-in values saved.
    static const std::string KEY_SAVEGAME_VERSION;
    static const std::string KEY_STARTING_MAP;
    static const std::string KEY_STARTING_POINT;
    static const std::string KEY_KEYBOARD_ACTION;
    static const std::string KEY_KEYBOARD_ATTACK;
    static const std::string KEY_KEYBOARD_ITEM_1;
    static const std::string KEY_KEYBOARD_ITEM_2;
    static const std::string KEY_KEYBOARD_PAUSE;
    static const std::string KEY_KEYBOARD_RIGHT;
    static const std::string KEY_KEYBOARD_UP;
    static const std::string KEY_KEYBOARD_LEFT;
    static const std::string KEY_KEYBOARD_DOWN;
    static const std::string KEY_JOYPAD_ACTION;
    static const std::string KEY_JOYPAD_ATTACK;
    static const std::string KEY_JOYPAD_ITEM_1;
    static const std::string KEY_JOYPAD_ITEM_2;
    static const std::string KEY_JOYPAD_PAUSE;
    static const std::string KEY_JOYPAD_RIGHT;
    static const std::string KEY_JOYPAD_UP;
    static const std::string KEY_JOYPAD_LEFT;
    static const std::string KEY_JOYPAD_DOWN;
    static const std::string KEY_CURRENT_LIFE;
    static const std::string KEY_CURRENT_MONEY;
    static const std::string KEY_CURRENT_MAGIC;
    static const std::string KEY_MAX_LIFE;
    static const std::string KEY_MAX_MONEY;
    static const std::string KEY_MAX_MAGIC;
    static const std::string KEY_ITEM_SLOT_1;
    static const std::string KEY_ITEM_SLOT_2;
    static const std::string KEY_ABILITY_TUNIC;
    static const std::string KEY_ABILITY_SWORD;
    static const std::string KEY_ABILITY_SWORD_KNOWLEDGE;
    static const std::string KEY_ABILITY_SHIELD;
    static const std::string KEY_ABILITY_LIFT;
    static const std::string KEY_ABILITY_SWIM;
    static const std::string KEY_ABILITY_JUMP_OVER_WATER;
    static const std::string KEY_ABILITY_RUN;
    static const std::string KEY_ABILITY_PUSH;
    static const std::string KEY_ABILITY_GRAB;
    static const std::string KEY_ABILITY_PULL;
    static const std::string KEY_ABILITY_DETECT_WEAK_WALLS;
    static const std::string KEY_ABILITY_GET_BACK_FROM_DEATH;

    // creation and destruction
    Savegame(MainLoop& main_loop, const std::string& file_name);

    // file state
    bool is_empty() const;
    void initialize();
    void save();
    const std::string& get_file_name() const;

    // data
    bool is_string(const std::string& key) const;
    std::string get_string(const std::string& key) const;
    void set_string(const std::string& key, const std::string& value);
    bool is_integer(const std::string& key) const;
    int get_integer(const std::string& key) const;
    void set_integer(const std::string& key, int value);
    bool is_boolean(const std::string& key) const;
    bool get_boolean(const std::string& key) const;
    void set_boolean(const std::string& key, bool value);
    bool is_set(const std::string& key) const;
    void unset(const std::string& key);

    void set_initial_values();
    void set_default_keyboard_controls();
    void set_default_joypad_controls();
    void post_process_existing_savegame();

    // unsaved data
    MainLoop& get_main_loop();
    LuaContext& get_lua_context();
    const Equipment& get_equipment() const;
    Equipment& get_equipment();
    const Game* get_game() const;
    Game* get_game();
    void set_game(Game* game);
    void notify_game_started();
    void notify_game_finished();

    virtual const std::string& get_lua_type_name() const override;

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
    std::string file_name;   /**< Savegame file name relative to the quest write directory. */
    MainLoop& main_loop;
    Equipment equipment;
    Game* game;              /**< nullptr if this savegame is not currently running */

    void import_from_file();
    static int l_newindex(lua_State* l);

};

}

#endif

