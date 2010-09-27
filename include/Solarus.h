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
#ifndef SOLARUS_SOLARUS_H
#define SOLARUS_SOLARUS_H

#include "Common.h"
#include "lua/Scripts.h"

/**
 * @brief Main class of the game engine.
 *
 * It starts the program and handles the succession of its screens.
 */
class Solarus {

  private:

    Screen *current_screen;		/**< the screen currently displayed (may be the title screen,
					 * the selection menu, the game, etc.) */
    Surface *root_surface;		/**< the surface where everything is drawn (always 320*240) */
    DebugKeys *debug_keys;		/**< special keys to debug the game, e.g. to traverse walls (disabled in release mode) */
    Scripts scripts;			/**< all scripts that are currently running */
    bool exiting;			/**< indicates that the program is about to stop */

    void launch_adventure_mode(Savegame *savegame);

    void notify_event(InputEvent &event);
    void display();
    void update();

  public:

    Solarus(int argc, char **argv);
    ~Solarus();

    void main_loop();
    void set_exiting();
    bool is_exiting();
    Scripts& get_scripts();

    void skip_menus();
    DebugKeys& get_debug_keys();
};

#endif

