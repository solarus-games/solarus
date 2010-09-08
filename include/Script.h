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
#include <list>

struct lua_State;

/**
 * @brief Interface between the engine C++ code and a Lua script.
 *
 * This class provides a API that allows the C++ and the Lua script to communicate
 * in both directions.
 * - The API contains C++ functions that Lua scripts can call to act on the game.
 *   Examples: starting a dialog or getting the current number of rupees.
 * - The API also contains names of Lua functions that the C++ engine invokes if they exist.
 *   Such Lua functions are called events. They are used to notify the script that
 *   and event just happened. Example: a dialog has just finished.
 *
 * This class only provide functions that are common to any kind of script and is meant to be inherited.
 * The subclasses are specific to particular kinds of script
 * (such as map scripts or enemy scripts) and provide additional, specific fonctions.
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
    bool call_script_function(const std::string &function_name, int arg1, const std::string &arg2, int arg3);
    bool call_script_function(const std::string &function_name, int arg1);
    bool call_script_function(const std::string &function_name, int arg1, int arg2);
    bool call_script_function(const std::string &function_name, bool arg1);

    // calling a C++ function from the script
    static void called_by_script(lua_State *context, int nb_arguments, Script **script);

    static FunctionAvailableToScript 
      l_hero_freeze,
      l_hero_unfreeze,
      l_hero_set_pause_enabled,
      l_dialog_start,
      l_dialog_set_variable,
      l_dialog_set_style,
      l_hud_set_enabled,
      l_play_sound,
      l_play_music,
      l_timer_start,
      l_timer_stop,
      l_camera_move,
      l_camera_restore,
      l_savegame_get_string,
      l_savegame_get_integer,
      l_savegame_get_boolean,
      l_savegame_set_string,
      l_savegame_set_integer,
      l_savegame_set_boolean,
      l_savegame_get_name,
      l_equipment_get_life,
      l_equipment_add_life,
      l_equipment_remove_life,
      l_equipment_get_money,
      l_equipment_add_money,
      l_equipment_remove_money,
      l_equipment_get_ability,
      l_equipment_get_item,
      l_equipment_set_item,
      l_equipment_get_item_amount,
      l_equipment_add_item_amount,
      l_equipment_remove_item_amount,
      l_treasure_give,
      l_treasure_give_with_amount;

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
    void event_dialog_started(const MessageId &message_id);
    void event_dialog_finished(const MessageId &first_message_id, int answer);
    void event_camera_reached_target(void);
    void event_camera_back(void);
    void event_treasure_obtaining(int savegame_variable, const std::string &item, int variant);
    void event_treasure_obtained(int savegame_variable, const std::string &item, int variant);
};

#endif

