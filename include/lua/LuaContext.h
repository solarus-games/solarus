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
 * TODO: menu scripts, map scripts, enemy scripts and item scripts currently
 * have still their own Lua world (they inherit Script).
 * Once they are all converted to this shared Lua context,
 * the Script class will be removed.
 */
class LuaContext: public Script {

  public:

    LuaContext(MainLoop& main_loop);
    ~LuaContext();

    void load_menu(const std::string& menu_id);
    void unload_menu(const std::string& menu_id);
    void start_menu(const std::string& menu_id);
    void stop_menu(const std::string& menu_id);
    void update_menu(const std::string& menu_id);
    void display_menu(const std::string& menu_id, Surface& dst_surface);
    void notify_input_menu(const std::string& menu_id, InputEvent& event);

  private:

    void load(const std::string& script_name);
    bool find_local_function(const std::string& function_name);
    bool find_local_function(int index, const std::string& function_name);

    void register_menu_module();
    void push_menu_script(const std::string& menu_id);

    void menu_on_started();
    void menu_on_update();
    void menu_on_display(Surface& dst_surface);
    void menu_on_key_pressed(InputEvent& event);
    void menu_on_key_released(InputEvent& event);
    void menu_on_joypad_button_pressed(InputEvent& event);
    void menu_on_joypad_button_released(InputEvent& event);
    void menu_on_joypad_axis_moved(InputEvent& event);
    void menu_on_joypad_hat_moved(InputEvent& event);
    void menu_on_direction_pressed(InputEvent& event);
};

#endif

