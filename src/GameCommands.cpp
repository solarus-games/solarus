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
#include "GameCommands.h"
#include "Game.h"
#include "Savegame.h"
#include "Equipment.h"
#include "DialogBox.h"
#include "StringResource.h"
#include "menus/PauseMenu.h"
#include "entities/Hero.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"

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
 * @brief Constructor.
 * @param game the game
 */
GameCommands::GameCommands(Game& game):
  game(game),
  customizing(false),
  command_to_customize(NONE){

  // Load the commands from the savegame.
  for (int i = 0; i < NB_COMMANDS; i++) {

    GameCommand command = GameCommand(i);

    // Keyboard.
    InputEvent::KeyboardKey keyboard_key = get_saved_keyboard_binding(command);
    keyboard_mapping[keyboard_key] = command;

    // Joypad.
    const std::string& joypad_string = get_saved_joypad_binding(command);
    joypad_mapping[joypad_string] = command;

    // State of game commands.
    commands_pressed[command] = false;
  }
}

/**
 * @brief Destructor.
 */
GameCommands::~GameCommands() {
}

/**
 * @brief Returns the savegame.
 * @return The savegame.
 */
Savegame& GameCommands::get_savegame() {
  return game.get_savegame();
}

/**
 * @brief Returns whether the specified game command is pressed.
 *
 * The command can be activated from the keyboard or the joypad.
 *
 * @param command A game command.
 * @return true if this game command is currently pressed.
 */
bool GameCommands::is_command_pressed(GameCommand command) {
  return commands_pressed[command];
}

/**
 * @brief Returns the direction corresponding to the directional commands
 * currently pressed by the player.
 * @return The direction (0 to 7), or -1 if no directional command is pressed
 * or if the combination of directional command is not a valid direction.
 */
int GameCommands::get_wanted_direction8() {

  uint16_t direction_mask = 0x0000;
  if (is_command_pressed(RIGHT)) {
    direction_mask |= direction_masks[0];
  }
  if (is_command_pressed(UP)) {
    direction_mask |= direction_masks[1];
  }
  if (is_command_pressed(LEFT)) {
    direction_mask |= direction_masks[2];
  }
  if (is_command_pressed(DOWN)) {
    direction_mask |= direction_masks[3];
  }

  return masks_to_directions8[direction_mask];
}

/**
 * @brief This function is called by the game when a low-level event occurs.
 * @param event An input event.
 */
void GameCommands::notify_input(InputEvent& event) {

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
 * @brief This function is called when a low-level keyboard key is pressed.
 * @param keyboard_key_pressed The key pressed.
 */
void GameCommands::keyboard_key_pressed(InputEvent::KeyboardKey keyboard_key_pressed) {

  // Retrieve the game command (if any) corresponding to this keyboard key.
  GameCommand command_pressed = NONE;
  if (keyboard_mapping.find(keyboard_key_pressed) != keyboard_mapping.end()) {
    command_pressed = keyboard_mapping[keyboard_key_pressed];
  }

  if (!customizing) {
    // If the key is mapped, notify the game.
    if (command_pressed != NONE) {
      game_command_pressed(command_pressed);
    }
  }
  else {
    customizing = false;

    if (command_pressed != command_to_customize) {
      // Consider this keyboard key as the new mapping for the game command being customized.

      InputEvent::KeyboardKey previous_keyboard_key = get_keyboard_binding(command_to_customize);
      if (command_pressed != NONE) {
        // This keyboard key is already assigned to a game command.
        keyboard_mapping[previous_keyboard_key] = command_pressed;
        set_saved_keyboard_binding(command_pressed, keyboard_key_pressed);
      }
      else {
        keyboard_mapping.erase(previous_keyboard_key);
      }
      keyboard_mapping[keyboard_key_pressed] = command_to_customize;
      set_saved_keyboard_binding(command_to_customize, keyboard_key_pressed);

      commands_pressed[command_to_customize] = true;
    }
  }
}

/**
 * @brief This function is called when a low-level keyboard key is released.
 * @param keyboard_control_released The key released.
 */
void GameCommands::keyboard_key_released(InputEvent::KeyboardKey keyboard_key_released) {

  // Retrieve the game command (if any) corresponding to this keyboard key.
  GameCommand command_released = NONE;
  if (keyboard_mapping.find(keyboard_key_released) != keyboard_mapping.end()) {
    command_released = keyboard_mapping[keyboard_key_released];
  }

  // If the keyboard key is mapped, notify the game.
  if (command_released != 0) {
    game_command_released(command_released);
  }
}

/**
 * @brief This function is called when a joypad button is pressed.
 * @param button The button pressed.
 */
void GameCommands::joypad_button_pressed(int button) {

  // Retrieve the game command (if any) corresponding to this joypad button.
  std::ostringstream oss;
  oss << "button " << button;
  const std::string& joypad_string = oss.str();
  GameCommand command_pressed = NONE;
  if (joypad_mapping.find(joypad_string) != joypad_mapping.end()) {
    command_pressed = joypad_mapping[joypad_string];
  }

  if (!customizing) {
    // If the joypad button is mapped, notify the game.
    if (command_pressed != 0) {
      game_command_pressed(command_pressed);
    }
  }
  else {
    customizing = false;

    if (command_pressed != command_to_customize) {
      // Consider this button as the new mapping for the game command being customized.

      const std::string& previous_joypad_string = get_joypad_binding(command_to_customize);
      if (command_pressed != NONE) {
        // This button is already assigned to a game command.
        joypad_mapping[previous_joypad_string] = command_pressed;
        set_saved_joypad_binding(command_pressed, previous_joypad_string);
      }
      else {
        joypad_mapping.erase(previous_joypad_string);
      }
      joypad_mapping[joypad_string] = command_to_customize;
      set_saved_joypad_binding(command_to_customize, joypad_string);

      commands_pressed[command_to_customize] = true;
    }
  }
}

/**
 * @brief This function is called when a joypad button is released.
 * @param button The button released.
 */
void GameCommands::joypad_button_released(int button) {

  // Retrieve the game command (if any) corresponding to this joypad button.
  std::ostringstream oss;
  oss << "button " << button;
  const std::string& joypad_string = oss.str();
  GameCommand command_released = NONE;
  if (joypad_mapping.find(joypad_string) != joypad_mapping.end()) {
    command_released = joypad_mapping[joypad_string];
  }

  // If the key is mapped, notify the game.
  if (command_released != 0) {
    game_command_released(command_released);
  }
}

/**
 * @brief This function is called when a joypad axis is moved.
 * @param axis The axis moved.
 * @param state The new axis direction (-1: left or up, 0: centered, 1: right or down).
 */
void GameCommands::joypad_axis_moved(int axis, int state) {

  if (state == 0) {
    // Axis in centered position.

    std::ostringstream oss;
    oss << "axis " << axis << " +";
    GameCommand command_released = NONE;
    if (joypad_mapping.find(oss.str()) != joypad_mapping.end()) {
      command_released = joypad_mapping[oss.str()];
    }
    if (command_released != NONE) {
      game_command_released(command_released);
    }

    oss.str("");
    oss << "axis " << axis << " -";
    command_released = NONE;
    if (joypad_mapping.find(oss.str()) != joypad_mapping.end()) {
      command_released = joypad_mapping[oss.str()];
    }
    if (command_released != NONE) {
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

    GameCommand command_pressed = joypad_mapping[joypad_string];
    GameCommand inverse_command_pressed = joypad_mapping[inverse_joypad_string];

    if (!customizing) {

      // If the command is mapped, notify the game.
      if (command_pressed != NONE) {
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

        const std::string& previous_joypad_string = get_joypad_binding(command_to_customize);
        if (command_pressed != NONE) {
          // This axis movement is already assigned to a game command.
          joypad_mapping[previous_joypad_string] = command_pressed;
          set_saved_joypad_binding(command_pressed, previous_joypad_string);
        }
        else {
          joypad_mapping.erase(previous_joypad_string);
        }
        joypad_mapping[joypad_string] = command_to_customize;
        set_saved_joypad_binding(command_to_customize, joypad_string);

        commands_pressed[command_to_customize] = true;
      }
    }
  }
}

/**
 * @brief This function is called when a joypad hat is moved.
 * @param hat The hat moved.
 * @param direction The new hat position (-1: centered, 0 to 7: a direction).
 */
void GameCommands::joypad_hat_moved(int hat, int value) {

  // hat in centered position
  if (value == -1) {

    for (int i = 0; i < 4; i++) {

      std::ostringstream oss;
      oss << "hat " << hat << ' ' << direction_names[i];
      GameCommand game_key = joypad_mapping[oss.str()];

      if (game_key != 0) {
        game_key_released(game_key);
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
    const std::string &joypad_string = oss.str();
    GameCommand game_key = joypad_mapping[joypad_string];

    std::ostringstream oss_inv;
    oss_inv << "hat " << hat << ' ' << direction_names[(direction_1 + 2) % 4];
    const std::string &inverse_joypad_string = oss_inv.str();
    GameCommand inverse_game_key = joypad_mapping[inverse_joypad_string];

    GameCommand game_key_2 = NONE;
    GameCommand inverse_game_key_2 = NONE;

    if (direction_2 != -1) {
      std::ostringstream oss;
      oss << "hat " << hat << ' ' << direction_names[direction_2];
      const std::string &joypad_string_2 = oss.str();
      game_key_2 = joypad_mapping[joypad_string_2];

      std::ostringstream oss_inv;
      oss_inv << "hat " << hat << ' ' << direction_names[(direction_2 + 2) % 4];
      const std::string &inverse_joypad_string_2 = oss_inv.str();
      inverse_game_key_2 = joypad_mapping[inverse_joypad_string_2];
    }
    else {
      std::ostringstream oss;
      oss << "hat " << hat << ' ' << direction_names[(direction_1 + 1) % 4];
      const std::string &joypad_string_2 = oss.str();
      game_key_2 = joypad_mapping[joypad_string_2];

      std::ostringstream oss_inv;
      oss_inv << "hat " << hat << ' ' << direction_names[(direction_1 + 3) % 4];
      const std::string &inverse_joypad_string_2 = oss_inv.str();
      inverse_game_key_2 = joypad_mapping[inverse_joypad_string_2];
    }

    if (!customizing) {

      // if the key is mapped (otherwise we just ignore it)
      if (game_key != 0) {

	// notify the game
	if (is_key_pressed(inverse_game_key)) {
	  game_key_released(inverse_game_key);
	}
	game_key_pressed(game_key);
      }

      if (direction_2 != -1) {
        if (is_key_pressed(inverse_game_key_2)) {
          game_key_released(inverse_game_key_2);
        }
        game_key_pressed(game_key_2);
      }
      else {
        if (is_key_pressed(game_key_2)) {
          game_key_released(game_key_2);
        }
        if (is_key_pressed(inverse_game_key_2)) {
          game_key_released(inverse_game_key_2);
        }
      }
    }
    else {
      customizing = false;

      if (game_key != key_to_customize) {
	// consider this hat movement as the new mapping for the game key being customized

        const std::string &previous_joypad_string = get_joypad_string(key_to_customize);
        if (game_key != 0) {
          // this hat movement was already assigned to a game key
          joypad_mapping[previous_joypad_string] = game_key;
          set_saved_joypad_string(game_key, previous_joypad_string);
        }
        else {
          joypad_mapping.erase(previous_joypad_string);
        }
        joypad_mapping[joypad_string] = key_to_customize;
        set_saved_joypad_string(key_to_customize, joypad_string);

        keys_pressed[key_to_customize - 1] = true;
      }
    }
  }
}

/**
 * @brief This function is called when a game key is pressed.
 *
 * This event may come from the keyboard or the joypad.
 *
 * @param key the game key pressed
 */
void GameCommands::game_key_pressed(GameCommand key) {

  int index = key - 1;
  if (keys_pressed[index]) {
    return;
  }

  keys_pressed[index] = true;
  game.key_pressed(key);
}

/**
 * @brief This function is called when a game key is pressed.
 *
 * This event may come from the keyboard or the joypad.
 *
 * @param key the game key pressed
 */
void GameCommands::game_key_released(GameCommand key) {

  int index = key - 1;
  if (!keys_pressed[index]) {
    return;
  }

  keys_pressed[index] = false;
  game.key_released(key);
}


/**
 * @brief Returns the low-level keyboard key where the specified game key
 * is currently mapped.
 * @param game_key a game key
 * @return the keyboard key corresponding this game key
 */
InputEvent::KeyboardKey GameCommands::get_keyboard_key(GameCommand game_key) {

  std::map<InputEvent::KeyboardKey, GameCommand>::const_iterator it;
  for (it = keyboard_mapping.begin(); it != keyboard_mapping.end(); it++) {

    if (it->second == game_key) {
      return it->first;
    }
  }

  Debug::die(StringConcat() << "No keyboard key is defined for game key '" << get_key_name(game_key) << "'");
  return InputEvent::KEY_NONE;
}

/**
 * @brief Returns a string representing the keyboard key associated to the
 * specified game command.
 * @param game_command A game command.
 * @return A string representing the associated low-level keyboard key.
 */
const std::string& GameCommands::get_keyboard_string(GameCommand game_key) {

  InputEvent::KeyboardKey keyboard_key = get_keyboard_binding(game_key);
  return InputEvent::get_keyboard_key_name(keyboard_key);
}

/**
 * @brief Returns a string representing the joypad action where the
 * specified game key is currently mapped.
 * @param game_key a game key
 * @return the joypad action corresponding this game key
 */
const std::string& GameCommands::get_joypad_string(GameCommand game_key) {

  std::map<std::string, GameCommand>::const_iterator it;
  for (it = joypad_mapping.begin(); it != joypad_mapping.end(); it++) {

    if (it->second == game_key) {
      return it->first;
    }
  }

  Debug::die(StringConcat() << "No joypad action is defined for game key '" << get_key_name(game_key) << "'");
  return "";
}

/**
 * @brief Returns the name of the savegame variable that stores the keyboard
 * mapping of a game key.
 * @param game_key A game key.
 * @return The savegame variable that stores the keyboard key for this game key.
 */
const std::string& GameCommands::get_keyboard_key_savegame_variable(GameCommand game_key) {

  static const std::string savegame_variables[] = {
      "",
      Savegame::KEY_KEYBOARD_ACTION,
      Savegame::KEY_KEYBOARD_SWORD,
      Savegame::KEY_KEYBOARD_ITEM_1,
      Savegame::KEY_KEYBOARD_ITEM_2,
      Savegame::KEY_KEYBOARD_PAUSE,
      Savegame::KEY_KEYBOARD_RIGHT,
      Savegame::KEY_KEYBOARD_UP,
      Savegame::KEY_KEYBOARD_LEFT,
      Savegame::KEY_KEYBOARD_DOWN
  };

  return savegame_variables[game_key];
}

/**
 * @brief Returns the name of the savegame variable that stores the joypad
 * mapping of a game key.
 * @param game_key A game key.
 * @return The savegame variable that stores the joypad action for this game key.
 */
const std::string& GameCommands::get_joypad_string_savegame_variable(GameCommand game_key) {

  static const std::string savegame_variables[] = {
      "",
      Savegame::KEY_JOYPAD_ACTION,
      Savegame::KEY_JOYPAD_SWORD,
      Savegame::KEY_JOYPAD_ITEM_1,
      Savegame::KEY_JOYPAD_ITEM_2,
      Savegame::KEY_JOYPAD_PAUSE,
      Savegame::KEY_JOYPAD_RIGHT,
      Savegame::KEY_JOYPAD_UP,
      Savegame::KEY_JOYPAD_LEFT,
      Savegame::KEY_JOYPAD_DOWN
  };

  return savegame_variables[game_key];
}

/**
 * @brief Determines from the savegame the low-level keyboard key where the
 * specified game key is mapped.
 * @param game_key A game key.
 * @return The keyboard key corresponding this game key in the savegame.
 */
InputEvent::KeyboardKey GameCommands::get_saved_keyboard_key(
    GameCommand game_key) {

  const std::string& savegame_variable = get_keyboard_key_savegame_variable(game_key);
  const std::string& keyboard_key_name = savegame.get_string(savegame_variable);
  return InputEvent::get_keyboard_key_by_name(keyboard_key_name);
}

/**
 * @brief Saves the low-level keyboard key where the specified game key is mapped.
 * @param game_key A game key.
 * @return The keyboard key corresponding this game key in the savegame.
 */
void GameCommands::set_saved_keyboard_key(
    GameCommand game_key, InputEvent::KeyboardKey key) {

  const std::string& savegame_variable = get_keyboard_key_savegame_variable(game_key);
  const std::string& keyboard_key_name = InputEvent::get_keyboard_key_name(key);
  savegame.set_string(savegame_variable, keyboard_key_name);
}

/**
 * @brief Determines from the savegame the low-level joypad action where the
 * specified game key is mapped.
 * @param game_key A game key.
 * @return The joypad action corresponding this game key in the savegame.
 */
const std::string& GameCommands::get_saved_joypad_string(
    GameCommand game_key) {

  const std::string& savegame_variable = get_joypad_string_savegame_variable(game_key);
  return savegame.get_string(savegame_variable);
}

/**
 * @brief Saves the low-level joypad action where the specified game key is mapped.
 * @param game_key A game key.
 * @return The joypad action corresponding this game key in the savegame.
 */
void GameCommands::set_saved_joypad_string(
    GameCommand game_key, const std::string& joypad_string) {

  const std::string& savegame_variable = get_joypad_string_savegame_variable(game_key);
  savegame.set_string(savegame_variable, joypad_string);
}

// customization

/**
 * @brief Notifies the control manager that the specified key is going to be customized.
 *
 * After this function is called, the next keyboard or joypad event received will
 * not be treated normally; it will be considered as the new keyboard or joypad
 * binding for this game key. Then, the keyboard and joypad events will be treated
 * normally again. Call is_customization_done() to know when this happens.
 *
 * @param key the game key to customize
 */
void GameCommands::customize(GameCommand key) {
  this->customizing = true;
  this->key_to_customize = key;
}

/**
 * @brief Returns whether the player is currently customizing a key.
 * @return true if the player is currently customizing a key 
 */
bool GameCommands::is_customizing() {
  return customizing;
}

/**
 * @brief When the player is customizing a key, returns the key that is being customized.
 * @return the key being customize
 */
GameCommands::GameCommand GameCommands::get_key_to_customize() {

  Debug::check_assertion(is_customizing(), "The player is not customizing a key");
  return key_to_customize;
}

/**
 * @brief Returns whether the customization process started by calling the customize()
 * function is done.
 *
 * This function returns whether the key has received a new binding
 * and the controls are now treated normally again.
 *
 * @return true if no game key is being customized
 */
bool GameCommands::is_customization_done() {
  return !customizing;
}

