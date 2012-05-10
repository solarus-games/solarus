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
#include "lowlevel/StringConcat.h"

/**
 * @brief Creates a custom screen.
 * @param main_loop The Solarus root object.
 * @param screen_name Name of the Lua script that controls the menu to show
 * in this screen (no extension), relative to the screens directory.
 */
CustomScreen::CustomScreen(MainLoop& main_loop,
    const std::string& screen_name):
  Screen(main_loop),
  menu(main_loop.get_lua_context(),
      StringConcat() << "screens/" << screen_name) {

}

/**
 * @brief Destroys the screen.
 */
CustomScreen::~CustomScreen() {

}

/**
 * @brief Updates the screen.
 */
void CustomScreen::update() {

  menu.update();
}

/**
 * @brief Displays the screen.
 * @param dst_surface the surface to draw
 */
void CustomScreen::display(Surface& dst_surface) {

  menu.display(dst_surface);
}

/**
 * @brief This function is called when there is an input event.
 * @param event the event to handle
 */
void CustomScreen::notify_input(InputEvent& event) {

  menu.notify_input(event);
}

