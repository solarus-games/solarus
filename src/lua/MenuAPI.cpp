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
#include "menus/CustomMenu.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"
#include "lowlevel/Surface.h"
#include <lua.hpp>

static const std::string& on_started_name = "on_started";
static const std::string& on_update_name = "on_update";
static const std::string& on_display_name = "on_display";
static const std::string& on_key_pressed_name = "on_key_pressed";
static const std::string& on_key_released_name = "on_key_released";
static const std::string& on_joyad_button_pressed_name = "on_joyad_button_pressed";
static const std::string& on_joyad_button_released_name = "on_joyad_button_released";
static const std::string& on_joyad_axis_moved_name = "on_joyad_axis_moved";
static const std::string& on_joyad_hat_moved_name = "on_joyad_hat_moved";
static const std::string& on_direction_pressed_name = "on_direction_pressed";

/**
 * @brief Initializes the menu features provided to Lua.
 */
void LuaContext::register_menu_module() {

  // Create a table sol.menus to store all menus.
                                  // ...
  lua_getglobal(l, "sol");
                                  // ... sol
  lua_newtable(l);
                                  // ... sol menus
  lua_setfield(l, -2, "menus");
                                  // ... sol
  lua_pop(l, 1);
                                  // ...

  // TODO simplify the storage method:
  // use luaL_ref or move more management code to Lua?
}

/**
 * @brief Loads a menu class into this Lua world.
 * @param menu The menu script to load.
 */
void LuaContext::start_menu(CustomMenu& menu) {

                                  // ...
  lua_getglobal(l, "sol");
                                  // ... sol
  lua_getfield(l, -1, "menus");
                                  // ... sol menus
  lua_pushlightuserdata(l, &menu);
                                  // ... sol menus cmenu
  load(l, menu.get_id());
                                  // ... sol menus cmenu menu_fct
  call_script(0, 1, menu.get_id());
                                  // ... sol menus cmenu menu
  lua_pushvalue(l, -1);
                                  // ... sol menus cmenu menu menu
  lua_insert(l, -3);
                                  // ... sol menus menu cmenu menu
  lua_settable(l, -4);
                                  // ... sol menus menu
  menu_on_started();
                                  // ... sol menus menu
  lua_pop(l, 3);
                                  // ...
}

/**
 * @brief Stops a scripted menu previously started.
 * @param menu The scripted menu to stop.
 */
void LuaContext::stop_menu(CustomMenu& menu) {

                                  // ...
  lua_getglobal(l, "sol");
                                  // ... sol
  lua_getfield(l, -1, "menus");
                                  // ... sol menus
  lua_pushlightuserdata(l, &menu);
                                  // ... sol menus cmenu
  lua_pushnil(l);
                                  // ... sol menus cmenu nil
  lua_settable(l, -3);
                                  // ... sol menus
  lua_pop(l, 2);
                                  // ...
  // TODO stop timers attached to this scripted menu
}

/**
 * @brief Pushes a menu onto the stack.
 * @param l The Lua context.
 * @param menu A scripted menu.
 */
void LuaContext::push_menu(lua_State* l, CustomMenu& menu) {

                                  // ...
  lua_getglobal(l, "sol");
                                  // ... sol
  lua_getfield(l, -1, "menus");
                                  // ... sol menus
  lua_pushlightuserdata(l, &menu);
                                  // ... sol menus cmenu
  lua_gettable(l, -2);
                                  // ... sol menus menu
  lua_remove(l, -2);
                                  // ... sol menu
  lua_remove(l, -2);
                                  // ... menu
}

/**
 * @brief Updates a scripted menu.
 *
 * This function is called at each cycle.
 *
 * @param menu A scripted menu.
 */
void LuaContext::update_menu(CustomMenu& menu) {

  push_menu(l, menu);
  menu_on_update();
  lua_pop(l, 1);
}

/**
 * @brief Displays the content of a scripted menu on a surface.
 * @param menu A scripted menu.
 * @param dst_surface The destination surface.
 */
void LuaContext::display_menu(CustomMenu& menu, Surface& dst_surface) {

  push_menu(l, menu);
  menu_on_display(dst_surface);
  lua_pop(l, 1);
}

/**
 * @brief Notifies a scripted menu that a low-level input event has occurred.
 * @param menu A scripted menu.
 * @param event The event to handle.
 */
void LuaContext::notify_input_menu(CustomMenu& menu, InputEvent& event) {

  // Get the menu in Lua.
  push_menu(l, menu);

  // Call the Lua function(s) corresponding to this input event.
  if (event.is_keyboard_event()) {
    // Keyboard.
    if (event.is_keyboard_key_pressed()) {
      menu_on_key_pressed(event);
    }
    else if (event.is_keyboard_key_released()) {
      menu_on_key_released(event);
    }
  }
  else if (event.is_joypad_event()) {
    // Joypad.
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
    // Keyboard or joypad direction.
    menu_on_direction_pressed(event);
  }

  // Pop the menu.
  lua_pop(l, 1);
}

/**
 * @brief Notifies the menu script on top of the stack
 * that it has just started.
 */
void LuaContext::menu_on_started() {

  if (find_method(on_started_name)) {
    call_script(1, 0, on_started_name);
  }
}

/**
 * @brief Updates the scripted menu on top of the stack.
 *
 * This function is called at each cycle.
 */
void LuaContext::menu_on_update() {

  if (find_method(-1, on_update_name)) {
    call_script(1, 0, on_update_name);
  }
}

/**
 * @brief Displays the scripted menu on top of the stack.
 * @param dst_surface The destination surface.
 */
void LuaContext::menu_on_display(Surface& dst_surface) {

  if (find_method(on_display_name)) {
    push_surface(l, dst_surface);
    call_script(2, 0, on_display_name);
  }
}

/**
 * @brief Notifies the scripted menu on top of the stack
 * that a keyboard key was just pressed
 * (including if it is a directional key).
 * @param event The corresponding input event.
 */
void LuaContext::menu_on_key_pressed(InputEvent& event) {

  if (find_method(on_key_pressed_name)) {

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
      call_script(3, 0, on_key_pressed_name);
    }
    else {
      // The method exists but the key is unknown.
      lua_pop(l, 2);
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

  if (find_method(on_key_released_name)) {

    const std::string& key_name = input_get_key_name(event.get_keyboard_key());
    if (!key_name.empty()) { // This key exists in the Lua API.
      lua_pushstring(l, key_name.c_str());
      call_script(2, 0, on_key_released_name);
    }
    else {
      // The method exists but the key is unknown.
      lua_pop(l, 2);
    }
  }
}

/**
 * @brief Notifies the scripted menu on top of the stack
 * that a joypad button was just pressed.
 * @param event The corresponding input event.
 */
void LuaContext::menu_on_joypad_button_pressed(InputEvent& event) {

  if (find_method(on_joyad_button_pressed_name)) {
    int button = event.get_joypad_button();

    lua_pushinteger(l, button);
    call_script(2, 0, on_joyad_button_pressed_name);
  }
}

/**
 * @brief Notifies the scripted menu on top of the stack
 * that a joypad button was just released.
 * @param event The corresponding input event.
 */
void LuaContext::menu_on_joypad_button_released(InputEvent& event) {

  if (find_method(on_joyad_button_released_name)) {
    int button = event.get_joypad_button();

    lua_pushinteger(l, button);
    call_script(2, 0, on_joyad_button_released_name);
  }
}

/**
 * @brief Notifies the scripted menu on top of the stack
 * that a joypad axis was just moved.
 * @param event The corresponding input event.
 */
void LuaContext::menu_on_joypad_axis_moved(InputEvent& event) {

  if (find_method(on_joyad_axis_moved_name)) {
    int axis = event.get_joypad_axis();
    int state = event.get_joypad_axis_state();

    lua_pushinteger(l, axis);
    lua_pushinteger(l, state);
    call_script(3, 0, on_joyad_axis_moved_name);
  }
}

/**
 * @brief Notifies the scripted menu on top of the stack
 * that a joypad hat was just moved.
 * @param event The corresponding input event.
 */
void LuaContext::menu_on_joypad_hat_moved(InputEvent& event) {

  if (find_method(on_joyad_hat_moved_name)) {
    int hat = event.get_joypad_hat();
    int direction8 = event.get_joypad_hat_direction();

    lua_pushinteger(l, hat);
    lua_pushinteger(l, direction8);
    call_script(3, 0, on_joyad_hat_moved_name);
  }
}

/**
 * @brief Notifies the menu script on top of the stack
 * that a directional keyboard key was just pressed
 * or that a joypad directional command has just changed.
 * @param event The corresponding input event.
 */
void LuaContext::menu_on_direction_pressed(InputEvent& event) {

  if (find_method(on_direction_pressed_name)) {
    int direction8 = event.get_direction();

    lua_pushinteger(l, direction8);
    call_script(2, 0, on_direction_pressed_name);
  }
}

