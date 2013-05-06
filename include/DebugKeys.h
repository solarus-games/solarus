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
#ifndef SOLARUS_DEBUG_KEYS_H
#define SOLARUS_DEBUG_KEYS_H

#include "Common.h"
#include "lowlevel/InputEvent.h"

/**
 * @brief Provides special keys to debug the game.
 *
 * This class provides some special keys that make it easier to debug the game,
 * including by allowing the hero to traverse walls.
 * This class exists for debugging purposes only and does nothing in official
 * releases (i.e. when SOLARUS_DEBUG_KEYS is not defined)
 */
class DebugKeys {

  private:

    MainLoop& main_loop;   /**< the Solarus main loop object */

  public:

    DebugKeys(MainLoop& main_loop);
    ~DebugKeys();

    MainLoop& get_main_loop();

    void update();
};

#endif

