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
#ifndef SOLARUS_GAME_COMMANDS_H
#define SOLARUS_GAME_COMMANDS_H

#include "Common.h"
#include "lowlevel/InputEvent.h"
#include <string>
#include <map>

/**
 * @brief Stores the mapping between the in-game high-level commands
 * and their keyboard and joypad bindings.
 *
 * This class receives the low-level keyboard and joypad events that occur
 * during the game and then notifies the appropriate objects that a built-in
 * game command was pressed or released.
 * What we call a game command is a high-level notion such as "action" or
 * "attack".
 * The corresponding low-level input event can be a keyboard event or a
 * joypad event.
 */
class GameCommands {

  public:

    /**
     * @brief The built-in commands recognized by the engine during a game.
     *
     * These high-level commands can be mapped onto the keyboard and the joypad.
     */
    enum GameCommand {
      NONE = -1,
      ACTION,
      SWORD,
      ITEM_1,
      ITEM_2,
      PAUSE,
      RIGHT,
      UP,
      LEFT,
      DOWN,
      NB_COMMANDS
    };

    GameCommands(Game& game);
    ~GameCommands();

    InputEvent::KeyboardKey get_keyboard_binding(GameCommand command);
    const std::string& get_joypad_binding(GameCommand command);

    void notify_input(InputEvent& event);
    bool is_command_pressed(GameCommand command);
    int get_wanted_direction8();

    void customize(GameCommand command);
    bool is_customizing();
    GameCommand get_command_to_customize();
    bool is_customization_done();

  private:

    Savegame& get_savegame();

    // High-level resulting commands.
    void game_command_pressed(GameCommand command);
    void game_command_released(GameCommand command);

    // Keyboard mapping.
    void keyboard_key_pressed(InputEvent::KeyboardKey keyboard_key_pressed);
    void keyboard_key_released(InputEvent::KeyboardKey keyboard_key_released);
    const std::string& get_keyboard_binding_savegame_variable(GameCommand command);
    InputEvent::KeyboardKey get_saved_keyboard_binding(GameCommand command);
    void set_saved_keyboard_binding(GameCommand command, InputEvent::KeyboardKey key);

    // Joypad mapping.
    void joypad_button_pressed(int button);
    void joypad_button_released(int button);
    void joypad_axis_moved(int axis, int direction);
    void joypad_hat_moved(int hat, int direction);
    const std::string& get_joypad_binding_savegame_variable(GameCommand command);
    const std::string& get_saved_joypad_binding(GameCommand command);
    void set_saved_joypad_binding(GameCommand command, const std::string& joypad_string);

    Game& game;                          /**< The game we are controlling. */
    std::map<InputEvent::KeyboardKey, GameCommand>
        keyboard_mapping;                /**< Associates each game command to the
                                          * keyboard key that triggers it. */
    std::map<std::string, GameCommand>
        joypad_mapping;                  /**< Associates each game command to the
                                          * joypad action that triggers it. */
    std::map<GameCommand, bool>
        commands_pressed;                /**< Memorizes the state of each game command. */

    bool customizing;                    /**< Indicates that the next keyboard or
                                          * joypad event will be considered as the
                                          * new binding for a game command. */
    GameCommand command_to_customize;    /**< The game command being customized
                                          * when customizing is true. */

    static const uint16_t
        direction_masks[4];              /**< Bit mask associated to each direction:
                                          * this allows to store any combination of
                                          * the four directions into an integer. */
    static const int
        masks_to_directions8[16];        /**< Associates to each combination of
                                          * directional command a direction between 0
                                          * and 7 (-1 means none). */
    static const std::string
        direction_names[4];              /**< English name of each arrow direction,
                                          * used to save a joypad action as a string. */
};

#endif

