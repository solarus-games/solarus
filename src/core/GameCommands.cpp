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
#include "solarus/core/Debug.h"
#include "solarus/core/Equipment.h"
#include "solarus/core/Game.h"
#include "solarus/core/GameCommands.h"
#include "solarus/core/Savegame.h"
#include "solarus/entities/Hero.h"
#include "solarus/lua/LuaContext.h"
#include <lua.hpp>
#include <sstream>

namespace Solarus {

/**
 * \brief Lua name of each value of the Command enum.
 */
const std::map<GameCommand, std::string> GameCommands::command_names = {
    { GameCommand::NONE, "" },
    { GameCommand::ACTION, "action" },
    { GameCommand::ATTACK, "attack" },
    { GameCommand::ITEM_1, "item_1" },
    { GameCommand::ITEM_2, "item_2" },
    { GameCommand::PAUSE, "pause" },
    { GameCommand::RIGHT, "right" },
    { GameCommand::UP, "up" },
    { GameCommand::LEFT, "left" },
    { GameCommand::DOWN, "down" }
};

const std::string GameCommands::direction_names[4] = {
    "right",
    "up",
    "left",
    "down"
};

const uint16_t GameCommands::direction_masks[4] = {
    0x0001,
    0x0002,
    0x0004,
    0x0008
};

const int GameCommands::masks_to_directions8[] = {
    -1,  // none: stop
     0,  // right
     2,  // up
     1,  // right + up
     4,  // left
    -1,  // left + right: stop
     3,  // left + up
    -1,  // left + right + up: stop
     6,  // down
     7,  // down + right
    -1,  // down + up: stop
    -1,  // down + right + up: stop
     5,  // down + left
    -1,  // down + left + right: stop
    -1,  // down + left + up: stop
    -1,  // down + left + right + up: stop
};

/**
 * \brief Constructor.
 * \param game the game
 */
GameCommands::GameCommands(Game& game):
  game(game),
  customizing(false),
  command_to_customize(GameCommand::NONE),
  customize_callback_ref() {

  // Load the commands from the savegame.
  for (const auto& kvp : command_names) {

    GameCommand command = kvp.first;
    if (command == GameCommand::NONE) {
      continue;
    }

    // Keyboard.
    InputEvent::KeyboardKey keyboard_key = get_saved_keyboard_binding(command);
    keyboard_mapping[keyboard_key] = command;

    // Joypad.
    const std::string& joypad_string = get_saved_joypad_binding(command);
    joypad_mapping[joypad_string] = command;
  }
}

/**
 * \brief Returns the savegame.
 * \return The savegame.
 */
Savegame& GameCommands::get_savegame() {
  return game.get_savegame();
}

/**
 * \brief Returns the savegame.
 * \return The savegame.
 */
const Savegame& GameCommands::get_savegame() const {
  return game.get_savegame();
}

/**
 * \brief Returns whether the specified game command is pressed.
 *
 * The command can be activated from the keyboard or the joypad.
 *
 * \param command A game command.
 * \return true if this game command is currently pressed.
 */
bool GameCommands::is_command_pressed(GameCommand command) const {
  return commands_pressed.find(command) != commands_pressed.end();
}

/**
 * \brief Returns the direction corresponding to the directional commands
 * currently pressed by the player.
 * \return The direction (0 to 7), or -1 if no directional command is pressed
 * or if the combination of directional command is not a valid direction.
 */
int GameCommands::get_wanted_direction8() const {

  uint16_t direction_mask = 0x0000;
  if (is_command_pressed(GameCommand::RIGHT)) {
    direction_mask |= direction_masks[0];
  }
  if (is_command_pressed(GameCommand::UP)) {
    direction_mask |= direction_masks[1];
  }
  if (is_command_pressed(GameCommand::LEFT)) {
    direction_mask |= direction_masks[2];
  }
  if (is_command_pressed(GameCommand::DOWN)) {
    direction_mask |= direction_masks[3];
  }

  return masks_to_directions8[direction_mask];
}

/**
 * \brief This function is called by the game when a low-level event occurs.
 * \param event An input event.
 */
void GameCommands::notify_input(const InputEvent& event) {

  // If no game command is being customized, we look for a binding
  // for this input event and we ignore the event if no binding is found.
  // If a command is being customized, we consider instead this event as
  // the new binding for this game command.

  if (event.is_keyboard_key_pressed()) {
    keyboard_key_pressed(event.get_keyboard_key());
  }
  else if (event.is_keyboard_key_released()) {
    keyboard_key_released(event.get_keyboard_key());
  }
  else if (event.is_joypad_button_pressed()) {
    joypad_button_pressed(event.get_joypad_button());
  }
  else if (event.is_joypad_button_released()) {
    joypad_button_released(event.get_joypad_button());
  }
  else if (event.is_joypad_axis_moved()) {
    joypad_axis_moved(event.get_joypad_axis(), event.get_joypad_axis_state());
  }
  else if (event.is_joypad_hat_moved()) {
    joypad_hat_moved(event.get_joypad_hat(), event.get_joypad_hat_direction());
  }
}

/**
 * \brief This function is called when a low-level keyboard key is pressed.
 * \param keyboard_key_pressed The key pressed.
 */
void GameCommands::keyboard_key_pressed(InputEvent::KeyboardKey keyboard_key_pressed) {

  // Retrieve the game command (if any) corresponding to this keyboard key.
  const GameCommand command_pressed = get_command_from_keyboard(keyboard_key_pressed);

  if (!customizing) {
    // If the key is mapped, notify the game.
    if (command_pressed != GameCommand::NONE) {
      game_command_pressed(command_pressed);
    }
  }
  else {
    customizing = false;

    if (command_pressed != command_to_customize) {
      // Consider this keyboard key as the new mapping for the game command being customized.
      set_keyboard_binding(command_to_customize, keyboard_key_pressed);
      commands_pressed.insert(command_to_customize);
    }
    do_customization_callback();
  }
}

/**
 * \brief This function is called when a low-level keyboard key is released.
 * \param keyboard_control_released The key released.
 */
void GameCommands::keyboard_key_released(InputEvent::KeyboardKey keyboard_key_released) {

  // Retrieve the game command (if any) corresponding to this keyboard key.
  GameCommand command_released = get_command_from_keyboard(keyboard_key_released);

  // If the keyboard key is mapped, notify the game.
  if (command_released != GameCommand::NONE) {
    game_command_released(command_released);
  }
}

/**
 * \brief This function is called when a joypad button is pressed.
 * \param button The button pressed.
 */
void GameCommands::joypad_button_pressed(int button) {

  // Retrieve the game command (if any) corresponding to this joypad button.
  std::ostringstream oss;
  oss << "button " << button;
  const std::string& joypad_string = oss.str();
  GameCommand command_pressed = get_command_from_joypad(joypad_string);

  if (!customizing) {
    // If the joypad button is mapped, notify the game.
    if (command_pressed != GameCommand::NONE) {
      game_command_pressed(command_pressed);
    }
  }
  else {
    customizing = false;

    if (command_pressed != command_to_customize) {
      // Consider this button as the new mapping for the game command being customized.
      set_joypad_binding(command_to_customize, joypad_string);
      commands_pressed.insert(command_to_customize);
    }
    do_customization_callback();
  }
}

/**
 * \brief This function is called when a joypad button is released.
 * \param button The button released.
 */
void GameCommands::joypad_button_released(int button) {

  // Retrieve the game command (if any) corresponding to this joypad button.
  std::ostringstream oss;
  oss << "button " << button;
  const std::string& joypad_string = oss.str();
  GameCommand command_released = get_command_from_joypad(joypad_string);

  // If the key is mapped, notify the game.
  if (command_released != GameCommand::NONE) {
    game_command_released(command_released);
  }
}

/**
 * \brief This function is called when a joypad axis is moved.
 * \param axis The axis moved.
 * \param state The new axis direction (-1: left or up, 0: centered, 1: right or down).
 */
void GameCommands::joypad_axis_moved(int axis, int state) {

  if (state == 0) {
    // Axis in centered position.

    std::ostringstream oss;
    oss << "axis " << axis << " +";
    GameCommand command_released = get_command_from_joypad(oss.str());
    if (command_released != GameCommand::NONE) {
      game_command_released(command_released);
    }

    oss.str("");
    oss << "axis " << axis << " -";
    command_released = get_command_from_joypad(oss.str());
    if (command_released != GameCommand::NONE) {
      game_command_released(command_released);
    }
  }
  else {
    // Axis not centered.

    std::ostringstream oss;
    oss << "axis " << axis << ((state > 0) ? " +" : " -");
    const std::string& joypad_string = oss.str();

    oss.str("");
    oss << "axis " << axis << ((state > 0) ? " -" : " +");
    const std::string& inverse_joypad_string = oss.str();

    GameCommand command_pressed = get_command_from_joypad(joypad_string);
    GameCommand inverse_command_pressed = get_command_from_joypad(inverse_joypad_string);

    if (!customizing) {

      // If the command is mapped, notify the game.
      if (command_pressed != GameCommand::NONE) {
        if (is_command_pressed(inverse_command_pressed)) {
          game_command_released(inverse_command_pressed);
        }
        game_command_pressed(command_pressed);
      }
    }
    else {
      customizing = false;

      if (command_pressed != command_to_customize) {
        // Consider this axis movement as the new mapping for the game command being customized.
        set_joypad_binding(command_to_customize, joypad_string);
        commands_pressed.insert(command_to_customize);
      }
      do_customization_callback();
    }
  }
}

/**
 * \brief This function is called when a joypad hat is moved.
 * \param hat The hat moved.
 * \param direction The new hat position (-1: centered, 0 to 7: a direction).
 */
void GameCommands::joypad_hat_moved(int hat, int value) {

  if (value == -1) {
    // Hat in centered position.

    for (int i = 0; i < 4; i++) {

      std::ostringstream oss;
      oss << "hat " << hat << ' ' << direction_names[i];
      GameCommand command_released = get_command_from_joypad(oss.str());

      if (command_released != GameCommand::NONE) {
        game_command_released(command_released);
      }
    }
  }
  else {

    int direction_1 = -1;
    int direction_2 = -1;

    switch (value) {

    case 0: // right
      direction_1 = 0;
      break;

    case 1: // right-up
      direction_1 = 1;
      direction_2 = 0;
      break;

    case 2: // up
      direction_1 = 1;
      break;

    case 3: // left-up
      direction_1 = 1;
      direction_2 = 2;
      break;

    case 4: // left
      direction_1 = 2;
      break;

    case 5: // left-down
      direction_1 = 3;
      direction_2 = 2;
      break;

    case 6: // down
      direction_1 = 3;
      break;

    case 7: // right-down
      direction_1 = 3;
      direction_2 = 0;
    }

    std::ostringstream oss;
    oss << "hat " << hat << ' ' << direction_names[direction_1];
    const std::string& joypad_string_1 = oss.str();
    GameCommand command_1 = get_command_from_joypad(joypad_string_1);

    oss.str("");
    oss << "hat " << hat << ' ' << direction_names[(direction_1 + 2) % 4];
    const std::string& inverse_joypad_string_1 = oss.str();
    GameCommand inverse_command_1 = get_command_from_joypad(inverse_joypad_string_1);

    GameCommand command_2 = GameCommand::NONE;
    GameCommand inverse_command_2 = GameCommand::NONE;

    if (direction_2 != -1) {
      oss.str("");
      oss << "hat " << hat << ' ' << direction_names[direction_2];
      const std::string& joypad_string_2 = oss.str();
      command_2 = get_command_from_joypad(joypad_string_2);

      oss.str("");
      oss << "hat " << hat << ' ' << direction_names[(direction_2 + 2) % 4];
      const std::string& inverse_joypad_string_2 = oss.str();
      inverse_command_2 = get_command_from_joypad(inverse_joypad_string_2);
    }
    else {
      std::ostringstream oss;
      oss << "hat " << hat << ' ' << direction_names[(direction_1 + 1) % 4];
      const std::string& joypad_string_2 = oss.str();
      command_2 = get_command_from_joypad(joypad_string_2);

      oss.str("");
      oss << "hat " << hat << ' ' << direction_names[(direction_1 + 3) % 4];
      const std::string& inverse_joypad_string_2 = oss.str();
      inverse_command_2 = get_command_from_joypad(inverse_joypad_string_2);
    }

    if (!customizing) {

      // If the key is mapped, notify the game.
      if (command_1 != GameCommand::NONE) {

        if (is_command_pressed(inverse_command_1)) {
          game_command_released(inverse_command_1);
        }
        game_command_pressed(command_1);
      }

      if (direction_2 != -1) {
        if (is_command_pressed(inverse_command_2)) {
          game_command_released(inverse_command_2);
        }
        game_command_pressed(command_2);
      }
      else {
        if (is_command_pressed(command_2)) {
          game_command_released(command_2);
        }
        if (is_command_pressed(inverse_command_2)) {
          game_command_released(inverse_command_2);
        }
      }
    }
    else {
      customizing = false;

      if (command_1 != command_to_customize) {
        // Consider this hat movement as the new mapping for the game command being customized.
        set_joypad_binding(command_to_customize, joypad_string_1);
        commands_pressed.insert(command_to_customize);
      }
      do_customization_callback();
    }
  }
}

/**
 * \brief This function is called when a game command is pressed.
 *
 * This event may come from the keyboard or the joypad.
 *
 * \param command The game command pressed.
 */
void GameCommands::game_command_pressed(GameCommand command) {

  commands_pressed.insert(command);
  game.notify_command_pressed(command);
}

/**
 * \brief This function is called when a game command is pressed.
 *
 * This event may come from the keyboard or the joypad.
 *
 * \param command The game command released.
 */
void GameCommands::game_command_released(GameCommand command) {

  commands_pressed.erase(command);
  game.notify_command_released(command);
}

/**
 * \brief Returns the low-level keyboard key where the specified game command
 * is currently mapped.
 * \param command A game command.
 * \return The keyboard key mapped to this game command, or InputEvent::KEY_GameCommand::NONE
 * if the command is not mapped to a keyboard key.
 */
InputEvent::KeyboardKey GameCommands::get_keyboard_binding(GameCommand command) const {

  for (const auto& kvp: keyboard_mapping) {

    if (kvp.second == command) {
      return kvp.first;
    }
  }

  return InputEvent::KeyboardKey::NONE;
}

/**
 * \brief Maps the specified keyboard key to a new game command.
 *
 * If this key was already mapped to a command, both commands are switched.
 * (The old command gets the previous key of the new command.)
 *
 * \param command A game command.
 * \param key The keyboard key to map to this game command, or InputEvent::KEY_NONE
 * to unmap the command.
 */
void GameCommands::set_keyboard_binding(GameCommand command, InputEvent::KeyboardKey key) {

  InputEvent::KeyboardKey previous_key = get_keyboard_binding(command);
  GameCommand previous_command = get_command_from_keyboard(key);

  if (previous_key != InputEvent::KeyboardKey::NONE) {
    // The command was already assigned.
    if (previous_command != GameCommand::NONE) {
      // This key is already mapped to a command.
      keyboard_mapping[previous_key] = previous_command;
      set_saved_keyboard_binding(previous_command, previous_key);
    }
    else {
      keyboard_mapping.erase(previous_key);
    }
  }

  if (key != InputEvent::KeyboardKey::NONE) {
      keyboard_mapping[key] = command;
  }
  set_saved_keyboard_binding(command, key);
}

/**
 * \brief Returns a string representing the joypad action where the specified
 * game command is currently mapped.
 * \param command A game command.
 * \return The joypad action mapped to this game command, or an empty string if
 * this game command is not mapped to a joypad action.
 */
const std::string& GameCommands::get_joypad_binding(GameCommand command) const {

  for (const auto& kvp: joypad_mapping) {

    if (kvp.second == command) {
      return kvp.first;
    }
  }

  static const std::string empty_string;
  return empty_string;
}

/**
 * \brief Maps the specified joypad action to a new game command.
 *
 * If this joypad action was already mapped to a command, both commands are switched.
 * (The old command gets the previous joypad action of the new command.)
 *
 * \param command A game command.
 * \param joypad_string A string describing the joypad action to map to this
 * game command, or an empty string to unmap the command.
 */
void GameCommands::set_joypad_binding(GameCommand command, const std::string& joypad_string) {

  const std::string& previous_joypad_string = get_joypad_binding(command);
  GameCommand previous_command = get_command_from_joypad(joypad_string);

  if (!previous_joypad_string.empty()) {
    // The command was already assigned.
    if (previous_command != GameCommand::NONE) {
      // This joypad action is already mapped to a command.
      joypad_mapping[previous_joypad_string] = previous_command;
      set_saved_joypad_binding(previous_command, previous_joypad_string);
    }
    else {
      joypad_mapping.erase(previous_joypad_string);
    }
  }

  if (!joypad_string.empty()) {
    joypad_mapping[joypad_string] = command;
  }
  set_saved_joypad_binding(command, joypad_string);
}

/**
 * \brief Returns the name of the savegame variable that stores the keyboard
 * mapping of a game command.
 * \param command A game command.
 * \return The savegame variable that stores the keyboard key mapped to this
 * game command, or an empty string if this command is GameCommand::NONE.
 */
const std::string& GameCommands::get_keyboard_binding_savegame_variable(
    GameCommand command) const {

  static const std::map<GameCommand, std::string> savegame_variables = {
      { GameCommand::NONE, "" },
      { GameCommand::ACTION, Savegame::KEY_KEYBOARD_ACTION },
      { GameCommand::ATTACK, Savegame::KEY_KEYBOARD_ATTACK },
      { GameCommand::ITEM_1, Savegame::KEY_KEYBOARD_ITEM_1 },
      { GameCommand::ITEM_2, Savegame::KEY_KEYBOARD_ITEM_2 },
      { GameCommand::PAUSE, Savegame::KEY_KEYBOARD_PAUSE },
      { GameCommand::RIGHT, Savegame::KEY_KEYBOARD_RIGHT },
      { GameCommand::UP, Savegame::KEY_KEYBOARD_UP },
      { GameCommand::LEFT, Savegame::KEY_KEYBOARD_LEFT },
      { GameCommand::DOWN, Savegame::KEY_KEYBOARD_DOWN }
  };

  return savegame_variables.find(command)->second;
}

/**
 * \brief Returns the name of the savegame variable that stores the joypad
 * mapping of a game command.
 * \param command A game command.
 * \return The savegame variable that stores the joypad action mapped to this
 * game command, or an empty string if this command is GameCommand::NONE.
 */
const std::string& GameCommands::get_joypad_binding_savegame_variable(
    GameCommand command) const {

  static const std::map<GameCommand, std::string> savegame_variables = {
      { GameCommand::NONE, "" },
      { GameCommand::ACTION, Savegame::KEY_JOYPAD_ACTION },
      { GameCommand::ATTACK, Savegame::KEY_JOYPAD_ATTACK },
      { GameCommand::ITEM_1, Savegame::KEY_JOYPAD_ITEM_1 },
      { GameCommand::ITEM_2, Savegame::KEY_JOYPAD_ITEM_2 },
      { GameCommand::PAUSE, Savegame::KEY_JOYPAD_PAUSE },
      { GameCommand::RIGHT, Savegame::KEY_JOYPAD_RIGHT },
      { GameCommand::UP, Savegame::KEY_JOYPAD_UP },
      { GameCommand::LEFT, Savegame::KEY_JOYPAD_LEFT },
      { GameCommand::DOWN, Savegame::KEY_JOYPAD_DOWN }
  };

  return savegame_variables.find(command)->second;
}

/**
 * \brief Determines from the savegame the low-level keyboard key where the
 * specified game command is mapped.
 * \param command A game command.
 * \return The keyboard key mapped to this game command in the savegame.
 */
InputEvent::KeyboardKey GameCommands::get_saved_keyboard_binding(
    GameCommand command) const {

  const std::string& savegame_variable = get_keyboard_binding_savegame_variable(command);
  const std::string& keyboard_key_name = get_savegame().get_string(savegame_variable);
  return name_to_enum(keyboard_key_name, InputEvent::KeyboardKey::NONE);
}

/**
 * \brief Saves the low-level keyboard command where the specified game key is
 * mapped.
 * \param command A game command.
 * \param keyboard_key The keyboard key to map to this game command in the
 * savegame.
 */
void GameCommands::set_saved_keyboard_binding(
    GameCommand command, InputEvent::KeyboardKey keyboard_key) {

  const std::string& savegame_variable = get_keyboard_binding_savegame_variable(command);
  const std::string& keyboard_key_name = enum_to_name(keyboard_key);
  get_savegame().set_string(savegame_variable, keyboard_key_name);
}

/**
 * \brief Returns the game command (if any) associated to the specified
 * keyboard key.
 * \param key A keyboard key.
 * \return The game command mapped to that key or GameCommand::NONE.
 */
GameCommand GameCommands::get_command_from_keyboard(
    InputEvent::KeyboardKey key) const {

  const auto& it = keyboard_mapping.find(key);
  if (it != keyboard_mapping.end()) {
    return it->second;
  }

  return GameCommand::NONE;
}

/**
 * \brief Determines from the savegame the low-level joypad action where the
 * specified game command is mapped.
 * \param command A game command.
 * \return The joypad action mapped to this game command in the savegame.
 */
std::string GameCommands::get_saved_joypad_binding(
    GameCommand command) const {

  const std::string& savegame_variable = get_joypad_binding_savegame_variable(command);
  return get_savegame().get_string(savegame_variable);
}

/**
 * \brief Saves the low-level joypad action where the specified game command
 * is mapped.
 * \param command A game command.
 * \return The joypad action to map to this game command in the savegame.
 */
void GameCommands::set_saved_joypad_binding(
    GameCommand command, const std::string& joypad_string) {

  const std::string& savegame_variable = get_joypad_binding_savegame_variable(command);
  get_savegame().set_string(savegame_variable, joypad_string);
}

/**
 * \brief Returns the game command (if any) associated to the specified
 * joypad action.
 * \param joypad_string A joypad action.
 * \return The game command mapped to that joypad action or GameCommand::NONE.
 */
GameCommand GameCommands::get_command_from_joypad(
    const std::string& joypad_string) const {

  const auto& it = joypad_mapping.find(joypad_string);
  if (it != joypad_mapping.end()) {
    return it->second;
  }

  return GameCommand::NONE;
}

// customization

/**
 * \brief Sets the specified command to be customized.
 *
 * After this function is called, the next keyboard or joypad event received will
 * not be treated normally; it will be considered as the new keyboard or joypad
 * binding for this game key. Then, keyboard and joypad events will be treated
 * normally again.
 *
 * \param command The command to customize.
 * \param callback_ref Lua ref to a function to call when the customization
 * finishes, or an empty ref.
 */
void GameCommands::customize(
    GameCommand command,
    const ScopedLuaRef& callback_ref
) {
  this->customizing = true;
  this->command_to_customize = command;
  this->customize_callback_ref = callback_ref;
}

/**
 * \brief Returns whether the player is currently customizing a command.
 * \return true if the player is currently customizing a command.
 */
bool GameCommands::is_customizing() const {
  return customizing;
}

/**
 * \brief When the player is customizing a command, returns the command that
 * is being customized.
 * \return The command being customized.
 */
GameCommand GameCommands::get_command_to_customize() const {

  Debug::check_assertion(is_customizing(),
      "The player is not customizing a command");
  return command_to_customize;
}

/**
 * \brief Calls the Lua function that was registered to be called after a
 * command customization phase.
 */
void GameCommands::do_customization_callback() {

  customize_callback_ref.clear_and_call("capture command callback");
}

/**
 * \brief Returns whether a string describes a valid joypad action.
 *
 * The string should have one of the following forms:
 * - "button X" where X is the index of a joypad button.
 * - "axis X +" where X is the index of a joypad axis.
 * - "axis X -" where X is the index of a joypad axis.
 * - "hat X Y" where X is the index of a joypad hat and Y is a direction (0 to 7).
 *
 * \param joypad_string The string to check.
 * \return true if this string is a valid joypad action.
 */
bool GameCommands::is_joypad_string_valid(const std::string& /* joypad_string */) {

  // TODO
  return true;
}

/**
 * \brief Returns the name of a game command.
 * \param command a game command.
 * \return The name of this command, or an empty string if the command is GameCommand::NONE.
 */
const std::string& GameCommands::get_command_name(GameCommand command) {

  return command_names.find(command)->second;
}

/**
 * \brief Returns a game command given its Lua name.
 * \param command_name Lua name of a game command.
 * \return The corresponding game command, or GameCommand::NONE if the name is invalid.
 */
GameCommand GameCommands::get_command_by_name(
    const std::string& command_name) {

  for (const auto& kvp : command_names) {
    if (kvp.second == command_name) {
      return kvp.first;
    }
  }
  return GameCommand::NONE;
}

}

