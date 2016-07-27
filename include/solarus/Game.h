/*
 * Copyright (C) 2006-2016 Christopho, Solarus - http://www.solarus-games.org
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
#ifndef SOLARUS_GAME_H
#define SOLARUS_GAME_H

#include "solarus/Common.h"
#include "solarus/entities/HeroPtr.h"
#include "solarus/lowlevel/Point.h"
#include "solarus/lowlevel/SurfacePtr.h"
#include "solarus/CommandsEffects.h"
#include "solarus/DialogBoxSystem.h"
#include "solarus/GameCommand.h"
#include "solarus/Transition.h"
#include "solarus/GameCommands.h"
#include "solarus/entities/NonAnimatedRegions.h"
#include <memory>
#include <string>

namespace Solarus {

class CommandsEffects;
class Equipment;
class GameCommands;
class InputEvent;
class LuaContext;
class MainLoop;
class Map;
class Savegame;

/**
 * \brief Represents the game currently running.
 *
 * The game shows the current map and handles all game elements.
 */
class SOLARUS_API Game {

  public:

    // creation
    Game(MainLoop& main_loop, const std::shared_ptr<Savegame>& savegame);

    // no copy operations
    Game(const Game& game) = delete;
    Game& operator=(const Game& game) = delete;

    void start();
    void stop();
    void restart();

    // global objects
    MainLoop& get_main_loop();
    LuaContext& get_lua_context();
    const HeroPtr& get_hero();
    GameCommands& get_commands();
    const GameCommands& get_commands() const;
    CommandsEffects& get_commands_effects();
    Savegame& get_savegame();
    const Savegame& get_savegame() const;
    Equipment& get_equipment();
    const Equipment& get_equipment() const;

    // functions called by the main loop
    bool notify_input(const InputEvent& event);
    void update();
    void draw(const SurfacePtr& dst_surface);

    // game controls
    void notify_command_pressed(GameCommand command);
    void notify_command_released(GameCommand command);

    // simulate commands
    void simulate_command_pressed(GameCommand command);
    void simulate_command_released(GameCommand command);

    // map
    bool has_current_map() const;
    Map& get_current_map();
    void set_current_map(const std::string& map_id, const std::string& destination_name,
        Transition::Style transition_style);

    // world
    bool get_crystal_state() const;
    void change_crystal_state();

    // current game state
    bool is_paused() const;
    bool is_dialog_enabled() const;
    bool is_playing_transition() const;
    bool is_showing_game_over() const;
    bool is_suspended_by_camera() const;
    bool is_suspended_by_script() const;
    bool is_suspended() const; // true if at least one of the 5 functions above returns true

    // pause
    bool can_pause() const;
    bool can_unpause() const;
    bool is_pause_allowed() const;
    void set_pause_allowed(bool pause_allowed);
    void set_paused(bool paused);

    // dialogs
    void start_dialog(
        const std::string& dialog_id,
        const ScopedLuaRef& info_ref,
        const ScopedLuaRef& callback_ref
    );
    void stop_dialog(const ScopedLuaRef& status_ref);

    // game over
    void start_game_over();
    void stop_game_over();

    // Suspend manually.
    void set_suspended_by_script(bool suspended);

private:

    // main objects
    MainLoop& main_loop;       /**< the main loop object */
    std::shared_ptr<Savegame>
        savegame;              /**< the game data saved */
    HeroPtr hero;              /**< The hero entity.  */

    // current game state (elements currently shown)
    bool pause_allowed;        /**< indicates that the player is allowed to use the pause command */
    bool paused;               /**< indicates that the game is paused */
    DialogBoxSystem
        dialog_box;            /**< The dialog box manager. */
    bool showing_game_over;    /**< Whether a game-over sequence is currently active. */
    bool suspended_by_script;  /**< Whether the game is manually suspended. */
    bool started;              /**< true if this game is running, false if it is not yet started or being closed. */
    bool restarting;           /**< true if the game will be restarted */

    // controls
    std::unique_ptr<GameCommands>
        commands;              /**< this object receives the keyboard and joypad events */
    CommandsEffects
        commands_effects;      /**< current effect associated to the main game keys
                                * (represented on the HUD by the action icon, the objects icons, etc.) */

    // map
    std::shared_ptr<Map>
        current_map;           /**< the map currently displayed */
    std::shared_ptr<Map>
        next_map;              /**< the map where the hero is going to; if not nullptr, it means that the hero
                                * is changing from current_map to next_map */
    SurfacePtr
        previous_map_surface;  /**< a copy of the previous map surface for transition effects that display two maps */

    Transition::Style
        transition_style;      /**< the transition style between the current map and the next one */
    std::unique_ptr<Transition>
        transition;            /**< the transition currently shown, or nullptr if no transition is playing */

    // world (i.e. the current set of maps)
    bool crystal_state;        /**< indicates that a crystal has been enabled (i.e. the orange blocks are raised) */

    // update functions
    void update_commands_effects();
    void update_transitions();
    void update_gameover_sequence();
    void notify_map_changed();

};

}

#endif

