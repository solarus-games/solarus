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
#include "lowlevel/FileTools.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"
#include <sstream>
#include <lua.hpp>

static const std::string& on_update_name = "on_update";
static const std::string& on_key_pressed_name = "on_key_pressed";
static const std::string& on_key_released_name = "on_key_released";
static const std::string& on_joyad_button_pressed_name = "on_joyad_button_pressed";
static const std::string& on_joyad_button_released_name = "on_joyad_button_released";
static const std::string& on_joyad_axis_moved_name = "on_joyad_axis_moved";
static const std::string& on_joyad_hat_moved_name = "on_joyad_hat_moved";
static const std::string& on_direction_pressed_name = "on_direction_pressed";
static const std::string& on_started_name = "on_started";
static const std::string& on_finished_name = "on_finished";

/**
 * @brief Creates a Lua context.
 * @param main_loop The Solarus main loop manager.
 */
LuaContext::LuaContext(MainLoop& main_loop):
  Script(main_loop) {

}

/**
 * @brief Destroys this Lua context.
 */
LuaContext::~LuaContext() {

  this->exit();
}

/**
 * @brief Initializes Lua.
 */
void LuaContext::initialize() {

  Script::initialize();
  register_menu_module();

  // Make require() able to load Lua files even from the data.solarus archive.
                                  // ...
  lua_getglobal(l, "sol");
                                  // ... sol
  lua_pushcfunction(l, l_loader);
                                  // ... sol loader
  lua_setfield(l, -2, "loader");
                                  // ... sol
  luaL_dostring(l, "table.insert(package.loaders, 2, sol.loader)");
                                  // ... sol
  lua_pushnil(l);
                                  // ... sol nil
  lua_setfield(l, -2, "loader");
                                  // ... sol
  lua_pop(l, 1);
                                  // ...

  // Load the main file.
  load(l, "main");
  call_function(0, 0, "main");
}

/**
 * @brief Cleans Lua.
 */
void LuaContext::exit() {

  if (l != NULL) {
    main_on_finished();
  }
  Script::exit();
}

/**
 * @brief A loader that makes require() able to load quest scripts.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::l_loader(lua_State* l) {

  const std::string& script_name = luaL_checkstring(l, 1);
  bool exists = load_if_exists(l, script_name);

  if (!exists) {
    std::ostringstream oss;
    oss << "\n\tno quest file '" << script_name << ".lua' in 'data' or 'data.solarus'";
    lua_pushstring(l, oss.str().c_str());
  }
  return 1;
}

/**
 * @brief Opens a script and lets it on top of the stack as a function.
 * @param l A Lua state.
 * @param script_name File name of the script without extension,
 * relative to the data directory.
 */
void LuaContext::load(lua_State* l, const std::string& script_name) {

  if (!load_if_exists(l, script_name)) {
    Debug::die(StringConcat() << "Cannot find script file '"
        << script_name << "'");
  }
}

/**
 * @brief Opens a script if it exists and lets it on top of the stack as a
 * function.
 * @param l A Lua state.
 * @param script_name File name of the script without extension,
 * relative to the data directory.
 * @return true if the script exists and was loaded.
 */
bool LuaContext::load_if_exists(lua_State* l,
    const std::string& script_name) {

  // Determine the file name (.lua).
  std::ostringstream oss;
  oss << script_name << ".lua";
  std::string file_name = oss.str();

  if (FileTools::data_file_exists(file_name)) {
    // Load the file.
    size_t size;
    char* buffer;
    FileTools::data_file_open_buffer(file_name, &buffer, &size);
    int result = luaL_loadbuffer(l, buffer, size, file_name.c_str());
    FileTools::data_file_close_buffer(buffer);

    if (result != 0)
    {
      Debug::die(StringConcat() << "Error: failed to load script '"
          << script_name << "': " << lua_tostring(l, -1));
    }
    return true;
  }
  return false;
}

/**
 * @brief Gets a local Lua function from the environment of another one
 * on top of the stack.
 *
 * This is equivalent to find_local_function(-1, function_name).
 *
 * @param function_name Name of the function to find in the environment of the
 * first one.
 * @return true if the function was found.
 */
bool LuaContext::find_local_function(const std::string& function_name) {

  return find_local_function(-1, function_name);
}

/**
 * @brief Gets a local Lua function from the environment of another one.
 *
 * The function found is placed on top the stack if it exists.
 * If the function is not found, the stack is left unchanged.
 *
 * @param index Index of an existing function in the stack.
 * @param function_name Name of the function to find in the environment of the
 * first one.
 * @return true if the function was found.
 */
bool LuaContext::find_local_function(int index, const std::string& function_name) {

                                  // ... f1 ...
  lua_getfenv(l, index);
                                  // ... f1 ... env
  lua_getfield(l, -1, function_name.c_str());
                                  // ... f1 ... env f2/?
  bool exists = lua_isfunction(l, -1);

  // Restore the stack.
  if (exists) {
    lua_remove(l, -2);
                                  // ... f1 ... f2
  }
  else {
    lua_pop(l, 2);
                                  // ... f1 ...
  }

  return exists;
}

/**
 * @brief Gets a method of the object on top of the stack.
 *
 * This is equivalent to find_method(-1, function_name).
 *
 * @param function_name Name of the function to find in the object.
 * @return true if the function was found.
 */
bool LuaContext::find_method(const std::string& function_name) {

  return find_method(-1, function_name);
}

/**
 * @brief Gets a method of an object.
 *
 * If the method exists, the method and the object are both pushed
 * so that you can call the method immediately with the object as first parameter.
 * If the method is not found, the stack is left unchanged.
 *
 * @param index Index of the object in the stack.
 * @param function_name Name of the function to find in the object.
 * @return true if the function was found.
 */
bool LuaContext::find_method(int index, const std::string& function_name) {

  index = get_positive_index(l, index);

                                  // ... object ...
  lua_getfield(l, index, function_name.c_str());
                                  // ... object ... method/?

  bool exists = lua_isfunction(l, -1);

  if (exists) {
                                  // ... object ... method
    lua_pushvalue(l, index);
                                  // ... object ... method object
  }
  else {
    // Restore the stack.
    lua_pop(l, 1);
                                  // ... object ...
  }

  return exists;
}

/**
 * @brief Updates the Lua world.
 *
 * This function is called at each cycle.
 * sol.main.on_update() is called if it exists.
 */
void LuaContext::update() {

  Script::update();

  // Call sol.main.on_update().
  main_on_update();
}

/**
 * @brief Notifies the general Lua script that an input event has just occurred.
 *
 * The appropriate callback in sol.main is notified.
 *
 * @param event The input event to handle.
 */
void LuaContext::notify_input(InputEvent& event) {

  // Call the appropriate callback in sol.main (if it exists).
  push_main(l);
  on_input(event);
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_update() method of the object on top of the stack.
 */
void LuaContext::on_update() {

  if (find_method(on_update_name)) {
    call_function(1, 0, on_update_name);
  }
}

/**
 * @brief Calls an input callback method of the object on top of the stack.
 * @param event The input event to forward.
 */
void LuaContext::on_input(InputEvent& event) {

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
 * @brief Notifies the object on top of the stack
 * that a keyboard key was just pressed
 * (including if it is a directional key).
 * @param event The corresponding input event.
 */
void LuaContext::on_key_pressed(InputEvent& event) {

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
      call_function(3, 0, on_key_pressed_name);
    }
    else {
      // The method exists but the key is unknown.
      lua_pop(l, 1);
    }
  }
}

/**
 * @brief Notifies the object on top of the stack
 * that a keyboard key was just released
 * (including if it is a directional key).
 * @param event The corresponding input event.
 */
void LuaContext::on_key_released(InputEvent& event) {

  if (find_method(on_key_released_name)) {

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
 * @brief Notifies the object on top of the stack
 * that a joypad button was just pressed.
 * @param event The corresponding input event.
 */
void LuaContext::on_joypad_button_pressed(InputEvent& event) {

  if (find_method(on_joyad_button_pressed_name)) {
    int button = event.get_joypad_button();

    lua_pushinteger(l, button);
    call_function(1, 0, on_joyad_button_pressed_name);
  }
}

/**
 * @brief Notifies the object on top of the stack
 * that a joypad button was just released.
 * @param event The corresponding input event.
 */
void LuaContext::on_joypad_button_released(InputEvent& event) {

  if (find_method(on_joyad_button_released_name)) {
    int button = event.get_joypad_button();

    lua_pushinteger(l, button);
    call_function(1, 0, on_joyad_button_released_name);
  }
}

/**
 * @brief Notifies the object on top of the stack
 * that a joypad axis was just moved.
 * @param event The corresponding input event.
 */
void LuaContext::on_joypad_axis_moved(InputEvent& event) {

  if (find_method(on_joyad_axis_moved_name)) {
    int axis = event.get_joypad_axis();
    int state = event.get_joypad_axis_state();

    lua_pushinteger(l, axis);
    lua_pushinteger(l, state);
    call_function(2, 0, on_joyad_axis_moved_name);
  }
}

/**
 * @brief Notifies the object on top of the stack
 * that a joypad hat was just moved.
 * @param event The corresponding input event.
 */
void LuaContext::on_joypad_hat_moved(InputEvent& event) {

  if (find_method(on_joyad_hat_moved_name)) {
    int hat = event.get_joypad_hat();
    int direction8 = event.get_joypad_hat_direction();

    lua_pushinteger(l, hat);
    lua_pushinteger(l, direction8);
    call_function(2, 0, on_joyad_hat_moved_name);
  }
}

/**
 * @brief Notifies the object on top of the stack
 * that a directional keyboard key was just pressed
 * or that a joypad directional command has just changed.
 * @param event The corresponding input event.
 */
void LuaContext::on_direction_pressed(InputEvent& event) {

  if (find_method(on_direction_pressed_name)) {
    int direction8 = event.get_direction();

    lua_pushinteger(l, direction8);
    call_function(1, 0, on_direction_pressed_name);
  }
}

/**
 * @brief Calls the on_started() method of the object on top of the stack.
 */
void LuaContext::on_started() {

  if (find_method(on_started_name)) {
    call_function(1, 0, on_started_name);
  }
}

/**
 * @brief Calls the on_finished() method of the object on top of the stack.
 */
void LuaContext::on_finished() {

  if (find_method(on_finished_name)) {
    call_function(1, 0, on_finished_name);
  }
}

