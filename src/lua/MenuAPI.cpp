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

static const std::string& on_started_name = "event_menu_started";
static const std::string& on_update_name = "event_update";
static const std::string& on_display_name = "event_display";
static const std::string& on_key_pressed_name = "event_key_pressed";
static const std::string& on_key_released_name = "event_key_released";
static const std::string& on_joyad_button_pressed_name = "event_joyad_button_pressed";
static const std::string& on_joyad_button_released_name = "event_joyad_button_released";
static const std::string& on_joyad_axis_moved_name = "event_joyad_axis_moved";
static const std::string& on_joyad_hat_moved_name = "event_joyad_hat_moved";
static const std::string& on_direction_pressed_name = "event_direction_pressed";

/**
 * @brief Initializes the menu features provided to Lua.
 */
void LuaContext::register_menu_module() {

  // Create a table sol.menus to store all menu scripts.
                                  // ...
  lua_getglobal(l, "sol");
                                  // ... sol
  lua_newtable(l);
                                  // ... sol menus
  lua_setfield(l, -2, "menus");
                                  // ... sol
  lua_pop(l, 1);
                                  // ...
}

/**
 * @brief Loads the script of a menu into this Lua world.
 * @param menu_id file name of the script without extension,
 * relative to the data directory.
 */
void LuaContext::load_menu(const std::string& menu_id) {

                                  // ...
  lua_getglobal(l, "sol");
                                  // ... sol
  lua_getfield(l, -1, "menus");
                                  // ... sol menus
  load(menu_id);
                                  // ... sol menus menu_function
  lua_setfield(l, -2, menu_id.c_str());
                                  // ... sol menus
  lua_pop(l, 2);
                                  // ...
}

/**
 * @brief Pushes on top of the stack the previously loaded function of a menu.
 * @param menu_id id of the menu script to get
 */
void LuaContext::push_menu_script(const std::string& menu_id) {
                                  // ...
  lua_getglobal(l, "sol");
                                  // ... sol
  lua_getfield(l, -1, "menus");
                                  // ... sol menus
  lua_getfield(l, -1, menu_id.c_str());
                                  // ... sol menus menu_fct/nil
  Debug::check_assertion(!lua_isnil(l, -1), StringConcat()
      << "Unknown menu '" << menu_id << "'");
                                  // ... sol menus menu_fct
  lua_remove(l, -2);
                                  // ... sol menu_fct
  lua_remove(l, -2);
                                  // ... menu_fct
}

/**
 * @brief Starts a menu script previously loaded.
 * @param menu_id Id of the menu to start.
 */
void LuaContext::start_menu(const std::string& menu_id) {

                                  // ...
  push_menu_script(menu_id);
                                  // ... menu_fct
  lua_pushvalue(l, -1);
                                  // ... menu_fct menu_fct
  call_script(0, 0, menu_id);
                                  // ... menu_fct
  menu_on_started();
  lua_pop(l, 1);
                                  // ...
}

/**
 * @brief Updates a scripted menu.
 *
 * This function is called at each cycle.
 *
 * @param menu_id Id of a menu.
 */
void LuaContext::update_menu(const std::string& menu_id) {

  push_menu_script(menu_id);
  menu_on_update();
  lua_pop(l, 1);
}

/**
 * @brief Displays the content of a scripted menu on a surface.
 * @param menu_id Id of a menu.
 * @param dst_surface The destination surface.
 */
void LuaContext::display_menu(const std::string& menu_id, Surface& dst_surface) {

  push_menu_script(menu_id);
  menu_on_display(dst_surface);
  lua_pop(l, 1);
}

/**
 * @brief Notifies a scripted menu that a low-level input event has occurred.
 * @param menu_id Id of a menu.
 * @param event The event to handle.
 */
void LuaContext::notify_input_menu(const std::string& menu_id, InputEvent& event) {

  // get the menu script
  push_menu_script(menu_id);

  // call the Lua function(s) corresponding to this input event
  if (event.is_keyboard_event()) {
    // keyboard
    if (event.is_keyboard_key_pressed()) {
      menu_on_key_pressed(event);
    }
    else if (event.is_keyboard_key_released()) {
      menu_on_key_released(event);
    }
  }
  else if (event.is_joypad_event()) {
    // joypad
    if (event.is_joypad_button_pressed()) {
      menu_on_joypad_button_pressed(event);
    }
    else if (event.is_joypad_button_released()) {
      menu_on_joypad_button_released(event);
    }
    else if (event.is_joypad_axis_moved()) {
      menu_on_joypad_axis_moved(event);
    }
    else if (event.is_joypad_hat_moved()) {
      menu_on_joypad_hat_moved(event);
    }
  }

  if (event.is_direction_pressed()) {
    // keyboard or joypad direction
    menu_on_direction_pressed(event);
  }

  // pop the menu script
  lua_pop(l, 1);
}

/**
 * @brief Notifies the menu script on top of the stack
 * that it has just started.
 */
void LuaContext::menu_on_started() {

  if (find_local_function(-1, on_started_name)) {
    call_script(0, 0, on_started_name);
  }
}

/**
 * @brief Updates the scripted menu on top of the stack.
 *
 * This function is called at each cycle.
 */
void LuaContext::menu_on_update() {

  if (find_local_function(-1, on_update_name)) {
    call_script(0, 0, on_update_name);
  }
}

/**
 * @brief Displays the scripted menu on top of the stack.
 * @param dst_surface The destination surface.
 */
void LuaContext::menu_on_display(Surface& dst_surface) {

  if (find_local_function(on_display_name)) {
    push_userdata(l, dst_surface);
    call_script(1, 0, on_display_name);
  }
}

/**
 * @brief Notifies the scripted menu on top of the stack
 * that a keyboard key was just pressed
 * (including if it is a directional key).
 * @param event The corresponding input event.
 */
void LuaContext::menu_on_key_pressed(InputEvent& event) {

  if (find_local_function(on_key_pressed_name)) {

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
      call_script(2, 0, on_key_pressed_name);
    }
    else {
      lua_pop(l, 1);
    }
  }
}

/**
 * @brief Notifies the scripted menu on top of the stack
 * that a keyboard key was just released
 * (including if it is a directional key).
 * @param event The corresponding input event.
 */
void LuaContext::menu_on_key_released(InputEvent& event) {

  if (find_local_function(on_key_released_name)) {

    const std::string& key_name = input_get_key_name(event.get_keyboard_key());
    if (!key_name.empty()) { // this key exists in the Lua API
      lua_pushstring(l, key_name.c_str());
      call_script(1, 0, on_key_released_name);
    }
    else {
      lua_pop(l, 1);
    }
  }
}

/**
 * @brief Notifies the scripted menu on top of the stack
 * that a joypad button was just pressed.
 * @param event The corresponding input event.
 */
void LuaContext::menu_on_joypad_button_pressed(InputEvent& event) {

  if (find_local_function(on_joyad_button_pressed_name)) {
    int button = event.get_joypad_button();

    lua_pushinteger(l, button);
    call_script(1, 0, on_joyad_button_pressed_name);
  }
}

/**
 * @brief Notifies the scripted menu on top of the stack
 * that a joypad button was just released.
 * @param event The corresponding input event.
 */
void LuaContext::menu_on_joypad_button_released(InputEvent& event) {

  if (find_local_function(on_joyad_button_released_name)) {
    int button = event.get_joypad_button();

    lua_pushinteger(l, button);
    call_script(1, 0, on_joyad_button_released_name);
  }
}

/**
 * @brief Notifies the scripted menu on top of the stack
 * that a joypad axis was just moved.
 * @param event The corresponding input event.
 */
void LuaContext::menu_on_joypad_axis_moved(InputEvent& event) {

  if (find_local_function(on_joyad_axis_moved_name)) {
    int axis = event.get_joypad_axis();
    int state = event.get_joypad_axis_state();

    lua_pushinteger(l, axis);
    lua_pushinteger(l, state);
    call_script(2, 0, on_joyad_axis_moved_name);
  }
}

/**
 * @brief Notifies the scripted menu on top of the stack
 * that a joypad hat was just moved.
 * @param event The corresponding input event.
 */
void LuaContext::menu_on_joypad_hat_moved(InputEvent& event) {

  if (find_global_function(on_joyad_hat_moved_name)) {
    int hat = event.get_joypad_hat();
    int direction8 = event.get_joypad_hat_direction();

    lua_pushinteger(l, hat);
    lua_pushinteger(l, direction8);
    call_script(2, 0, on_joyad_hat_moved_name);
  }
}

/**
 * @brief Notifies the menu script on top of the stack
 * that a directional keyboard key was just pressed
 * or that a joypad directional command has just changed.
 * @param event The corresponding input event.
 */
void LuaContext::menu_on_direction_pressed(InputEvent& event) {

  if (find_global_function(on_direction_pressed_name)) {
    int direction8 = event.get_direction();

    lua_pushinteger(l, direction8);
    call_script(1, 0, on_direction_pressed_name);
  }
}

