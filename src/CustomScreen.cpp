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
#include "CustomScreen.h"
#include "Game.h"
#include "Solarus.h"
#include "lowlevel/StringConcat.h"

/**
 * @brief Creates a custom screen.
 * @param solarus the Solarus object
 * @param screen_name name of the Lua script that controls this screen
 * (no extension), relative to the screens directory
 */
CustomScreen::CustomScreen(Solarus& solarus, const std::string& screen_name):
  Screen(solarus),
  script(StringConcat() << "screens/" << screen_name, this) {

  script.event_menu_started();
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

  solarus.set_exiting();
}

/**
 * @brief Shows another custom screen that will replace this one.
 * @param screen_name name of the Lua script that controls the new screen
 */
void CustomScreen::start_screen(const std::string& screen_name) {

  set_next_screen(new CustomScreen(solarus, screen_name));
}

/**
 * @brief Shows a game that will replace this custom screen.
 * @param screen_name name of the Lua script that controls the new screen
 */
void CustomScreen::start_game(const std::string& savegame_file) {

  Savegame savegame(savegame_file);
  set_next_screen(new Game(solarus, savegame));
}

/**
 * @brief Updates the screen.
 */
void CustomScreen::update() {

  script.update();
}

/**
 * @brief Displays the screen.
 * @param dst_surface the surface to draw
 */
void CustomScreen::display(Surface& dst_surface) {

  script.event_display(dst_surface);
}

/**
 * @brief This function is called when there is an input event.
 * @param event the event to handle
 */
void CustomScreen::notify_event(InputEvent& event) {

}

