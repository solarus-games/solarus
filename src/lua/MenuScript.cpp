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
#include "lua/MenuScript.h"
#include "lowlevel/Debug.h"
#include "lowlevel/Surface.h"
#include "CustomScreen.h"
#include <lua.hpp>

/**
 * @brief Constructor.
 * @param file_name script file to load
 * @param screen the custom screen to control from this script (or NULL)
 */
MenuScript::MenuScript(const std::string& file_name, CustomScreen* screen):
  Script(INPUT_API),
  screen(screen) {

  load(file_name);
}

/**
 * @brief Destructor.
 */
MenuScript::~MenuScript() {

}

/**
 * @brief Returns the screen controlled by this script (if any).
 *
 * Scripts that want to control the screen must redefine this function.
 *
 * @return the screen controlled by this script
 */
CustomScreen& MenuScript::get_screen() {

  Debug::check_assertion(screen != NULL,
      "This menu script does not control the screen");

  return *screen;
}

/**
 * @brief This function is called when there is a low-level input event
 * in this menu.
 * @param event the event to handle
 */
void MenuScript::notify_input(InputEvent& event) {

  // call the Lua function(s) corresponding to this input event

  if (event.is_keyboard_event()) {
    // keyboard
    if (event.is_keyboard_key_pressed()) {
      event_key_pressed(event);
    }
    else if (event.is_keyboard_key_released()) {
      event_key_released(event);
    }
  }
  else if (event.is_joypad_event()) {
    // joypad
    if (event.is_joypad_button_pressed()) {
      event_joypad_button_pressed(event);
    }
    else if (event.is_joypad_button_released()) {
      event_joypad_button_released(event);
    }
    else if (event.is_joypad_axis_moved()) {
      event_joypad_axis_moved(event);
    }
    else if (event.is_joypad_hat_moved()) {
      event_joypad_hat_moved(event);
    }
  }

  if (event.is_direction_pressed()) {
    // keyboard or joypad direction
    event_direction_pressed(event);
  }
}

/**
 * @brief Notifies the script that the menu starts.
 */
void MenuScript::event_menu_started() {

  const std::string function_name("event_menu_started");
  if (find_global_function(function_name)) {
    call_script(0, 0, function_name);
  }
}

/**
 * @brief Displays the content of the menu on a surface
 * @param dst_surface the destination surface
 */
void MenuScript::event_display(Surface& dst_surface) {

  const std::string function_name("event_display");
  if (find_global_function(function_name)) {
    push_userdata(l, dst_surface);
    call_script(1, 0, function_name);
  }
}

/**
 * @brief Notifies the script that a keyboard key was just pressed
 * (including if it is a directional key).
 * @param event the corresponding input event
 */
void MenuScript::event_key_pressed(InputEvent& event) {

  const std::string function_name("event_key_pressed");
  if (find_global_function(function_name)) {

    const std::string& key_name = input_get_key_name(event.get_keyboard_key());
    if (!key_name.empty()) { // this key exists in the Lua API

      lua_pushstring(l, key_name.c_str());
      lua_newtable(l);

      if (event.is_with_shift()) {
        lua_pushnil(l);
        lua_setfield(l, -2, "shift");
      }

      if (event.is_with_control()) {
        lua_pushnil(l);
        lua_setfield(l, -2, "control");
      }

      if (event.is_with_alt()) {
        lua_pushnil(l);
        lua_setfield(l, -2, "alt");
      }
      call_script(2, 0, function_name);
    }
  }
}

/**
 * @brief Notifies the script that a keyboard key was just released
 * (including if it is a directional key).
 * @param event the corresponding input event
 */
void MenuScript::event_key_released(InputEvent& event) {

  const std::string function_name("event_key_released");
  if (find_global_function(function_name)) {

    const std::string& key_name = input_get_key_name(event.get_keyboard_key());
    if (!key_name.empty()) { // this key exists in the Lua API
      lua_pushstring(l, key_name.c_str());
      call_script(1, 0, function_name);
    }
  }
}

/**
 * @brief Notifies the script that a joypad button was just pressed.
 * @param event the corresponding input event
 */
void MenuScript::event_joypad_button_pressed(InputEvent& event) {

  const std::string function_name("event_joypad_button_pressed");
  if (find_global_function(function_name)) {
    int button = event.get_joypad_button();

    lua_pushinteger(l, button);
    call_script(1, 0, function_name);
  }
}

/**
 * @brief Notifies the script that a joypad button was just released.
 * @param event the corresponding input event
 */
void MenuScript::event_joypad_button_released(InputEvent& event) {

  const std::string function_name("event_joypad_button_released");
  if (find_global_function(function_name)) {
    int button = event.get_joypad_button();

    lua_pushinteger(l, button);
    call_script(1, 0, function_name);
  }
}

/**
 * @brief Notifies the script that a joypad axis was just moved.
 * @param event the corresponding input event
 */
void MenuScript::event_joypad_axis_moved(InputEvent& event) {

  const std::string function_name("event_joypad_axis_moved");
  if (find_global_function(function_name)) {
    int axis = event.get_joypad_axis();
    int state = event.get_joypad_axis_state();

    lua_pushinteger(l, axis);
    lua_pushinteger(l, state);
    call_script(2, 0, function_name);
  }
}

/**
 * @brief Notifies the script that a joypad hat was just moved.
 * @param event the corresponding input event
 */
void MenuScript::event_joypad_hat_moved(InputEvent& event) {

  const std::string function_name("event_joypad_hat_moved");
  if (find_global_function(function_name)) {
    int hat = event.get_joypad_hat();
    int direction8 = event.get_joypad_hat_direction();

    lua_pushinteger(l, hat);
    lua_pushinteger(l, direction8);
    call_script(2, 0, function_name);
  }
}

/**
 * @brief Notifies the script that a directional keyboard key was just pressed
 * or that a joypad directional command has just changed.
 * @param event the corresponding input event
 */
void MenuScript::event_direction_pressed(InputEvent& event) {

  const std::string function_name("event_direction_pressed");
  if (find_global_function(function_name)) {
    int direction8 = event.get_direction();

    lua_pushinteger(l, direction8);
    call_script(1, 0, function_name);
  }
}

