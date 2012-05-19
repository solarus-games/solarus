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
#include <lua.hpp>

static const std::string& on_started_name = "on_started";
static const std::string& on_update_name = "on_update";

/**
 * @brief Initializes the events features provided to Lua.
 */
void LuaContext::register_events_module() {

  // sol.events = { }
  lua_getglobal(l, "sol");
  lua_newtable(l);
  lua_setfield(l, -2, "events");
  lua_pop(l, 1);
}


/**
 * @brief Notifies the Lua event manager that the program starts.
 *
 * This function is called once at the beginning of the program.
 * sol.events.on_started() is called if it exists.
 */
void LuaContext::events_on_started() {

  if (find_event_function(on_started_name)) {
    call_function(0, 0, on_started_name);
  }
}

/**
 * @brief Updates the Lua event manager.
 *
 * This function is called at each cycle.
 * sol.events.on_update() is called if existing.
 */
void LuaContext::events_on_update() {

  if (find_event_function(on_update_name)) {
    // Call it.
    call_function(0, 0, on_update_name);
  }
}

/**
 * @brief Notifies the Lua event manager that an input event has just occurred.
 *
 * The appropriate callback in sol.events is notified if it exists.
 *
 * @param event The input event to handle.
 */
void LuaContext::events_on_input(InputEvent& event) {

  // Get the appropriate callback in sol.events.
  lua_getglobal(l, "sol");
                                  // ... sol
  lua_getfield(l, -1, "events");
                                  // ... sol events
  lua_remove(l, -2);
                                  // ... events
  // Call it.
  on_input(event);
                                  // ... events
  lua_pop(l, 1);
                                  // ...
}

