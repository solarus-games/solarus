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
#ifndef SOLARUS_SCRIPT_H
#define SOLARUS_SCRIPT_H

#include "Common.h"
#include "Treasure.h"
#include "InventoryItemId.h"
#include <list>

struct lua_State;

/**
 * @brief Abstract class for making an interface between the engine C++ code and a Lua script.
 */
class Script {

  protected:

    typedef int (FunctionAvailableToScript) (lua_State *l); /**< type of the functions that can be called by a Lua script */

    // game objects
    Game *game;                 /**< the game associated to this script */
    Hero *hero;                 /**< the hero */

    // script data
    lua_State* context;         /**< the execution context of the Lua script */
    std::list<Timer*> timers;   /**< the timers currently running for this script */

    // calling a script function from C++
    bool call_script_function(const std::string &function_name);
    bool call_script_function(const std::string &function_name, const std::string &arg1);
    bool call_script_function(const std::string &function_name, const std::string &arg1, int arg2);
    bool call_script_function(const std::string &function_name, const std::string &arg1, int arg2, int arg3);
    bool call_script_function(const std::string &function_name, int arg1);
    bool call_script_function(const std::string &function_name, int arg1, int arg2);
    bool call_script_function(const std::string &function_name, bool arg1);

    // calling a C++ function from the script
    static void called_by_script(lua_State *context, int nb_arguments, Script **script);

    static FunctionAvailableToScript 
      l_freeze,
      l_unfreeze,
      l_start_message,
      l_set_message_variable,
      l_dialog_set_style,
      l_hud_set_enabled,
      l_play_sound,
      l_play_music,
      l_start_timer,
      l_stop_timer,
      l_move_camera,
      l_restore_camera,
      l_savegame_get_string,
      l_savegame_get_integer,
      l_savegame_get_boolean,
      l_savegame_set_string,
      l_savegame_set_integer,
      l_savegame_set_boolean,
      l_get_rupees,
      l_remove_rupees,
      l_equipment_get_tunic,
      l_equipment_get_sword,
      l_equipment_get_shield,
      l_inventory_item_get,
      l_inventory_item_set,
      l_inventory_item_get_amount,
      l_inventory_item_remove_amount,
      l_inventory_item_is_bottle,
      l_give_treasure,
      l_give_treasure_with_amount,
      l_player_get_name,
      l_player_set_pause_enabled;

    // initialization
    void load(const std::string &script_name);
    virtual void register_available_functions(void);

    // timers
    void add_timer(Timer *timer);
    void remove_timer(const std::string &callback_name);

  public:

    // loading and closing a script
    Script(Game *game);
    virtual ~Script(void);

    // update functions
    virtual void update(void);
    virtual void set_suspended(bool suspended);

    // C++ functions that call script functions
    void event_update(void);
    void event_set_suspended(bool suspended);
    void event_message_started(const MessageId &message_id);
    void event_message_sequence_finished(const MessageId &first_message_id, int answer);
    void event_camera_reached_target(void);
    void event_camera_back(void);
    void event_obtaining_treasure(Treasure::Content content, int savegame_variable);
    void event_obtained_treasure(Treasure::Content content, int savegame_variable);
};

#endif

