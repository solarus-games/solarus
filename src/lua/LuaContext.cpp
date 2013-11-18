/*
 * Copyright (C) 2006-2013 Christopho, Solarus - http://www.solarus-games.org
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
#include "entities/ShopTreasure.h"
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
#include "Timer.h"
#include <sstream>
#include <iomanip>

std::map<lua_State*, LuaContext*> LuaContext::lua_contexts;

/**
 * \brief Creates a Lua context.
 * \param main_loop The Solarus main loop manager.
 */
LuaContext::LuaContext(MainLoop& main_loop):
  l(NULL),
  main_loop(main_loop) {

}

/**
 * \brief Destroys this Lua context.
 */
LuaContext::~LuaContext() {

  this->exit();
}

/**
 * \brief Returns the LuaContext object that encapsulates a Lua state.
 * \param l A Lua state.
 * \return The LuaContext object encapsulating this Lua state.
 */
LuaContext& LuaContext::get_lua_context(lua_State* l) {

  std::map<lua_State*, LuaContext*>::iterator it = lua_contexts.find(l);

  Debug::check_assertion(it != lua_contexts.end(),
      "This Lua state does not belong to a LuaContext object");

  return *it->second;
}

/**
 * \brief Returns the internal Lua state encapsulated by this LuaContext object.
 * \return The internal Lua state.
 */
lua_State* LuaContext::get_internal_state() {
  return l;
}

/**
 * \brief Returns the Solarus main loop object.
 * \return The main loop manager.
 */
MainLoop& LuaContext::get_main_loop() {
  return main_loop;
}

/**
 * \brief Initializes Lua.
 */
void LuaContext::initialize() {

  // Create an execution context.
  l = luaL_newstate();
  lua_atpanic(l, l_panic);
  luaL_openlibs(l);

  // Associate this LuaContext object to the lua_State pointer.
  lua_contexts[l] = this;

  // Create a table that will keep track of all userdata.
                                  // --
  lua_newtable(l);
                                  // all_udata
  lua_newtable(l);
                                  // all_udata meta
  lua_pushstring(l, "v");
                                  // all_udata meta "v"
  lua_setfield(l, -2, "__mode");
                                  // all_udata meta
  lua_setmetatable(l, -2);
                                  // all_udata
  lua_setfield(l, LUA_REGISTRYINDEX, "sol.all_userdata");
                                  // --

  // Allow userdata to be indexable if they want.
  lua_newtable(l);
                                  // udata_tables
  lua_setfield(l, LUA_REGISTRYINDEX, "sol.userdata_tables");
                                  // --

  // Create the sol table that will contain the whole Solarus API.
  lua_newtable(l);
  lua_setglobal(l, "sol");

  // Register the C++ functions and types accessible by Lua.
  register_modules();

  // Make require() able to load Lua files even from the
  // data.solarus or data.solarus.zip archive.
                                  // --
  lua_getglobal(l, "sol");
                                  // -- sol
  lua_pushcfunction(l, l_loader);
                                  // -- sol loader
  lua_setfield(l, -2, "loader");
                                  // -- sol
  luaL_dostring(l, "table.insert(package.loaders, 2, sol.loader)");
                                  // -- sol
  lua_pushnil(l);
                                  // -- sol nil
  lua_setfield(l, -2, "loader");
                                  // -- sol
  lua_pop(l, 1);
                                  // --

  // Execute the main file.
  do_file_if_exists(l, "main");
  main_on_started();
}

/**
 * \brief Cleans Lua.
 */
void LuaContext::exit() {

  if (l != NULL) {
    // Call sol.main.on_finished() if it exists.
    main_on_finished();

    // Destroy unfinished objects.
    destroy_menus();
    destroy_timers();
    destroy_drawables();

    // Finalize Lua.
    lua_close(l);
    lua_contexts.erase(l);
    l = NULL;
  }
}

/**
 * \brief Updates the Lua world.
 *
 * This function is called at each cycle.
 * sol.main.on_update() is called if it exists.
 */
void LuaContext::update() {

  update_drawables();
  update_movements();
  update_menus();
  update_timers();

  // Call sol.main.on_update().
  main_on_update();
}

/**
 * \brief Notifies Lua that an input event has just occurred.
 *
 * The appropriate callback in sol.main is notified.
 *
 * \param event The input event to handle.
 * \return \c true if the event was handled and should stop being propagated.
 */
bool LuaContext::notify_input(const InputEvent& event) {

  // Call the appropriate callback in sol.main (if it exists).
  return main_on_input(event);
}

/**
 * \brief Notifies Lua that a map has just been started.
 *
 * The Lua file of this map is automatically loaded.
 *
 * \param map The map started.
 * \param destination The destination point used if it's a normal one,
 * NULL otherwise.
 */
void LuaContext::run_map(Map& map, Destination* destination) {

  // Compute the file name, depending on the id of the map.
  std::string file_name = std::string("maps/") + map.get_id();

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
  call_function(1, 0, file_name.c_str());

  // Call the map:on_started() callback.
  map_on_started(map, destination);
}

/**
 * \brief Notifies Lua that a map has just been suspended or resumed.
 * \param map A map.
 * \param suspended \c true if it is suspended, false if it is resumed.
 */
void LuaContext::notify_map_suspended(Map& map, bool suspended) {

  notify_timers_map_suspended(suspended);   // Notify timers.
  map_on_suspended(map, suspended);  // Call map:on_suspended()
}

/**
 * \brief Notifies the Lua world that an equipment item has just been created.
 *
 * The Lua file of this item is automatically loaded.
 *
 * \param item The item.
 */
void LuaContext::run_item(EquipmentItem& item) {

  // Compute the file name, depending on the id of the equipment item.
  std::string file_name = (std::string) "items/" + item.get_name();

  // Load the item's code.
  if (load_file_if_exists(l, file_name)) {

    // Run it with the item userdata as parameter.
    push_item(l, item);
    call_function(1, 0, file_name.c_str());

    // Call the item:on_created() callback.
    item_on_created(item);
  }
}

/**
 * \brief Notifies the Lua world that an enemy has just been added to the map.
 *
 * The Lua file of this enemy is automatically loaded.
 *
 * \param enemy The enemy.
 */
void LuaContext::run_enemy(Enemy& enemy) {

  // Compute the file name, depending on enemy's breed.
  std::string file_name = (std::string) "enemies/" + enemy.get_breed();

  // Load the enemy's code.
  if (load_file_if_exists(l, file_name)) {

    // Run it with the enemy userdata as parameter.
    push_enemy(l, enemy);
    call_function(1, 0, file_name.c_str());

    enemy_on_suspended(enemy, enemy.is_suspended());
    enemy_on_created(enemy);
  }
}

/**
 * \brief Notifies Lua that the sequence started by a call to
 * map:move_camera() has reached its target.
 * \param map The current map.
 */
void LuaContext::notify_camera_reached_target(Map& map) {

  // Set a timer to execute the function.
  lua_settop(l, 0);
  push_map(l, map);
  lua_getfield(l, LUA_REGISTRYINDEX, "sol.camera_delay_before");
  lua_pushcfunction(l, l_camera_do_callback);
  timer_api_start(l);
  Timer& timer = check_timer(l, -1);
  timer.set_suspended_with_map(false);
  lua_settop(l, 0);
}

/**
 * \brief Notifies Lua that a dialog starts.
 * \param game The game.
 * \param dialog The dialog that is becoming active.
 * \param info_ref Lua ref to an optional info parameter to pass to
 * Lua, or LUA_REFNIL.
 * \return true if Lua handles the dialog, false otherwise.
 */
bool LuaContext::notify_dialog_started(
    Game& game,
    const Dialog& dialog,
    int info_ref) {

  return game_on_dialog_started(game, dialog, info_ref);
}

/**
 * \brief Notifies Lua that a dialog is finished.
 * \param game The game.
 * \param dialog The dialog that was active.
 * \param callback_ref Lua ref of the function to call, or LUA_REFNIL.
 * \param status_ref Lua ref to a status value to pass to the callback.
 * "skipped" means that the dialog was canceled by the user.
 */
void LuaContext::notify_dialog_finished(
    Game& game,
    const Dialog& dialog,
    int callback_ref,
    int status_ref) {

  game_on_dialog_finished(game, dialog);

  // Execute the callback after game:on_dialog_finished()
  // because the callback may start another dialog.
  if (callback_ref != LUA_REFNIL) {
    push_callback(callback_ref);
    destroy_ref(callback_ref);
    if (status_ref != LUA_REFNIL) {
      push_ref(l, status_ref);
      destroy_ref(status_ref);
    }
    else {
      // No status.
      lua_pushnil(l);
    }
    call_function(1, 0, "dialog callback");
  }
  else {
    // No callback: the status if ignored if any.
    destroy_ref(status_ref);
  }
}

/**
 * \brief Like luaL_error() but with an std::string message.
 * \param l A Lua state.
 * \param message Error message.
 * \return This function never returns.
 */
int LuaContext::error(lua_State* l, const std::string& message) {
  return luaL_error(l, message.c_str());
}

/**
 * \brief Like luaL_argerror() but with an std::string message.
 * \param l A Lua state.
 * \param arg_index Index of an argument in the stack.
 * \param message Error message.
 * \return This function never returns.
 */
int LuaContext::arg_error(lua_State* l, int arg_index, const std::string& message) {
  return luaL_argerror(l, arg_index, message.c_str());
}

/**
 * \brief Checks that a table field is a number and returns it as an integer.
 *
 * This function acts like lua_getfield() followed by luaL_checkint().
 *
 * \param l A Lua state.
 * \param table_index Index of a table in the stack.
 * \param key Key of the field to get in that table.
 * \return The wanted field as an integer.
 */
int LuaContext::check_int_field(
    lua_State* l, int table_index, const std::string& key) {

  lua_getfield(l, table_index, key.c_str());
  if (!lua_isnumber(l, -1)) {
    arg_error(l, table_index, StringConcat() <<
        "Bad field '" << key << "' (integer expected, got " <<
        luaL_typename(l, -1) << ")");
  }

  int value = int(lua_tointeger(l, -1));
  lua_pop(l, 1);
  return value;
}

/**
 * \brief Like check_int_field() but with a default value.
 *
 * This function acts like lua_getfield() followed by luaL_optint().
 *
 * \param l A Lua state.
 * \param table_index Index of a table in the stack.
 * \param key Key of the field to get in that table.
 * \param default_value The default value to return if the field is \c nil.
 * \return The wanted field as an integer.
 */
int LuaContext::opt_int_field(
    lua_State* l, int table_index, const std::string& key, int default_value) {

  lua_getfield(l, table_index, key.c_str());
  int value = default_value;
  if (!lua_isnil(l, -1)) {

    if (!lua_isnumber(l, -1)) {
      arg_error(l, table_index, StringConcat() <<
          "Bad field '" << key << "' (integer expected, got " <<
          luaL_typename(l, -1) << ")");
    }
    value = int(lua_tointeger(l, -1));
  }

  lua_pop(l, 1);
  return value;
}

/**
 * \brief Checks that a table field is a number and returns it as a double.
 *
 * This function acts like lua_getfield() followed by luaL_checknumber().
 *
 * \param l A Lua state.
 * \param table_index Index of a table in the stack.
 * \param key Key of the field to get in that table.
 * \return The wanted field as a double.
 */
double LuaContext::check_number_field(
    lua_State* l, int table_index, const std::string& key) {

  lua_getfield(l, table_index, key.c_str());
  if (!lua_isnumber(l, -1)) {
    arg_error(l, table_index, StringConcat() <<
        "Bad field '" << key << "' (number expected, got " <<
        luaL_typename(l, -1) << ")");
  }

  int value = lua_tonumber(l, -1);
  lua_pop(l, 1);
  return value;
}

/**
 * \brief Like check_number_field() but with a default value.
 *
 * This function acts like lua_getfield() followed by luaL_optnumber().
 *
 * \param l A Lua state.
 * \param table_index Index of a table in the stack.
 * \param key Key of the field to get in that table.
 * \param default_value The default value to return if the field is \c nil.
 * \return The wanted field as a double.
 */
double LuaContext::opt_number_field(
    lua_State* l, int table_index, const std::string& key, double default_value) {

  lua_getfield(l, table_index, key.c_str());
  double value = default_value;
  if (!lua_isnil(l, -1)) {

    if (!lua_isnumber(l, -1)) {
      arg_error(l, table_index, StringConcat() <<
          "Bad field '" << key << "' (number expected, got " <<
          luaL_typename(l, -1) << ")");
    }
    value = lua_tonumber(l, -1);
  }

  lua_pop(l, 1);
  return value;
}

/**
 * \brief Checks that a table field is a string and returns it.
 *
 * This function acts like lua_getfield() followed by luaL_checkstring().
 *
 * \param l A Lua state.
 * \param table_index Index of a table in the stack.
 * \param key Key of the field to get in that table.
 * \return The wanted field as an string.
 */
const std::string LuaContext::check_string_field(
    lua_State* l, int table_index, const std::string& key) {

  lua_getfield(l, table_index, key.c_str());
  if (!lua_isstring(l, -1)) {
    arg_error(l, table_index, StringConcat() <<
        "Bad field '" << key << "' (string expected, got " <<
        luaL_typename(l, -1) << ")");
  }

  const std::string& value = lua_tostring(l, -1);
  lua_pop(l, 1);
  return value;
}

/**
 * \brief Like check_string_field() but with a default value.
 *
 * This function acts like lua_getfield() followed by luaL_optstring().
 *
 * \param l A Lua state.
 * \param table_index Index of a table in the stack.
 * \param key Key of the field to get in that table.
 * \param default_value The default value to return if the field is \c nil.
 * \return The wanted field as a string.
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
      arg_error(l, table_index, StringConcat() <<
          "Bad field '" << key << "' (string expected, got " <<
          luaL_typename(l, -1) << ")");
    }
    value = lua_tostring(l, -1);
  }

  lua_pop(l, 1);
  return value;
}

/**
 * \brief Checks that a table field is a boolean and returns it.
 *
 * This function acts like lua_getfield() followed by luaL_checktype()
 * and lua_toboolean().
 *
 * \param l A Lua state.
 * \param table_index Index of a table in the stack.
 * \param key Key of the field to get in that table.
 * \return The wanted field as a boolean.
 */
bool LuaContext::check_boolean_field(
    lua_State* l, int table_index, const std::string& key) {

  lua_getfield(l, table_index, key.c_str());
  if (lua_type(l, -1) != LUA_TBOOLEAN) {
    arg_error(l, table_index, StringConcat() <<
        "Bad field '" << key << "' (boolean expected, got " <<
        luaL_typename(l, -1) << ")");
  }

  bool value = lua_toboolean(l, -1);
  lua_pop(l, 1);
  return value;
}

/**
 * \brief Like check_boolean_field() but with a default value.
 *
 * This function acts like lua_getfield() followed by lua_toboolean().
 *
 * \param l A Lua state.
 * \param table_index Index of a table in the stack.
 * \param key Key of the field to get in that table.
 * \param default_value The default value to return if the field is \c nil.
 * \return The wanted field as a string.
 */
bool LuaContext::opt_boolean_field(
    lua_State* l, int table_index, const std::string& key, bool default_value) {

  lua_getfield(l, table_index, key.c_str());
  bool value = default_value;
  if (!lua_isnil(l, -1)) {

    if (lua_type(l, -1) != LUA_TBOOLEAN) {
      arg_error(l, table_index, StringConcat() <<
          "Bad field '" << key << "' (boolean expected, got " <<
          luaL_typename(l, -1) << ")");
    }
    value = lua_toboolean(l, -1);
  }

  lua_pop(l, 1);
  return value;
}

/**
 * \brief Checks that a table field is a function and returns a ref to it.
 *
 * This function acts like lua_getfield() followed by lua_isfunction() and
 * luaL_ref().
 *
 * \param l A Lua state.
 * \param table_index Index of a table in the stack.
 * \param key Key of the field to get in that table.
 * \return The wanted field as a Lua ref to the function.
 */
int LuaContext::check_function_field(
    lua_State* l, int table_index, const std::string& key) {

  lua_getfield(l, table_index, key.c_str());
  if (!lua_isfunction(l, -1)) {
    arg_error(l, table_index, StringConcat() <<
        "Bad field '" << key << "' (function expected, got " <<
        luaL_typename(l, -1) << ")");
  }

  int ref = luaL_ref(l, LUA_REGISTRYINDEX);
  return ref;
}

/**
 * \brief Like check_function_field() but the field is optional.
 *
 * This function acts like lua_getfield() followed by lua_isfunction() and
 * luaL_ref().
 *
 * \param l A Lua state.
 * \param table_index Index of a table in the stack.
 * \param key Key of the field to get in that table.
 * \return The wanted field as a Lua ref to the function, or LUA_REFNIL.
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
      arg_error(l, table_index, StringConcat() <<
          "Bad field '" << key << "' (function expected, got " <<
          luaL_typename(l, -1) << ")");
    }
    ref = luaL_ref(l, LUA_REGISTRYINDEX);
  }

  return ref;
}

/**
 * \brief Creates a reference to the Lua value on top of the stack and pops this
 * value.
 * \return The reference created.
 */
int LuaContext::create_ref() {

  return luaL_ref(l, LUA_REGISTRYINDEX);
}

/**
 * \brief Releases a Lua reference.
 * \param ref The Lua reference to free.
 */
void LuaContext::destroy_ref(int ref) {

  luaL_unref(l, LUA_REGISTRYINDEX, ref);
}

/**
 * \brief Calls a function stored in the registry with a reference and
 * releases this reference.
 * \param callback_ref Reference of the function to call (if LUA_REFNIL,
 * nothing is done).
 */
void LuaContext::do_callback(int callback_ref) {

  if (callback_ref != LUA_REFNIL) {
    push_callback(callback_ref);
    call_function(0, 0, "callback");
    cancel_callback(callback_ref);
  }
}

/**
 * \brief Pushes onto the stack a function stored as a Lua ref.
 * \param callback_ref Reference of the function to call (must be
 * a valid ref).
 */
void LuaContext::push_callback(int callback_ref) {

  push_ref(l, callback_ref);
#ifndef NDEBUG
  if (!lua_isfunction(l, -1)) {
    Debug::die(StringConcat()
        << "There is no callback with ref " << callback_ref
        << " (function expected, got " << luaL_typename(l, -1)
        << "). Did you already invoke or cancel it?"
    );
#endif
}

/**
 * \brief Releases the reference to a Lua callback.
 *
 * The callback may then be collected by Lua.
 *
 * \param callback_ref reference of the function (if LUA_REFNIL,
 * nothing is done)
 */
void LuaContext::cancel_callback(int callback_ref) {

  if (callback_ref != LUA_REFNIL) {

#ifndef NDEBUG
    // Check that the callback is canceled only once.
    // Otherwise, a duplicate call to luaL_unref() silently breaks the
    // uniqueness of Lua refs.
    push_ref(l, callback_ref);
    if (!lua_isfunction(l, -1)) {
      Debug::die(StringConcat()
          << "There is no callback with ref " << callback_ref
          << " (function expected, got " << luaL_typename(l, -1)
          << "). Did you already invoke or cancel it?"
      );
      lua_pop(l, 1);
    }
#endif

    destroy_ref(callback_ref);
  }
}

/**
 * \brief Returns whether a userdata has an entry with the specified key.
 *
 * Userdata can have entries like tables thanks to special __index and
 * __newindex metamethods.
 *
 * Version with const char*, better for performance if you don't have an
 * std::string representation of the key.
 *
 * \param userdata A userdata.
 * \param key String key to test.
 * \return \c true if this key exists on the userdata.
 */
bool LuaContext::userdata_has_field(ExportableToLua& userdata,
    const char* key) const {

  if (!userdata.is_with_lua_table()) {
    return false;
  }

  const std::map<ExportableToLua*, std::set<std::string> >::const_iterator it =
      userdata_fields.find(&userdata);
  if (it == userdata_fields.end()) {
    return false;
  }

  return it->second.find(key) != it->second.end();
}

/**
 * \brief Returns whether a userdata has an entry with the specified key.
 *
 * Userdata can have entries like tables thanks to special __index and
 * __newindex metamethods.
 *
 * Version with std::string, better for performance if you already have an
 * std::string representation of the key.
 *
 * \param userdata A userdata.
 * \param key String key to test.
 * \return \c true if this key exists on the userdata.
 */
bool LuaContext::userdata_has_field(ExportableToLua& userdata,
    const std::string& key) const {

  if (!userdata.is_with_lua_table()) {
    return false;
  }

  const std::map<ExportableToLua*, std::set<std::string> >::const_iterator it =
      userdata_fields.find(&userdata);
  if (it == userdata_fields.end()) {
    return false;
  }

  return it->second.find(key) != it->second.end();
}

/**
 * \brief Gets a method of the object on top of the stack.
 *
 * This is equivalent to find_method(-1, function_name).
 *
 * \param function_name Name of the function to find in the object.
 * This is not an const std::string& but a const char* on purpose to avoid
 * costly conversions as this function is called very often.
 * \return true if the function was found.
 */
bool LuaContext::find_method(const char* function_name) {

  return find_method(-1, function_name);
}

/**
 * \brief Gets a method of an object.
 *
 * If the method exists, the method and the object are both pushed
 * so that you can call the method immediately with the object as first parameter.
 * If the method is not found, the stack is left unchanged.
 *
 * \param index Index of the object in the stack.
 * \param function_name Name of the function to find in the object.
 * This is not an const std::string& but a const char* on purpose to avoid
 * costly conversions as this function is called very often.
 *
 * \return true if the function was found.
 */
bool LuaContext::find_method(int index, const char* function_name) {

  index = get_positive_index(l, index);
                                  // ... object ...
  lua_getfield(l, index, function_name);
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
 * \brief Calls the Lua function with its arguments on top of the stack.
 *
 * This function is like lua_pcall, except that it additionaly handles the
 * error message if an error occurs in the Lua code (the error is printed).
 * This function leaves the results on the stack if there is no error,
 * and leaves nothing on the stack in case of error.
 *
 * \param nb_arguments number of arguments placed on the Lua stack above the
 * function to call
 * \param nb_results number of results expected (you get them on the stack if
 * there is no error)
 * \param function_name A name describing the Lua function (only used to print
 * the error message if any).
 * This is not an const std::string& but a const char* on purpose to avoid
 * costly conversions as this function is called very often.
 * \return true in case of success
 */
bool LuaContext::call_function(
    int nb_arguments,
    int nb_results,
    const char* function_name) {

  return call_function(l, nb_arguments, nb_results, function_name);
}

/**
 * \brief Calls the Lua function with its arguments on top of the stack.
 *
 * This function is like lua_pcall, except that it additionaly handles the
 * error message if an error occurs in the Lua code (the error is printed).
 * This function leaves the results on the stack if there is no error,
 * and leaves nothing on the stack in case of error.
 *
 * \param l A Lua state.
 * \param nb_arguments Number of arguments placed on the Lua stack above the
 * function to call.
 * \param nb_results Number of results expected (you get them on the stack if
 * there is no error).
 * \param function_name A name describing the Lua function (only used to print
 * the error message if any).
 * This is not an const std::string& but a const char* on purpose to avoid
 * costly conversions as this function is called very often.
 * \return true in case of success.
 */
bool LuaContext::call_function(
    lua_State* l,
    int nb_arguments,
    int nb_results,
    const char* function_name) {

  if (lua_pcall(l, nb_arguments, nb_results, 0) != 0) {
    Debug::error(StringConcat() << "In " << function_name << "(): "
        << lua_tostring(l, -1));
    lua_pop(l, 1);
    return false;
  }

  return true;
}

/**
 * \brief Opens a script and lets it on top of the stack as a function.
 * \param l A Lua state.
 * \param script_name File name of the script without extension,
 * relative to the data directory.
 */
void LuaContext::load_file(lua_State* l, const std::string& script_name) {

  if (!load_file_if_exists(l, script_name)) {
    Debug::die(StringConcat() << "Cannot find script file '"
        << script_name << "'");
  }
}

/**
 * \brief Opens a script if it exists and lets it on top of the stack as a
 * function.
 *
 * If the file does not exist, the stack is left intact and false is returned.
 *
 * \param l A Lua state.
 * \param script_name File name of the script with or without extension,
 * relative to the data directory.
 * \return true if the file exists and was loaded.
 */
bool LuaContext::load_file_if_exists(lua_State* l, const std::string& script_name) {

  // Determine the file name (possibly adding ".lua").
  std::string file_name(script_name);

  if (!FileTools::data_file_exists(file_name)) {
    std::ostringstream oss;
    oss << script_name << ".lua";
    file_name = oss.str();
  }

  if (FileTools::data_file_exists(file_name)) {
    // Load the file.
    size_t size;
    char* buffer;
    FileTools::data_file_open_buffer(file_name, &buffer, &size);
    int result = luaL_loadbuffer(l, buffer, size, file_name.c_str());
    FileTools::data_file_close_buffer(buffer);

    if (result != 0) {
      Debug::error(StringConcat() << "Failed to load script '"
          << script_name << "': " << lua_tostring(l, -1));
    }
    return true;
  }
  return false;
}

/**
 * \brief Opens a Lua file and executes it.
 *
 * This function just calls load_file() and call_function().
 * The file must exist.
 *
 * \param l A Lua state.
 * \param script_name File name of the script without extension,
 * relative to the data directory.
 */
void LuaContext::do_file(lua_State* l, const std::string& script_name) {

  load_file(l, script_name);
  call_function(l, 0, 0, script_name.c_str());
}

/**
 * \brief Opens a Lua file if it exists and executes it without arguments.
 *
 * This function just calls load_file_if_exists() and call_function().
 * Nothing is done if the file does not exists.
 *
 * \param l A Lua state.
 * \param script_name File name of the script without extension,
 * relative to the data directory.
 * \return true if the file exists and was successfully executed.
 */
bool LuaContext::do_file_if_exists(lua_State* l, const std::string& script_name) {

  if (load_file_if_exists(l, script_name)) {
    call_function(l, 0, 0, script_name.c_str());
    return true;
  }
  return false;
}

/**
 * \brief For an index in the Lua stack, returns an equivalent positive index.
 *
 * Pseudo-indexes are unchanged.
 *
 * \param l A Lua state.
 * \param index An index in the stack.
 * \return The corresponding positive index.
 */
int LuaContext::get_positive_index(lua_State* l, int index) {

  int positive_index = index;
  if (index < 0 && index >= -lua_gettop(l)) {
    positive_index = lua_gettop(l) + index + 1;
  }

  return positive_index;
}

/**
 * \brief Prints on a line the content of the Lua stack for debugging purposes.
 * \param l A Lua state.
 */
void LuaContext::print_stack(lua_State* l) {

  int i;
  int top = lua_gettop(l);

  for (i = 1; i <= top; i++) {

    int type = lua_type(l, i);
    switch (type) {

      case LUA_TSTRING:
        std::cout << "\"" << lua_tostring(l, i) << "\"";
        break;

      case LUA_TBOOLEAN:
        std::cout << (lua_toboolean(l, i) ? "true" : "false");
        break;

      case LUA_TNUMBER:
        std::cout << lua_tonumber(l, i);
        break;

      case LUA_TLIGHTUSERDATA:
        std::cout << "lightuserdata:" << lua_touserdata(l, i);
        break;

      case LUA_TUSERDATA:
      {
        ExportableToLua* userdata = *(static_cast<ExportableToLua**>(
            lua_touserdata(l, i)));
        const std::string& lua_type_name = userdata->get_lua_type_name();
        std::cout << lua_type_name.substr(lua_type_name.find_last_of('.') + 1);
        break;
      }

      default:
        std::cout << lua_typename(l, type);
        break;

    }
    std::cout << " ";
  }
  std::cout << std::endl;
}

/**
 * \brief Returns whether the specified name is a valid Lua identifier.
 * \param name The name to check.
 * \return true if the name only contains alphanumeric characters or '_' and
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
 * \brief Defines some C++ functions into a Lua table.
 * \param module_name name of the table that will contain the functions
 * (e.g. "sol.main")
 * \param functions list of functions to define in the table
 * (must end with {NULLL, NULL})
 */
void LuaContext::register_functions(const std::string& module_name,
    const luaL_Reg* functions) {

  // create a table and fill it with the functions
  luaL_register(l, module_name.c_str(), functions);
  lua_pop(l, 1);
}

/**
 * \brief Defines some C++ functions into a new Lua userdata type.
 * \param module_name name of the table that will contain the functions
 * (e.g. "sol.movement") - this string will also identify the type
 * \param functions list of functions to define on the type
 * (must end with {NULLL, NULL})
 * \param metamethods metamethods to define on the type (can be NULL)
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
                                  // module mt nil module
    lua_setfield(l, -3, "__index");
                                  // module mt nil
  }
  else {
                                  // module mt __index module
    lua_setfield(l, -3, "usual_index");
                                  // module mt __index
  }
  lua_pop(l, 3);
                                  // --
}

/**
 * \brief Tells the Lua context what C++ functions it can call.
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
  register_file_module();
  register_menu_module();
  register_language_module();
}

/**
 * \brief Pushes a Lua value from its reference.
 * \param l A Lua state.
 * \param ref A Lua reference.
 */
void LuaContext::push_ref(lua_State* l, int ref) {
  lua_rawgeti(l, LUA_REGISTRYINDEX, ref);
}

/**
 * \brief Pushes a string.
 * \param l A Lua state.
 * \param text A string value.
 */
void LuaContext::push_string(lua_State* l, const std::string& text) {
  lua_pushstring(l, text.c_str());
}

/**
 * \brief Pushes a userdata onto the stack.
 * \param l a Lua context
 * \param userdata a userdata
 */
void LuaContext::push_userdata(lua_State* l, ExportableToLua& userdata) {

  // See if this userdata already exists.
  lua_getfield(l, LUA_REGISTRYINDEX, "sol.all_userdata");
                                  // ... all_udata
  lua_pushlightuserdata(l, &userdata);
                                  // ... all_udata lightudata
  lua_gettable(l, -2);
                                  // ... all_udata udata/nil
  if (!lua_isnil(l, -1)) {
                                  // ... all_udata udata
    lua_remove(l, -2);
                                  // ... udata
  }
  else {
    // Create a new userdata.

    if (!userdata.is_known_to_lua()) {
      // This is the first time we create a Lua userdata for this object.
      userdata.set_known_to_lua(true);
    }

                                  // ... all_udata nil
    lua_pop(l, 1);
                                  // ... all_udata
    lua_pushlightuserdata(l, &userdata);
                                  // ... all_udata lightudata
    userdata.increment_refcount();
    ExportableToLua** block_address = static_cast<ExportableToLua**>(
        lua_newuserdata(l, sizeof(ExportableToLua*)));
    *block_address = &userdata;
                                  // ... all_udata lightudata udata
    luaL_getmetatable(l, userdata.get_lua_type_name().c_str());
                                  // ... all_udata lightudata udata mt

#ifndef NDEBUG
    Debug::check_assertion(!lua_isnil(l, -1),
        std::string("Userdata of type '" + userdata.get_lua_type_name()
        + "' has no metatable, this is a memory leak"));

    lua_getfield(l, -1, "__gc");
                                  // ... all_udata lightudata udata mt gc
    Debug::check_assertion(lua_isfunction(l, -1),
        std::string("Userdata of type '") + userdata.get_lua_type_name()
        + "' must have the __gc function LuaContext::userdata_meta_gc");
                                  // ... all_udata lightudata udata mt gc
    lua_pop(l, 1);
                                  // ... all_udata lightudata udata mt
#endif

    lua_setmetatable(l, -2);
                                  // ... all_udata lightudata udata
    // Keep track of our new userdata.
    lua_pushvalue(l, -1);
                                  // ... all_udata lightudata udata udata
    lua_insert(l, -4);
                                  // ... udata all_udata lightudata udata
    lua_settable(l, -3);
                                  // ... udata all_udata
    lua_pop(l, 1);
                                  // ... udata
  }
}

/**
 * \brief Pushes a color onto the stack.
 * \param l A Lua context.
 * \param color A color.
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
 * \brief Returns whether a value is a userdata of a given type.
 * \param l a Lua context
 * \param index an index in the stack
 * \param module_name name of a userdata metatable in the registry
 * \return true if the value is a userdata with this metatable
 */
bool LuaContext::is_userdata(lua_State* l, int index,
    const std::string& module_name) {

  index = get_positive_index(l, index);

                                  // ... udata ...
  void *udata = lua_touserdata(l, index);
  if (udata == NULL) {
    // This is not a userdata.
    return false;
  }
  if (!lua_getmetatable(l, index)) {
    // The userdata has no metatable.
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
 * \brief Checks that the value at the given index is userdata of the
 * specified type and returns it.
 * \param l A Lua state.
 * \param index An index in the Lua stack.
 * \param module_name Name identifying the userdata type.
 * \return The userdata at this index.
 */
ExportableToLua& LuaContext::check_userdata(lua_State* l, int index,
    const std::string& module_name) {

  index = get_positive_index(l, index);

  ExportableToLua** userdata = static_cast<ExportableToLua**>(
    luaL_checkudata(l, index, module_name.c_str()));
  return **userdata;
}

/**
 * \brief Returns whether a value is a color.
 * \param l A Lua context.
 * \param index An index in the stack.
 * \return true if the value is a color, that is, an array with three integers.
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
 * \brief Checks that the value at the given index is a color and returns it.
 * \param l a Lua state
 * \param index an index in the Lua stack
 * \return the color at this index
 */
Color LuaContext::check_color(lua_State* l, int index) {

  index = get_positive_index(l, index);

  luaL_checktype(l, index, LUA_TTABLE);
  lua_rawgeti(l, index, 1);
  lua_rawgeti(l, index, 2);
  lua_rawgeti(l, index, 3);
  Color color(luaL_checkint(l, -3),
    luaL_checkint(l, -2),
    luaL_checkint(l, -1));
  lua_pop(l, 3);

  return color;
}

/**
 * \brief Finalizer of a userdata type.
 * \param l a Lua state
 * \return number of values to return to Lua
 */
int LuaContext::userdata_meta_gc(lua_State* l) {

  ExportableToLua* userdata =
      *(static_cast<ExportableToLua**>(lua_touserdata(l, 1)));

  // Note that the full userdata disappears from Lua but it may come back later!
  // So we need to keep its table if the refcount is not zero.
  // The full userdata is destroyed, but if the refcount is zero, the light
  // userdata and its table persist.

  // We don't need to remove the entry from sol.all_userdata
  // because it is already done: that table is weak on its values and the
  // value was the full userdata.

  userdata->decrement_refcount();
  if (userdata->get_refcount() == 0) {

    if (userdata->is_with_lua_table()) {
      // Remove the table associated to this userdata.
      // Otherwise, if the same pointer gets reallocated, a new userdata will get
      // its table from this deleted one!
                                    // udata
      lua_getfield(l, LUA_REGISTRYINDEX, "sol.userdata_tables");
                                    // udata udata_tables
      lua_pushlightuserdata(l, userdata);
                                    // udata udata_tables lightudata
      lua_pushnil(l);
                                    // udata udata_tables lightudata nil
      lua_settable(l, -3);
                                    // udata udata_tables
      lua_pop(l, 1);
                                    // udata
      get_lua_context(l).userdata_fields.erase(userdata);
    }
    delete userdata;
  }

  return 0;
}

/**
 * \brief Implementation of __newindex that allows userdata to be like tables.
 *
 * Lua code can make "object[key] = value" if object is a userdata with this
 * __newindex metamethod.
 *
 * This metamethod must be used with its corresponding __index
 * metamethod (see userdata_meta_index_as_table).
 *
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::userdata_meta_newindex_as_table(lua_State* l) {

  luaL_checktype(l, 1, LUA_TUSERDATA);
  luaL_checkany(l, 2);
  luaL_checkany(l, 3);

  ExportableToLua* userdata =
      *(static_cast<ExportableToLua**>(lua_touserdata(l, 1)));

  // The user wants to make udata[key] = value but udata is a userdata.
  // So what we make instead is udata_tables[udata][key] = value.
  // This redirection is totally transparent from the Lua side.

  lua_getfield(l, LUA_REGISTRYINDEX, "sol.userdata_tables");
                                  // ... udata_tables
  lua_pushlightuserdata(l, userdata);
                                  // ... udata_tables lightudata
  lua_gettable(l, -2);
                                  // ... udata_tables udata_table/nil
  if (lua_isnil(l, -1)) {
    // Create the userdata table if it does not exist yet.
    userdata->set_with_lua_table(true);
                                  // ... udata_tables nil
    lua_pop(l, 1);
                                  // ... udata_tables
    lua_newtable(l);
                                  // ... udata_tables udata_table
    lua_pushlightuserdata(l, userdata);
                                  // ... udata_tables udata_table lightudata
    lua_pushvalue(l, -2);
                                  // ... udata_tables udata_table lightudata udata_table
    lua_settable(l, -4);
                                  // ... udata_tables udata_table
  }
  lua_pushvalue(l, 2);
                                  // ... udata_tables udata_table key
  lua_pushvalue(l, 3);
                                  // ... udata_tables udata_table key value
  lua_settable(l, -3);
                                  // ... udata_tables udata_table

  if (lua_isstring(l, 2)) {
    if (!lua_isnil(l, 3)) {
      // Add the key to the list of existing strings keys on this userdata.
      get_lua_context(l).userdata_fields[userdata].insert(lua_tostring(l, 2));
    }
    else {
      // Assigning nil: remove the key from the list.
      get_lua_context(l).userdata_fields[userdata].erase(lua_tostring(l, 2));
    }
  }

  return 0;
}

/**
 * \brief Implementation of __index that allows userdata to be like tables.
 *
 * Lua code can get "object[key]" for an arbitrary key previously set.
 *
 * This metamethod must be used with its corresponding __newindex
 * metamethod (see userdata_meta_newindex_as_table).
 *
 * \brief Implementation of __index for the type game.
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
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

  ExportableToLua* userdata =
      *(static_cast<ExportableToLua**>(lua_touserdata(l, 1)));
  LuaContext& lua_context = get_lua_context(l);

  bool found = false;
  // If the userdata actually has a table, lookup this table, unless we already
  // know that we won't find it (because we know all the existing string keys).
  if (userdata->is_with_lua_table() &&
      (!lua_isstring(l, 2) || lua_context.userdata_has_field(*userdata, lua_tostring(l, 2)))) {

    lua_getfield(l, LUA_REGISTRYINDEX, "sol.userdata_tables");
                                  // ... udata_tables
    lua_pushlightuserdata(l, userdata);
                                  // ... udata_tables lightudata
    lua_gettable(l, -2);
                                  // ... udata_tables udata_table/nil
    Debug::check_assertion(!lua_isnil(l, -1), "Missing userdata table");
    lua_pushvalue(l, 2);
                                  // ... udata_tables udata_table key
    lua_gettable(l, -2);
                                  // ... udata_tables udata_table value
    found = !lua_isnil(l, -1);
  }

  // Nothing in the userdata's table: do the usual __index instead
  // (look in the userdata's type).
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
 * \brief Calls the on_started() method of the object on top of the stack.
 */
void LuaContext::on_started() {

  if (find_method("on_started")) {
    call_function(1, 0, "on_started");
  }
}

/**
 * \brief Calls the on_finished() method of the object on top of the stack.
 */
void LuaContext::on_finished() {

  if (find_method("on_finished")) {
    call_function(1, 0, "on_finished");
  }
}

/**
 * \brief Calls the on_update() method of the object on top of the stack.
 */
void LuaContext::on_update() {

  if (find_method("on_update")) {
    call_function(1, 0, "on_update");
  }
}

/**
 * \brief Calls the on_draw() method of the object on top of the stack.
 * \param dst_surface The destination surface.
 */
void LuaContext::on_draw(Surface& dst_surface) {

  if (find_method("on_draw")) {
    push_surface(l, dst_surface);
    call_function(2, 0, "on_draw");
  }
}

/**
 * \brief Calls the on_pre_draw() method of the object on top of the stack.
 * \param dst_surface The destination surface.
 */
void LuaContext::on_pre_draw(Surface& dst_surface) {

  if (find_method("on_pre_draw")) {
    push_surface(l, dst_surface);
    call_function(2, 0, "on_pre_draw");
  }
}

/**
 * \brief Calls the on_post_draw() method of the object on top of the stack.
 * \param dst_surface The destination surface.
 */
void LuaContext::on_post_draw(Surface& dst_surface) {

  if (find_method("on_post_draw")) {
    push_surface(l, dst_surface);
    call_function(2, 0, "on_post_draw");
  }
}

/**
 * \brief Calls the on_suspended() method of the object on top of the stack.
 * \param suspended true to suspend the object, false to unsuspend it.
 */
void LuaContext::on_suspended(bool suspended) {

  if (find_method("on_suspended")) {
    lua_pushboolean(l, suspended);
    call_function(2, 0, "on_suspended");
  }
}

/**
 * \brief Calls the on_paused() method of the object on top of the stack.
 */
void LuaContext::on_paused() {

  if (find_method("on_paused")) {
    call_function(1, 0, "on_paused");
  }
}

/**
 * \brief Calls the on_unpaused() method of the object on top of the stack.
 */
void LuaContext::on_unpaused() {

  if (find_method("on_unpaused")) {
    call_function(1, 0, "on_unpaused");
  }
}

/**
 * \brief Calls the on_dialog_started() method of the object on top of the stack.
 * \param dialog The dialog that just started.
 * \param info_ref Lua ref to the info parameter to pass to the method,
 * or LUA_REFNIL. Will be unref'd if the method exists.
 * \return true if the on_dialog_started() method is defined.
 */
bool LuaContext::on_dialog_started(const Dialog& dialog, int info_ref) {

  if (find_method("on_dialog_started")) {
    push_dialog(l, dialog);
    if (info_ref == LUA_REFNIL) {
      lua_pushnil(l);
    }
    else {
      push_ref(l, info_ref);
      destroy_ref(info_ref);
    }
    call_function(3, 0, "on_dialog_started");
    return true;
  }
  return false;
}

/**
 * \brief Calls the on_dialog_finished() method of the object on top of the stack.
 * \param dialog The dialog that has just finished.
 */
void LuaContext::on_dialog_finished(const Dialog& dialog) {

  if (find_method("on_dialog_finished")) {
    push_dialog(l, dialog);
    call_function(2, 0, "on_dialog_finished");
  }
}

/**
 * \brief Calls the on_game_over_started() method of the object on top of the stack.
 */
bool LuaContext::on_game_over_started() {

  if (find_method("on_game_over_started")) {
    call_function(1, 0, "on_game_over_started");
    return true;
  }
  return false;
}

/**
 * \brief Calls the on_game_over_finished() method of the object on top of the stack.
 */
void LuaContext::on_game_over_finished() {

  if (find_method("on_game_over_finished")) {
    call_function(1, 0, "on_game_over_finished");
  }
}

/**
 * \brief Calls an input callback method of the object on top of the stack.
 * \param event The input event to forward.
 * \return \c true if the event was handled and should stop being propagated.
 */
bool LuaContext::on_input(const InputEvent& event) {

  // Call the Lua function(s) corresponding to this input event.
  bool handled = false;
  if (event.is_keyboard_event()) {
    // Keyboard.
    if (event.is_keyboard_key_pressed()) {
      handled = on_key_pressed(event) || handled;
    }
    else if (event.is_keyboard_key_released()) {
      handled = on_key_released(event) || handled;
    }
  }
  else if (event.is_character_pressed()) {
    handled = on_character_pressed(event) || handled;
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

  return handled;
}

/**
 * \brief Notifies the object on top of the stack
 * that a keyboard key was just pressed
 * (including if it is a directional key or a character).
 * \param event The corresponding input event.
 * \return \c true if the event was handled and should stop being propagated.
 */
bool LuaContext::on_key_pressed(const InputEvent& event) {

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
      bool success = call_function(3, 1, "on_key_pressed");
      if (!success) {
        // Something was wrong in the script: don't propagate the input to other objects.
        handled = true;
      }
      else {
        handled = lua_toboolean(l, -1);
        lua_pop(l, 1);
      }
    }
    else {
      // The method exists but the key is unknown.
      lua_pop(l, 2);  // Pop the object and the method.
    }
  }
  return handled;
}

/**
 * \brief Notifies the object on top of the stack
 * that a character was just pressed with the keyboard.
 * \param event The corresponding input event.
 * \return \c true if the event was handled and should stop being propagated.
 */
bool LuaContext::on_character_pressed(const InputEvent& event) {

  bool handled = false;
  if (find_method("on_character_pressed")) {

    const std::string& character = event.get_character();
    push_string(l, character);
    bool success = call_function(2, 1, "on_character_pressed");
    if (!success) {
      // Something was wrong in the script: don't propagate the input to other objects.
      handled = true;
    }
    else {
      handled = lua_toboolean(l, -1);
      lua_pop(l, 1);
    }
  }
  return handled;
}

/**
 * \brief Notifies the object on top of the stack
 * that a keyboard key was just released
 * (including if it is a directional key).
 * \param event The corresponding input event.
 * \return \c true if the event was handled and should stop being propagated.
 */
bool LuaContext::on_key_released(const InputEvent& event) {

  bool handled = false;
  if (find_method("on_key_released")) {

    const std::string& key_name = InputEvent::get_keyboard_key_name(event.get_keyboard_key());
    if (!key_name.empty()) { // This key exists in the Solarus API.
      push_string(l, key_name);
      bool success = call_function(2, 1, "on_key_released");
      if (!success) {
        // Something was wrong in the script: don't propagate the input to other objects.
        handled = true;
      }
      else {
        handled = lua_toboolean(l, -1);
        lua_pop(l, 1);
      }
    }
    else {
      // The method exists but the key is unknown.
      lua_pop(l, 2);  // Pop the object and the method.
    }
  }
  return handled;
}

/**
 * \brief Notifies the object on top of the stack
 * that a joypad button was just pressed.
 * \param event The corresponding input event.
 * \return \c true if the event was handled and should stop being propagated.
 */
bool LuaContext::on_joypad_button_pressed(const InputEvent& event) {

  bool handled = false;
  if (find_method("on_joyad_button_pressed")) {
    int button = event.get_joypad_button();

    lua_pushinteger(l, button);
    bool success = call_function(2, 1, "on_joyad_button_pressed");
    if (!success) {
      // Something was wrong in the script: don't propagate the input to other objects.
      handled = true;
    }
    else {
      handled = lua_toboolean(l, -1);
      lua_pop(l, 1);
    }
  }
  return handled;
}

/**
 * \brief Notifies the object on top of the stack
 * that a joypad button was just released.
 * \param event The corresponding input event.
 * \return \c true if the event was handled and should stop being propagated.
 */
bool LuaContext::on_joypad_button_released(const InputEvent& event) {

  bool handled = false;
  if (find_method("on_joyad_button_released")) {
    int button = event.get_joypad_button();

    lua_pushinteger(l, button);
    bool success = call_function(2, 1, "on_joyad_button_released");
    if (!success) {
      // something was wrong in the script: don't propagate the input to other objects.
      handled = true;
    }
    else {
      handled = lua_toboolean(l, -1);
      lua_pop(l, 1);
    }
  }
  return handled;
}

/**
 * \brief Notifies the object on top of the stack
 * that a joypad axis was just moved.
 * \param event The corresponding input event.
 * \return \c true if the event was handled and should stop being propagated.
 */
bool LuaContext::on_joypad_axis_moved(const InputEvent& event) {

  bool handled = false;
  if (find_method("on_joyad_axis_moved")) {
    int axis = event.get_joypad_axis();
    int state = event.get_joypad_axis_state();

    lua_pushinteger(l, axis);
    lua_pushinteger(l, state);
    bool success = call_function(3, 1, "on_joyad_axis_moved");
    if (!success) {
      // something was wrong in the script: don't propagate the input to other objects.
      handled = true;
    }
    else {
      handled = lua_toboolean(l, -1);
      lua_pop(l, 1);
    }
  }
  return handled;
}

/**
 * \brief Notifies the object on top of the stack
 * that a joypad hat was just moved.
 * \param event The corresponding input event.
 * \return \c true if the event was handled and should stop being propagated.
 */
bool LuaContext::on_joypad_hat_moved(const InputEvent& event) {

  bool handled = false;
  if (find_method("on_joyad_hat_moved")) {
    int hat = event.get_joypad_hat();
    int direction8 = event.get_joypad_hat_direction();

    lua_pushinteger(l, hat);
    lua_pushinteger(l, direction8);
    bool success = call_function(3, 1, "on_joyad_hat_moved");
    if (!success) {
      // something was wrong in the script: don't propagate the input to other objects.
      handled = true;
    }
    else {
      handled = lua_toboolean(l, -1);
      lua_pop(l, 1);
    }
  }
  return handled;
}

/**
 * \brief Calls the on_command_pressed() method of the object on top of the stack.
 * \param command The game command just pressed.
 */
bool LuaContext::on_command_pressed(GameCommands::Command command) {

  bool handled = false;
  if (find_method("on_command_pressed")) {
    push_string(l, GameCommands::get_command_name(command));
    bool success = call_function(2, 1, "on_command_pressed");
    if (!success) {
      // Something was wrong in the script: don't propagate the command to other objects.
      handled = true;
    }
    else {
      handled = lua_toboolean(l, -1);
      lua_pop(l, 1);
    }
  }
  return handled;
}

/**
 * \brief Calls the on_command_released() method of the object on top of the stack.
 * \param command The game command just pressed.
 */
bool LuaContext::on_command_released(GameCommands::Command command) {

  bool handled = false;
  if (find_method("on_command_released")) {
    push_string(l, GameCommands::get_command_name(command));
    bool success = call_function(2, 1, "on_command_released");
    if (!success) {
      // Something was wrong in the script: don't propagate the command to other objects.
      handled = true;
    }
    else {
      handled = lua_toboolean(l, -1);
      lua_pop(l, 1);
    }
  }
  return handled;
}

/**
 * \brief Calls the on_animation_finished() method of the object on top of the stack.
 * \param animation Name of the animation finished.
 */
void LuaContext::on_animation_finished(const std::string& animation) {

  if (find_method("on_animation_finished")) {
    push_string(l, animation);
    call_function(2, 0, "on_animation_finished");
  }
}

/**
 * \brief Calls the on_animation_changed() method of the object on top of the stack.
 * \param animation Name of the new animation.
 */
void LuaContext::on_animation_changed(const std::string& animation) {

  if (find_method("on_animation_changed")) {
    push_string(l, animation);
    call_function(2, 0, "on_animation_changed");
  }
}

/**
 * \brief Calls the on_direction_changed() method of the object on top of the stack.
 * \param animation Name of the sprite animation.
 * \param frame The new frame.
 */
void LuaContext::on_direction_changed(
    const std::string& animation, int direction) {

  if (find_method("on_direction_changed")) {
    push_string(l, animation);
    lua_pushinteger(l, direction);
    call_function(3, 0, "on_direction_changed");
  }
}

/**
 * \brief Calls the on_frame_changed() method of the object on top of the stack.
 * \param animation Name of the sprite animation.
 * \param frame The new frame.
 */
void LuaContext::on_frame_changed(const std::string& animation, int frame) {

  if (find_method("on_frame_changed")) {
    push_string(l, animation);
    lua_pushinteger(l, frame);
    call_function(3, 0, "on_frame_changed");
  }
}

/**
 * \brief Calls the on_position_changed() method of the object on top of the stack.
 */
void LuaContext::on_position_changed() {

  if (find_method("on_position_changed")) {
    call_function(1, 0, "on_position_changed");
  }
}

/**
 * \brief Calls the on_obstacle_reached() method of the object on top of the stack.
 */
void LuaContext::on_obstacle_reached() {

  if (find_method("on_obstacle_reached")) {
    call_function(1, 0, "on_obstacle_reached");
  }
}

/**
 * \brief Calls the on_changed() method of the object on top of the stack.
 */
void LuaContext::on_changed() {

  if (find_method("on_changed")) {
    call_function(1, 0, "on_changed");
  }
}

/**
 * \brief Calls the on_started() method of the object on top of the stack.
 * \param destination The destination point used (NULL if it's a special one).
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
 * \brief Calls the on_opening_transition_finished() method of the object on top of the stack.
 * \param destination The destination point used (NULL if it's a special one).
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
 * \brief Calls the on_camera_back() method of the object on top of the stack.
 */
void LuaContext::on_camera_back() {

  if (find_method("on_camera_back")) {
    call_function(1, 0, "on_camera_back");
  }
}

/**
 * \brief Calls the on_obtaining_treasure() method of the object on top of the stack.
 * \param treasure The treasure being obtained.
 */
void LuaContext::on_obtaining_treasure(const Treasure& treasure) {

  if (find_method("on_obtaining_treasure")) {
    push_item(l, treasure.get_item());
    lua_pushinteger(l, treasure.get_variant());
    if (!treasure.is_saved()) {
      lua_pushnil(l);
    }
    else {
      lua_pushstring(l, treasure.get_savegame_variable().c_str());
    }
    call_function(4, 0, "on_obtaining_treasure");
  }
}

/**
 * \brief Calls the on_obtained_treasure() method of the object on top of the stack.
 * \param treasure The treasure just obtained.
 */
void LuaContext::on_obtained_treasure(const Treasure& treasure) {

  if (find_method("on_obtained_treasure")) {
    push_item(l, treasure.get_item());
    lua_pushinteger(l, treasure.get_variant());
    if (!treasure.is_saved()) {
      lua_pushnil(l);
    }
    else {
      lua_pushstring(l, treasure.get_savegame_variable().c_str());
    }
    call_function(4, 0, "on_obtained_treasure");
  }
}

/**
 * \brief Calls the on_state_changed() method of the object on top of the stack.
 * \param state_name A name describing the new state.
 */
void LuaContext::on_state_changed(const std::string& state_name) {

  if (find_method("on_state_changed")) {
    push_string(l, state_name);
    call_function(2, 0, "on_state_changed");
  }
}

/**
 * \brief Calls the on_activating() method of the object on top of the stack.
 */
void LuaContext::on_activating() {

  if (find_method("on_activating")) {
    call_function(1, 0, "on_activating");
  }
}

/**
 * \brief Calls the on_activating() method of the object on top of the stack.
 * \param direction Direction to pass as parameter.
 */
void LuaContext::on_activating(int direction) {

  if (find_method("on_activating")) {
    lua_pushinteger(l, direction);
    call_function(2, 0, "on_activating");
  }
}

/**
 * \brief Calls the on_activated() method of the object on top of the stack.
 */
void LuaContext::on_activated() {

  if (find_method("on_activated")) {
    call_function(1, 0, "on_activated");
  }
}

/**
 * \brief Calls the on_activated() method of the object on top of the stack.
 * \param direction Direction to pass as parameter.
 */
void LuaContext::on_activated(int direction) {

  if (find_method("on_activated")) {
    lua_pushinteger(l, direction);
    call_function(2, 0, "on_activated");
  }
}

/**
 * \brief Calls the on_inactivated_repeat() method of the object on top of the stack.
 */
void LuaContext::on_activated_repeat() {

  if (find_method("on_activated_repeat")) {
    call_function(1, 0, "on_activated_repeat");
  }
}

/**
 * \brief Calls the on_inactivated() method of the object on top of the stack.
 */
void LuaContext::on_inactivated() {

  if (find_method("on_inactivated")) {
    call_function(1, 0, "on_inactivated");
  }
}

/**
 * \brief Calls the on_left() method of the object on top of the stack.
 */
void LuaContext::on_left() {

  if (find_method("on_left")) {
    call_function(1, 0, "on_left");
  }
}

/**
 * \brief Calls the on_npc_interaction() method of the object on top of the stack.
 * \param npc An NPC.
 */
void LuaContext::on_npc_interaction(NPC& npc) {

  if (find_method("on_npc_interaction")) {
    push_npc(l, npc);
    call_function(2, 0, "on_npc_interaction");
  }
}

/**
 * \brief Calls the on_npc_interaction_item() method of the object on top of the stack.
 * \param npc An NPC.
 * \param item_used The equipment item used.
 * \return true if an interaction occurred.
 */
bool LuaContext::on_npc_interaction_item(NPC& npc, EquipmentItem& item_used) {

  bool interacted = false;
  if (find_method("on_npc_interaction_item")) {
    push_npc(l, npc);
    push_item(l, item_used);
    bool success = call_function(3, 1, "on_npc_interaction_item");
    if (!success) {
      // Something was wrong in the script: don't propagate the event to other objects.
      interacted = true;
    }
    else {
      interacted = lua_toboolean(l, -1);
      lua_pop(l, 1);
    }
  }
  return interacted;
}

/**
 * \brief Calls the on_interaction() method of the object on top of the stack.
 */
void LuaContext::on_interaction() {

  if (find_method("on_interaction")) {
    call_function(1, 0, "on_interaction");
  }
}

/**
 * \brief Calls the on_interaction_item() method of the object on top of the stack.
 * \param item_used The equipment item used.
 * \return true if an interaction occurred.
 */
bool LuaContext::on_interaction_item(EquipmentItem& item) {

  bool interacted = false;
  if (find_method("on_interaction_item")) {
    push_item(l, item);
    bool success = call_function(2, 1, "on_interaction_item");
    if (!success) {
      // Something was wrong in the script: don't propagate the event to other objects.
      interacted = true;
    }
    else {
      interacted = lua_toboolean(l, -1);
      lua_pop(l, 1);
    }
  }
  return interacted;
}

/**
 * \brief Calls the on_npc_collision_fire() method of the object on top of the stack.
 * \param npc An NPC.
 */
void LuaContext::on_npc_collision_fire(NPC& npc) {

  if (find_method("on_npc_collision_fire")) {
    push_npc(l, npc);
    call_function(2, 0, "on_npc_collision_fire");
  }
}

/**
 * \brief Calls the on_collision_fire() method of the object on top of the stack.
 */
void LuaContext::on_collision_fire() {

  if (find_method("on_collision_fire")) {
    call_function(1, 0, "on_collision_fire");
  }
}

/**
 * \brief Calls the on_collision_explosion() method of the object on top of the stack.
 */
void LuaContext::on_collision_explosion() {

  if (find_method("on_collision_explosion")) {
    call_function(1, 0, "on_collision_explosion");
  }
}

/**
 * \brief Calls the on_empty() method of the object on top of the stack.
 * \return \c true if the on_empty() method is defined.
 */
bool LuaContext::on_empty() {

  if (find_method("on_empty")) {
    call_function(1, 0, "on_empty");
    return true;
  }
  return false;
}

/**
 * \brief Calls the on_buying() method of the object on top of the stack.
 * \return true if the player is allowed to buy the item.
 */
bool LuaContext::on_buying() {

  bool can_buy = true;
  if (find_method("on_buying")) {
    bool success = call_function(1, 1, "on_buying");
    if (!success) {
      // Something was wrong in the script: don't let the player buy the item.
      can_buy = false;
    }
    else {
      can_buy = lua_toboolean(l, -1);
      lua_pop(l, 1);
    }
  }
  return can_buy;
}

/**
 * \brief Calls the on_bought() method of the object on top of the stack.
 */
void LuaContext::on_bought() {

  if (find_method("on_bought")) {
    call_function(1, 0, "on_bought");
  }
}

/**
 * \brief Calls the on_opened() method of the object on top of the stack.
 */
void LuaContext::on_opened() {

  if (find_method("on_opened")) {
    call_function(1, 0, "on_opened");
  }
}

/**
 * \brief Calls the on_closed() method of the object on top of the stack.
 */
void LuaContext::on_closed() {

  if (find_method("on_closed")) {
    call_function(1, 0, "on_closed");
  }
}

/**
 * \brief Calls the on_moving() method of the object on top of the stack.
 */
void LuaContext::on_moving() {

  if (find_method("on_moving")) {
    call_function(1, 0, "on_moving");
  }
}

/**
 * \brief Calls the on_moved() method of the object on top of the stack.
 */
void LuaContext::on_moved() {

  if (find_method("on_moved")) {
    call_function(1, 0, "on_moved");
  }
}

/**
 * \brief Calls the on_map_changed() method of the object on top of the stack.
 * \param map The new active map.
 */
void LuaContext::on_map_changed(Map& map) {

  if (find_method("on_map_changed")) {
    push_map(l, map);
    call_function(2, 0, "on_map_changed");
  }
}

/**
 * \brief Calls the on_pickable_created() method of the object on top of the stack.
 * \param pickable A pickable treasure.
 */
void LuaContext::on_pickable_created(Pickable& pickable) {

  if (find_method("on_pickable_created")) {
    push_entity(l, pickable);
    call_function(2, 0, "on_pickable_created");
  }
}

/**
 * \brief Calls the on_pickable_movement_changed() method of the object on top of the stack.
 * \param pickable A pickable treasure.
 * \param movement The movement of this pickable treasure.
 */
void LuaContext::on_pickable_movement_changed(Pickable& pickable, Movement& movement) {

  if (find_method("on_pickable_movement_changed")) {
    push_entity(l, pickable);
    push_movement(l, movement);
    call_function(3, 0, "on_pickable_movement_changed");
  }
}

/**
 * \brief Calls the on_variant_changed() method of the object on top of the stack.
 * \param variant Variant of an equipment item.
 */
void LuaContext::on_variant_changed(int variant) {

  if (find_method("on_variant_changed")) {
    lua_pushinteger(l, variant);
    call_function(2, 0, "on_variant_changed");
  }
}

/**
 * \brief Calls the on_amount_changed() method of the object on top of the stack.
 * \param amount Amount of an equipment item.
 */
void LuaContext::on_amount_changed(int amount) {

  if (find_method("on_amount_changed")) {
    lua_pushinteger(l, amount);
    call_function(2, 0, "on_amount_changed");
  }
}

/**
 * \brief Calls the on_obtaining() method of the object on top of the stack.
 * \param treasure The treasure being obtained.
 */
void LuaContext::on_obtaining(const Treasure& treasure) {

  if (find_method("on_obtaining")) {
    lua_pushinteger(l, treasure.get_variant());
    if (!treasure.is_saved()) {
      lua_pushnil(l);
    }
    else {
      lua_pushstring(l, treasure.get_savegame_variable().c_str());
    }
    call_function(3, 0, "on_obtaining");
  }
}

/**
 * \brief Calls the on_obtained() method of the object on top of the stack.
 * \param treasure The treasure just obtained.
 */
void LuaContext::on_obtained(const Treasure& treasure) {

  if (find_method("on_obtained")) {
    lua_pushinteger(l, treasure.get_variant());
    if (!treasure.is_saved()) {
      lua_pushnil(l);
    }
    else {
      lua_pushstring(l, treasure.get_savegame_variable().c_str());
    }
    call_function(3, 0, "on_obtained");
  }
}

/**
 * \brief Calls the on_using() method of the object on top of the stack.
 */
void LuaContext::on_using() {

  if (find_method("on_using")) {
    call_function(1, 0, "on_using");
  }
}

/**
 * \brief Calls the on_ability_used() method of the object on top of the stack.
 * \param ability_name Id of a built-in ability.
 */
void LuaContext::on_ability_used(const std::string& ability_name) {

  if (find_method("on_ability_used")) {
    push_string(l, ability_name);
    call_function(2, 0, "on_ability_used");
  }
}

/**
 * \brief Calls the on_created() method of the object on top of the stack.
 */
void LuaContext::on_created() {

  if (find_method("on_created")) {
    call_function(1, 0, "on_created");
  }
}

/**
 * \brief Calls the on_removed() method of the object on top of the stack.
 */
void LuaContext::on_removed() {

  if (find_method("on_removed")) {
    call_function(1, 0, "on_removed");
  }
}

/**
 * \brief Calls the on_enabled() method of the object on top of the stack.
 */
void LuaContext::on_enabled() {

  if (find_method("on_enabled")) {
    call_function(1, 0, "on_enabled");
  }
}

/**
 * \brief Calls the on_disabled() method of the object on top of the stack.
 */
void LuaContext::on_disabled() {

  if (find_method("on_disabled")) {
    call_function(1, 0, "on_disabled");
  }
}

/**
 * \brief Calls the on_restarted() method of the object on top of the stack.
 */
void LuaContext::on_restarted() {

  if (find_method("on_restarted")) {
    call_function(1, 0, "on_restarted");
  }
}

/**
 * \brief Calls the on_pre_draw() method of the object on top of the stack.
 */
void LuaContext::on_pre_draw() {

  if (find_method("on_pre_draw")) {
    call_function(1, 0, "on_pre_draw");
  }
}

/**
 * \brief Calls the on_post_draw() method of the object on top of the stack.
 */
void LuaContext::on_post_draw() {

  if (find_method("on_post_draw")) {
    call_function(1, 0, "on_post_draw");
  }
}

/**
 * \brief Calls the on_position_changed() method of the object on top of the stack.
 * \param xy The new position.
 * \param layer The new layer.
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
 * \brief Calls the on_obstacle_reached() method of the object on top of the stack.
 * \param movement The movement that reached an obstacle.
 */
void LuaContext::on_obstacle_reached(Movement& movement) {

  if (find_method("on_obstacle_reached")) {
    push_movement(l, movement);
    call_function(2, 0, "on_obstacle_reached");
  }
}

/**
 * \brief Calls the on_movement_changed() method of the object on top of the stack.
 * \param movement A movement.
 */
void LuaContext::on_movement_changed(Movement& movement) {

  if (find_method("on_movement_changed")) {
    push_movement(l, movement);
    call_function(2, 0, "on_movement_changed");
  }
}

/**
 * \brief Calls the on_movement_finished() method of the object on top of the stack.
 */
void LuaContext::on_movement_finished() {

  if (find_method("on_movement_finished")) {
    call_function(1, 0, "on_movement_finished");
  }
}

/**
 * \brief Calls the on_collision_enemy() method of the object on top of the stack.
 * \param other_enemy Another enemy colliding with the object on top of the stack.
 * \param other_sprite Colliding sprite of the other enemy.
 * \param this_sprite Colliding sprite of the first enemy.
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
 * \brief Calls the on_custom_attack_received() method of the object on top of the stack.
 * \param attack The attack received.
 * \param sprite The sprite that receives the attack if any.
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
 * \brief Calls the on_hurt() method of the object on top of the stack.
 * \param attack The attack received.
 * \param life_lost Number of life points just lost.
 */
void LuaContext::on_hurt(EnemyAttack attack, int life_lost) {

  if (find_method("on_hurt")) {
    push_string(l, enemy_attack_names[attack]);
    lua_pushinteger(l, life_lost);
    call_function(3, 0, "on_hurt");
  }
}

/**
 * \brief Calls the on_dying() method of the object on top of the stack.
 */
void LuaContext::on_dying() {

  if (find_method("on_dying")) {
    call_function(1, 0, "on_dying");
  }
}

/**
 * \brief Calls the on_dead() method of the object on top of the stack.
 */
void LuaContext::on_dead() {

  if (find_method("on_dead")) {
    call_function(1, 0, "on_dead");
  }
}

/**
 * \brief Calls the on_immobilized() method of the object on top of the stack.
 */
void LuaContext::on_immobilized() {

  if (find_method("on_immobilized")) {
    call_function(1, 0, "on_immobilized");
  }
}

/**
 * \brief Function called when an unprotected Lua error occurs.
 * \param l The Lua context.
 * \return Number of values to return to Lua.
 */
int LuaContext::l_panic(lua_State* l) {

  const std::string& error = luaL_checkstring(l, 1);

  Debug::die(error);

  return 0;
}

/**
 * \brief A loader that makes require() able to load Lua files
 * from the quest data directory or archive.
 * \param l The Lua context.
 * \return Number of values to return to Lua.
 */
int LuaContext::l_loader(lua_State* l) {

  const std::string& script_name = luaL_checkstring(l, 1);
  bool exists = load_file_if_exists(l, script_name);

  if (!exists) {
    std::ostringstream oss;
    oss << std::endl << "\tno quest file '" << script_name
      << ".lua' in 'data/', 'data.solarus' or 'data.solarus.zip'";
    push_string(l, oss.str());
  }
  return 1;
}

