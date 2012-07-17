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
#ifndef SOLARUS_LUA_CONTEXT_H
#define SOLARUS_LUA_CONTEXT_H

#include "Common.h"
#include "lua/Script.h"  // TODO remove

/**
 * @brief This class represents the only Lua context that runs
 * dynamic scripts at runtime.
 *
 * Such scripts includes map scripts, enemy scripts, menu scripts, etc.
 * Data files are not managed by this class even if they are written in Lua.
 *
 * TODO: map scripts, enemy scripts and item scripts currently
 * have still their own Lua world (they inherit Script).
 * Once they are all converted to this shared Lua context,
 * the Script class will be removed.
 */
class LuaContext: public Script {

  public:

    static const std::string video_module_name;     /**< sol.video */
    static const std::string menu_module_name;      /**< sol.menu */
    static const std::string language_module_name;  /**< sol.language */

    LuaContext(MainLoop& main_loop);
    ~LuaContext();

    // Main loop from C++.
    void initialize();
    void exit();
    void update();
    void notify_input(InputEvent& event);

    // Main Lua script (sol.main).
    void main_on_update();
    void main_on_input(InputEvent& event);
    void main_on_started();
    void main_on_finished();

    // Lua menus events.
    void menu_on_update(int menu_ref);
    void menu_on_display(int menu_ref, Surface& dst_surface);
    void menu_on_input(int menu_ref, InputEvent& event);
    void menu_on_started(int menu_ref);
    void menu_on_finished(int menu_ref);

  private:

    static void load(lua_State* l, const std::string& script_name);
    static bool load_if_exists(lua_State* l, const std::string& script_name);
    bool find_local_function(int index, const std::string& function_name);
    bool find_local_function(const std::string& function_name);
    bool find_method(int index, const std::string& function_name);
    bool find_method(const std::string& function_name);

    static int l_loader(lua_State* l);

    void register_video_module();
    void register_menu_module();
    void register_language_module();

    void on_update();
    void on_input(InputEvent& event);
    void on_key_pressed(InputEvent& event);
    void on_key_released(InputEvent& event);
    void on_joypad_button_pressed(InputEvent& event);
    void on_joypad_button_released(InputEvent& event);
    void on_joypad_axis_moved(InputEvent& event);
    void on_joypad_hat_moved(InputEvent& event);
    void on_direction_pressed(InputEvent& event);
    void on_started();
    void on_finished();
};

#endif

