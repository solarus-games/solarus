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
#include "lua/LuaContext.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"
#include "lowlevel/Surface.h"
#include <lua.hpp>

static const std::string& on_started_name = "event_menu_started";
static const std::string& on_display_name = "event_display";

/**
 * @brief Initializes the menu features provided to Lua.
 */
void LuaContext::register_menu_module() {

  // Create a table sol.menus to store all menu scripts.
                                  // ...
  lua_getglobal(l, "sol");
                                  // ... sol
  lua_newtable(l);
                                  // ... sol menus
  lua_setfield(l, -2, "menus");
                                  // ... sol
  lua_pop(l, 1);
                                  // ...
}

/**
 * @brief Loads the script of a menu into this Lua world.
 * @param menu_id file name of the script without extension,
 * relative to the data directory.
 */
void LuaContext::load_menu(const std::string& menu_id) {

                                  // ...
  lua_getglobal(l, "sol");
                                  // ... sol
  lua_getfield(l, -1, "menus");
                                  // ... sol menus
  load(menu_id);
                                  // ... sol menus menu_function
  lua_setfield(l, -2, menu_id.c_str());
                                  // ... sol menus
  lua_pop(l, 2);
                                  // ...
}

/**
 * @brief Pushes on top of the stack the previously loaded function of a menu.
 * @param menu_id id of the menu script to get
 */
void LuaContext::push_menu_script(const std::string& menu_id) {
                                  // ...
  lua_getglobal(l, "sol");
                                  // ... sol
  lua_getfield(l, -1, "menus");
                                  // ... sol menus
  lua_getfield(l, -1, menu_id.c_str());
                                  // ... sol menus menu_fct/nil
  Debug::check_assertion(!lua_isnil(l, -1), StringConcat()
      << "Unknown menu '" << menu_id << "'");
                                  // ... sol menus menu_fct
  lua_remove(l, -2);
                                  // ... sol menu_fct
  lua_remove(l, -2);
                                  // ... menu_fct
}

/**
 * @brief Starts a menu script previously loaded.
 * @param menu_id Id of the menu to start.
 */
void LuaContext::run_menu(const std::string& menu_id) {

                                  // ...
  push_menu_script(menu_id);
                                  // ... menu_fct
  call_script(0, 0, menu_id);
                                  // ...
}

/**
 * @brief Notifies a menu script that its menu has just started.
 * @param menu_id Id of a menu.
 */
void LuaContext::menu_on_started(const std::string& menu_id) {

  push_menu_script(menu_id);
  if (find_local_function(-1, on_started_name)) {
    call_script(0, 0, on_started_name);
  }
  lua_pop(l, 1);
}

/**
 * @brief Displays the content of a menu on a surface.
 * @param menu_id Id of a menu.
 * @param dst_surface The destination surface.
 */
void LuaContext::menu_on_display(const std::string& menu_id, Surface& dst_surface) {

  push_menu_script(menu_id);
  if (find_local_function(-1, on_display_name)) {
    push_userdata(l, dst_surface);
    call_script(1, 0, on_display_name);
  }
  lua_pop(l, 1);
}


