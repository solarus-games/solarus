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
#include <map>

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

    std::map<int, Sprite*> sprites;					/**< the sprites accessible from this script */
    int next_sprite_handle;						/**< next handle of a sprite */

    std::map<int, Movement*> movements;					/**< the movements accessible from this script */
    std::map<int, Movement*> unassigned_movements;			/**< the movements accessible from this script and that
    									 * are not assigned to an object yet (the script has to delete them) */
    int next_movement_handle;						/**< handle of the next movement created */

    // calling a Lua function from C++
    bool find_lua_function(const std::string &function_name);
    bool notify_script(const std::string &function_name, const std::string &format = "", ...);

    // calling a C++ function from Lua (and also retreive the instance of Script)
    static void called_by_script(lua_State *context, int nb_arguments, Script **script);

    static FunctionAvailableToScript 

      // audio
      l_play_sound,
      l_play_music,

      // timers
      l_timer_start,
      l_timer_stop,

      // sprites
      l_sprite_get_animation,
      l_sprite_set_animation,
      l_sprite_get_direction,
      l_sprite_set_direction,
      l_sprite_get_frame,
      l_sprite_set_frame,
      l_sprite_get_frame_delay,
      l_sprite_set_frame_delay,
      l_sprite_is_paused,
      l_sprite_set_paused,
      l_sprite_set_animation_ignore_suspend,
      l_sprite_fade,

      // movements
      l_pixel_movement_create,
      l_random_movement_create,
      l_path_movement_create,
      l_random_path_movement_create,
      l_jump_movement_create,
      l_movement_get_property,
      l_movement_set_property;

    // initialization
    void load(const std::string &script_name);
    virtual void register_available_functions();

    // timers
    void add_timer(Timer *timer);
    void remove_timer(const std::string &callback_name);
    virtual bool is_new_timer_suspended(void);

    // sprites
    int create_sprite_handle(Sprite &sprite);
    Sprite& get_sprite(int sprite_handle);

    // movements
    int create_movement_handle(Movement &movement);
    Movement& get_movement(int movement_handle);
    Movement& start_movement(int movement_handle);

    // debugging
    void print_stack();

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

