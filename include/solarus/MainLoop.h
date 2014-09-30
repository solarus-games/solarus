/*
 * Copyright (C) 2006-2014 Christopho, Solarus - http://www.solarus-games.org
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
#ifndef SOLARUS_MAIN_LOOP_H
#define SOLARUS_MAIN_LOOP_H

#include "solarus/Common.h"
#include "solarus/lowlevel/SurfacePtr.h"
#include <memory>

namespace Solarus {

class Arguments;
class Game;
class InputEvent;
class LuaContext;

/**
 * \brief Main class of the game engine.
 *
 * It starts the program and handles the succession of its screens.
 */
class MainLoop {

  public:

    MainLoop(const Arguments& args);
    ~MainLoop();

    void run();
    void step();

    void set_exiting();
    bool is_exiting();
    void set_resetting();
    bool is_resetting();
    Game* get_game();
    void set_game(Game* game);

    LuaContext& get_lua_context();

  private:

    void check_input();

    std::unique_ptr<LuaContext>
        lua_context;              /**< The Lua world where scripts are run. */
    SurfacePtr root_surface;      /**< The surface where everything is drawn. */
    std::unique_ptr<Game> game;   /**< The current game if any, nullptr otherwise. */
    Game* next_game;              /**< The game to start at next cycle (nullptr means resetting the game). */
    bool exiting;                 /**< Indicates that the program is about to stop. */

    void notify_input(const InputEvent& event);
    void draw();
    void update();

};

}

#endif

