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
#include "entities/Destination.h"
#include "entities/Switch.h"
#include "entities/Sensor.h"
#include "entities/NPC.h"
#include "entities/Chest.h"
#include "entities/ShopItem.h"
#include "entities/Door.h"
#include "entities/Block.h"
#include "entities/Enemy.h"
#include "entities/Pickable.h"
#include "lowlevel/FileTools.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"
#include "EquipmentItem.h"
#include "Treasure.h"
#include "Map.h"
#include <sstream>
#include <iomanip>
#include <lua.hpp>

/**
 * @brief Creates a Lua context.
 * @param main_loop The Solarus main loop manager.
 */
LuaContext::LuaContext(MainLoop& main_loop):
  l(NULL),
  main_loop(main_loop) {

}

/**
 * @brief Destroys this Lua context.
 */
LuaContext::~LuaContext() {

  this->exit();
}

/**
 * @brief Returns the LuaContext object that encapsulates a Lua state.
 * @param l A Lua state.
 * @return The LuaContext object encapsulating this Lua state.
 */
LuaContext& LuaContext::get_lua_context(lua_State* l) {

  // Retrieve the LuaContext object.
  lua_getfield(l, LUA_REGISTRYINDEX, "sol.cpp_object");
  LuaContext* lua_context = static_cast<LuaContext*>(lua_touserdata(l, -1));
  lua_pop(l, 1);

  Debug::check_assertion(lua_context != NULL,
      "This Lua state does not belong to a LuaContext object");

  return *lua_context;
}

/**
 * @brief Returns the Solarus main loop object.
 * @return The main loop manager.
 */
MainLoop& LuaContext::get_main_loop() {
  return main_loop;
}

/**
 * @brief Initializes Lua.
 */
void LuaContext::initialize() {

  // Create an execution context.
  l = lua_open();
  luaL_openlibs(l);

  // Put a pointer to this LuaContext object in the Lua context.
                                  // --
  lua_pushlightuserdata(l, this);
                                  // this
  lua_setfield(l, LUA_REGISTRYINDEX, "sol.cpp_object");
                                  // --

  // Allow userdata to be indexable if they want.
  lua_newtable(l);
                                  // udata_tables
  lua_newtable(l);
                                  // udata_tables meta
  lua_pushstring(l, "__mode");
                                  // udata_tables meta "__mode"
  lua_pushstring(l, "k");
                                  // udata_tables meta "__mode" "k"
  lua_settable(l, -3);
                                  // udata_tables meta
  lua_setmetatable(l, -2);
                                  // udata_tables
  lua_setfield(l, LUA_REGISTRYINDEX, "sol.userdata_tables");
                                  // --

  // Create the sol table that will contain the whole Solarus API.
  lua_newtable(l);
  lua_setglobal(l, "sol");

  // Register the C++ functions and types accessible by Lua.
  register_modules();

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

  // Execute the main file.
  do_file(l, "main");
  main_on_started();
}

/**
 * @brief Cleans Lua.
 */
void LuaContext::exit() {

  if (l != NULL) {
    // Call sol.main.on_finished() if it exists.
    main_on_finished();

    // Destroy unfinished menus and timers.
    remove_menus();
    remove_timers();

    // Finalize Lua.
    lua_close(l);
    l = NULL;
  }
}

/**
 * @brief Updates the Lua world.
 *
 * This function is called at each cycle.
 * sol.main.on_update() is called if it exists.
 */
void LuaContext::update() {

  update_drawables();
  update_timers();

  // Call sol.main.on_update().
  main_on_update();
}

/**
 * @brief Notifies Lua that an input event has just occurred.
 *
 * The appropriate callback in sol.main is notified.
 *
 * @param event The input event to handle.
 * @return \c true if the event was handled and should stop being propagated.
 */
bool LuaContext::notify_input(InputEvent& event) {

  // Call the appropriate callback in sol.main (if it exists).
  return main_on_input(event);
}

/**
 * @brief Notifies Lua that a map has just been started.
 *
 * The Lua file of this map is automatically loaded.
 *
 * @param map The map started.
 * @param destination The destination point used if it's a normal one,
 * NULL otherwise.
 */
void LuaContext::run_map(Map& map, Destination* destination) {

  // Compute the file name, depending on the id of the map.
  std::string file_name = std::string("maps/") + map.get_id() + ".script";

  // Load the map's code.
  load_file(l, file_name);
                                  // map_fun

  // Set a special environment to access map entities like global variables.
  lua_newtable(l);
                                  // map_fun env
  lua_newtable(l);
                                  // map_fun env env_mt
  push_map(l, map);
                                  // map_fun env env_mt map
  lua_pushcclosure(l, l_get_map_entity_or_global, 1);
                                  // map_fun env env_mt __index
  lua_setfield(l, -2, "__index");
                                  // map_fun env env_mt
  lua_setmetatable(l, -2);
                                  // map_fun env
  lua_setfenv(l, -2);
                                  // map_fun

  // Run the map's code with the map userdata as parameter.
  push_map(l, map);
  call_function(1, 0, file_name);

  // Call the map:on_started() callback.
  map_on_started(map, destination);
}

/**
 * @brief Notifies Lua that a map has just been suspended or resumed.
 * @param map A map.
 * @param suspended \c true if it is suspended, false if it is resumed.
 */
void LuaContext::notify_map_suspended(Map& map, bool suspended) {

  notify_timers_map_suspended(suspended);   // Notify timers.
  map_on_suspended(map, suspended);  // Call map:on_suspended()
}

/**
 * @brief Notifies the Lua world that an equipment item has just been created.
 *
 * The Lua file of this item is automatically loaded.
 *
 * @param item The item.
 */
void LuaContext::run_item(EquipmentItem& item) {

  // Compute the file name, depending on the id of the equipment item.
  std::string file_name = (std::string) "items/" + item.get_name();

  // Load the item's code.
  if (load_file_if_exists(l, file_name)) {

    // Run it with the item userdata as parameter.
    push_item(l, item);
    call_function(1, 0, file_name);

    // Call the item:on_created() callback.
    item_on_created(item);
  }
}

/**
 * @brief Notifies the Lua world that an enemy has just been added to the map.
 *
 * The Lua file of this enemy is automatically loaded.
 *
 * @param enemy The enemy.
 */
void LuaContext::run_enemy(Enemy& enemy) {

  // Compute the file name, depending on enemy's breed.
  std::string file_name = (std::string) "enemies/" + enemy.get_breed();

  // Load the enemy's code.
  if (load_file_if_exists(l, file_name)) {

    // Run it with the enemy userdata as parameter.
    push_enemy(l, enemy);
    call_function(1, 0, file_name);

    enemy_on_suspended(enemy, enemy.is_suspended());
    enemy_on_created(enemy);
  }
}

/**
 * @brief Notifies Lua that the sequence started by a call to
 * map:move_camera() has reached its target.
 * @param map The current map.
 */
void LuaContext::notify_camera_reached_target(Map& map) {

  // Set a timer to execute the function.
  lua_settop(l, 0);
  push_map(l, map);
  lua_getfield(l, LUA_REGISTRYINDEX, "sol.camera_delay_before");
  lua_pushcfunction(l, l_camera_do_callback);
  timer_api_start(l);
}

/**
 * @brief Notifies Lua that a dialog is finished.
 * @param callback_ref Lua ref of the function to call, if any.
 * @param answer Answer of the dialog if there was a question.
 */
void LuaContext::notify_dialog_finished(int callback_ref, int answer) {

  if (callback_ref != LUA_REFNIL) {
    push_callback(callback_ref);
    lua_pushinteger(l, answer);
    call_function(1, 0, "dialog callback");
    destroy_ref(callback_ref);
  }
}

/**
 * @brief Checks that a table field is a number and returns it as an integer.
 *
 * This function acts like lua_getfield() followed by luaL_checkint().
 *
 * @param l A Lua state.
 * @param table_index Index of a table in the stack.
 * @param key Key of the field to get in that table.
 * @return The wanted field as an integer.
 */
int LuaContext::check_int_field(
    lua_State* l, int table_index, const std::string& key) {

  lua_getfield(l, table_index, key.c_str());
  if (!lua_isnumber(l, -1)) {
    luaL_argerror(l, table_index, (StringConcat() <<
        "Bad field '" << key << "' (integer expected, got " <<
        luaL_typename(l, -1)).c_str()
    );
  }

  int value = lua_tointeger(l, -1);
  lua_pop(l, 1);
  return value;
}

/**
 * @brief Like check_int_field() but with a default value.
 *
 * This function acts like lua_getfield() followed by luaL_optint().
 *
 * @param l A Lua state.
 * @param table_index Index of a table in the stack.
 * @param key Key of the field to get in that table.
 * @param default_value The default value to return if the field is \c nil.
 * @return The wanted field as an integer.
 */
int LuaContext::opt_int_field(
    lua_State* l, int table_index, const std::string& key, int default_value) {

  lua_getfield(l, table_index, key.c_str());
  int value = default_value;
  if (!lua_isnil(l, -1)) {

    if (!lua_isnumber(l, -1)) {
      luaL_argerror(l, table_index, (StringConcat() <<
          "Bad field '" << key << "' (integer expected, got " <<
          luaL_typename(l, -1)).c_str()
      );
    }
    value = lua_tointeger(l, -1);
  }

  lua_pop(l, 1);
  return value;
}

/**
 * @brief Checks that a table field is a number and returns it as a double.
 *
 * This function acts like lua_getfield() followed by luaL_checknumber().
 *
 * @param l A Lua state.
 * @param table_index Index of a table in the stack.
 * @param key Key of the field to get in that table.
 * @return The wanted field as a double.
 */
double LuaContext::check_number_field(
    lua_State* l, int table_index, const std::string& key) {

  lua_getfield(l, table_index, key.c_str());
  if (!lua_isnumber(l, -1)) {
    luaL_argerror(l, table_index, (StringConcat() <<
        "Bad field '" << key << "' (number expected, got " <<
        luaL_typename(l, -1)).c_str()
    );
  }

  int value = lua_tonumber(l, -1);
  lua_pop(l, 1);
  return value;
}

/**
 * @brief Like check_number_field() but with a default value.
 *
 * This function acts like lua_getfield() followed by luaL_optnumber().
 *
 * @param l A Lua state.
 * @param table_index Index of a table in the stack.
 * @param key Key of the field to get in that table.
 * @param default_value The default value to return if the field is \c nil.
 * @return The wanted field as a double.
 */
double LuaContext::opt_number_field(
    lua_State* l, int table_index, const std::string& key, double default_value) {

  lua_getfield(l, table_index, key.c_str());
  double value = default_value;
  if (!lua_isnil(l, -1)) {

    if (!lua_isnumber(l, -1)) {
      luaL_argerror(l, table_index, (StringConcat() <<
          "Bad field '" << key << "' (number expected, got " <<
          luaL_typename(l, -1)).c_str()
      );
    }
    value = lua_tonumber(l, -1);
  }

  lua_pop(l, 1);
  return value;
}

/**
 * @brief Checks that a table field is a string and returns it.
 *
 * This function acts like lua_getfield() followed by luaL_checkstring().
 *
 * @param l A Lua state.
 * @param table_index Index of a table in the stack.
 * @param key Key of the field to get in that table.
 * @return The wanted field as an string.
 */
const std::string LuaContext::check_string_field(
    lua_State* l, int table_index, const std::string& key) {

  lua_getfield(l, table_index, key.c_str());
  if (!lua_isstring(l, -1)) {
    luaL_argerror(l, table_index, (StringConcat() <<
        "Bad field '" << key << "' (string expected, got " <<
        luaL_typename(l, -1)).c_str()
    );
  }

  const std::string& value = lua_tostring(l, -1);
  lua_pop(l, 1);
  return value;
}

/**
 * @brief Like check_string_field() but with a default value.
 *
 * This function acts like lua_getfield() followed by luaL_optstring().
 *
 * @param l A Lua state.
 * @param table_index Index of a table in the stack.
 * @param key Key of the field to get in that table.
 * @param default_value The default value to return if the field is \c nil.
 * @return The wanted field as a string.
 */
const std::string LuaContext::opt_string_field(
    lua_State* l, int table_index, const std::string& key, const std::string& default_value) {

  lua_getfield(l, table_index, key.c_str());
  std::string value;
  if (lua_isnil(l, -1)) {
    value = default_value;
  }
  else {
    if (!lua_isstring(l, -1)) {
      luaL_argerror(l, table_index, (StringConcat() <<
          "Bad field '" << key << "' (string expected, got " <<
          luaL_typename(l, -1)).c_str()
      );
    }
    value = lua_tostring(l, -1);
  }

  lua_pop(l, 1);
  return value;
}

/**
 * @brief Checks that a table field is a boolean and returns it.
 *
 * This function acts like lua_getfield() followed by luaL_checktype()
 * and lua_toboolean().
 *
 * @param l A Lua state.
 * @param table_index Index of a table in the stack.
 * @param key Key of the field to get in that table.
 * @return The wanted field as a boolean.
 */
bool LuaContext::check_boolean_field(
    lua_State* l, int table_index, const std::string& key) {

  lua_getfield(l, table_index, key.c_str());
  if (lua_type(l, -1) != LUA_TBOOLEAN) {
    luaL_argerror(l, table_index, (StringConcat() <<
        "Bad field '" << key << "' (boolean expected, got " <<
        luaL_typename(l, -1)).c_str()
    );
  }

  bool value = lua_toboolean(l, -1);
  lua_pop(l, 1);
  return value;
}

/**
 * @brief Like check_boolean_field() but with a default value.
 *
 * This function acts like lua_getfield() followed by lua_toboolean().
 *
 * @param l A Lua state.
 * @param table_index Index of a table in the stack.
 * @param key Key of the field to get in that table.
 * @param default_value The default value to return if the field is \c nil.
 * @return The wanted field as a string.
 */
bool LuaContext::opt_boolean_field(
    lua_State* l, int table_index, const std::string& key, bool default_value) {

  lua_getfield(l, table_index, key.c_str());
  bool value = default_value;
  if (!lua_isnil(l, -1)) {

    if (lua_type(l, -1) != LUA_TBOOLEAN) {
      luaL_argerror(l, table_index, (StringConcat() <<
          "Bad field '" << key << "' (boolean expected, got " <<
          luaL_typename(l, -1)).c_str()
      );
    }
    value = lua_toboolean(l, -1);
  }

  lua_pop(l, 1);
  return value;
}

/**
 * @brief Checks that a table field is a function and returns a ref to it.
 *
 * This function acts like lua_getfield() followed by lua_isfunction() and
 * luaL_ref().
 *
 * @param l A Lua state.
 * @param table_index Index of a table in the stack.
 * @param key Key of the field to get in that table.
 * @return The wanted field as a Lua ref to the function.
 */
int LuaContext::check_function_field(
    lua_State* l, int table_index, const std::string& key) {

  lua_getfield(l, table_index, key.c_str());
  if (!lua_isfunction(l, -1)) {
    luaL_argerror(l, table_index, (StringConcat() <<
        "Bad field '" << key << "' (function expected, got " <<
        luaL_typename(l, -1)).c_str()
    );
  }

  int ref = luaL_ref(l, LUA_REGISTRYINDEX);
  return ref;
}

/**
 * @brief Like check_function_field() but the field is optional.
 *
 * This function acts like lua_getfield() followed by lua_isfunction() and
 * luaL_ref().
 *
 * @param l A Lua state.
 * @param table_index Index of a table in the stack.
 * @param key Key of the field to get in that table.
 * @return The wanted field as a Lua ref to the function, or LUA_REFNIL.
 */
int LuaContext::opt_function_field(
    lua_State* l, int table_index, const std::string& key) {

  lua_getfield(l, table_index, key.c_str());
  int ref = LUA_REFNIL;
  if (lua_isnil(l, -1)) {
    lua_pop(l, 1);
  }
  else {
    if (!lua_isfunction(l, -1)) {
      luaL_argerror(l, table_index, (StringConcat() <<
          "Bad field '" << key << "' (function expected, got " <<
          luaL_typename(l, -1)).c_str()
      );
    }
    ref = luaL_ref(l, LUA_REGISTRYINDEX);
  }

  return ref;
}

/**
 * @brief Creates a reference to the Lua value on top of the stack.
 * @return The reference created.
 */
int LuaContext::create_ref() {
  return luaL_ref(l, LUA_REGISTRYINDEX);
}

/**
 * @brief Releases a Lua reference.
 * @param ref The Lua reference to free.
 */
void LuaContext::destroy_ref(int ref) {
  luaL_unref(l, LUA_REGISTRYINDEX, ref);
}

/**
 * @brief Calls a function stored in the registry with a reference and
 * releases this reference.
 * @param callback_ref Reference of the function to call (if LUA_REFNIL,
 * nothing is done).
 */
void LuaContext::do_callback(int callback_ref) {

  if (callback_ref != LUA_REFNIL) {
    push_callback(callback_ref);
    call_function(0, 0, "callback");
    destroy_ref(callback_ref);
  }
}

/**
 * @brief Pushes onto the stack a function stored as a Lua ref.
 * @param callback_ref Reference of the function to call (must be
 * a valid ref).
 */
void LuaContext::push_callback(int callback_ref) {

  push_ref(l, callback_ref);
  if (!lua_isfunction(l, -1)) {
    Debug::die(StringConcat() << "No such Lua callback (function expected, got "
        << luaL_typename(l, -1) << ")");
  }
}

/**
 * @brief Releases the reference to a Lua callback.
 *
 * The callback may then be collected by Lua.
 *
 * @param callback_ref reference of the function (if LUA_REFNIL,
 * nothing is done)
 */
void LuaContext::cancel_callback(int callback_ref) {
  destroy_ref(callback_ref);
}

/**
 * @brief Looks up the specified global Lua function and places it onto the stack if it exists.
 *
 * If the function is not found, the stack is left unchanged.
 *
 * @param function_name Name of the function to find.
 * @return true if the function was found.
 */
bool LuaContext::find_global_function(const std::string& function_name) {

  if (l == NULL) {
    return false;
  }

  lua_getglobal(l, function_name.c_str());

  bool exists = lua_isfunction(l, -1);
  if (!exists) {  // Restore the stack.
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
 * @brief Calls the Lua function with its arguments on top of the stack.
 *
 * This function is like lua_pcall, except that it additionaly handles the
 * error message if an error occurs in the Lua code (the error is printed).
 * This function leaves the results on the stack if there is no error,
 * and leaves nothing on the stack in case of error.
 *
 * @param nb_arguments number of arguments placed on the Lua stack above the
 * function to call
 * @param nb_results number of results expected (you get them on the stack if
 * there is no error)
 * @param function_name a name describing the Lua function (only used to print
 * the error message if any)
 * @return true in case of success
 */
bool LuaContext::call_function(int nb_arguments, int nb_results,
    const std::string& function_name) {

  return call_function(l, nb_arguments, nb_results, function_name);
}

/**
 * @brief Calls the Lua function with its arguments on top of the stack.
 *
 * This function is like lua_pcall, except that it additionaly handles the
 * error message if an error occurs in the Lua code (the error is printed).
 * This function leaves the results on the stack if there is no error,
 * and leaves nothing on the stack in case of error.
 *
 * @param l A Lua state.
 * @param nb_arguments Number of arguments placed on the Lua stack above the
 * function to call.
 * @param nb_results Number of results expected (you get them on the stack if
 * there is no error).
 * @param function_name A name describing the Lua function (only used to print
 * the error message if any).
 * @return true in case of success.
 */
bool LuaContext::call_function(lua_State* l, int nb_arguments, int nb_results,
    const std::string& function_name) {

  if (lua_pcall(l, nb_arguments, nb_results, 0) != 0) {
    Debug::print(StringConcat() << "Error in " << function_name << "(): "
        << lua_tostring(l, -1));
    lua_pop(l, 1);
    return false;
  }

  return true;
}

/**
 * @brief Opens a script and lets it on top of the stack as a function.
 * @param l A Lua state.
 * @param script_name File name of the script without extension,
 * relative to the data directory.
 */
void LuaContext::load_file(lua_State* l, const std::string& script_name) {

  if (!load_file_if_exists(l, script_name)) {
    Debug::die(StringConcat() << "Cannot find script file '"
        << script_name << "'");
  }
}

/**
 * @brief Opens a script if it exists and lets it on top of the stack as a
 * function.
 *
 * If the file does not exist, the stack is left intact and false is returned.
 *
 * @param l A Lua state.
 * @param script_name File name of the script without extension,
 * relative to the data directory.
 * @return true if the file exists and was loaded.
 */
bool LuaContext::load_file_if_exists(lua_State* l, const std::string& script_name) {

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

    if (result != 0) {
      Debug::print(StringConcat() << "Error: failed to load script '"
          << script_name << "': " << lua_tostring(l, -1));
    }
    return true;
  }
  return false;
}

/**
 * @brief Opens a Lua file and executes it.
 *
 * This function just calls load_file() and call_function().
 * The file must exist.
 *
 * @param l A Lua state.
 * @param script_name File name of the script without extension,
 * relative to the data directory.
 */
void LuaContext::do_file(lua_State* l, const std::string& script_name) {

  load_file(l, script_name);
  call_function(l, 0, 0, script_name);
}

/**
 * @brief Opens a Lua file if it exists and executes it without arguments.
 *
 * This function just calls load_file_if_exists() and call_function().
 * Nothing is done if the file does not exists.
 *
 * @param l A Lua state.
 * @param script_name File name of the script without extension,
 * relative to the data directory.
 * @return true if the file exists and was successfully executed.
 */
bool LuaContext::do_file_if_exists(lua_State* l, const std::string& script_name) {

  if (load_file_if_exists(l, script_name)) {
    call_function(l, 0, 0, script_name);
    return true;
  }
  return false;
}

/**
 * @brief For an index in the Lua stack, returns an equivalent positive index.
 * @param l A Lua state.
 * @param index An index in the stack (positive or negative).
 * @return The corresponding positive index.
 */
int LuaContext::get_positive_index(lua_State* l, int index) {

  if (index < 0) {
    index = lua_gettop(l) + index + 1;
  }
  return index;
}

/**
 * @brief Prints on a line the content of the Lua stack for debugging purposes.
 */
void LuaContext::print_stack() {

  int i;
  int top = lua_gettop(l);

  for (i = 1; i <= top; i++) {

    int type = lua_type(l, i);
    switch (type) {

      case LUA_TSTRING:
        std::cout << lua_tostring(l, i);
        break;

      case LUA_TBOOLEAN:
        std::cout << (lua_toboolean(l, i) ? "true" : "false");
        break;

      case LUA_TNUMBER:
        std::cout << lua_tonumber(l, i);
        break;

      default:
        std::cout << lua_typename(l, type);
        break;

    }
    std::cout << " ";
  }
  std::cout << std::endl;
}

/**
 * @brief Returns whether the specified name is a valid Lua identifier.
 * @param name The name to check.
 * @return true if the name only contains alphanumeric characters or '_' and
 * does not start with a digit.
 */
bool LuaContext::is_valid_lua_identifier(const std::string& name) {

  if (name.empty() || (name[0] >= '0' && name[0] <= '9')) {
    return false;
  }

  bool valid = true;
  for (std::string::const_iterator it = name.begin(); it != name.end() && valid; it++) {
    char character = *it;
    valid = (character == '_' ||
        (character >= 'a' && character <= 'z') ||
        (character >= 'A' && character <= 'Z') ||
        (character >= '0' && character <= '9'));
  }
  return valid;
}

/**
 * @brief Defines some C++ functions into a Lua table.
 * @param module_name name of the table that will contain the functions
 * (e.g. "sol.main")
 * @param functions list of functions to define in the table
 * (must end with {NULLL, NULL})
 */
void LuaContext::register_functions(const std::string& module_name,
    const luaL_Reg* functions) {

  // create a table and fill it with the functions
  luaL_register(l, module_name.c_str(), functions);
  lua_pop(l, 1);
}

/**
 * @brief Defines some C++ functions into a new Lua userdata type.
 * @param module_name name of the table that will contain the functions
 * (e.g. "sol.movement") - this string will also identify the type
 * @param functions list of functions to define on the type
 * (must end with {NULLL, NULL})
 * @param metamethods metamethods to define on the type (can be NULL)
 */
void LuaContext::register_type(const std::string& module_name,
    const luaL_Reg* methods, const luaL_Reg* metamethods) {

  // create a table and fill it with the methods
  luaL_register(l, module_name.c_str(), methods);
                                  // module
  // create the metatable for the type, add it to the Lua registry
  luaL_newmetatable(l, module_name.c_str());
                                  // module mt
  if (metamethods != NULL) {
    // fill the metatable
    luaL_register(l, NULL, metamethods);
                                  // module mt
  }

  // make metatable.__index = module
  // (or metatable.usual_index = module if __index is already defined)
  lua_getfield(l, -1, "__index");
                                  // module mt __index/nil
  lua_pushvalue(l, -3);
                                  // module mt __index/nil module
  if (lua_isnil(l, -2)) {
    lua_setfield(l, -3, "__index");
                                  // module mt nil
  }
  else {
    lua_setfield(l, -3, "usual_index");
                                  // module mt __index
  }
  lua_pop(l, 3);
                                  // --
}

/**
 * @brief Tells the Lua context what C++ functions it can call.
 */
void LuaContext::register_modules() {

  // modules available to all scripts
  register_main_module();
  register_game_module();
  register_map_module();
  register_entity_module();
  register_audio_module();
  register_timer_module();
  register_surface_module();
  register_text_surface_module();
  register_sprite_module();
  register_movement_module();
  register_item_module();
  register_input_module();
  register_video_module();
  register_menu_module();
  register_language_module();
}

/**
 * @brief Pushes a Lua value from its reference.
 * @param l A Lua state.
 * @param ref A Lua reference.
 */
void LuaContext::push_ref(lua_State* l, int ref) {
  lua_rawgeti(l, LUA_REGISTRYINDEX, ref);
}

/**
 * @brief Pushes a string.
 * @param l A Lua state.
 * @param text A string value.
 */
void LuaContext::push_string(lua_State* l, const std::string& text) {
  lua_pushstring(l, text.c_str());
}

/**
 * @brief Pushes a userdata onto the stack.
 * @param l a Lua context
 * @param userdata a userdata
 */
void LuaContext::push_userdata(lua_State* l, ExportableToLua& userdata) {

  // TODO to avoid creating N userdata values for the same pointer,
  // lookup in userdata_tables whether the userdata already exists.
  // If yes, don't increment the refcount and don't create a new one, just push it.
  // => Repetitive update() will no longer constantly allocate new values.
  // => A special __eq will no longer be needed.

  userdata.increment_refcount();
                                  // ...
  ExportableToLua** block_address = static_cast<ExportableToLua**>(
      lua_newuserdata(l, sizeof(ExportableToLua*)));
  *block_address = &userdata;
                                  // ... userdata
  luaL_getmetatable(l, userdata.get_lua_type_name().c_str());
                                  // ... userdata mt
  Debug::check_assertion(!lua_isnil(l, -1), StringConcat() <<
      "Userdata of type '" << userdata.get_lua_type_name()
      << "' has not metatable, this is a memory leak");
  lua_setmetatable(l, -2);
                                  // ... userdata
}

/**
 * @brief Pushes a color onto the stack.
 * @param l A Lua context.
 * @param color A color.
 */
void LuaContext::push_color(lua_State* l, const Color& color) {

  int r, g, b;
  color.get_components(r, g, b);
  lua_newtable(l);
  lua_pushinteger(l, r);
  lua_rawseti(l, -2, 1);
  lua_pushinteger(l, g);
  lua_rawseti(l, -2, 2);
  lua_pushinteger(l, b);
  lua_rawseti(l, -2, 3);
}

/**
 * @brief Returns whether a value is a userdata of a given type.
 * @param l a Lua context
 * @param index an index in the stack
 * @param module_name name of a userdata metatable in the registry
 * @return true if the value is a userdata with this metatable
 */
bool LuaContext::is_userdata(lua_State* l, int index,
    const std::string& module_name) {

  index = get_positive_index(l, index);

                                  // ... udata ...
  void *udata = lua_touserdata(l, index);
  if (udata == NULL) {
    // it's not a userdata
    return false;
  }
  if (!lua_getmetatable(l, index)) {
    // the userdata has no metatable
    return false;
  }
                                  // ... udata ... mt_found
  lua_getfield(l, LUA_REGISTRYINDEX, module_name.c_str());
                                  // ... udata ... mt_found mt_expected
  bool result = lua_rawequal(l, -1, -2);
  lua_pop(l, 2);
                                  // ... udata ...
  return result;
}

/**
 * @brief Checks that the value at the given index is userdata of the
 * specified type and returns it.
 * @param l a Lua state
 * @param index an index in the Lua stack
 * @param module_name name identifying the userdata type
 * @return the userdata at this index
 */
ExportableToLua& LuaContext::check_userdata(lua_State* l, int index,
    const std::string& module_name) {

  index = get_positive_index(l, index);

  ExportableToLua** userdata = static_cast<ExportableToLua**>(
    luaL_checkudata(l, index, module_name.c_str()));
  return **userdata;
}

/**
 * @brief Returns whether a value is a color.
 * @param l A Lua context.
 * @param index An index in the stack.
 * @return true if the value is a color, that is, an array with three integers.
 */
bool LuaContext::is_color(lua_State* l, int index) {

  bool result = false;
  if (lua_type(l, index) != LUA_TTABLE) {
    lua_rawgeti(l, index, 1);
    lua_rawgeti(l, index, 2);
    lua_rawgeti(l, index, 3);
    result = lua_isnumber(l, -3) && lua_isnumber(l, -2) && lua_isnumber(l, -1);
    lua_pop(l, 3);
  }
  return result;
}

/**
 * @brief Checks that the value at the given index is a color and returns it.
 * @param l a Lua state
 * @param index an index in the Lua stack
 * @return the color at this index
 */
Color LuaContext::check_color(lua_State* l, int index) {

  index = get_positive_index(l, index);

  luaL_checktype(l, index, LUA_TTABLE);
  lua_rawgeti(l, index, 1);
  lua_rawgeti(l, index, 2);
  lua_rawgeti(l, index, 3);
  Color color(luaL_checkinteger(l, -3),
    luaL_checkinteger(l, -2),
    luaL_checkinteger(l, -1));
  lua_pop(l, 3);

  return color;
}

/**
 * @brief Compares two userdata of the same type.
 * @param l a Lua state
 * @return number of values to return to Lua
 */
int LuaContext::userdata_meta_eq(lua_State* l) {

  ExportableToLua** userdata1 =
      static_cast<ExportableToLua**>(lua_touserdata(l, 1));
  ExportableToLua** userdata2 =
      static_cast<ExportableToLua**>(lua_touserdata(l, 2));

  lua_pushboolean(l, *userdata1 == *userdata2);

  return 1;
}

/**
 * @brief Finalizer of a userdata type.
 * @param l a Lua state
 * @return number of values to return to Lua
 */
int LuaContext::userdata_meta_gc(lua_State* l) {

  ExportableToLua* userdata =
      *(static_cast<ExportableToLua**>(lua_touserdata(l, 1)));

  userdata->decrement_refcount();
  if (userdata->get_refcount() == 0) {
    delete userdata;
  }

  return 0;
}

/**
 * @brief Implementation of __newindex that allows userdata to be like tables.
 *
 * Lua code can make "object[key] = value" if object is a userdata with this
 * __newindex metamethod.
 *
 * This metamethod must be used with its corresponding __index
 * metamethod (see userdata_meta_index_as_table).
 *
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::userdata_meta_newindex_as_table(lua_State* l) {

  luaL_checktype(l, 1, LUA_TUSERDATA);
  luaL_checkany(l, 2);
  luaL_checkany(l, 3);

  ExportableToLua** userdata =
      static_cast<ExportableToLua**>(lua_touserdata(l, 1));

  /* The user wants to make udata[key] = value but udata is a userdata.
   * So what we make instead is udata_tables[udata][key] = value.
   * This redirection is totally transparent from the Lua side.
   */

  lua_pushstring(l, "sol.userdata_tables");
  lua_gettable(l, LUA_REGISTRYINDEX);
                                  // ... udata_tables
  lua_pushlightuserdata(l, *userdata);
                                  // ... udata_tables udata
  lua_gettable(l, -2);
                                  // ... udata_tables udata_table/nil
  if (lua_isnil(l, -1)) {
    // Create the userdata table if it does not exist yet.
                                  // ... udata_tables nil
    lua_pop(l, 1);
                                  // ... udata_tables
    lua_newtable(l);
                                  // ... udata_tables udata_table
    lua_pushlightuserdata(l, *userdata);
                                  // ... udata_tables udata_table udata
    lua_pushvalue(l, -2);
                                  // ... udata_tables udata_table udata udata_table
    lua_settable(l, -4);
                                  // ... udata_tables udata_table
  }
  lua_pushvalue(l, 2);
                                  // ... udata_tables udata_table key
  lua_pushvalue(l, 3);
                                  // ... udata_tables udata_table key value
  lua_settable(l, -3);
                                  // ... udata_tables udata_table
  return 0;
}

/**
 * @brief Implementation of __index that allows userdata to be like tables.
 *
 * Lua code can get "object[key]" for an arbitrary key previously set.
 *
 * This metamethod must be used with its corresponding __newindex
 * metamethod (see userdata_meta_newindex_as_table).
 *
 * @brief Implementation of __index for the type game.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::userdata_meta_index_as_table(lua_State* l) {

  /* The user wants to make udata[key] but udata is a userdata.
   * So what we retrieve instead is udata_tables[udata][key].
   * This redirection is totally transparent from the Lua side.
   * If udata_tables[udata][key] does not exist, we fall back
   * to the usual __index for userdata, i.e. we look for a method
   * in its type.
   */

  luaL_checktype(l, 1, LUA_TUSERDATA);
  luaL_checkany(l, 2);

  ExportableToLua** userdata =
      static_cast<ExportableToLua**>(lua_touserdata(l, 1));

  bool found = false;
  lua_pushstring(l, "sol.userdata_tables");
  lua_gettable(l, LUA_REGISTRYINDEX);
                                  // ... udata_tables
  lua_pushlightuserdata(l, *userdata);
                                  // ... udata_tables udata
  lua_gettable(l, -2);
                                  // ... udata_tables udata_table/nil
  if (!lua_isnil(l, -1)) {
    lua_pushvalue(l, 2);
                                  // ... udata_tables udata_table key
    lua_gettable(l, -2);
                                  // ... udata_tables udata_table value
    found = !lua_isnil(l, -1);
  }

  // Nothing in the userdata's table: do the usual __index instead
  // (look in the userdata's type)
  if (!found) {
    lua_getmetatable(l, 1);
                                  // ... meta
    lua_getfield(l, -1, "usual_index");
                                  // ... meta module
    lua_pushvalue(l, 2);
                                  // ... meta module key
    lua_gettable(l, -2);
                                  // ... meta module value
  }

  return 1;
}

/**
 * @brief Calls the on_started() method of the object on top of the stack.
 */
void LuaContext::on_started() {

  if (find_method("on_started")) {
    call_function(1, 0, "on_started");
  }
}

/**
 * @brief Calls the on_finished() method of the object on top of the stack.
 */
void LuaContext::on_finished() {

  if (find_method("on_finished")) {
    call_function(1, 0, "on_finished");
  }
}

/**
 * @brief Calls the on_update() method of the object on top of the stack.
 */
void LuaContext::on_update() {

  if (find_method("on_update")) {
    call_function(1, 0, "on_update");
  }
}

/**
 * @brief Calls the on_draw() method of the object on top of the stack.
 * @param dst_surface The destination surface.
 */
void LuaContext::on_draw(Surface& dst_surface) {

  if (find_method("on_draw")) {
    push_surface(l, dst_surface);
    call_function(2, 0, "on_draw");
  }
}

/**
 * @brief Calls the on_pre_draw() method of the object on top of the stack.
 * @param dst_surface The destination surface.
 */
void LuaContext::on_pre_draw(Surface& dst_surface) {

  if (find_method("on_pre_draw")) {
    push_surface(l, dst_surface);
    call_function(2, 0, "on_pre_draw");
  }
}

/**
 * @brief Calls the on_post_draw() method of the object on top of the stack.
 * @param dst_surface The destination surface.
 */
void LuaContext::on_post_draw(Surface& dst_surface) {

  if (find_method("on_post_draw")) {
    push_surface(l, dst_surface);
    call_function(2, 0, "on_post_draw");
  }
}

/**
 * @brief Calls the on_suspended() method of the object on top of the stack.
 * @param suspended true to suspend the object, false to unsuspend it.
 */
void LuaContext::on_suspended(bool suspended) {

  if (find_method("on_suspended")) {
    lua_pushboolean(l, suspended);
    call_function(2, 0, "on_suspended");
  }
}

/**
 * @brief Calls the on_paused() method of the object on top of the stack.
 */
void LuaContext::on_paused() {

  if (find_method("on_paused")) {
    call_function(1, 0, "on_paused");
  }
}

/**
 * @brief Calls the on_unpaused() method of the object on top of the stack.
 */
void LuaContext::on_unpaused() {

  if (find_method("on_unpaused")) {
    call_function(1, 0, "on_unpaused");
  }
}

/**
 * @brief Calls an input callback method of the object on top of the stack.
 * @param event The input event to forward.
 * @return \c true if the event was handled and should stop being propagated.
 */
bool LuaContext::on_input(InputEvent& event) {

  // Call the Lua function(s) corresponding to this input event.
  bool handled = false;
  if (event.is_keyboard_event()) {
    // Keyboard.
    if (event.is_keyboard_key_pressed()) {
      handled = on_key_pressed(event) || handled;
      if (event.is_character_pressed()) {
        handled = on_character_pressed(event) || handled;
      }
    }
    else if (event.is_keyboard_key_released()) {
      handled = on_key_released(event) || handled;
    }
  }
  else if (event.is_joypad_event()) {
    // Joypad.
    if (event.is_joypad_button_pressed()) {
      handled = on_joypad_button_pressed(event) || handled;
    }
    else if (event.is_joypad_button_released()) {
      handled = on_joypad_button_released(event) || handled;
    }
    else if (event.is_joypad_axis_moved()) {
      handled = on_joypad_axis_moved(event) || handled;
    }
    else if (event.is_joypad_hat_moved()) {
      handled = on_joypad_hat_moved(event) || handled;
    }
  }

  if (event.is_direction_pressed()) {
    // Keyboard or joypad direction.
    handled = on_direction_pressed(event) || handled;
  }

  return handled;
}

/**
 * @brief Notifies the object on top of the stack
 * that a keyboard key was just pressed
 * (including if it is a directional key or a character).
 * @param event The corresponding input event.
 * @return \c true if the event was handled and should stop being propagated.
 */
bool LuaContext::on_key_pressed(InputEvent& event) {

  bool handled = false;
  if (find_method("on_key_pressed")) {

    const std::string& key_name = InputEvent::get_keyboard_key_name(event.get_keyboard_key());
    if (!key_name.empty()) { // This key exists in the Solarus API.

      push_string(l, key_name);
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
      call_function(3, 1, "on_key_pressed");
      handled = lua_toboolean(l, -1);
      lua_pop(l, 1);
    }
    else {
      // The method exists but the key is unknown.
      lua_pop(l, 2);  // Pop the object and the method.
    }
  }
  return handled;
}

/**
 * @brief Notifies the object on top of the stack
 * that a character was just pressed with the keyboard.
 * @param event The corresponding input event.
 * @return \c true if the event was handled and should stop being propagated.
 */
bool LuaContext::on_character_pressed(InputEvent& event) {

  bool handled = false;
  if (find_method("on_character_pressed")) {

    const std::string& character = event.get_character();
    push_string(l, character);
    call_function(2, 1, "on_character_pressed");
    handled = lua_toboolean(l, -1);
    lua_pop(l, 1);
  }
  return handled;
}

/**
 * @brief Notifies the object on top of the stack
 * that a keyboard key was just released
 * (including if it is a directional key).
 * @param event The corresponding input event.
 * @return \c true if the event was handled and should stop being propagated.
 */
bool LuaContext::on_key_released(InputEvent& event) {

  bool handled = false;
  if (find_method("on_key_released")) {

    const std::string& key_name = InputEvent::get_keyboard_key_name(event.get_keyboard_key());
    if (!key_name.empty()) { // This key exists in the Solarus API.
      push_string(l, key_name);
      call_function(2, 1, "on_key_released");
      handled = lua_toboolean(l, -1);
      lua_pop(l, 1);
    }
    else {
      // The method exists but the key is unknown.
      lua_pop(l, 2);  // Pop the object and the method.
    }
  }
  return handled;
}

/**
 * @brief Notifies the object on top of the stack
 * that a joypad button was just pressed.
 * @param event The corresponding input event.
 * @return \c true if the event was handled and should stop being propagated.
 */
bool LuaContext::on_joypad_button_pressed(InputEvent& event) {

  bool handled = false;
  if (find_method("on_joyad_button_pressed")) {
    int button = event.get_joypad_button();

    lua_pushinteger(l, button);
    call_function(2, 1, "on_joyad_button_pressed");
    handled = lua_toboolean(l, -1);
    lua_pop(l, 1);
  }
  return handled;
}

/**
 * @brief Notifies the object on top of the stack
 * that a joypad button was just released.
 * @param event The corresponding input event.
 * @return \c true if the event was handled and should stop being propagated.
 */
bool LuaContext::on_joypad_button_released(InputEvent& event) {

  bool handled = false;
  if (find_method("on_joyad_button_released")) {
    int button = event.get_joypad_button();

    lua_pushinteger(l, button);
    call_function(2, 1, "on_joyad_button_released");
    handled = lua_toboolean(l, -1);
    lua_pop(l, 1);
  }
  return handled;
}

/**
 * @brief Notifies the object on top of the stack
 * that a joypad axis was just moved.
 * @param event The corresponding input event.
 * @return \c true if the event was handled and should stop being propagated.
 */
bool LuaContext::on_joypad_axis_moved(InputEvent& event) {

  bool handled = false;
  if (find_method("on_joyad_axis_moved")) {
    int axis = event.get_joypad_axis();
    int state = event.get_joypad_axis_state();

    lua_pushinteger(l, axis);
    lua_pushinteger(l, state);
    call_function(3, 1, "on_joyad_axis_moved");
    handled = lua_toboolean(l, -1);
    lua_pop(l, 1);
  }
  return handled;
}

/**
 * @brief Notifies the object on top of the stack
 * that a joypad hat was just moved.
 * @param event The corresponding input event.
 * @return \c true if the event was handled and should stop being propagated.
 */
bool LuaContext::on_joypad_hat_moved(InputEvent& event) {

  bool handled = false;
  if (find_method("on_joyad_hat_moved")) {
    int hat = event.get_joypad_hat();
    int direction8 = event.get_joypad_hat_direction();

    lua_pushinteger(l, hat);
    lua_pushinteger(l, direction8);
    call_function(3, 1, "on_joyad_hat_moved");
    handled = lua_toboolean(l, -1);
    lua_pop(l, 1);
  }
  return handled;
}

/**
 * @brief Notifies the object on top of the stack
 * that a directional keyboard key was just pressed
 * or that a joypad directional command has just changed.
 * @param event The corresponding input event.
 * @return \c true if the event was handled and should stop being propagated.
 */
bool LuaContext::on_direction_pressed(InputEvent& event) {

  bool handled = false;
  if (find_method("on_direction_pressed")) {
    int direction8 = event.get_direction();

    lua_pushinteger(l, direction8);
    call_function(2, 1, "on_direction_pressed");
    handled = lua_toboolean(l, -1);
    lua_pop(l, 1);
  }
  return handled;
}

/**
 * @brief Calls the on_animation_finished() method of the object on top of the stack.
 * @param animation Name of the animation finished.
 */
void LuaContext::on_animation_finished(const std::string& animation) {

  if (find_method("on_animation_finished")) {
    push_string(l, animation);
    call_function(2, 0, "on_animation_finished");
  }
}

/**
 * @brief Calls the on_frame_changed() method of the object on top of the stack.
 * @param animation Name of the sprite animation.
 * @param frame The new frame.
 */
void LuaContext::on_frame_changed(const std::string& animation, int frame) {

  if (find_method("on_frame_changed")) {
    push_string(l, animation);
    lua_pushinteger(l, frame);
    call_function(3, 0, "on_frame_changed");
  }
}

/**
 * @brief Calls the on_started() method of the object on top of the stack.
 * @param destination The destination point used (NULL if it's a special one).
 */
void LuaContext::on_started(Destination* destination) {

  if (find_method("on_started")) {
    if (destination == NULL) {
      lua_pushnil(l);
    }
    else {
      push_entity(l, *destination);
    }
    call_function(2, 0, "on_started");
  }
}

/**
 * @brief Calls the on_opening_transition_finished() method of the object on top of the stack.
 * @param destination The destination point used (NULL if it's a special one).
 */
void LuaContext::on_opening_transition_finished(Destination* destination) {

  if (find_method("on_opening_transition_finished")) {
    if (destination == NULL) {
      lua_pushnil(l);
    }
    else {
      push_entity(l, *destination);
    }
    call_function(2, 0, "on_opening_transition_finished");
  }
}

/**
 * @brief Calls the on_camera_back() method of the object on top of the stack.
 */
void LuaContext::on_camera_back() {

  if (find_method("on_camera_back")) {
    call_function(1, 0, "on_camera_back");
  }
}

/**
 * @brief Calls the on_obtaining_treasure() method of the object on top of the stack.
 * @param treasure The treasure being obtained.
 */
void LuaContext::on_obtaining_treasure(const Treasure& treasure) {

  if (find_method("on_obtaining_treasure")) {
    push_string(l, treasure.get_item_name());
    lua_pushinteger(l, treasure.get_variant());
    lua_pushstring(l, treasure.get_savegame_variable().c_str());
    call_function(4, 0, "on_obtaining_treasure");
  }
}

/**
 * @brief Calls the on_obtained_treasure() method of the object on top of the stack.
 * @param treasure The treasure just obtained.
 */
void LuaContext::on_obtained_treasure(const Treasure& treasure) {

  if (find_method("on_obtained_treasure")) {
    push_string(l, treasure.get_item_name());
    lua_pushinteger(l, treasure.get_variant());
    lua_pushstring(l, treasure.get_savegame_variable().c_str());
    call_function(4, 0, "on_obtained_treasure");
  }
}

/**
 * @brief Calls the on_victory_finished() method of the object on top of the stack.
 */
void LuaContext::on_victory_finished() {

  if (find_method("on_victory_finished")) {
    call_function(1, 0, "on_victory_finished");
  }
}

/**
 * @brief Calls the on_activated() method of the object on top of the stack.
 */
void LuaContext::on_activated() {

  if (find_method("on_activated")) {
    call_function(1, 0, "on_activated");
  }
}

/**
 * @brief Calls the on_inactivated_repeat() method of the object on top of the stack.
 */
void LuaContext::on_activated_repeat() {

  if (find_method("on_inactivated_repeat")) {
    call_function(1, 0, "on_inactivated_repeat");
  }
}

/**
 * @brief Calls the on_inactivated() method of the object on top of the stack.
 */
void LuaContext::on_inactivated() {

  if (find_method("on_inactivated")) {
    call_function(1, 0, "on_inactivated");
  }
}

/**
 * @brief Calls the on_left() method of the object on top of the stack.
 */
void LuaContext::on_left() {

  if (find_method("on_left")) {
    call_function(1, 0, "on_left");
  }
}

/**
 * @brief Calls the on_npc_interaction() method of the object on top of the stack.
 * @param npc An NPC.
 */
void LuaContext::on_npc_interaction(NPC& npc) {

  if (find_method("on_npc_interaction")) {
    push_npc(l, npc);
    call_function(2, 0, "on_npc_interaction");
  }
}

/**
 * @brief Calls the on_npc_interaction_item() method of the object on top of the stack.
 * @param npc An NPC.
 * @param item_used The equipment item used.
 * @return true if an interaction occurred.
 */
bool LuaContext::on_npc_interaction_item(NPC& npc, EquipmentItem& item_used) {

  bool interacted = false;
  if (find_method("on_npc_interaction_item")) {
    push_npc(l, npc);
    push_item(l, item_used);
    call_function(3, 1, "on_npc_interaction_item");
    interacted = lua_toboolean(l, -1);
    lua_pop(l, 1);
  }
  return interacted;
}

/**
 * @brief Calls the on_interaction() method of the object on top of the stack.
 */
void LuaContext::on_interaction() {

  if (find_method("on_interaction")) {
    call_function(1, 0, "on_interaction");
  }
}

/**
 * @brief Calls the on_interaction_item() method of the object on top of the stack.
 * @param item_used The equipment item used.
 * @return true if an interaction occurred.
 */
bool LuaContext::on_interaction_item(EquipmentItem& item) {

  bool interacted = false;
  if (find_method("on_interaction_item")) {
    push_item(l, item);
    call_function(2, 1, "on_interaction_item");
    interacted = lua_toboolean(l, -1);
    lua_pop(l, 1);
  }
  return interacted;
}

/**
 * @brief Calls the on_npc_collision_fire() method of the object on top of the stack.
 * @param npc An NPC.
 */
void LuaContext::on_npc_collision_fire(NPC& npc) {

  if (find_method("on_npc_collision_fire")) {
    push_npc(l, npc);
    call_function(2, 0, "on_npc_collision_fire");
  }
}

/**
 * @brief Calls the on_collision_fire() method of the object on top of the stack.
 */
void LuaContext::on_collision_fire() {

  if (find_method("on_collision_fire")) {
    call_function(1, 0, "on_collision_fire");
  }
}

/**
 * @brief Calls the on_collision_explosion() method of the object on top of the stack.
 */
void LuaContext::on_collision_explosion() {

  if (find_method("on_collision_explosion")) {
    call_function(1, 0, "on_collision_explosion");
  }
}

/**
 * @brief Calls the on_empty() method of the object on top of the stack.
 * @return \c true if the on_empty() method is defined.
 */
bool LuaContext::on_empty() {

  if (find_method("on_empty")) {
    call_function(1, 0, "on_empty");
    return true;
  }
  return false;
}

/**
 * @brief Calls the on_buying() method of the object on top of the stack.
 * @return true if the player is allowed to buy the item.
 */
bool LuaContext::on_buying() {

  bool can_buy = true;
  if (find_method("on_buying")) {
    call_function(1, 1, "on_buying");
    can_buy = lua_toboolean(l, -1);
    lua_pop(l, 1);
  }
  return can_buy;
}

/**
 * @brief Calls the on_bought() method of the object on top of the stack.
 */
void LuaContext::on_bought() {

  if (find_method("on_bought")) {
    call_function(1, 0, "on_bought");
  }
}

/**
 * @brief Calls the on_open() method of the object on top of the stack.
 */
void LuaContext::on_open() {

  if (find_method("on_open")) {
    call_function(1, 0, "on_open");
  }
}

/**
 * @brief Calls the on_closed() method of the object on top of the stack.
 */
void LuaContext::on_closed() {

  if (find_method("on_closed")) {
    call_function(1, 0, "on_closed");
  }
}

/**
 * @brief Calls the on_moved() method of the object on top of the stack.
 */
void LuaContext::on_moved() {

  if (find_method("on_moved")) {
    call_function(1, 0, "on_moved");
  }
}

/**
 * @brief Calls the on_map_changed() method of the object on top of the stack.
 * @param map The new active map.
 */
void LuaContext::on_map_changed(Map& map) {

  if (find_method("on_map_changed")) {
    push_map(l, map);
    call_function(2, 0, "on_map_changed");
  }
}

/**
 * @brief Calls the on_pickable_created() method of the object on top of the stack.
 * @param pickable A pickable treasure.
 */
void LuaContext::on_pickable_created(Pickable& pickable) {

  if (find_method("on_pickable_created")) {
    push_entity(l, pickable);
    call_function(2, 0, "on_pickable_created");
  }
}

/**
 * @brief Calls the on_pickable_movement_changed() method of the object on top of the stack.
 * @param pickable A pickable treasure.
 * @param movement The movement of this pickable treasure.
 */
void LuaContext::on_pickable_movement_changed(Pickable& pickable, Movement& movement) {

  if (find_method("on_pickable_movement_changed")) {
    push_entity(l, pickable);
    push_movement(l, movement);
    call_function(3, 0, "on_pickable_movement_changed");
  }
}

/**
 * @brief Calls the on_variant_changed() method of the object on top of the stack.
 * @param enemy Variant of an inventory item.
 */
void LuaContext::on_variant_changed(int variant) {

  if (find_method("on_variant_changed")) {
    lua_pushinteger(l, variant);
    call_function(2, 0, "on_variant_changed");
  }
}

/**
 * @brief Calls the on_amount_changed() method of the object on top of the stack.
 * @param amount Amount of an equipment item.
 */
void LuaContext::on_amount_changed(int amount) {

  if (find_method("on_amount_changed")) {
    lua_pushinteger(l, amount);
    call_function(2, 0, "on_amount_changed");
  }
}

/**
 * @brief Calls the on_obtaining() method of the object on top of the stack.
 * @param treasure The treasure being obtained.
 */
void LuaContext::on_obtaining(const Treasure& treasure) {

  if (find_method("on_obtaining")) {
    lua_pushinteger(l, treasure.get_variant());
    lua_pushstring(l, treasure.get_savegame_variable().c_str());
    call_function(3, 0, "on_obtaining");
  }
}

/**
 * @brief Calls the on_obtained() method of the object on top of the stack.
 * @param treasure The treasure just obtained.
 */
void LuaContext::on_obtained(const Treasure& treasure) {

  if (find_method("on_obtained")) {
    lua_pushinteger(l, treasure.get_variant());
    lua_pushstring(l, treasure.get_savegame_variable().c_str());
    call_function(3, 0, "on_obtained");
  }
}

/**
 * @brief Calls the on_using() method of the object on top of the stack.
 */
void LuaContext::on_using() {

  if (find_method("on_using")) {
    call_function(1, 0, "on_using");
  }
}

/**
 * @brief Calls the on_ability_used() method of the object on top of the stack.
 * @param ability_name Id of a built-in ability.
 */
void LuaContext::on_ability_used(const std::string& ability_name) {

  if (find_method("on_ability_used")) {
    push_string(l, ability_name);
    call_function(2, 0, "on_ability_used");
  }
}

/**
 * @brief Calls the on_created() method of the object on top of the stack.
 */
void LuaContext::on_created() {

  if (find_method("on_created")) {
    call_function(1, 0, "on_created");
  }
}

/**
 * @brief Calls the on_removed() method of the object on top of the stack.
 */
void LuaContext::on_removed() {

  if (find_method("on_removed")) {
    call_function(1, 0, "on_removed");
  }
}

/**
 * @brief Calls the on_enabled() method of the object on top of the stack.
 */
void LuaContext::on_enabled() {

  if (find_method("on_enabled")) {
    call_function(1, 0, "on_enabled");
  }
}

/**
 * @brief Calls the on_disabled() method of the object on top of the stack.
 */
void LuaContext::on_disabled() {

  if (find_method("on_disabled")) {
    call_function(1, 0, "on_disabled");
  }
}

/**
 * @brief Calls the on_restarted() method of the object on top of the stack.
 */
void LuaContext::on_restarted() {

  if (find_method("on_restarted")) {
    call_function(1, 0, "on_restarted");
  }
}

/**
 * @brief Calls the on_pre_draw() method of the object on top of the stack.
 */
void LuaContext::on_pre_draw() {

  if (find_method("on_pre_draw")) {
    call_function(1, 0, "on_pre_draw");
  }
}

/**
 * @brief Calls the on_post_draw() method of the object on top of the stack.
 */
void LuaContext::on_post_draw() {

  if (find_method("on_post_draw")) {
    call_function(1, 0, "on_post_draw");
  }
}

/**
 * @brief Calls the on_position_changed() method of the object on top of the stack.
 * @param xy The new position.
 * @param layer The new layer.
 */
void LuaContext::on_position_changed(const Rectangle& xy, Layer layer) {

  if (find_method("on_position_changed")) {
    lua_pushinteger(l, xy.get_x());
    lua_pushinteger(l, xy.get_y());
    lua_pushinteger(l, layer);
    call_function(4, 0, "on_position_changed");
  }
}

/**
 * @brief Calls the on_obstacle_reached() method of the object on top of the stack.
 * @param movement The movement that reached an obstacle.
 */
void LuaContext::on_obstacle_reached(Movement& movement) {

  if (find_method("on_obstacle_reached")) {
    push_movement(l, movement);
    call_function(2, 0, "on_obstacle_reached");
  }
}

/**
 * @brief Calls the on_movement_changed() method of the object on top of the stack.
 * @param movement A movement.
 */
void LuaContext::on_movement_changed(Movement& movement) {

  if (find_method("on_movement_changed")) {
    push_movement(l, movement);
    call_function(2, 0, "on_movement_changed");
  }
}

/**
 * @brief Calls the on_movement_finished() method of the object on top of the stack.
 */
void LuaContext::on_movement_finished() {

  if (find_method("on_movement_finished")) {
    call_function(1, 0, "on_movement_finished");
  }
}

/**
 * @brief Calls the on_sprite_animation_finished() method of the object on top of the stack.
 * @param sprite A sprite whose animation has just finished.
 * @param animation Name of the animation finished.
 */
void LuaContext::on_sprite_animation_finished(Sprite& sprite, const std::string& animation) {

  if (find_method("on_sprite_animation_finished")) {
    push_sprite(l, sprite);
    push_string(l, animation);
    call_function(3, 0, "on_sprite_animation_finished");
  }
}

/**
 * @brief Calls the on_sprite_frame_changed() method of the object on top of the stack.
 * @param sprite A sprite whose animation frame has just changed.
 * @param animation Name of the sprite animation.
 * @param frame The new frame.
 */
void LuaContext::on_sprite_frame_changed(Sprite& sprite, const std::string& animation, int frame) {

  if (find_method("on_sprite_frame_changed")) {
    push_sprite(l, sprite);
    push_string(l, animation);
    lua_pushinteger(l, frame);
    call_function(4, 0, "on_sprite_frame_changed");
  }
}

/**
 * @brief Calls the on_collision_enemy() method of the object on top of the stack.
 * @param other_enemy Another enemy colliding with the object on top of the stack.
 * @param other_sprite Colliding sprite of the other enemy.
 * @param this_sprite Colliding sprite of the first enemy.
 */
void LuaContext::on_collision_enemy(Enemy& other_enemy, Sprite& other_sprite, Sprite& this_sprite) {

  if (find_method("on_collision_enemy")) {
    push_enemy(l, other_enemy);
    push_sprite(l, other_sprite);
    push_sprite(l, this_sprite);
    call_function(4, 0, "on_collision_enemy");
  }
}

/**
 * @brief Calls the on_custom_attack_received() method of the object on top of the stack.
 * @param attack The attack received.
 * @param sprite The sprite that receives the attack if any.
 */
void LuaContext::on_custom_attack_received(EnemyAttack attack, Sprite* sprite) {

  if (find_method("on_custom_attack_received")) {
    push_string(l, enemy_attack_names[attack]);
    if (sprite != NULL) {
      // Pixel-precise collision.
      push_sprite(l, *sprite);
      call_function(3, 0, "on_custom_attack_received");
    }
    else {
      call_function(2, 0, "on_custom_attack_received");
    }
  }
}

/**
 * @brief Calls the on_hurt() method of the object on top of the stack.
 * @param attack The attack received.
 * @param life_lost Number of life points just lost.
 */
void LuaContext::on_hurt(EnemyAttack attack, int life_lost) {

  if (find_method("on_hurt")) {
    push_string(l, enemy_attack_names[attack]);
    lua_pushinteger(l, life_lost);
    call_function(3, 0, "on_hurt");
  }
}

/**
 * @brief Calls the on_dying() method of the object on top of the stack.
 */
void LuaContext::on_dying() {

  if (find_method("on_dying")) {
    call_function(1, 0, "on_dying");
  }
}

/**
 * @brief Calls the on_dead() method of the object on top of the stack.
 */
void LuaContext::on_dead() {

  if (find_method("on_dead")) {
    call_function(1, 0, "on_dead");
  }
}

/**
 * @brief Calls the on_immobilized() method of the object on top of the stack.
 */
void LuaContext::on_immobilized() {

  if (find_method("on_immobilized")) {
    call_function(1, 0, "on_immobilized");
  }
}

/**
 * @brief A loader that makes require() able to load Lua files
 * from the quest data directory or archive.
 * @param l The Lua context.
 * @return Number of values to return to Lua.
 */
int LuaContext::l_loader(lua_State* l) {

  const std::string& script_name = luaL_checkstring(l, 1);
  bool exists = load_file_if_exists(l, script_name);

  if (!exists) {
    std::ostringstream oss;
    oss << std::endl << "\tno quest file '" << script_name
      << ".lua' in 'data' or 'data.solarus'";
    push_string(l, oss.str());
  }
  return 1;
}

