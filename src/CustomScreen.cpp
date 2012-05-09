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
#include "Game.h"
#include "MainLoop.h"
#include "lua/LuaContext.h"
#include "lowlevel/StringConcat.h"

/**
 * @brief Creates a custom screen.
 * @param main_loop the Solarus main loop object
 * @param screen_name name of the Lua script that controls this screen
 * (no extension), relative to the screens directory
 */
CustomScreen::CustomScreen(MainLoop& main_loop, const std::string& screen_name):
  Screen(main_loop) {

  // Load the menu script.
  std::ostringstream oss;
  oss << "screens/" << screen_name;
  menu_id = oss.str();

  LuaContext& lua_context = get_lua_context();
  lua_context.load_menu(menu_id);
  lua_context.run_menu(menu_id);
  lua_context.menu_on_started(menu_id);
}

/**
 * @brief Destroys the screen.
 */
CustomScreen::~CustomScreen() {

}

/**
 * @brief Exits the program.
 *
 * This function returns and the program stops after the current cycle.
 */
void CustomScreen::exit() {

  main_loop.set_exiting();
}

/**
 * @brief Shows another custom screen that will replace this one.
 * @param screen_name name of the Lua script that controls the new screen
 */
void CustomScreen::start_screen(const std::string& screen_name) {

  set_next_screen(new CustomScreen(main_loop, screen_name));
}

/**
 * @brief Shows a game that will replace this custom screen.
 * @param savegame_file savegame to load, relative to the savegames directory
 */
void CustomScreen::start_game(const std::string& savegame_file) {

  Savegame savegame(savegame_file);
  set_next_screen(new Game(main_loop, savegame));
}

/**
 * @brief Updates the screen.
 */
void CustomScreen::update() {

  // TODO script.update();
}

/**
 * @brief Displays the screen.
 * @param dst_surface the surface to draw
 */
void CustomScreen::display(Surface& dst_surface) {

  get_lua_context().menu_on_display(menu_id, dst_surface);
}

/**
 * @brief This function is called when there is an input event.
 * @param event the event to handle
 */
void CustomScreen::notify_input(InputEvent& event) {

  // TODO script.notify_input(event);
}

