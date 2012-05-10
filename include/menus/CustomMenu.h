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
#ifndef SOLARUS_CUSTOM_MENU_H
#define SOLARUS_CUSTOM_MENU_H

#include "Common.h"

/**
 * @brief A menu controlled by a Lua script.
 *
 * The menu may be used as an entire phase of the program (a screen)
 * or as a piece of an interface (pause menu, HUD, etc.).
 */
class CustomMenu {

  public:

    // creation and destruction
    CustomMenu(LuaContext& lua_context, const std::string& menu_id);
    ~CustomMenu();

    const std::string& get_id() const;

    // update and display
    void notify_input(InputEvent& event);
    void update();
    void display(Surface& dst_surface);

  private:

    std::string id;           /**< Id of this menu (also the script file name). */
    LuaContext& lua_context;  /**< The Lua context that shows this menu. */
};

#endif


