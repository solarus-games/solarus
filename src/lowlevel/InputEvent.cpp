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
#include "lowlevel/InputEvent.h"

const int InputEvent::KEYBOARD_ENUM_VERSION = 1;
const InputEvent::KeyboardKey InputEvent::directional_keys[] = { KEY_RIGHT, KEY_UP, KEY_LEFT, KEY_DOWN, KEY_NONE };
SDL_Joystick *InputEvent::joystick;

/**
 * Initializes the input event manager.
 */
void InputEvent::initialize(void) {

  // initialize the keyboard
  SDL_EnableUNICODE(SDL_ENABLE);
  SDL_EnableKeyRepeat(0, 0);

  // initialize the joypad
  if (SDL_NumJoysticks() > 0) {
    joystick = SDL_JoystickOpen(0);
  }
  else {
    joystick = NULL;
  }
}

/**
 * Quits the input event manager.
 */
void InputEvent::quit(void) {

  if (SDL_JoystickOpened(0)) {
    SDL_JoystickClose(joystick);
  }
}

/**
 * Creates a keyboard event.
 * @param event the internal event to encapsulate
 */
InputEvent::InputEvent(const SDL_Event &event):
  internal_event(event) {

}

/**
 * Destructor.
 */
InputEvent::~InputEvent(void) {

}

/**
 * Returns the first event from the event queue, or NULL
 * if there is no event.
 * @return the current event to handle, or NULL if there is no event
 */
InputEvent * InputEvent::get_event(void) {

  InputEvent *result = NULL;
  SDL_Event internal_event;
  if (SDL_PollEvent(&internal_event)) {

    // ignore intermediate positions of joystick axis
    if (internal_event.type != SDL_JOYAXISMOTION
	|| internal_event.jaxis.value <= 1000
	|| internal_event.jaxis.value >= 10000) {

      result = new InputEvent(internal_event);
    }
  }

  return result;
}

// global information

/**
 * Sets the keyboard repeat preferences.
 * @param initial_delay delay in milliseconds before the event begins repeating (0 means no repeating)
 * @param interval interval in milliseconds between two events while repeating
 */
void InputEvent::set_key_repeat(int delay, int interval) {
  SDL_EnableKeyRepeat(delay, interval);
}

/**
 * Returns whether the SHIFT key is currently down.
 * There is no distinction between the right and left SHIFT keys in this function.
 * @return true if the SHIFT key is currently down
 */
bool InputEvent::is_shift_down(void) {
  
  SDLMod mod = SDL_GetModState();
  return mod & (KMOD_LSHIFT | KMOD_RSHIFT);
}

/**
 * Returns whether the CTRL key is currently down.
 * There is no distinction between the right and left CTRL keys in this function.
 * @return true if the CTRL key is currently down
 */
bool InputEvent::is_control_down(void) {
  
  SDLMod mod = SDL_GetModState();
  return mod & (KMOD_LCTRL | KMOD_RCTRL);
}

/**
 * Returns whether the ALT key is currently down.
 * There is no distinction between the right and left ALT keys in this function.
 * @return true if the ALT key is currently down
 */
bool InputEvent::is_alt_down(void) {
  
  SDLMod mod = SDL_GetModState();
  return mod & (KMOD_LALT | KMOD_RALT);
}

// event type

/**
 * Returns whether this event is a keyboard event.
 * @return true if this is a keyboard event
 */
bool InputEvent::is_keyboard_event(void) {

  return internal_event.type == SDL_KEYDOWN
    || internal_event.type == SDL_KEYUP;
}

/**
 * Returns whether this event is a joypad event.
 * @return true if this is a joypad event
 */
bool InputEvent::is_joypad_event(void) {

  return internal_event.type == SDL_JOYAXISMOTION
    || internal_event.type == SDL_JOYHATMOTION
    || internal_event.type == SDL_JOYBUTTONDOWN
    || internal_event.type == SDL_JOYBUTTONUP;
}

/**
 * Returns whether this event is a window event.
 * @return true if this is a window event
 */
bool InputEvent::is_window_event(void) {

  return internal_event.type == SDL_QUIT; // other SDL window events are ignored
}

// keyboard

/**
 * Returns whether this event is a keyboard event
 * corresponding to pressing a key.
 * @return true if this is a key pressed event
 */
bool InputEvent::is_keyboard_key_pressed(void) {

  return internal_event.type == SDL_KEYDOWN;
}

/**
 * Returns whether this event is a keyboard event
 * corresponding to pressing a specific key.
 * @param key the key to test
 * @return true if this event corresponds to pressing that key
 */
bool InputEvent::is_keyboard_key_pressed(KeyboardKey key) {

  return is_keyboard_key_pressed()
    && get_keyboard_key() == key;
}

/**
 * Returns whether this event is a keyboard event
 * corresponding to pressing one of the specified keys.
 * @param keys an array of the keys to test, terminated by KEY_NONE
 * @return true if this event corresponds to pressing one of those keys
 */
bool InputEvent::is_keyboard_key_pressed(const KeyboardKey *keys) {

  while (*keys != KEY_NONE) {

    if (is_keyboard_key_pressed(*keys)) {
      return true;
    }
    keys++;
  }

  return false;
}

/**
 * Returns whether this event is a keyboard event
 * corresponding to pressing one of the four directional keys.
 * @return true if this event corresponds to pressing one of the four directional keys
 */
bool InputEvent::is_keyboard_direction_key_pressed(void) {

  return is_keyboard_key_pressed(directional_keys);
}

/**
 * Returns whether this event is a keyboard event
 * corresponding to pressing a key other than the four directional keys.
 * @return true if this event corresponds to pressing a key other than the four directional keys
 */
bool InputEvent::is_keyboard_non_direction_key_pressed(void) {

  return is_keyboard_key_pressed()
    && !is_keyboard_direction_key_pressed();
}

/**
 * Returns whether this event is a keyboard event
 * corresponding to releasing a key.
 * @return true if this is a key released event
 */
bool InputEvent::is_keyboard_key_released(void) {

  return internal_event.type == SDL_KEYUP;
}

/**
 * Returns whether this event is a keyboard event
 * corresponding to releasing a specific key.
 * @param key the key to test
 * @return true if this event corresponds to releasing that key
 */
bool InputEvent::is_keyboard_key_released(KeyboardKey key) {

  return is_keyboard_key_released()
    && get_keyboard_key() == key;
}

/**
 * Returns whether this event is a keyboard event
 * corresponding to releasing one of the specified keys.
 * @param keys an array of the keys to test, terminated by KEY_NONE
 * @return true if this event corresponds to releasing one of those keys
 */
bool InputEvent::is_keyboard_key_released(const KeyboardKey *keys) {

  while (*keys != KEY_NONE) {
    
    if (is_keyboard_key_released(*keys)) {
      return true;
    }
    keys++;
  }

  return false;
}

/**
 * Returns whether this event is a keyboard event
 * corresponding to releasing one of the four directional keys.
 * @return true if this event corresponds to releasing one of the four directional keys
 */
bool InputEvent::is_keyboard_direction_key_released(void) {

  return is_keyboard_key_released(directional_keys);
}

/**
 * Returns whether this event is a keyboard event
 * corresponding to releasing a key other than the four directional keys.
 * @return true if this event corresponds to releasing a key other than the four directional keys
 */
bool InputEvent::is_keyboard_non_direction_key_released(void) {

  return is_keyboard_key_released()
    && !is_keyboard_direction_key_released();
}

/**
 * Returns whether the SHIFT key was pressed
 * when this keyboard event occured.
 * If this is not a keyboard event, false is returned.
 * @return true if SHIFT was pressed during this keyboard event
 */
bool InputEvent::is_with_shift(void) {

  return is_keyboard_event()
    && (internal_event.key.keysym.mod & KMOD_SHIFT);
}

/**
 * Returns whether the CTRL key was pressed
 * when this keyboard event occured.
 * If this is not a keyboard event, false is returned.
 * @return true if CTRL was pressed during this keyboard event
 */
bool InputEvent::is_with_control(void) {

  return is_keyboard_event()
    && (internal_event.key.keysym.mod & KMOD_CTRL);
}

/**
 * Returns whether the ALT key was pressed
 * when this keyboard event occured.
 * If this is not a keyboard event, false is returned.
 * @return true if ALT was pressed during this keyboard event
 */
bool InputEvent::is_with_alt(void) {

  return is_keyboard_event()
    && (internal_event.key.keysym.mod & KMOD_ALT);
}

/**
 * Returns the key that was pressed or released during
 * this keyboard event.
 * If this is not a keyboard event, KEY_NONE is returned.
 * @return the key of this keyboard event
 */
InputEvent::KeyboardKey InputEvent::get_keyboard_key(void) {

  if (!is_keyboard_event()) {
    return KEY_NONE;
  }

  return KeyboardKey(internal_event.key.keysym.sym);
}

/**
 * Returns the human-readable name of a keyboard key.
 * @param key a keyboard key
 * @return the corresponding name
 */
const std::string InputEvent::get_keyboard_key_name(KeyboardKey key) {
  return SDL_GetKeyName(SDLKey(key));
}

// joypad

/**
 * Returns whether this event is a joypad event
 * corresponding to pressing a joypad button.
 * @return true if this is a joypad button pressed event.
 */
bool InputEvent::is_joypad_button_pressed(void) {

  return internal_event.type == SDL_JOYBUTTONDOWN;
}

/**
 * Returns whether this event is a joypad event
 * corresponding to releasing a joypad button.
 * @return true if this is a joypad button released event.
 */
bool InputEvent::is_joypad_button_released(void) {

  return internal_event.type == SDL_JOYBUTTONUP;
}

/**
 * Returns the button that was pressed or released during
 * this joypad button event.
 * If this is not a joypad button event, -1 is returned.
 * @return the button number of this joypad button event
 */
int InputEvent::get_joypad_button(void) {

  if (!is_joypad_button_pressed() && !is_joypad_button_released()) {
    return -1;
  }

  return internal_event.jbutton.button;
}

/**
 * Returns whether this event is a joypad event
 * corresponding to moving a joypad axis.
 * @return true if this is a joypad axis event
 */
bool InputEvent::is_joypad_axis_moved(void) {

  return internal_event.type == SDL_JOYAXISMOTION;
}

/**
 * Returns the axis that was moved during this joypad axis event.
 * The axis is identified by an integer (usually,
 * 0 and 1 represents the x and y axis of a joystick respectively).
 * If this is not a joypad axis event, -1 is returned.
 * @return the axis index of this joypad axis event
 */
int InputEvent::get_joypad_axis(void) {

  if (!is_joypad_axis_moved()) {
    return -1;
  }

  return internal_event.jaxis.axis;
}

/**
 * Returns the new state of the axis that was moved during
 * this joypad axis event.
 * If this is not a joypad axis event, 0 is returned.
 * @return the new state of the axis moved during this joypad axis event:
 * -1 (left or up), 0 (centered) or 1 (right or down)
 */
int InputEvent::get_joypad_axis_state(void) {

  if (!is_joypad_axis_moved()) {
    return 0;
  }
 
  int result;
  int value = internal_event.jaxis.value;
  if (abs(value) < 10000) {
    result = 0;
  }
  else {
    result = (value > 0) ? 1 : -1;
  }

  return result;
}

/**
 * Returns whether the axis that was moved during this
 * joypad axis event is now at its initial position
 * (i.e. it was released).
 * If this is not a joypad axis event, false is returned.
 * @return true if the axis is back to its initial position
 */
bool InputEvent::is_joypad_axis_centered(void) {

  return is_joypad_axis_moved()
    && get_joypad_axis_state() == 0;
}

/**
 * Returns whether this event is a joypad event
 * corresponding to moving a joypad hat.
 * @return true if this is a joypad hat event
 */
bool InputEvent::is_joypad_hat_moved(void) {

  return internal_event.type == SDL_JOYHATMOTION;
}

/**
 * Returns the hat that was moved during
 * this joypad hat event.
 * If this is not a joypad hat event, -1 is returned.
 * @return the hat index of this joypad hat event
 */
int InputEvent::get_joypad_hat(void) {

  if (!is_joypad_hat_moved()) {
    return -1;
  }

  return internal_event.jhat.hat;
}

/**
 * Returns the new direction of the hat that was moved during
 * this joypad hat event.
 * The value returned is -1 if the hat is centered,
 * and 0 to 7 if the hat is is one of the eight main directions.
 * If this is not a joypad hat event, -1 is returned.
 * @return the new direction of the hat moved during this joypad hat event
 */
int InputEvent::get_joypad_hat_direction(void) {

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
 * Returns whether the hat that was moved during this
 * joypad hat event is now at its initial position
 * (i.e. it was released).
 * If this is not a joypad hat event, false is returned.
 * @return true if the hat is back to its initial position
 */
bool InputEvent::is_joypad_hat_centered(void) {

  return is_joypad_hat_moved()
    && get_joypad_hat_direction() == -1;
}


// functions common to keyboard and joypad events

/**
 * Returns the direction that was pressed or released
 * during this keyboard or joypad event.
 * If this is not a keyboard or joypad event, or if
 * the control was not a direction, -1 is returned.
 * @return the direction (0 to 7) or -1 if there is no direction
 */
int InputEvent::get_direction(void) {

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
 * Returns whether this keyboard or joypad event
 * corresponds to pressing something: a key, a button
 * or a direction.
 * If this is not a keyboard or joypad event, false is returned.
 * @return true if something was pressed
 */
bool InputEvent::is_pressed(void) {

  return is_keyboard_key_pressed()
    || is_direction_pressed()
    || is_joypad_button_pressed();
}

/**
 * Returns whether this keyboard or joypad event
 * corresponds to pressing a direction.
 * If this is not a keyboard or joypad event, false is returned.
 * @return true if a direction was pressed
 */
bool InputEvent::is_direction_pressed(void) {

  return is_keyboard_direction_key_pressed()
    || (is_joypad_axis_moved() && !is_joypad_axis_centered())
    || (is_joypad_hat_moved() && !is_joypad_hat_centered());
}

/**
 * Returns whether this keyboard or joypad event
 * corresponds to pressing something other than a direction.
 * If this is not a keyboard or joypad event, false is returned.
 * @return true if something other that a direction was pressed
 */
bool InputEvent::is_non_direction_pressed(void) {

  return is_pressed()
    && !!is_direction_pressed();
}

/**
 * Returns whether this keyboard or joypad event
 * corresponds to releasing something: a key, a button
 * or a direction.
 * If this is not a keyboard or joypad event, false is returned.
 * @return true if something was released
 */
bool InputEvent::is_released(void) {

  return is_keyboard_key_released()
    || is_joypad_button_released()
    || (is_joypad_axis_moved() && is_joypad_axis_centered())
    || (is_joypad_hat_moved() && is_joypad_hat_centered());
}

// window event

/**
 * Returns whether this event corresponds to
 * the user closing the window.
 * @return true if this is a window closing event
 */
bool InputEvent::is_window_closing(void) {

  return internal_event.type == SDL_QUIT;
}

