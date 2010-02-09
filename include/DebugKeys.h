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
#ifndef SOLARUS_DEBUG_KEYS_H
#define SOLARUS_DEBUG_KEYS_H

#include "Common.h"
#include "lowlevel/InputEvent.h"

/**
 * This class provides some special keys that make it easier to debug the game, including
 * by allowing the player to traverse walls.
 * This class exists for debugging purposes only and does nothing in official releases (i.e. when DEBUG_LEVEL < 2)
 */
class DebugKeys {

  private:

    Solarus *solarus;      /**< the Solarus object */
    Game *game;            /**< the current game, or NULL if no game is started */

  public:

    DebugKeys(Solarus *solarus);
    ~DebugKeys(void);

    void set_game(Game *game);

    void key_pressed(InputEvent::KeyboardKey key);
    void key_released(InputEvent::KeyboardKey key);
    void update(void);
};

#endif

