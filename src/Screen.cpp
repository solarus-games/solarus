/*
 * Copyright (C) 2009 Christopho, Solarus - http://www.solarus-engine.org
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
#include "Solarus.h"

/**
 * @brief Creates a screen.
 * @param solarus the application object (some screens need it)
 */
Screen::Screen(Solarus &solarus):
  solarus(solarus), screen_finished(false), next_screen(NULL) {

}

/**
 * @brief Destroys the screen.
 */
Screen::~Screen() {

}

/**
 * @brief This function can be called by the current screen
 * to indicates that it is finished and to specify
 * the next screen.
 * @param next_screen the next screen to show, or NULL
 * to restart the program.
 */
void Screen::set_next_screen(Screen *next_screen) {
  this->screen_finished = true;
  this->next_screen = next_screen;
}

/**
 * @brief When this screen is finished, returns the
 * screen that should be displayed now.
 * @return the next screen
 */
Screen * Screen::get_next_screen() {
  return next_screen;
}

/**
 * @brief Returns whether this screen is finished.
 *
 * If true is returned, the get_next_screen()
 * function indicates what screen should be
 * displayed now.
 *
 * @return true if the screen finished
 */
bool Screen::is_screen_finished() {
  return screen_finished;
}

/**
 * @brief Returns the list of scripts currently running.
 * @return the current scripts
 */
Scripts& Screen::get_scripts() {
  return solarus.get_scripts();
}

