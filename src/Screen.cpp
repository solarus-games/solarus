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
#include "Screen.h"
#include "MainLoop.h"

/**
 * @brief Creates a screen.
 * @param main_loop The Solarus main loop object.
 */
Screen::Screen(MainLoop& main_loop):
  main_loop(main_loop) {

}

/**
 * @brief Destroys the screen.
 */
Screen::~Screen() {

}

/**
 * @brief Returns the Solarus main loop that controls this screen.
 * @return The Solarus main loop manager.
 */
MainLoop& Screen::get_main_loop() {
  return main_loop;
}

/**
 * @brief Returns the shared Lua context.
 * @return The Lua context where all scripts are run.
 */
LuaContext& Screen::get_lua_context() {
  return get_main_loop().get_lua_context();
}

