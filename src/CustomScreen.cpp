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
#include "CustomScreen.h"
#include "MainLoop.h"
#include "lua/LuaContext.h"
#include "lowlevel/StringConcat.h"

/**
 * @brief Creates a custom screen.
 * @param main_loop The Solarus root object.
 * @param screen_ref Lua ref of the object that controls the menu to show.
 */
CustomScreen::CustomScreen(MainLoop& main_loop,
    int screen_ref):
  Screen(main_loop),
  screen_ref(screen_ref) {


}

/**
 * @brief Destroys the screen.
 */
CustomScreen::~CustomScreen() {

  get_lua_context().destroy_ref(screen_ref);
}

/**
 * @brief Displays the screen.
 *
 * This is what's special about CustomScreen, it doesn't draw anything itself
 * but instead delegates that call to Lua entirely.
 *
 * @param dst_surface the surface to draw
 */
void CustomScreen::display(Surface& dst_surface) {

  // Delegate the call to the custom screen object.
  get_lua_context().notify_screen_display(dst_surface, screen_ref);
}

/**
 * @brief Updates the screen.
 * This does not need to be forwarded since the Lua api provides events for
 * this on its own.
 */
void CustomScreen::update() {
}

/**
 * @brief This function is called when there is an input event.
 *
 * This does not need to be forwarded since the Lua api provides events for
 * this on its own.
 *
 * @param event the event to handle
 */
void CustomScreen::notify_input(InputEvent& event) {
}

