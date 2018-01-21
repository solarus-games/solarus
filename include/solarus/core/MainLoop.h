/*
 * Copyright (C) 2006-2018 Christopho, Solarus - http://www.solarus-games.org
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

#include "solarus/core/Common.h"
#include "solarus/core/ResourceProvider.h"
#include "solarus/graphics/SurfacePtr.h"
#include <atomic>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

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
class SOLARUS_API MainLoop {

  public:

    explicit MainLoop(const Arguments& args);
    ~MainLoop();

    void run();
    void step();

    void set_exiting();
    bool is_exiting();
    void set_resetting();
    bool is_resetting();
    Game* get_game();
    void set_game(Game* game);
    ResourceProvider& get_resource_provider();
    int push_lua_command(const std::string& command);

    LuaContext& get_lua_context();

  private:

    void check_input();
    void notify_input(const InputEvent& event);
    void draw();
    void update();

    void load_quest_properties();
    void initialize_lua_console();
    void quit_lua_console();

    std::unique_ptr<LuaContext>
        lua_context;              /**< The Lua world where scripts are run. */
    ResourceProvider
        resource_provider;        /**< Resource cache of the quest. */
    SurfacePtr root_surface;      /**< The surface where everything is drawn. */
    std::unique_ptr<Game> game;   /**< The current game if any, nullptr otherwise. */
    Game* next_game;              /**< The game to start at next cycle (nullptr means resetting the game). */
    std::atomic<bool> exiting;    /**< Indicates that the program is about to stop. */
    uint32_t debug_lag;           /**< Artificial lag added to each frame.
                                   * Useful to debug issues that only happen on slow systems. */
    bool turbo;                   /**< Whether to run the simulation as fast as possible
                                   * rather than following real time. */

    std::thread stdin_thread;     /**< Separate thread that reads Lua commands on stdin. */
    std::vector<std::string>
        lua_commands;             /**< Lua commands to run next cycle. */
    std::mutex
        lua_commands_mutex;       /**< Lock for the list of scheduled Lua commands. */
    int num_lua_commands_pushed;  /**< Counter of Lua commands requested. */
    int num_lua_commands_done;    /**< Counter of Lua commands executed. */

};

}

#endif

