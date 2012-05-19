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

/**
 * @brief Creates a Lua context.
 * @param main_loop The Solarus main loop manager.
 */
LuaContext::LuaContext(MainLoop& main_loop):
  Script(main_loop) {

  initialize_lua_context();

  register_menu_module();
  register_events_module();

  start_main_script();
}

/**
 * @brief Destroys this Lua context.
 */
LuaContext::~LuaContext() {

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
 * @brief Gets a local Lua function from the sol.events table.
 *
 * @param function_name Name of the function to find in the sol.events table.
 *
 * @return true if the function was found.
 */
bool LuaContext::find_event_function(const std::string& function_name) {

  // Debug::print(function_name);

  lua_getglobal(l, "sol");
                                  // sol
  lua_getfield(l, -1, "events");
                                  // sol sol.events
  lua_remove(l, -2);
                                  // sol.events
  lua_getfield(l, -1, function_name.c_str());
                                  // sol.events sol.events.function
  lua_remove(l, -2);
                                  // sol.events.function

  bool exists = lua_isfunction(l, -1);

  // Restore the stack.
  if (!exists) {
    lua_pop(l, 1);
  }

  return exists;
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

  if (index < 0) {
    // ensure a positive index
    index = lua_gettop(l) + index + 1;
  }

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
 * @brief Loads and executes main.lua.
 */
void LuaContext::start_main_script() {

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
