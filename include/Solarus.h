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


/** @mainpage
 *
 * This is the documentation of the C++ code of Solarus.
 * Solarus is a free, open-source engine for Zelda-like 2D games.
 *
 * The engine is an executable binary that can run a quest.
 * A quest is a data package containing the graphics, the sounds, the maps
 * and the dialogs.
 *
 * The engine classes are organized in several directories:
 * - (root directory): the main classes of the engine;
 * - lowlevel: encapsulates all libraries and provides low-level access to resources
 *   (the rest of the code is independent from libraries like SDL, OpenAL, etc.);
 * - menus: the menus displayed before or during the game;
 * - entities: all objects that are present on a map: the tiles, the hero, the enemies, the chests, etc.
 *   (most of the code is there);
 * - enemies: all types of enemies with their specific behavior;
 * - movements: many types of movements that can be applied to objects;
 * - hud: the head-up display that appears during the game.
 */

#ifndef SOLARUS_SOLARUS_H
#define SOLARUS_SOLARUS_H

#include "Common.h"

/**
 * Main class of the game engine. It contains objects to
 * initialize the program and handles the succession of its screens.
 */
class Solarus {

  private:

    Screen *current_screen;      /**< the screen currently displayed (the title screen,
				  * the selection menu, the game, etc.) */
    Surface *root_surface;       /**< the surface where everything is drawn (always 320*240) */
    DebugKeys *debug_keys;       /**< special keys to debug the game, e.g. by traversing walls (disabled in release mode) */
    bool exiting;                /**< indicates that the program is about to stop */

    void launch_adventure_mode(Savegame *savegame);

    void notify_event(InputEvent &event);
    void display(void);
    void update(void);

  public:

    Solarus(int argc, char **argv);
    ~Solarus(void);

    void main(void);

    void skip_menus(void);
    DebugKeys * get_debug_keys(void);

    void set_exiting(void);
    bool is_exiting(void);
};

#endif

