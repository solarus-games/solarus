/*
 * Copyright (C) 2006-2013 Christopho, Solarus - http://www.solarus-games.org
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
#include "lowlevel/InputEvent.h"
#include "lowlevel/VideoManager.h"
#include <cstdlib>  // std::abs

const InputEvent::KeyboardKey InputEvent::directional_keys[] = {
    KEY_RIGHT,
    KEY_UP,
    KEY_LEFT,
    KEY_DOWN,
    KEY_NONE
};
bool InputEvent::joypad_enabled = false;
SDL_Joystick* InputEvent::joystick = NULL;
std::map<InputEvent::KeyboardKey, std::string> InputEvent::keyboard_key_names;
bool InputEvent::repeat_keyboard = false;

/**
 * \brief Initializes the input event manager.
 */
void InputEvent::initialize() {

  // Initialize text events.
  SDL_StartTextInput();

  // Initialize the joypad.
  set_joypad_enabled(true);

  // Initialize the map of keyboard key names.
  keyboard_key_names[InputEvent::KEY_NONE]                  = "";
  keyboard_key_names[InputEvent::KEY_BACKSPACE]             = "backspace";
  keyboard_key_names[InputEvent::KEY_TABULATION]            = "tab";
  keyboard_key_names[InputEvent::KEY_CLEAR]                 = "clear";
  keyboard_key_names[InputEvent::KEY_RETURN]                = "return";
  keyboard_key_names[InputEvent::KEY_PAUSE]                 = "pause";
  keyboard_key_names[InputEvent::KEY_ESCAPE]                = "escape";
  keyboard_key_names[InputEvent::KEY_SPACE]                 = "space";
  keyboard_key_names[InputEvent::KEY_EXCLAMATION_MARK]      = "!";
  keyboard_key_names[InputEvent::KEY_DOULE_QUOTE]           = "\"";
  keyboard_key_names[InputEvent::KEY_HASH]                  = "#";
  keyboard_key_names[InputEvent::KEY_DOLLAR]                = "$";
  keyboard_key_names[InputEvent::KEY_AMPERSAND]             = "&";
  keyboard_key_names[InputEvent::KEY_SINGLE_QUOTE]          = "'";
  keyboard_key_names[InputEvent::KEY_LEFT_PARENTHESIS]      = "(";
  keyboard_key_names[InputEvent::KEY_RIGHT_PARENTHESIS]     = ")";
  keyboard_key_names[InputEvent::KEY_ASTERISK]              = "*";
  keyboard_key_names[InputEvent::KEY_PLUS]                  = "+";
  keyboard_key_names[InputEvent::KEY_COMMA]                 = ",";
  keyboard_key_names[InputEvent::KEY_MINUS]                 = "-";
  keyboard_key_names[InputEvent::KEY_PERIOD]                = ".";
  keyboard_key_names[InputEvent::KEY_SLASH]                 = "/";
  keyboard_key_names[InputEvent::KEY_0]                     = "0";
  keyboard_key_names[InputEvent::KEY_1]                     = "1";
  keyboard_key_names[InputEvent::KEY_2]                     = "2";
  keyboard_key_names[InputEvent::KEY_3]                     = "3";
  keyboard_key_names[InputEvent::KEY_4]                     = "4";
  keyboard_key_names[InputEvent::KEY_5]                     = "5";
  keyboard_key_names[InputEvent::KEY_6]                     = "6";
  keyboard_key_names[InputEvent::KEY_7]                     = "7";
  keyboard_key_names[InputEvent::KEY_8]                     = "8";
  keyboard_key_names[InputEvent::KEY_9]                     = "9";
  keyboard_key_names[InputEvent::KEY_COLON]                 = ".";
  keyboard_key_names[InputEvent::KEY_SEMICOLON]             = ":";
  keyboard_key_names[InputEvent::KEY_LESS]                  = "<";
  keyboard_key_names[InputEvent::KEY_EQUALS]                = "=";
  keyboard_key_names[InputEvent::KEY_GREATER]               = ">";
  keyboard_key_names[InputEvent::KEY_QUESTION_MARK]         = "?";
  keyboard_key_names[InputEvent::KEY_AT]                    = "@";
  keyboard_key_names[InputEvent::KEY_LEFT_BRACKET]          = "[";
  keyboard_key_names[InputEvent::KEY_BACKSLASH]             = "\\";
  keyboard_key_names[InputEvent::KEY_RIGHT_BRACKET]         = "]";
  keyboard_key_names[InputEvent::KEY_CARET]                 = "^";
  keyboard_key_names[InputEvent::KEY_UNDERSCORE]            = "_";
  keyboard_key_names[InputEvent::KEY_BACKQUOTE]             = "`";
  keyboard_key_names[InputEvent::KEY_a]                     = "a";
  keyboard_key_names[InputEvent::KEY_b]                     = "b";
  keyboard_key_names[InputEvent::KEY_c]                     = "c";
  keyboard_key_names[InputEvent::KEY_d]                     = "d";
  keyboard_key_names[InputEvent::KEY_e]                     = "e";
  keyboard_key_names[InputEvent::KEY_f]                     = "f";
  keyboard_key_names[InputEvent::KEY_g]                     = "g";
  keyboard_key_names[InputEvent::KEY_h]                     = "h";
  keyboard_key_names[InputEvent::KEY_i]                     = "i";
  keyboard_key_names[InputEvent::KEY_j]                     = "j";
  keyboard_key_names[InputEvent::KEY_k]                     = "k";
  keyboard_key_names[InputEvent::KEY_l]                     = "l";
  keyboard_key_names[InputEvent::KEY_m]                     = "m";
  keyboard_key_names[InputEvent::KEY_n]                     = "n";
  keyboard_key_names[InputEvent::KEY_o]                     = "o";
  keyboard_key_names[InputEvent::KEY_p]                     = "p";
  keyboard_key_names[InputEvent::KEY_q]                     = "q";
  keyboard_key_names[InputEvent::KEY_r]                     = "r";
  keyboard_key_names[InputEvent::KEY_s]                     = "s";
  keyboard_key_names[InputEvent::KEY_t]                     = "t";
  keyboard_key_names[InputEvent::KEY_u]                     = "u";
  keyboard_key_names[InputEvent::KEY_v]                     = "v";
  keyboard_key_names[InputEvent::KEY_w]                     = "w";
  keyboard_key_names[InputEvent::KEY_x]                     = "x";
  keyboard_key_names[InputEvent::KEY_y]                     = "y";
  keyboard_key_names[InputEvent::KEY_z]                     = "z";
  keyboard_key_names[InputEvent::KEY_DELETE]                = "delete";
  keyboard_key_names[InputEvent::KEY_KP0]                   = "kp 0";
  keyboard_key_names[InputEvent::KEY_KP1]                   = "kp 1";
  keyboard_key_names[InputEvent::KEY_KP2]                   = "kp 2";
  keyboard_key_names[InputEvent::KEY_KP3]                   = "kp 3";
  keyboard_key_names[InputEvent::KEY_KP4]                   = "kp 4";
  keyboard_key_names[InputEvent::KEY_KP5]                   = "kp 5";
  keyboard_key_names[InputEvent::KEY_KP6]                   = "kp 6";
  keyboard_key_names[InputEvent::KEY_KP7]                   = "kp 7";
  keyboard_key_names[InputEvent::KEY_KP8]                   = "kp 8";
  keyboard_key_names[InputEvent::KEY_KP9]                   = "kp 9";
  keyboard_key_names[InputEvent::KEY_KP_PERIOD]             = "kp .";
  keyboard_key_names[InputEvent::KEY_KP_DIVIDE]             = "kp /";
  keyboard_key_names[InputEvent::KEY_KP_MULTIPLY]           = "kp *";
  keyboard_key_names[InputEvent::KEY_KP_MINUS]              = "kp -";
  keyboard_key_names[InputEvent::KEY_KP_PLUS]               = "kp +";
  keyboard_key_names[InputEvent::KEY_KP_ENTER]              = "kp return";
  keyboard_key_names[InputEvent::KEY_KP_EQUALS]             = "kp =";
  keyboard_key_names[InputEvent::KEY_UP]                    = "up";
  keyboard_key_names[InputEvent::KEY_DOWN]                  = "down";
  keyboard_key_names[InputEvent::KEY_RIGHT]                 = "right";
  keyboard_key_names[InputEvent::KEY_LEFT]                  = "left";
  keyboard_key_names[InputEvent::KEY_INSERT]                = "insert";
  keyboard_key_names[InputEvent::KEY_HOME]                  = "home";
  keyboard_key_names[InputEvent::KEY_END]                   = "end";
  keyboard_key_names[InputEvent::KEY_PAGE_UP]               = "page up";
  keyboard_key_names[InputEvent::KEY_PAGE_DOWN]             = "page down";
  keyboard_key_names[InputEvent::KEY_F1]                    = "f1";
  keyboard_key_names[InputEvent::KEY_F2]                    = "f2";
  keyboard_key_names[InputEvent::KEY_F3]                    = "f3";
  keyboard_key_names[InputEvent::KEY_F4]                    = "f4";
  keyboard_key_names[InputEvent::KEY_F5]                    = "f5";
  keyboard_key_names[InputEvent::KEY_F6]                    = "f6";
  keyboard_key_names[InputEvent::KEY_F7]                    = "f7";
  keyboard_key_names[InputEvent::KEY_F8]                    = "f8";
  keyboard_key_names[InputEvent::KEY_F9]                    = "f9";
  keyboard_key_names[InputEvent::KEY_F10]                   = "f10";
  keyboard_key_names[InputEvent::KEY_F11]                   = "f11";
  keyboard_key_names[InputEvent::KEY_F12]                   = "f12";
  keyboard_key_names[InputEvent::KEY_F13]                   = "f13";
  keyboard_key_names[InputEvent::KEY_F14]                   = "f14";
  keyboard_key_names[InputEvent::KEY_F15]                   = "f15";
  keyboard_key_names[InputEvent::KEY_NUMLOCK]               = "num lock";
  keyboard_key_names[InputEvent::KEY_CAPSLOCK]              = "caps lock";
  keyboard_key_names[InputEvent::KEY_SCROLLOCK]             = "scroll lock";
  keyboard_key_names[InputEvent::KEY_RIGHT_SHIFT]           = "right shift";
  keyboard_key_names[InputEvent::KEY_LEFT_SHIFT]            = "left shift";
  keyboard_key_names[InputEvent::KEY_RIGHT_CONTROL]         = "right control";
  keyboard_key_names[InputEvent::KEY_LEFT_CONTROL]          = "left control";
  keyboard_key_names[InputEvent::KEY_RIGHT_ALT]             = "right alt";
  keyboard_key_names[InputEvent::KEY_LEFT_ALT]              = "left alt";
  keyboard_key_names[InputEvent::KEY_RIGHT_META]            = "right meta";
  keyboard_key_names[InputEvent::KEY_LEFT_META]             = "left meta";
}

/**
 * \brief Quits the input event manager.
 */
void InputEvent::quit() {

  if (joystick != NULL) {
    SDL_JoystickClose(joystick);
  }
  SDL_StopTextInput();
}

/**
 * \brief Creates a keyboard event.
 * \param event The internal event to encapsulate.
 */
InputEvent::InputEvent(const SDL_Event& event):
  internal_event(event) {

}

/**
 * \brief Destructor.
 */
InputEvent::~InputEvent() {

}

/**
 * \brief Returns the first event from the event queue, or NULL
 * if there is no event.
 * \return the current event to handle, or NULL if there is no event
 */
InputEvent* InputEvent::get_event() {

  InputEvent* result = NULL;
  SDL_Event internal_event;
  if (SDL_PollEvent(&internal_event)) {

    // Notify the window manager that the viewport has changed.
    if (internal_event.type == SDL_WINDOWEVENT
        && internal_event.window.event == SDL_WINDOWEVENT_RESIZED) {
      VideoManager::get_instance()->update_viewport();
    }
    // ignore intermediate positions of joystick axis
    else if (internal_event.type != SDL_JOYAXISMOTION
        || internal_event.jaxis.value <= 1000
        || internal_event.jaxis.value >= 10000) {

      result = new InputEvent(internal_event);
    }
  }

  return result;
}

// global information

/**
 * \brief Sets the keyboard repeat preferences.
 *
 * If true, the delay and the interval are set from the OS's settings.
 *
 * \param repeat true to accept repeated keyboard event.
 */
void InputEvent::set_key_repeat(bool repeat) {
  repeat_keyboard = repeat;
}

/**
 * \brief Returns whether the SHIFT key is currently down.
 *
 * There is no distinction between the right and left SHIFT keys in this function.
 *
 * \return true if the SHIFT key is currently down
 */
bool InputEvent::is_shift_down() {

  SDL_Keymod mod = SDL_GetModState();
  return mod & (KMOD_LSHIFT | KMOD_RSHIFT);
}

/**
 * \brief Returns whether the CTRL key is currently down.
 *
 * There is no distinction between the right and left CTRL keys in this function.
 *
 * \return true if the CTRL key is currently down
 */
bool InputEvent::is_control_down() {

  SDL_Keymod mod = SDL_GetModState();
  return mod & (KMOD_LCTRL | KMOD_RCTRL);
}

/**
 * \brief Returns whether the ALT key is currently down.
 *
 * There is no distinction between the right and left ALT keys in this function.
 *
 * \return true if the ALT key is currently down
 */
bool InputEvent::is_alt_down() {

  SDL_Keymod mod = SDL_GetModState();
  return mod & (KMOD_LALT | KMOD_RALT);
}

/**
 * \brief Returns whether a keyboard key is currently down.
 * \param key A keyboard key.
 * \return \c true if this keyboard key is currently down.
 */
bool InputEvent::is_key_down(KeyboardKey key) {

  int num_keys = 0;
  const Uint8* keys_state = SDL_GetKeyboardState(&num_keys);
  SDL_Scancode scan_code = SDL_GetScancodeFromKey(SDL_Keycode(key));
  return keys_state[scan_code];
}

/**
 * \brief Returns whether a joypad button is currently down.
 * \param button A joypad button.
 * \return \c true if this joypad button is currently down.
 */
bool InputEvent::is_joypad_button_down(int button) {

  if (joystick == NULL) {
    return false;
  }

  return SDL_JoystickGetButton(joystick, button) != 0;
}

/**
 * \brief Returns the state of a joypad axis.
 * \param axis Index of a joypad axis.
 * \return The state of that axis:
 * -1 (left or up), 0 (centered) or 1 (right or down).
 */
int InputEvent::get_joypad_axis_state(int axis) {

  if (joystick == NULL) {
    return 0;
  }

  int state = SDL_JoystickGetAxis(joystick, axis);

  int result;
  if (std::abs(state) < 10000) {
    result = 0;
  }
  else {
    result = (state > 0) ? 1 : -1;
  }

  return result;
}

/**
 * \brief Returns the direction of a joypad hat.
 * \param hat Index of a joypad hat.
 * \return The direction of that hat (0 to 7, or -1 if centered).
 */
int InputEvent::get_joypad_hat_direction(int hat) {

  if (joystick == NULL) {
    return -1;
  }

  int state = SDL_JoystickGetHat(joystick, hat);
  int result = -1;

  switch (state) {

    case SDL_HAT_RIGHT:
      result = 0;
      break;

    case SDL_HAT_RIGHTUP:
      result = 1;
      break;

    case SDL_HAT_UP:
      result = 2;
      break;

    case SDL_HAT_LEFTUP:
      result = 3;
      break;

    case SDL_HAT_LEFT:
      result = 4;
      break;

    case SDL_HAT_LEFTDOWN:
      result = 5;
      break;

    case SDL_HAT_DOWN:
      result = 6;
      break;

    case SDL_HAT_RIGHTDOWN:
      result = 7;
      break;

  }

  return result;
}


// event type

/**
 * \brief Returns whether this event is a keyboard event.
 * \return true if this is a keyboard event
 */
bool InputEvent::is_keyboard_event() const {

  return (internal_event.type == SDL_KEYDOWN || internal_event.type == SDL_KEYUP)
    && (!internal_event.key.repeat || repeat_keyboard);
}

/**
 * \brief Returns whether this event is a joypad event.
 * \return true if this is a joypad event
 */
bool InputEvent::is_joypad_event() const {

  return internal_event.type == SDL_JOYAXISMOTION
    || internal_event.type == SDL_JOYHATMOTION
    || internal_event.type == SDL_JOYBUTTONDOWN
    || internal_event.type == SDL_JOYBUTTONUP;
}

/**
 * \brief Returns whether this event is a window event.
 * \return true if this is a window event
 */
bool InputEvent::is_window_event() const {

  return internal_event.type == SDL_QUIT; // other SDL window events are ignored
}

// keyboard

/**
 * \brief Returns whether this event is a keyboard event
 * corresponding to pressing a key.
 * \return true if this is a key pressed event
 */
bool InputEvent::is_keyboard_key_pressed() const {

  return internal_event.type == SDL_KEYDOWN
    && (!internal_event.key.repeat || repeat_keyboard);
}

/**
 * \brief Returns whether this event is a keyboard event
 * corresponding to pressing a specific key.
 * \param key the key to test
 * \return true if this event corresponds to pressing that key
 */
bool InputEvent::is_keyboard_key_pressed(KeyboardKey key) const {

  return is_keyboard_key_pressed()
    && get_keyboard_key() == key;
}

/**
 * \brief Returns whether this event is a keyboard event
 * corresponding to pressing one of the specified keys.
 * \param keys an array of the keys to test, terminated by KEY_NONE
 * \return true if this event corresponds to pressing one of those keys
 */
bool InputEvent::is_keyboard_key_pressed(const KeyboardKey* keys) const {

  while (*keys != KEY_NONE) {

    if (is_keyboard_key_pressed(*keys)) {
      return true;
    }
    keys++;
  }

  return false;
}

/**
 * \brief Returns whether this event is a keyboard event
 * corresponding to pressing one of the four directional keys.
 * \return true if this event corresponds to pressing one of the four directional keys
 */
bool InputEvent::is_keyboard_direction_key_pressed() const {

  return is_keyboard_key_pressed(directional_keys);
}

/**
 * \brief Returns whether this event is a keyboard event
 * corresponding to pressing a key other than the four directional keys.
 * \return true if this event corresponds to pressing a key other than the four directional keys
 */
bool InputEvent::is_keyboard_non_direction_key_pressed() const {

  return is_keyboard_key_pressed()
    && !is_keyboard_direction_key_pressed();
}

/**
 * \brief Returns whether this event is a keyboard event
 * corresponding to releasing a key.
 * \return true if this is a key released event
 */
bool InputEvent::is_keyboard_key_released() const {

  return internal_event.type == SDL_KEYUP
    && (!internal_event.key.repeat || repeat_keyboard);
}

/**
 * \brief Returns whether this event is a keyboard event
 * corresponding to releasing a specific key.
 * \param key the key to test
 * \return true if this event corresponds to releasing that key
 */
bool InputEvent::is_keyboard_key_released(KeyboardKey key) const {

  return is_keyboard_key_released()
    && get_keyboard_key() == key;
}

/**
 * \brief Returns whether this event is a keyboard event
 * corresponding to releasing one of the specified keys.
 * \param keys an array of the keys to test, terminated by KEY_NONE
 * \return true if this event corresponds to releasing one of those keys
 */
bool InputEvent::is_keyboard_key_released(const KeyboardKey* keys) const {

  while (*keys != KEY_NONE) {

    if (is_keyboard_key_released(*keys)) {
      return true;
    }
    keys++;
  }

  return false;
}

/**
 * \brief Returns whether this event is a keyboard event
 * corresponding to releasing one of the four directional keys.
 * \return true if this event corresponds to releasing one of the four directional keys
 */
bool InputEvent::is_keyboard_direction_key_released() const {

  return is_keyboard_key_released(directional_keys);
}

/**
 * \brief Returns whether this event is a keyboard event
 * corresponding to releasing a key other than the four directional keys.
 * \return true if this event corresponds to releasing a key other than the four directional keys
 */
bool InputEvent::is_keyboard_non_direction_key_released() const {

  return is_keyboard_key_released()
    && !is_keyboard_direction_key_released();
}

/**
 * \brief Returns whether the SHIFT key was pressed
 * when this keyboard event occured.
 *
 * If this is not a keyboard event, false is returned.
 *
 * \return true if SHIFT was pressed during this keyboard event
 */
bool InputEvent::is_with_shift() const {

  return is_keyboard_event()
    && (internal_event.key.keysym.mod & KMOD_SHIFT);
}

/**
 * \brief Returns whether the CTRL key was pressed
 * when this keyboard event occured.
 *
 * If this is not a keyboard event, false is returned.
 *
 * \return true if CTRL was pressed during this keyboard event
 */
bool InputEvent::is_with_control() const {

  return is_keyboard_event()
    && (internal_event.key.keysym.mod & KMOD_CTRL);
}

/**
 * \brief Returns whether the ALT key was pressed
 * when this keyboard event occured.
 *
 * If this is not a keyboard event, false is returned.
 *
 * \return true if ALT was pressed during this keyboard event
 */
bool InputEvent::is_with_alt() const {

  return is_keyboard_event()
    && (internal_event.key.keysym.mod & KMOD_ALT);
}

/**
 * \brief Returns the key that was pressed or released during
 * this keyboard event.
 *
 * If this is not a keyboard event, KEY_NONE is returned.
 * The raw key is returned. If you want the corresponding character if any,
 * see get_character().
 *
 * \return The key of this keyboard event.
 */
InputEvent::KeyboardKey InputEvent::get_keyboard_key() const {

  if (!is_keyboard_event()) {
    return KEY_NONE;
  }

  return KeyboardKey(internal_event.key.keysym.sym);
}

/**
 * \brief Returns the Lua name of a keyboard key.
 * \param key A keyboard key.
 * \return The corresponding name (or an empty string for KEY_NONE).
 */
const std::string& InputEvent::get_keyboard_key_name(KeyboardKey key) {
  return keyboard_key_names[key];
}

/**
 * \brief Returns a keyboard key given its name.
 * \param keyboard_key_name The name of a keyboard key.
 * \return The corresponding key, or KEY_NONE if this name is empty or unknown.
 */
InputEvent::KeyboardKey InputEvent::get_keyboard_key_by_name(const std::string& keyboard_key_name) {

  // TODO check that this traversal is not significant, otherwise make a reverse mapping.
  std::map<KeyboardKey, std::string>::iterator it;
  for (it = keyboard_key_names.begin(); it != keyboard_key_names.end(); it++) {
    if (it->second == keyboard_key_name) {
      return it->first;
    }
  }
  return KEY_NONE;
}

/**
 * \brief Returns whether this event is a text event.
 * \return true if this event corresponds to entered text.
 */
bool InputEvent::is_character_pressed() const {

  return internal_event.type == SDL_TEXTINPUT;
}

/**
 * \brief Returns a UTF-8 representation of the character that was pressed during this text event.
 * \return The UTF-8 string corresponding to the entered character, or an empty string if this is not a text event.
 */
const std::string InputEvent::get_character() const {

  return internal_event.text.text;
}

// joypad

/**
 * \brief Returns whether joypad support is enabled.
 *
 * This may be true even without any joypad plugged.
 *
 * \return true if joypad support is enabled.
 */
bool InputEvent::is_joypad_enabled() {

  return joypad_enabled;
}

/**
 * \brief Enables or disables joypad support.
 *
 * Joypad support may be enabled even without any joypad plugged.
 *
 * \param joypad_enabled true to enable joypad support, false to disable it.
 */
void InputEvent::set_joypad_enabled(bool joypad_enabled) {

  if (joypad_enabled != is_joypad_enabled()) {

    InputEvent::joypad_enabled = joypad_enabled;

    if (joystick != NULL) {
      SDL_JoystickClose(joystick);
      joystick = NULL;
    }

    if (joypad_enabled && SDL_NumJoysticks() > 0) {
        SDL_InitSubSystem(SDL_INIT_JOYSTICK);
        joystick = SDL_JoystickOpen(0);
    }
    else {
      SDL_JoystickEventState(SDL_IGNORE);
      SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
    }
  }
}

/**
 * \brief Returns whether this event is a joypad event
 * corresponding to pressing a joypad button.
 * \return true if this is a joypad button pressed event.
 */
bool InputEvent::is_joypad_button_pressed() const {

  return internal_event.type == SDL_JOYBUTTONDOWN;
}

/**
 * \brief Returns whether this event is a joypad event
 * corresponding to releasing a joypad button.
 * \return true if this is a joypad button released event.
 */
bool InputEvent::is_joypad_button_released() const {

  return internal_event.type == SDL_JOYBUTTONUP;
}

/**
 * \brief Returns the button that was pressed or released during
 * this joypad button event.
 *
 * If this is not a joypad button event, -1 is returned.
 *
 * \return the button number of this joypad button event
 */
int InputEvent::get_joypad_button() const {

  if (!is_joypad_button_pressed() && !is_joypad_button_released()) {
    return -1;
  }

  return internal_event.jbutton.button;
}

/**
 * \brief Returns whether this event is a joypad event
 * corresponding to moving a joypad axis.
 * \return true if this is a joypad axis event
 */
bool InputEvent::is_joypad_axis_moved() const {

  return internal_event.type == SDL_JOYAXISMOTION;
}

/**
 * \brief Returns the axis that was moved during this joypad axis event.
 *
 * The axis is identified by an integer (usually,
 * 0 and 1 represents the x and y axis of a joystick respectively).
 * If this is not a joypad axis event, -1 is returned.
 *
 * \return the axis index of this joypad axis event
 */
int InputEvent::get_joypad_axis() const {

  if (!is_joypad_axis_moved()) {
    return -1;
  }

  return internal_event.jaxis.axis;
}

/**
 * \brief Returns the new state of the axis that was moved during
 * this joypad axis event.
 *
 * If this is not a joypad axis event, 0 is returned.
 *
 * \return the new state of the axis moved during this joypad axis event:
 * -1 (left or up), 0 (centered) or 1 (right or down)
 */
int InputEvent::get_joypad_axis_state() const {

  if (!is_joypad_axis_moved()) {
    return 0;
  }

  int result;
  int value = internal_event.jaxis.value;
  if (std::abs(value) < 10000) {
    result = 0;
  }
  else {
    result = (value > 0) ? 1 : -1;
  }

  return result;
}

/**
 * \brief Returns whether the axis that was moved during this
 * joypad axis event is now at its initial position
 * (i.e. it was released).
 *
 * If this is not a joypad axis event, false is returned.
 *
 * \return true if the axis is back to its initial position
 */
bool InputEvent::is_joypad_axis_centered() const {

  return is_joypad_axis_moved()
    && get_joypad_axis_state() == 0;
}

/**
 * \brief Returns whether this event is a joypad event
 * corresponding to moving a joypad hat.
 * \return true if this is a joypad hat event
 */
bool InputEvent::is_joypad_hat_moved() const {

  return internal_event.type == SDL_JOYHATMOTION;
}

/**
 * \brief Returns the hat that was moved during
 * this joypad hat event.
 *
 * If this is not a joypad hat event, -1 is returned.
 *
 * \return the hat index of this joypad hat event
 */
int InputEvent::get_joypad_hat() const {

  if (!is_joypad_hat_moved()) {
    return -1;
  }

  return internal_event.jhat.hat;
}

/**
 * \brief Returns the new direction of the hat that was moved during
 * this joypad hat event.
 *
 * The value returned is -1 if the hat is centered,
 * and 0 to 7 if the hat is in one of the eight main directions.
 * If this is not a joypad hat event, -1 is returned.
 *
 * \return the new direction of the hat moved during this joypad hat event
 */
int InputEvent::get_joypad_hat_direction() const {

  if (!is_joypad_hat_moved()) {
    return -1;
  }

  int result = -1;

  switch (internal_event.jhat.value) {

    case SDL_HAT_RIGHT:
      result = 0;
      break;

    case SDL_HAT_RIGHTUP:
      result = 1;
      break;

    case SDL_HAT_UP:
      result = 2;
      break;

    case SDL_HAT_LEFTUP:
      result = 3;
      break;

    case SDL_HAT_LEFT:
      result = 4;
      break;

    case SDL_HAT_LEFTDOWN:
      result = 5;
      break;

    case SDL_HAT_DOWN:
      result = 6;
      break;

    case SDL_HAT_RIGHTDOWN:
      result = 7;
      break;

  }

  return result;
}

/**
 * \brief Returns whether the hat that was moved during this
 * joypad hat event is now at its initial position
 * (i.e. it was released).
 *
 * If this is not a joypad hat event, false is returned.
 *
 * \return true if the hat is back to its initial position
 */
bool InputEvent::is_joypad_hat_centered() const {

  return is_joypad_hat_moved()
    && get_joypad_hat_direction() == -1;
}


// functions common to keyboard and joypad events

/**
 * \brief Returns the direction that was pressed or released
 * during this keyboard or joypad event.
 *
 * If this is not a keyboard or joypad event, or if
 * the control was not a direction, -1 is returned.
 *
 * \return the direction (0 to 7) or -1 if there is no direction
 */
int InputEvent::get_direction() const {

  int result = -1;

  if (is_keyboard_direction_key_pressed()) {

    switch (get_keyboard_key()) {

      case KEY_RIGHT:
        result = 0;
        break;

      case KEY_UP:
        result = 2;
        break;

      case KEY_LEFT:
        result = 4;
        break;

      case KEY_DOWN:
        result = 6;
        break;

      default:
        break;
    }
  }
  else if (is_joypad_axis_moved() && !is_joypad_axis_centered()) {

    if (get_joypad_axis() % 2 == 0) {
      // we assume the axis is horizontal
      result = (get_joypad_axis_state() > 0) ? 0 : 4;
    }
    else {
      // we assume the axis is vertical
      result = (get_joypad_axis_state() > 0) ? 6 : 2;
    }
  }
  else if (is_joypad_hat_moved()) {
    result = get_joypad_hat_direction();
  }

  return result;
}

/**
 * \brief Returns whether this keyboard or joypad event
 * corresponds to pressing something.
 *
 * The thing pressed may be a key, a button or a direction.
 * If this is not a keyboard or joypad event, false is returned.
 *
 * \return true if something was pressed
 */
bool InputEvent::is_pressed() const {

  return is_keyboard_key_pressed()
    || is_direction_pressed()
    || is_joypad_button_pressed();
}

/**
 * \brief Returns whether this keyboard or joypad event
 * corresponds to pressing a direction.
 *
 * If this is not a keyboard or joypad event, false is returned.
 *
 * \return true if a direction was pressed
 */
bool InputEvent::is_direction_pressed() const {

  return is_keyboard_direction_key_pressed()
    || (is_joypad_axis_moved() && !is_joypad_axis_centered())
    || (is_joypad_hat_moved() && !is_joypad_hat_centered());
}

/**
 * \brief Returns whether this keyboard or joypad event
 * corresponds to pressing something other than a direction.
 *
 * If this is not a keyboard or joypad event, false is returned.
 *
 * \return true if something other that a direction was pressed
 */
bool InputEvent::is_non_direction_pressed() const {

  return is_pressed()
    && !is_direction_pressed();
}

/**
 * \brief Returns whether this keyboard or joypad event
 * corresponds to releasing something.
 *
 * The thing released may be a key, a button or a direction.
 * If this is not a keyboard or joypad event, false is returned.
 *
 * \return true if something was released
 */
bool InputEvent::is_released() const {

  return is_keyboard_key_released()
    || is_joypad_button_released()
    || (is_joypad_axis_moved() && is_joypad_axis_centered())
    || (is_joypad_hat_moved() && is_joypad_hat_centered());
}

// window event

/**
 * \brief Returns whether this event corresponds to
 * the user closing the window.
 * \return true if this is a window closing event
 */
bool InputEvent::is_window_closing() const {

  return internal_event.type == SDL_QUIT;
}

