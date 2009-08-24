/*
 * Copyright (C) 2009 Christopho, Zelda Solarus - http://www.zelda-solarus.com
 * 
 * Zelda: Mystery of Solarus DX is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Zelda: Mystery of Solarus DX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef ZSDX_DEBUG_KEYS_H
#define ZSDX_DEBUG_KEYS_H

#include "Common.h"

/**
 * This class provides some special keys to make it easier to test the game.
 * Those keys are here for debugging purposes only and do not exist in official releases.
 */
class DebugKeys {

  private:

    Game *game;

  public:

    DebugKeys(Game *game);
    ~DebugKeys(void);

    void key_pressed(const SDL_keysym &keysym);
    void key_released(const SDL_keysym &keysym);
};

#endif

