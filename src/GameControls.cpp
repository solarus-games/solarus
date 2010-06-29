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
#include "GameControls.h"
#include "Game.h"
#include "Savegame.h"
#include "Equipment.h"
#include "DialogBox.h"
#include "StringResource.h"
#include "menus/PauseMenu.h"
#include "entities/Hero.h"

const std::string GameControls::direction_names[] = {"right", "up", "left", "down"};

const uint16_t GameControls::arrows_masks[4] = {
  0x0001,
  0x0002,
  0x0004,
  0x0008
};

/**
 * @brief Associates to each possible combination of directional_keys
 * an angle in degrees.
 *
 * The angle is between 0 and 359, or -1 to indicate
 * that the movement is stopped.
 *
 * For example:
 *   uint16_t arrows_pressed = right_mask | up_mask;
 *   int angle = directions[arrows_pressed];
 * Here the angle is 45°.
*/
static const int arrows_angles[] = {
  -1,  // none: stop
  0,   // right
  90,  // up
  45,  // right + up
  180, // left
  -1,  // left + right: stop
  135, // left + up
  -1,  // left + right + up: stop
  270, // down
  315, // down + right
  -1,  // down + up: stop
  -1,  // down + right + up: stop
  225, // down + left
  -1,  // down + left + right: stop
  -1,  // down + left + up: stop
  -1,  // down + left + right + up: stop
};

/**
 * @brief Constructor.
 * @param game the game
 */
GameControls::GameControls(Game *game):
  game(game), savegame(game->get_savegame()), customizing(false) {

  // load the controls from the savegame
  for (int i = 0; i < 9; i++) {

    GameKey game_key = (GameKey) (i + 1);

    // keyboard
    int index = Savegame::KEYBOARD_ACTION_KEY + i;
    InputEvent::KeyboardKey keyboard_symbol = InputEvent::KeyboardKey(savegame->get_integer(index));
    keyboard_mapping[keyboard_symbol] = game_key;

    // joypad
    index = Savegame::JOYPAD_ACTION_KEY + i;
    const std::string &joypad_string = savegame->get_string(index);
    joypad_mapping[joypad_string] = game_key;

    // game key state
    keys_pressed[i] = false;

    // load the game key name in the current language
    std::ostringstream oss;
    oss << "controls.game_key_" << game_key;
    game_key_names[i] = StringResource::get_string(oss.str());
  }
}

/**
 * @brief Destructor.
 */
GameControls::~GameControls(void) {

}

/**
 * @brief Returns the name of a game key.
 * @param key a game key
 * @return the corresponding name
 */
const std::string& GameControls::get_key_name(GameKey key) {
  return game_key_names[key - 1];
}

/**
 * @brief Returns a string representing the keyboard key associated to
 * the specified game key.
 * @param game_key a game key
 * @return a string representing the corresponding keyboard key
 */
const std::string GameControls::get_keyboard_string(GameKey game_key) {

  InputEvent::KeyboardKey keyboard_key = get_keyboard_key(game_key);
  std::string keyboard_string = InputEvent::get_keyboard_key_name(keyboard_key);
  return keyboard_string;
}

/**
 * @brief Returns whether the specified game key is pressed.
 *
 * The key can be pressed from the keyboard or the joypad.
 *
 * @param game_key a game key
 * @return true if the game key is currently pressed
 */
bool GameControls::is_key_pressed(GameKey game_key) {
  return keys_pressed[game_key - 1];
}

/**
 * @brief Returns the direction corresponding to the arrow keys
 * currently pressed by the player.
 * @return the arrows direction (0 to 360), or -1
 * if no arrow is pressed
 */
int GameControls::get_arrows_direction(void) {

  uint16_t arrows_mask = 0x0000;
  if (is_key_pressed(RIGHT)) {
    arrows_mask |= 0x0001;
  }
  if (is_key_pressed(UP)) {
    arrows_mask |= 0x0002;
  }
  if (is_key_pressed(LEFT)) {
    arrows_mask |= 0x0004;
  }
  if (is_key_pressed(DOWN)) {
    arrows_mask |= 0x0008;
  }
  return arrows_angles[arrows_mask];
}

/**
 * @brief This function is called by the game when a low-level input event occurs.
 * @param event an input event
 */
void GameControls::notify_event(InputEvent &event) {

  /*
   * If no game key is being customized, we look for a binding
   * for this input event and we ignore the event if no binding is found.
   * If a key is being customized, we consider instead this event as
   * the new binding for this game key.
   */

  if (event.is_keyboard_key_pressed()) {
    key_pressed(event.get_keyboard_key());
  }
  else if (event.is_keyboard_key_released()) {
    key_released(event.get_keyboard_key());
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
 * @param keyboard_key_pressed the key pressed
 */
void GameControls::key_pressed(InputEvent::KeyboardKey keyboard_key_pressed) {

  // retrieve the game key corresponding to this keyboard key
  GameKey game_key = keyboard_mapping[keyboard_key_pressed];

  if (!customizing) {

    // if the key is mapped (otherwise we just ignore it)
    if (game_key != 0) {

      // notify the game
      game_key_pressed(game_key);
    }
  }
  else {
    customizing = false;

    if (game_key != key_to_customize) {
      // consider this key as the new mapping for the game key being customized

      InputEvent::KeyboardKey previous_keyboard_key = get_keyboard_key(key_to_customize);
      if (game_key != 0) {
	// this keyboard key was already assigned to a game key
	keyboard_mapping[previous_keyboard_key] = game_key;
	int index = Savegame::KEYBOARD_ACTION_KEY + game_key - 1;
	savegame->set_integer(index, previous_keyboard_key);
      }
      else {
	keyboard_mapping.erase(previous_keyboard_key);
      }
      keyboard_mapping[keyboard_key_pressed] = key_to_customize;
      int index = Savegame::KEYBOARD_ACTION_KEY + key_to_customize - 1;
      savegame->set_integer(index, keyboard_key_pressed);

      keys_pressed[key_to_customize - 1] = true;
    }
  }
}

/**
 * @brief This function is called when a low-level keyboard key is released.
 * @param keyboard_key_released the key released
 */
void GameControls::key_released(InputEvent::KeyboardKey keyboard_key_released) {

  // retrieve the game key corresponding to this keyboard key
  GameKey game_key = keyboard_mapping[keyboard_key_released];

  // if the key is mapped (otherwise we just ignore it)
  if (game_key != 0) {

    // notify the game
    game_key_released(game_key);
  }
}

/**
 * @brief This function is called when a joypad button is pressed.
 * @param button the button pressed
 */
void GameControls::joypad_button_pressed(int button) {

  // retrieve the game key corresponding to this joypad button
  std::ostringstream oss;
  oss << "button " << button;
  const std::string &joypad_string = oss.str();
  GameKey game_key = joypad_mapping[joypad_string];

  if (!customizing) {

    // if the key is mapped (otherwise we just ignore it)
    if (game_key != 0) {

      // notify the game
      game_key_pressed(game_key);
    }
  }
  else {
    customizing = false;

    if (game_key != key_to_customize) {
      // consider this button as the new mapping for the game key being customized

      const std::string &previous_joypad_string = get_joypad_string(key_to_customize);
      if (game_key != 0) {
	// this button was already assigned to a game key
	joypad_mapping[previous_joypad_string] = game_key;
	int index = Savegame::JOYPAD_ACTION_KEY + game_key - 1;
	savegame->set_string(index, previous_joypad_string);
      }
      else {
	joypad_mapping.erase(previous_joypad_string);
      }
      joypad_mapping[joypad_string] = key_to_customize;
      int index = Savegame::JOYPAD_ACTION_KEY + key_to_customize - 1;
      savegame->set_string(index, joypad_string);

      keys_pressed[key_to_customize - 1] = true;
    }
  }
}

/**
 * @brief This function is called when a joypad button is released.
 * @param button the button released
 */
void GameControls::joypad_button_released(int button) {

  // retrieve the game key corresponding to this joypad button
  std::ostringstream oss;
  oss << "button " << button;
  GameKey game_key = joypad_mapping[oss.str()];

  // if the key is mapped (otherwise we just ignore it)
  if (game_key != 0) {

    // notify the game
    game_key_released(game_key);
  }
}

/**
 * @brief This function is called when a joypad axis is moved.
 * @param axis the axis moved
 * @param state the new axis direction (-1: left or up, 0: centered, 1: right or down)
 */
void GameControls::joypad_axis_moved(int axis, int state) {

  if (state == 0) {
    // axis in centered position

    std::ostringstream oss_1;
    oss_1 << "axis " << axis << " +";
    GameKey game_key_1 = joypad_mapping[oss_1.str()];

    if (game_key_1 != 0) {
      game_key_released(game_key_1);
    }

    std::ostringstream oss_2;
    oss_2 << "axis " << axis << " -";
    GameKey game_key_2 = joypad_mapping[oss_2.str()];

    if (game_key_2 != 0) {
      game_key_released(game_key_2);
    }
  }
  else {
    // axis not centered 

    std::ostringstream oss;
    oss << "axis " << axis << ((state > 0) ? " +" : " -");
    const std::string &joypad_string = oss.str();

    std::ostringstream oss2;
    oss2 << "axis " << axis << ((state > 0) ? " -" : " +");
    const std::string &inverse_joypad_string = oss2.str();

    GameKey game_key = joypad_mapping[joypad_string];
    GameKey inverse_game_key = joypad_mapping[inverse_joypad_string];

    if (!customizing) {

      // if the key is mapped (otherwise we just ignore it)
      if (game_key != 0) {

	// notify the game
	if (is_key_pressed(inverse_game_key)) {
	  game_key_released(inverse_game_key);
	}
	game_key_pressed(game_key);
      }
    }
    else {
      customizing = false;

      if (game_key != key_to_customize) {
	// consider this axis movement as the new mapping for the game key being customized

	const std::string &previous_joypad_string = get_joypad_string(key_to_customize);
	if (game_key != 0) {
	  // this axis movement was already assigned to a game key
	  joypad_mapping[previous_joypad_string] = game_key;
	  int index = Savegame::JOYPAD_ACTION_KEY + game_key - 1;
	  savegame->set_string(index, previous_joypad_string);
	}
	else {
	  joypad_mapping.erase(previous_joypad_string);
	}
	joypad_mapping[joypad_string] = key_to_customize;
	int index = Savegame::JOYPAD_ACTION_KEY + key_to_customize - 1;
	savegame->set_string(index, joypad_string);

	keys_pressed[key_to_customize - 1] = true;
      }
    }
  }
}

/**
 * @brief This function is called when a joypad has is moved.
 * @param hat the hat moved
 * @param direction the new hat position (-1: centered, 0 to 7: a direction)
 */
void GameControls::joypad_hat_moved(int hat, int value) {

  // hat in centered position
  if (value == -1) {

    for (int i = 0; i < 4; i++) {

      std::ostringstream oss;
      oss << "hat " << hat << ' ' << direction_names[i];
      GameKey game_key = joypad_mapping[oss.str()];

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
    GameKey game_key = joypad_mapping[joypad_string];

    std::ostringstream oss_inv;
    oss_inv << "hat " << hat << ' ' << direction_names[(direction_1 + 2) % 4];
    const std::string &inverse_joypad_string = oss_inv.str();
    GameKey inverse_game_key = joypad_mapping[inverse_joypad_string];

    GameKey game_key_2 = NONE;
    GameKey inverse_game_key_2 = NONE;

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
	  int index = Savegame::JOYPAD_ACTION_KEY + game_key - 1;
	  savegame->set_string(index, previous_joypad_string);
	}
	else {
	  joypad_mapping.erase(previous_joypad_string);
	}
	joypad_mapping[joypad_string] = key_to_customize;
	int index = Savegame::JOYPAD_ACTION_KEY + key_to_customize - 1;
	savegame->set_string(index, joypad_string);

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
void GameControls::game_key_pressed(GameKey key) {

  int index = key - 1;
  if (keys_pressed[index]) {
    return;
  }

  keys_pressed[index] = true;
  game->key_pressed(key);
}

/**
 * @brief This function is called when a game key is pressed.
 *
 * This event may come from the keyboard or the joypad.
 *
 * @param key the game key pressed
 */
void GameControls::game_key_released(GameKey key) {

  int index = key - 1;
  if (!keys_pressed[index]) {
    return;
  }

  keys_pressed[index] = false;
  game->key_released(key);
}


/**
 * @brief Returns the low-level keyboard key where the specified game key
 * is currently mapped.
 * @param game_key a game key
 * @return the keyboard key corresponding this game key
 */
InputEvent::KeyboardKey GameControls::get_keyboard_key(GameKey game_key) {

  bool found = false;
  InputEvent::KeyboardKey keyboard_key = InputEvent::KEY_NONE;
  std::map<InputEvent::KeyboardKey, GameKey>::const_iterator it;
  for (it = keyboard_mapping.begin(); it != keyboard_mapping.end(); it++) {

    if (it->second == game_key) {
      keyboard_key = it->first;
      found = true;
    }
  }

  if (!found) {
    DIE("No keyboard key is defined for game key '" << get_key_name(game_key) << "'");
  }

  return keyboard_key;
}

/**
 * @brief Returns a string representing the joypad action where the
 * specified game key is currently mapped.
 * @param game_key a game key
 * @return the joypad action corresponding this game key
 */
const std::string & GameControls::get_joypad_string(GameKey game_key) {

  std::map<std::string, GameKey>::const_iterator it;
  for (it = joypad_mapping.begin(); it != joypad_mapping.end(); it++) {

    if (it->second == game_key) {
      return it->first;
    }
  }

  DIE("No joypad action is defined for game key '" << get_key_name(game_key) << "'");
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
void GameControls::customize(GameKey key) {
  this->customizing = true;
  this->key_to_customize = key;
}

/**
 * @brief Returns whether the player is currently customizing a key.
 * @return true if the player is currently customizing a key 
 */
bool GameControls::is_customizing(void) {
  return customizing;
}

/**
 * @brief When the player is customizing a key, returns the key that is being customized.
 * @return the key being customize
 */
GameControls::GameKey GameControls::get_key_to_customize(void) {
  if (!is_customizing()) {
    DIE("The player is not customizing a key");
  }
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
bool GameControls::is_customization_done(void) {
  return !customizing;
}

