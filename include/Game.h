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
#ifndef SOLARUS_GAME_H
#define SOLARUS_GAME_H

#include "Common.h"
#include "Transition.h"
#include "GameCommands.h"
#include "Savegame.h"
#include "DialogBox.h"

/**
 * @brief Represents the game currently running.
 *
 * The game shows the current map and displays all game elements:
 * the map entities, the HUD, the pause menu, the dialog box, etc.
 */
class Game {

  private:

    // main objects
    MainLoop& main_loop;       /**< the main loop object */
    Savegame* savegame;        /**< the game data saved */
    Hero* hero;

    // current game state (elements currently shown)
    bool pause_key_available;  /**< indicates that the player is allowed to use the pause key */
    PauseMenu* pause_menu;     /**< the current pause menu, or NULL if the game is not paused */
    GameoverSequence* gameover_sequence; /**< the game over sequence (if currently shown) */
    bool resetting;            /**< true if the program will be reset */
    bool restarting;           /**< true if the game will be restarted */

    // controls
    GameCommands* commands;    /**< this object receives the keyboard and joypad events */
    KeysEffect* keys_effect;   /**< current effect associated to the main game keys
                                * (represented on the HUD by the action icon, the objects icons, etc.) */

    // map
    Map* current_map;          /**< the map currently displayed */
    Map* next_map;             /**< the map where the hero is going to; if not NULL, it means that the hero
                                * is changing from current_map to next_map */
    Surface* previous_map_surface;  /**< a copy of the previous map surface for transition effects that display two maps */

    Transition::Style transition_style; /**< the transition style between the current map and the next one */
    Transition* transition;             /**< the transition currently shown, or NULL if no transition is playing */

    // world (i.e. the current set of maps)
    bool crystal_state;        /**< indicates that a crystal has been enabled (i.e. the orange blocks are raised) */

    // graphics
    HUD* hud;                  /**< the game HUD (displaying hearts, rupees, key icons, etc.) */
    bool hud_enabled;          /**< true if the HUD is currently displayed */
    DialogBox dialog_box;      /**< the dialog box manager */

    // update functions
    void update_keys_effect();
    void update_dialog_box();
    void update_transitions();
    void update_gameover_sequence();
    void notify_map_changed();

  public:

    // creation and destruction
    Game(MainLoop& main_loop, Savegame* savegame);
    ~Game();

    void start();
    void stop();

    // global objects
    MainLoop& get_main_loop();
    LuaContext& get_lua_context();
    Hero& get_hero();
    const Rectangle& get_hero_xy();
    GameCommands& get_commands();
    KeysEffect& get_keys_effect();
    Savegame& get_savegame();
    Equipment& get_equipment();

    // functions called by the main loop
    bool notify_input(InputEvent &event);
    void update();
    void draw(Surface& dst_surface);

    // game controls
    void notify_command_pressed(GameCommands::GameCommand command);
    void notify_command_released(GameCommands::GameCommand command);

    // map
    bool has_current_map();
    Map& get_current_map();
    void set_current_map(const std::string& map_id, const std::string& destination_name,
        Transition::Style transition_style);

    // world
    bool get_crystal_state();
    void change_crystal_state();

    // current game state
    bool is_paused();
    bool is_showing_dialog();
    bool is_playing_transition();
    bool is_showing_gameover();
    bool is_suspended(); // true if at least one of the three functions above returns true

    // HUD
    void set_hud_enabled(bool hud_enabled);

    // pause
    bool can_pause();
    bool is_pause_key_available();
    void set_pause_key_available(bool pause_key_available);
    void set_paused(bool paused);

    // dialog box
    DialogBox& get_dialog_box();

    // game over
    void start_gameover_sequence();
    void get_back_from_death();
    void reset();
    void restart();
};

#endif

