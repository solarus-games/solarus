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
 * Returns the human-readable name of a keyboard key.
 * @param key a keyboard key
 * @return the corresponding name
 */
const std::string InputEvent::get_keyboard_key_name(KeyboardKey key) {
  return SDL_GetKeyName(SDLKey(key));
}

/**
 * Sets the keyboard repeat preferences.
 * @param initial_delay delay in milliseconds before the event begins repeating (0 means no repeating)
 * @param interval interval in milliseconds between two events while repeating
 */
void InputEvent::set_key_repeat(int delay, int interval) {
  SDL_EnableKeyRepeat(delay, interval);
}

