/*
 * Copyright (C) 2009-2011 Christopho, Solarus - http://www.solarus-engine.org
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
#include "lua/MenuScript.h"
#include "lowlevel/Debug.h"
#include "CustomScreen.h"
#include <lua.hpp>

/**
 * @brief Constructor.
 * @param file_name script file to load
 * @param screen the custom screen to control from this script (or NULL)
 */
MenuScript::MenuScript(const std::string& file_name, CustomScreen* screen):
  Script(MAIN_API),
  screen(screen) {

  load(file_name);

  // create the surface to draw
}

/**
 * @brief Destructor.
 */
MenuScript::~MenuScript() {

}

/**
 * @brief Returns the screen controlled by this script (if any).
 *
 * Scripts that want to control the screen must redefine this function.
 *
 * @return the screen controlled by this script
 */
CustomScreen& MenuScript::get_screen() {

  Debug::check_assertion(screen != NULL,
      "This menu script does not control the screen");

  return *screen;
}

/**
 * @brief Notifies the script that the menu starts.
 */
void MenuScript::event_menu_started() {

  const std::string function_name("event_menu_started");
  find_lua_function(function_name);
  call_script(0, 0, function_name);
}

/**
 * @brief Displays the content of the menu on a surface
 * @param destination the destination surface
 */
void MenuScript::event_display(Surface& destination) {

  const std::string function_name("event_display");
  find_lua_function(function_name);
  push_surface(l, destination);
  call_script(1, 0, function_name);
}

