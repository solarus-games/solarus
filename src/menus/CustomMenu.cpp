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
#include "menus/CustomMenu.h"
#include "lua/LuaContext.h"

/**
 * @brief Creates a custom menu.
 * @param lua_context The Lua context.
 * @param menu_id Name of the Lua script that controls this menu
 * (no extension), relative to data directory.
 */
CustomMenu::CustomMenu(LuaContext& lua_context,
    const std::string& menu_id):
  id(menu_id),
  lua_context(lua_context) {

  // Load the menu script.
  lua_context.load_menu(*this);
  lua_context.start_menu(*this);
}

/**
 * @brief Destroys the screen.
 */
CustomMenu::~CustomMenu() {

  lua_context.stop_menu(*this);
  lua_context.unload_menu(*this);
}

/**
 * @brief Returns the id of this scripted menu.
 * @return Name of the Lua script that controls this menu
 * (no extension), relative to data directory.
 */
const std::string& CustomMenu::get_id() const {
  return id;
}

/**
 * @brief Updates the screen.
 */
void CustomMenu::update() {

  lua_context.update_menu(*this);
}

/**
 * @brief Displays the screen.
 * @param dst_surface the surface to draw
 */
void CustomMenu::display(Surface& dst_surface) {

  lua_context.display_menu(*this, dst_surface);
}

/**
 * @brief This function is called when there is an input event.
 * @param event the event to handle
 */
void CustomMenu::notify_input(InputEvent& event) {

  lua_context.notify_input_menu(*this, event);
}

