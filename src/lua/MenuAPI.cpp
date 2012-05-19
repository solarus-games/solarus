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
#include "lua/LuaContext.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"
#include "lowlevel/Surface.h"
#include <lua.hpp>

static const std::string& on_started_name = "start";
static const std::string& on_update_name = "update";
static const std::string& on_display_name = "on_display";
static const std::string& on_key_pressed_name = "key_pressed";
static const std::string& on_key_released_name = "key_released";
static const std::string& on_joyad_button_pressed_name = "joyad_button_pressed";
static const std::string& on_joyad_button_released_name = "joyad_button_released";
static const std::string& on_joyad_axis_moved_name = "joyad_axis_moved";
static const std::string& on_joyad_hat_moved_name = "joyad_hat_moved";
static const std::string& on_direction_pressed_name = "direction_pressed";

/**
 * @brief Initializes the menu features provided to Lua.
 */
void LuaContext::register_menu_module() {

}

/**
 *
 * @brief Notifies the lua context that the game starts.
 *
 * This function is called once at the beginning of the game.
 */
void LuaContext::notify_start() {

  // Nothing special here - If the function exists.
  if (find_event_function(on_started_name)) {
    // Call it.
    call_function(0, 0, "sol.events.start");
  }
}

/**
 * @brief Updates the Lua world.
 *
 * This function is called at each cycle.
 */
void LuaContext::update() {

  Script::update();

  // Nothing special here - If the function exists
  if(find_event_function(on_update_name)) {
    // Call it.
    call_function(0, 0, "sol.events.update");
  }

}

/**
 * @brief Calls the display event for a screen object.
 * @param dst_surface The destination surface.
 * @param screen_ref A reference to the screen object.
 */
void LuaContext::notify_screen_display(Surface& dst_surface, int screen_ref) {

  // Push the screen object.
  push_ref(l, screen_ref);

  // Find its on_display method.
  if (find_method(on_display_name)) {

    // Push the surface.
    push_surface(l, dst_surface);

    // Call it.
    call_function(2, 0, on_display_name);
  }

  // Pop the object ref.
  lua_pop(l, 1);
}

/**
 * @brief Calls the after_display event.
 * @param dst_surface The destination surface.
 */
void LuaContext::notify_after_display(Surface& dst_surface) {

  if (find_event_function("after_display")) {
    push_surface(l, dst_surface);
    call_function(1, 0, "sol.events.display");
  }
}

/**
 * @brief Delegates an input event.
 * @param event The event to handle.
 */
void LuaContext::notify_input(InputEvent& event) {

  // Call the Lua function(s) corresponding to this input event.
  if (event.is_keyboard_event()) {
    // Keyboard.
    if (event.is_keyboard_key_pressed()) {
      on_key_pressed(event);
    }
    else if (event.is_keyboard_key_released()) {
      on_key_released(event);
    }
  }
  else if (event.is_joypad_event()) {
    // Joypad.
    if (event.is_joypad_button_pressed()) {
      on_joypad_button_pressed(event);
    }
    else if (event.is_joypad_button_released()) {
      on_joypad_button_released(event);
    }
    else if (event.is_joypad_axis_moved()) {
      on_joypad_axis_moved(event);
    }
    else if (event.is_joypad_hat_moved()) {
      on_joypad_hat_moved(event);
    }
  }

  if (event.is_direction_pressed()) {
    // Keyboard or joypad direction.
    on_direction_pressed(event);
  }

}

/**
 * @brief Notifies the lua context that
 * a keyboard key was just pressed
 * (including if it is a directional key).
 * @param event The corresponding input event.
 */
void LuaContext::on_key_pressed(InputEvent& event) {

  if (find_event_function(on_key_pressed_name)) {

    const std::string& key_name = input_get_key_name(event.get_keyboard_key());
    if (!key_name.empty()) { // This key exists in the Lua API.

      lua_pushstring(l, key_name.c_str());
      lua_newtable(l);

      if (event.is_with_shift()) {
        lua_pushboolean(l, 1);
        lua_setfield(l, -2, "shift");
      }

      if (event.is_with_control()) {
        lua_pushboolean(l, 1);
        lua_setfield(l, -2, "control");
      }

      if (event.is_with_alt()) {
        lua_pushboolean(l, 1);
        lua_setfield(l, -2, "alt");
      }
      call_function(2, 0, on_key_pressed_name);
    }
    else {
      // The method exists but the key is unknown.
      lua_pop(l, 1);
    }
  }
}

/**
 * @brief Notifies the lua context
 * that a keyboard key was just released
 * (including if it is a directional key).
 * @param event The corresponding input event.
 */
void LuaContext::on_key_released(InputEvent& event) {

  if (find_event_function(on_key_released_name)) {

    const std::string& key_name = input_get_key_name(event.get_keyboard_key());
    if (!key_name.empty()) { // This key exists in the Lua API.
      lua_pushstring(l, key_name.c_str());
      call_function(1, 0, on_key_released_name);
    }
    else {
      // The method exists but the key is unknown.
      lua_pop(l, 1);
    }
  }
}

/**
 * @brief Notifies the lua context
 * that a joypad button was just pressed.
 * @param event The corresponding input event.
 */
void LuaContext::on_joypad_button_pressed(InputEvent& event) {

  if (find_event_function(on_joyad_button_pressed_name)) {
    int button = event.get_joypad_button();

    lua_pushinteger(l, button);
    call_function(1, 0, on_joyad_button_pressed_name);
  }
}

/**
 * @brief Notifies the lua context
 * that a joypad button was just released.
 * @param event The corresponding input event.
 */
void LuaContext::on_joypad_button_released(InputEvent& event) {

  if (find_event_function(on_joyad_button_released_name)) {
    int button = event.get_joypad_button();

    lua_pushinteger(l, button);
    call_function(1, 0, on_joyad_button_released_name);
  }
}

/**
 * @brief Notifies the lua context
 * that a joypad axis was just moved.
 * @param event The corresponding input event.
 */
void LuaContext::on_joypad_axis_moved(InputEvent& event) {

  if (find_event_function(on_joyad_axis_moved_name)) {
    int axis = event.get_joypad_axis();
    int state = event.get_joypad_axis_state();

    lua_pushinteger(l, axis);
    lua_pushinteger(l, state);
    call_function(2, 0, on_joyad_axis_moved_name);
  }
}

/**
 * @brief Notifies the lua context
 * that a joypad hat was just moved.
 * @param event The corresponding input event.
 */
void LuaContext::on_joypad_hat_moved(InputEvent& event) {

  if (find_event_function(on_joyad_hat_moved_name)) {
    int hat = event.get_joypad_hat();
    int direction8 = event.get_joypad_hat_direction();

    lua_pushinteger(l, hat);
    lua_pushinteger(l, direction8);
    call_function(2, 0, on_joyad_hat_moved_name);
  }
}

/**
 * @brief Notifies the menu script on top of the stack
 * that a directional keyboard key was just pressed
 * or that a joypad directional command has just changed.
 * @param event The corresponding input event.
 */
void LuaContext::on_direction_pressed(InputEvent& event) {

  if (find_event_function(on_direction_pressed_name)) {
    int direction8 = event.get_direction();

    lua_pushinteger(l, direction8);
    call_function(1, 0, on_direction_pressed_name);
  }
}

