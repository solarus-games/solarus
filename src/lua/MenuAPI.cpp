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
#include "lowlevel/Surface.h"
#include <lua.hpp>

static const std::string& on_display_name = "on_display";

/**
 * @brief Initializes the menu features provided to Lua.
 */
void LuaContext::register_menu_module() {

}

/**
 * @brief Calls the on_update() method of a Lua menu.
 * @param menu_ref A reference to the menu object.
 */
void LuaContext::menu_on_update(int menu_ref) {

  push_ref(l, menu_ref);
  on_update();
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_display() method of a Lua menu.
 * @param menu_ref A reference to the menu object.
 * @param dst_surface The destination surface.
 */
void LuaContext::menu_on_display(int menu_ref, Surface& dst_surface) {

  // Get the Lua menu.
  push_ref(l, menu_ref);

  // Find its on_display() method and call it.
  if (find_method(on_display_name)) {
    push_surface(l, dst_surface);
    call_function(2, 0, on_display_name);
  }

  // Remove the menu from the stack.
  lua_pop(l, 1);
}

/**
 * @brief Calls an input callback method of a Lua menu.
 * @param menu_ref A reference to the menu object.
 * @param event The input event to forward.
 */
void LuaContext::menu_on_input(int menu_ref, InputEvent& event) {

  // Get the Lua menu.
  push_ref(l, menu_ref);

  // Trigger its appropriate callback if it exists.
  on_input(event);

  // Remove the menu from the stack.
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_started() method of a Lua menu.
 * @param menu_ref A reference to the menu object.
 */
void LuaContext::menu_on_started(int menu_ref) {

  push_ref(l, menu_ref);
  enable_timers(l, -1);
  on_started();
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_finished() method of a Lua menu.
 * @param menu_ref A reference to the menu object.
 */
void LuaContext::menu_on_finished(int menu_ref) {

  push_ref(l, menu_ref);
  on_finished();
  disable_timers(l, -1);
  lua_pop(l, 1);
}

