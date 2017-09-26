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
#include "solarus/lowlevel/Debug.h"
#include "solarus/lowlevel/InputEvent.h"
#include "solarus/lowlevel/Logger.h"
#include "solarus/lowlevel/Rectangle.h"
#include "solarus/lowlevel/Video.h"
#include <SDL.h>
#include <cstdlib>  // std::abs

namespace Solarus {

const InputEvent::KeyboardKey InputEvent::directional_keys[] = {
    KeyboardKey::RIGHT,
    KeyboardKey::UP,
    KeyboardKey::LEFT,
    KeyboardKey::DOWN,
    KeyboardKey::NONE
};
bool InputEvent::initialized = false;
bool InputEvent::joypad_enabled = false;
SDL_Joystick* InputEvent::joystick = nullptr;
bool InputEvent::repeat_keyboard = false;
std::set<InputEvent::KeyboardKey> InputEvent::keys_pressed;
// Default the axis states to centered
std::vector<int> InputEvent::joypad_axis_state;

// Keyboard key names.
const std::string EnumInfoTraits<InputEvent::KeyboardKey>::pretty_name = "keyboard key";

const EnumInfo<InputEvent::KeyboardKey>::names_type EnumInfoTraits<InputEvent::KeyboardKey>::names = {

    { InputEvent::KeyboardKey::NONE,              "" },
    { InputEvent::KeyboardKey::BACKSPACE,         "backspace" },
    { InputEvent::KeyboardKey::TABULATION,        "tab" },
    { InputEvent::KeyboardKey::CLEAR,             "clear" },
    { InputEvent::KeyboardKey::RETURN,            "return" },
    { InputEvent::KeyboardKey::PAUSE,             "pause" },
    { InputEvent::KeyboardKey::ESCAPE,            "escape" },
    { InputEvent::KeyboardKey::SPACE,             "space" },
    { InputEvent::KeyboardKey::EXCLAMATION_MARK,  "!" },
    { InputEvent::KeyboardKey::DOULE_QUOTE,       "\"" },
    { InputEvent::KeyboardKey::HASH,              "#" },
    { InputEvent::KeyboardKey::DOLLAR,            "$" },
    { InputEvent::KeyboardKey::AMPERSAND,         "&" },
    { InputEvent::KeyboardKey::SINGLE_QUOTE,      "'" },
    { InputEvent::KeyboardKey::LEFT_PARENTHESIS,  "(" },
    { InputEvent::KeyboardKey::RIGHT_PARENTHESIS, ")" },
    { InputEvent::KeyboardKey::ASTERISK,          "*" },
    { InputEvent::KeyboardKey::PLUS,              "+" },
    { InputEvent::KeyboardKey::COMMA,             "," },
    { InputEvent::KeyboardKey::MINUS,             "-" },
    { InputEvent::KeyboardKey::PERIOD,            "." },
    { InputEvent::KeyboardKey::SLASH,             "/" },
    { InputEvent::KeyboardKey::NUMBER_0,          "0" },
    { InputEvent::KeyboardKey::NUMBER_1,          "1" },
    { InputEvent::KeyboardKey::NUMBER_2,          "2" },
    { InputEvent::KeyboardKey::NUMBER_3,          "3" },
    { InputEvent::KeyboardKey::NUMBER_4,          "4" },
    { InputEvent::KeyboardKey::NUMBER_5,          "5" },
    { InputEvent::KeyboardKey::NUMBER_6,          "6" },
    { InputEvent::KeyboardKey::NUMBER_7,          "7" },
    { InputEvent::KeyboardKey::NUMBER_8,          "8" },
    { InputEvent::KeyboardKey::NUMBER_9,          "9" },
    { InputEvent::KeyboardKey::COLON,             "." },
    { InputEvent::KeyboardKey::SEMICOLON,         ":" },
    { InputEvent::KeyboardKey::LESS,              "<" },
    { InputEvent::KeyboardKey::EQUALS,            "=" },
    { InputEvent::KeyboardKey::GREATER,           ">" },
    { InputEvent::KeyboardKey::QUESTION_MARK,     "?" },
    { InputEvent::KeyboardKey::AT,                "@" },
    { InputEvent::KeyboardKey::LEFT_BRACKET,      "[" },
    { InputEvent::KeyboardKey::BACKSLASH,         "\\" },
    { InputEvent::KeyboardKey::RIGHT_BRACKET,     "]" },
    { InputEvent::KeyboardKey::CARET,             "^" },
    { InputEvent::KeyboardKey::UNDERSCORE,        "_" },
    { InputEvent::KeyboardKey::BACKQUOTE,         "`" },
    { InputEvent::KeyboardKey::a,                 "a" },
    { InputEvent::KeyboardKey::b,                 "b" },
    { InputEvent::KeyboardKey::c,                 "c" },
    { InputEvent::KeyboardKey::d,                 "d" },
    { InputEvent::KeyboardKey::e,                 "e" },
    { InputEvent::KeyboardKey::f,                 "f" },
    { InputEvent::KeyboardKey::g,                 "g" },
    { InputEvent::KeyboardKey::h,                 "h" },
    { InputEvent::KeyboardKey::i,                 "i" },
    { InputEvent::KeyboardKey::j,                 "j" },
    { InputEvent::KeyboardKey::k,                 "k" },
    { InputEvent::KeyboardKey::l,                 "l" },
    { InputEvent::KeyboardKey::m,                 "m" },
    { InputEvent::KeyboardKey::n,                 "n" },
    { InputEvent::KeyboardKey::o,                 "o" },
    { InputEvent::KeyboardKey::p,                 "p" },
    { InputEvent::KeyboardKey::q,                 "q" },
    { InputEvent::KeyboardKey::r,                 "r" },
    { InputEvent::KeyboardKey::s,                 "s" },
    { InputEvent::KeyboardKey::t,                 "t" },
    { InputEvent::KeyboardKey::u,                 "u" },
    { InputEvent::KeyboardKey::v,                 "v" },
    { InputEvent::KeyboardKey::w,                 "w" },
    { InputEvent::KeyboardKey::x,                 "x" },
    { InputEvent::KeyboardKey::y,                 "y" },
    { InputEvent::KeyboardKey::z,                 "z" },
    { InputEvent::KeyboardKey::DELETE,            "delete" },
    { InputEvent::KeyboardKey::KP0,               "kp 0" },
    { InputEvent::KeyboardKey::KP1,               "kp 1" },
    { InputEvent::KeyboardKey::KP2,               "kp 2" },
    { InputEvent::KeyboardKey::KP3,               "kp 3" },
    { InputEvent::KeyboardKey::KP4,               "kp 4" },
    { InputEvent::KeyboardKey::KP5,               "kp 5" },
    { InputEvent::KeyboardKey::KP6,               "kp 6" },
    { InputEvent::KeyboardKey::KP7,               "kp 7" },
    { InputEvent::KeyboardKey::KP8,               "kp 8" },
    { InputEvent::KeyboardKey::KP9,               "kp 9" },
    { InputEvent::KeyboardKey::KP_PERIOD,         "kp ." },
    { InputEvent::KeyboardKey::KP_DIVIDE,         "kp /" },
    { InputEvent::KeyboardKey::KP_MULTIPLY,       "kp *" },
    { InputEvent::KeyboardKey::KP_MINUS,          "kp -" },
    { InputEvent::KeyboardKey::KP_PLUS,           "kp +" },
    { InputEvent::KeyboardKey::KP_ENTER,          "kp return" },
    { InputEvent::KeyboardKey::KP_EQUALS,         "kp =" },
    { InputEvent::KeyboardKey::UP,                "up" },
    { InputEvent::KeyboardKey::DOWN,              "down" },
    { InputEvent::KeyboardKey::RIGHT,             "right" },
    { InputEvent::KeyboardKey::LEFT,              "left" },
    { InputEvent::KeyboardKey::INSERT,            "insert" },
    { InputEvent::KeyboardKey::HOME,              "home" },
    { InputEvent::KeyboardKey::END,               "end" },
    { InputEvent::KeyboardKey::PAGE_UP,           "page up" },
    { InputEvent::KeyboardKey::PAGE_DOWN,         "page down" },
    { InputEvent::KeyboardKey::F1,                "f1" },
    { InputEvent::KeyboardKey::F2,                "f2" },
    { InputEvent::KeyboardKey::F3,                "f3" },
    { InputEvent::KeyboardKey::F4,                "f4" },
    { InputEvent::KeyboardKey::F5,                "f5" },
    { InputEvent::KeyboardKey::F6,                "f6" },
    { InputEvent::KeyboardKey::F7,                "f7" },
    { InputEvent::KeyboardKey::F8,                "f8" },
    { InputEvent::KeyboardKey::F9,                "f9" },
    { InputEvent::KeyboardKey::F10,               "f10" },
    { InputEvent::KeyboardKey::F11,               "f11" },
    { InputEvent::KeyboardKey::F12,               "f12" },
    { InputEvent::KeyboardKey::F13,               "f13" },
    { InputEvent::KeyboardKey::F14,               "f14" },
    { InputEvent::KeyboardKey::F15,               "f15" },
    { InputEvent::KeyboardKey::NUMLOCK,           "num lock" },
    { InputEvent::KeyboardKey::CAPSLOCK,          "caps lock" },
    { InputEvent::KeyboardKey::SCROLLOCK,         "scroll lock" },
    { InputEvent::KeyboardKey::RIGHT_SHIFT,       "right shift" },
    { InputEvent::KeyboardKey::LEFT_SHIFT,        "left shift" },
    { InputEvent::KeyboardKey::RIGHT_CONTROL,     "right control" },
    { InputEvent::KeyboardKey::LEFT_CONTROL,      "left control" },
    { InputEvent::KeyboardKey::RIGHT_ALT,         "right alt" },
    { InputEvent::KeyboardKey::LEFT_ALT,          "left alt" },
    { InputEvent::KeyboardKey::RIGHT_META,        "right meta" },
    { InputEvent::KeyboardKey::LEFT_META,         "left meta" }
};

// Mouse button names.
const std::string EnumInfoTraits<InputEvent::MouseButton>::pretty_name = "mouse button";

const EnumInfo<InputEvent::MouseButton>::names_type EnumInfoTraits<InputEvent::MouseButton>::names = {
    { InputEvent::MouseButton::NONE,   "" },
    { InputEvent::MouseButton::LEFT,   "left" },
    { InputEvent::MouseButton::MIDDLE, "middle" },
    { InputEvent::MouseButton::RIGHT,  "right" },
    { InputEvent::MouseButton::X1,     "x1" },
    { InputEvent::MouseButton::X2,     "x2" }
};


/**
 * \brief Initializes the input event manager.
 */
void InputEvent::initialize() {

  initialized = true;

  // Initialize text events.
  SDL_StartTextInput();

  // Initialize the joypad.
  set_joypad_enabled(true);
}

/**
 * \brief Quits the input event manager.
 */
void InputEvent::quit() {

  if (joystick != nullptr) {
    SDL_JoystickClose(joystick);
  }
  SDL_StopTextInput();

  joypad_enabled = false;
  joystick = nullptr;
  repeat_keyboard = false;
  keys_pressed.clear();
  joypad_axis_state.clear();
  initialized = false;
}

/**
 * \brief Returns whether the input event manager is initialized.
 */
bool InputEvent::is_initialized() {

  return initialized;
}

/**
 * \brief Creates a keyboard event.
 * \param event The internal event to encapsulate.
 */
InputEvent::InputEvent(const SDL_Event& event):
  internal_event(event) {

}

/**
 * \brief Returns the first event from the event queue, or nullptr
 * if there is no event.
 * \return The current event to handle.
 * If it is invalid, the event was suppressed but there may be more events
 * in the queue.
 * Returns nullptr if there is no more event in the queue.
 */
std::unique_ptr<InputEvent> InputEvent::get_event() {

  InputEvent* result = nullptr;
  SDL_Event internal_event;
  if (SDL_PollEvent(&internal_event)) {

    // If this is a joypad axis event
    if (internal_event.type == SDL_JOYAXISMOTION) {
      // Determine the current state of the axis
      int axis = internal_event.jaxis.axis;
      int value = internal_event.jaxis.value;
      int joystick_deadzone = 8000;
      if (axis == 0) {  // X axis
        int x_dir = 0;
        if (value < -joystick_deadzone) {
          // Left of dead zone
          x_dir = -1;
        } else if(value > joystick_deadzone) {
          // Right of dead zone
          x_dir =  1;
        } else {
          x_dir = 0;
        }
        joypad_axis_state[axis] = x_dir;
      } else if (axis == 1) {  // Y axis
        int y_dir = 0;
        if (value < -joystick_deadzone) {
          // Below dead zone
          y_dir = -1;
        } else if(value > joystick_deadzone) {
          // Above dead zone
          y_dir =  1;
        } else {
          y_dir = 0;
        }
        joypad_axis_state[axis] = y_dir;
      }
    }

    // Check if keyboard events are correct.
    // For some reason, when running Solarus from a Qt application
    // (which is not recommended)
    // multiple SDL_KEYUP events are generated when a key remains pressed
    // (Qt/SDL conflict). This fixes most problems but not all of them.
    else if (internal_event.type == SDL_KEYDOWN) {
      KeyboardKey key = static_cast<KeyboardKey>(internal_event.key.keysym.sym);
      if (!keys_pressed.insert(key).second) {
        // Already known as pressed: mark repeated.
        internal_event.key.repeat = 1;
      }
    }
    else if (internal_event.type == SDL_KEYUP) {
      KeyboardKey key = static_cast<KeyboardKey>(internal_event.key.keysym.sym);
      if (keys_pressed.erase(key) == 0) {
        // Already known as not pressed: mark repeated.
        internal_event.key.repeat = 1;
      }
    }

    // Always return a Solarus event if an SDL event occurred, so that
    // multiple SDL events in the same frame are all treated.
    result = new InputEvent(internal_event);
  }

  return std::unique_ptr<InputEvent>(result);
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
  return mod & KMOD_SHIFT;
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
  return mod & KMOD_CTRL;
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
  return mod & KMOD_ALT;
}

/**
 * \brief Returns whether the caps lock key is currently active.
 * \return \c true if the caps lock key is currently active.
 */
bool InputEvent::is_caps_lock_on() {

  SDL_Keymod mod = SDL_GetModState();
  return mod & KMOD_CAPS;
}

/**
 * \brief Returns whether the num lock key is currently active.
 * \return \c true if the num lock key is currently active.
 */
bool InputEvent::is_num_lock_on() {

  SDL_Keymod mod = SDL_GetModState();
  return mod & KMOD_NUM;
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

  if (joystick == nullptr) {
    return false;
  }

  return SDL_JoystickGetButton(joystick, button) != 0;
}

/**
 * \brief Returns whether a mouse button is currently down.
 * \param button A mouse button.
 * \return \c true if this mouse button is currently down.
 */
bool InputEvent::is_mouse_button_down(MouseButton button) {

  return (SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(static_cast<int>(button))) != 0;
}

/**
 * \brief Returns whether a finger is currently pressing the screen.
 * \param finger_id A finger ID.
 * \return \c true if this finger is currently down.
 */
bool InputEvent::is_finger_down(int finger_id) {

  for (int i = 0; i < SDL_GetNumTouchDevices(); ++i) {
    if (SDL_GetTouchFinger(SDL_GetTouchDevice(i), finger_id) != NULL) {
      return true;
    }
  }

  return false;
}

/**
 * \brief Returns the state of a joypad axis.
 * \param axis Index of a joypad axis.
 * \return The state of that axis:
 * -1 (left or up), 0 (centered) or 1 (right or down).
 */
int InputEvent::get_joypad_axis_state(int axis) {

  if (joystick == nullptr) {
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

  if (joystick == nullptr) {
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

/**
 * \brief Gets the x and y position of the mouse.
 * Values are in quest size coordinates.
 * \return The mouse position in quest coordinates.
 */
Point InputEvent::get_global_mouse_position() {

  int x, y;

  SDL_GetMouseState(&x, &y);

  return Video::window_to_quest_coordinates(Point(x, y));
}

/**
 * \brief Gets the x and y position of the finger.
 * Values are in quest size coordinates.
 * \param finger_id The ID of the finger.
 * \param[out] finger_xy The x and y position of the finger in quest coordinates.
 * \return \c false if the finger is not pressed.
 */
bool InputEvent::get_global_finger_position(int finger_id, Point& finger_xy) {

  SDL_Finger* finger;

  for (int i = 0; i < SDL_GetNumTouchDevices(); ++i) {
    finger = SDL_GetTouchFinger(SDL_GetTouchDevice(i), finger_id);

    if (finger != NULL) {
      const Size window_size = Video::get_window_size();
      const int x = finger->x * static_cast<float>(window_size.width);
      const int y = finger->y * static_cast<float>(window_size.height);

      finger_xy = Video::window_to_quest_coordinates(Point(x, y));
      return true;
    }
  }

  return false;
}

/**
 * \brief Gets the pressure of the finger.
 * \param finger_id The ID of the finger.
 * \param[out] finger_pressure The pressure of the finger
 * \return \c false if the finger is not pressed.
 */
bool InputEvent::get_global_finger_pressure(int finger_id, float& finger_pressure) {

  SDL_Finger* finger;

  for (int i = 0; i < SDL_GetNumTouchDevices(); ++i) {
    finger = SDL_GetTouchFinger(SDL_GetTouchDevice(i), finger_id);

    if (finger != NULL) {
      finger_pressure = finger->pressure;
      return true;
    }
  }

  return false;
}


// event type

/**
 * \brief Returns whether this is a valid event.
 * \return \c false if this object represents no event.
 */
bool InputEvent::is_valid() const {
  return internal_event.type == SDL_LASTEVENT;
}

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
 * \brief Returns whether this event is a mouse event.
 * \return true if this is a mouse event.
 */
bool InputEvent::is_mouse_event() const {

  return internal_event.type == SDL_MOUSEMOTION
    || internal_event.type == SDL_MOUSEBUTTONDOWN
    || internal_event.type == SDL_MOUSEBUTTONUP
    || internal_event.type == SDL_MOUSEWHEEL;
}

/**
 * \brief Returns whether this event is a finger event.
 * \return true if this is a finger event.
 */
bool InputEvent::is_finger_event() const {

  return internal_event.type == SDL_FINGERMOTION
    || internal_event.type == SDL_FINGERDOWN
    || internal_event.type == SDL_FINGERUP;
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

  while (*keys != KeyboardKey::NONE) {

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

  while (*keys != KeyboardKey::NONE) {

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
 * \return The key of this keyboard event, or KEY_NONE if this is not a
 * keyboard event or if the key is unknown.
 */
InputEvent::KeyboardKey InputEvent::get_keyboard_key() const {

  if (!is_keyboard_event()) {
    return KeyboardKey::NONE;
  }

  SDL_Keycode sdl_symbol = internal_event.key.keysym.sym;
  if (EnumInfoTraits<KeyboardKey>::names.find(static_cast<KeyboardKey>(sdl_symbol)) ==
      EnumInfoTraits<KeyboardKey>::names.end()) {
    return KeyboardKey::NONE;
  }

  return static_cast<KeyboardKey>(sdl_symbol);
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
std::string InputEvent::get_character() const {

  return internal_event.text.text;
}

/**
 * \brief Simulates pressing a keyboard key.
 * \param key The key to simulate.
 */
void InputEvent::simulate_key_pressed(KeyboardKey key) {

  SDL_Event event;
  event.type = SDL_KEYDOWN;
  event.key.keysym.sym = static_cast<SDL_Keycode>(key);
  event.key.repeat = 0;

  SDL_PushEvent(&event);
}

/**
 * \brief Simulates releasing a keyboard key.
 * \param key The key to simulate.
 */
void InputEvent::simulate_key_released(KeyboardKey key) {

  SDL_Event event;
  event.type = SDL_KEYUP;
  event.key.keysym.sym = static_cast<SDL_Keycode>(key);
  event.key.repeat = 0;

  SDL_PushEvent(&event);
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

    if (joystick != nullptr) {
      SDL_JoystickClose(joystick);
      joystick = nullptr;
      joypad_axis_state.clear();
    }

    if (joypad_enabled && SDL_NumJoysticks() > 0) {
        SDL_InitSubSystem(SDL_INIT_JOYSTICK);
        joystick = SDL_JoystickOpen(0);
        joypad_axis_state.assign(SDL_JoystickNumAxes(joystick), 0);
    }
    else {
      SDL_JoystickEventState(SDL_IGNORE);
      SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
    }

    Logger::info(std::string("Joypad support enabled: ") + (joypad_enabled ? "true" : "false"));
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


// mouse

/**
 * \brief Returns whether this event is a mouse event.
 * corresponding to pressing any button.
 * \return true if this event corresponds to pressing a mouse button.
 */
bool InputEvent::is_mouse_button_pressed() const {

  return internal_event.type == SDL_MOUSEBUTTONDOWN;
}

/**
 * \brief Returns whether this event is a mouse event.
 * corresponding to pressing a specific button.
 * \param button the button to test.
 * \return true if this event corresponds to pressing that mouse button.
 */
bool InputEvent::is_mouse_button_pressed(MouseButton button) const {

  return is_mouse_button_pressed()
    && static_cast<MouseButton>(internal_event.button.button) == button;
}

/**
 * \brief Returns whether this event is a mouse event.
 * corresponding to releasing any button.
 * \return true if this event corresponds to releasing a mouse button.
 */
bool InputEvent::is_mouse_button_released() const {

  return internal_event.type == SDL_MOUSEBUTTONUP;
}

/**
 * \brief Returns whether this event is a mouse event.
 * corresponding to releasing a specific button.
 * \param button the button to test.
 * \return true if this event corresponds to releasing that mouse button.
 */
bool InputEvent::is_mouse_button_released(MouseButton button) const {

  return is_mouse_button_released()
    && static_cast<MouseButton>(internal_event.button.button) == button;
}

/**
 * \brief Returns the button that was pressed or released during
 * this mouse event.
 *
 * If this is not a mouse event, MOUSE_BUTTON_NONE is returned.
 * \return The button of this mouse event.
 */
InputEvent::MouseButton InputEvent::get_mouse_button() const {

  if (!is_mouse_event()) {
    return MouseButton::NONE;
  }

  return static_cast<MouseButton>(internal_event.button.button);
}

/**
 * \brief Gets the x and y position of this mouse event, if any.
 * Values are in quest size coordinates.
 * \param[out] mouse_xy The x and y position of the mouse in this mouse event.
 * \return \c false if the mouse was not inside the quest displaying during
 * this event.
 */
bool InputEvent::get_mouse_position(Point& mouse_xy) const {

  Debug::check_assertion(is_mouse_event(), "Event is not a mouse event");

  return Video::renderer_to_quest_coordinates(
      Point(internal_event.button.x, internal_event.button.y), mouse_xy);
}

// touch finger

/**
 * \brief Returns whether this event is a finger event.
 * corresponding to pressing a finger.
 * \return true if this event corresponds to pressing a finger.
 */
bool InputEvent::is_finger_pressed() const {

  return internal_event.type == SDL_FINGERDOWN;
}

/**
 * \brief Returns whether this event is a finger event.
 * corresponding to pressing a specific finger.
 * \param finger_id The finger ID to test.
 * \return true if this event corresponds to pressing that finger.
 */
bool InputEvent::is_finger_pressed(int finger_id) const {

  return is_finger_pressed()
    && static_cast<int>(internal_event.tfinger.fingerId) == finger_id;
}

/**
 * \brief Returns whether this event is a finger event.
 * corresponding to releasing a finger.
 * \return true if this event corresponds to releasing a finger.
 */
bool InputEvent::is_finger_released() const {

  return internal_event.type == SDL_FINGERUP;
}

/**
 * \brief Returns whether this event is a finger event.
 * corresponding to releasing a specific finger.
 * \param finger_id The finger ID to test.
 * \return true if this event corresponds to releasing that finger.
 */
bool InputEvent::is_finger_released(int finger_id) const {

  return is_finger_released()
    && static_cast<int>(internal_event.tfinger.fingerId) == finger_id;
}

/**
 * \brief Returns whether this event is a finger event.
 * corresponding to moving a finger.
 * \return true if this event corresponds to moving a finger.
 */
bool InputEvent::is_finger_moved() const {

  return internal_event.type == SDL_FINGERMOTION;
}

/**
 * \brief Returns whether this event is a finger event.
 * corresponding to moving a specific finger.
 * \param finger_id The finger ID to test.
 * \return true if this event corresponds to moving that finger button.
 */
bool InputEvent::is_finger_moved(int finger_id) const {

  return is_finger_moved()
    && static_cast<int>(internal_event.tfinger.fingerId) == finger_id;
}

/**
 * \brief Returns the finger ID that was pressed, released or moved
 * during this finger event.
 *
 * If this is not a finger event, -1 is returned.
 * \return The finger ID of this finger event.
 */
int InputEvent::get_finger() const {

  Debug::check_assertion(is_finger_event(), "Event is not a touch finger event");

  return static_cast<int>(internal_event.tfinger.fingerId);
}

/**
 * \brief Gets the x and y position of this finger event, if any.
 * Values are in quest size coordinates.
 * \param[out] finger_xy The x and y position of the finger in this finger event.
 * \return \c false if the finger was not inside the quest displaying during
 * this event.
 */
Point InputEvent::get_finger_position() const {

  Debug::check_assertion(is_finger_event(), "Event is not a touch finger event");

  const Size window_size = Video::get_window_size();
  const int x = internal_event.tfinger.x * static_cast<float>(window_size.width);
  const int y = internal_event.tfinger.y * static_cast<float>(window_size.height);

  return Video::window_to_quest_coordinates(Point(x, y));
}

/**
 * \brief Gets the x and y moved distance of this finger event, if any.
 * Values are in quest size coordinates.
 * \return The distance moved in this finger event.
 */
Point InputEvent::get_finger_distance() const {

  Debug::check_assertion(is_finger_event(), "Event is not a touch finger event");

  const Size window_size = Video::get_window_size();
  const int x = internal_event.tfinger.x * static_cast<float>(window_size.width);
  const int y = internal_event.tfinger.y * static_cast<float>(window_size.height);

  return Video::window_to_quest_coordinates(Point(x, y));
}

/**
 * \brief Gets the pressure of this finger event, if any.
 * \return The pressure of this finger event.
 */
float InputEvent::get_finger_pressure() const {

  Debug::check_assertion(is_finger_event(), "Event is not a touch finger event");

  return internal_event.tfinger.pressure;
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

      case KeyboardKey::RIGHT:
        result = 0;
        break;

      case KeyboardKey::UP:
        result = 2;
        break;

      case KeyboardKey::LEFT:
        result = 4;
        break;

      case KeyboardKey::DOWN:
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
 * \brief Returns whether this keyboard, joypad or mouse event
 * corresponds to pressing something.
 *
 * The thing pressed may be a key, a button or a direction.
 * If this is not a keyboard, joypad or mouse event, false is returned.
 *
 * \return true if something was pressed
 */
bool InputEvent::is_pressed() const {

  return is_keyboard_key_pressed()
    || is_direction_pressed()
    || is_joypad_button_pressed()
    || is_mouse_button_pressed();
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
 * \brief Returns whether this keyboard, joypad or mouse event
 * corresponds to pressing something other than a direction.
 *
 * If this is not a keyboard, joypad or mouse event, false is returned.
 *
 * \return true if something other that a direction was pressed
 */
bool InputEvent::is_non_direction_pressed() const {

  return is_pressed()
    && !is_direction_pressed();
}

/**
 * \brief Returns whether this keyboard, joypad or mouse event
 * corresponds to releasing something.
 *
 * The thing released may be a key, a button or a direction.
 * If this is not a keyboard, joypad or mouse event, false is returned.
 *
 * \return true if something was released
 */
bool InputEvent::is_released() const {

  return is_keyboard_key_released()
    || is_joypad_button_released()
    || (is_joypad_axis_moved() && is_joypad_axis_centered())
    || (is_joypad_hat_moved() && is_joypad_hat_centered())
    || is_mouse_button_released();
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

}

