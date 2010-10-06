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
 * @brief Handles a Lua script that is running.
 *
 * This class and its subclasses provide an API that allows Lua scripts to call C++ functions.
 *
 * This particular class provide a few functions that are common to any kind of script.
 * The subclasses are specific to particular kinds of script
 * (such as map scripts or enemy scripts) and provide additional, specific fonctions.
 */
class Script {

  protected:

    typedef int (FunctionAvailableToScript) (lua_State *l);		/**< type of the functions that can be called by a Lua script */

    // script data
    lua_State* context;							/**< the execution context of the Lua script */
    std::list<Timer*> timers;						/**< the timers currently running for this script */

    // calling a Lua function from C++
    bool notify_script(const std::string &function_name);
    bool notify_script(const std::string &function_name, const std::string &arg1);
    bool notify_script(const std::string &function_name, const std::string &arg1, int arg2);
    bool notify_script(const std::string &function_name, const std::string &arg1, int arg2, int arg3);
    bool notify_script(const std::string &function_name, const std::string &arg1, const std::string &arg2, int arg3);
    bool notify_script(const std::string &function_name, int arg1, const std::string &arg2, int arg3);
    bool notify_script(const std::string &function_name, int arg1);
    bool notify_script(const std::string &function_name, int arg1, int arg2);
    bool notify_script(const std::string &function_name, bool arg1);

    // calling a C++ function from Lua
    static void called_by_script(lua_State *context, int nb_arguments, Script **script);

    static FunctionAvailableToScript 
      l_play_sound,
      l_play_music,
      l_timer_start,
      l_timer_stop;

    // initialization
    void load(const std::string &script_name);
    virtual void register_available_functions();

    // timers
    void add_timer(Timer *timer);
    void remove_timer(const std::string &callback_name);
    virtual bool is_new_timer_suspended(void);

  public:

    // loading and closing a script
    Script();
    virtual ~Script();

    // update functions
    virtual void update();

    // calling Lua from C++
    void event_update();
};

#endif

