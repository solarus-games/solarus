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
#include "lua/Script.h"
#include "movements/Movement.h"
#include "lowlevel/FileTools.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"
#include "lowlevel/Color.h"
#include "MainLoop.h"
#include "Map.h"
#include "Timer.h"
#include "Sprite.h"
#include <lua.hpp>
#include <sstream>
#include <cstdarg>

/**
 * @brief Creates a script.
 * @param main_loop The Solarus main loop manager.
 */
Script::Script(MainLoop& main_loop):
  l(NULL),
  main_loop(main_loop),
  current_game(NULL),
  current_screen(NULL) {

}

/**
 * @brief Desctructor.
 */
Script::~Script() {

  exit();
}

/**
 * @brief Initializes the Lua context for this script.
 */
void Script::initialize() {

  // Create an execution context.
  l = lua_open();
  luaL_openlibs(l);

  // Put a pointer to this Script object in the Lua context.
  lua_pushstring(l, "sol.cpp_object");
  lua_pushlightuserdata(l, this);
  lua_settable(l, LUA_REGISTRYINDEX);  // registry["sol.cpp_object"] = this

  // Allow userdata to behave like tables if they want.
  lua_pushstring(l, "sol.userdata_tables");
                                  // "sol..." udata_tables
  lua_newtable(l);
                                  // "sol..." udata_tables
  lua_settable(l, LUA_REGISTRYINDEX);  // registry["sol.userdata_tables"] = {}

  // Create the Solarus table that will be available to the script.
  lua_newtable(l);
  lua_setglobal(l, "sol");

  // register the C++ functions and types accessible by the script
  register_modules();
}

/**
 * @brief Uninitializes Lua.
 */
void Script::exit() {

  // Close the Lua execution context.
  if (l != NULL) {

    // Destroy unfinished timers.
    remove_timers();

    lua_close(l);
    l = NULL;
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
bool Script::load_file_if_exists(lua_State* l, const std::string& script_name) {

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
      Debug::die(StringConcat() << "Error: failed to load script '"
          << script_name << "': " << lua_tostring(l, -1));
    }
    return true;
  }
  return false;
}

/**
 * @brief Opens a script and lets it on top of the stack as a function.
 * @param l A Lua state.
 * @param script_name File name of the script without extension,
 * relative to the data directory.
 */
void Script::load_file(lua_State* l, const std::string& script_name) {

  if (!load_file_if_exists(l, script_name)) {
    Debug::die(StringConcat() << "Cannot find script file '"
        << script_name << "'");
  }
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
void Script::do_file(lua_State* l, const std::string& script_name) {

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
bool Script::do_file_if_exists(lua_State* l, const std::string& script_name) {

  if (load_file_if_exists(l, script_name)) {
    call_function(l, 0, 0, script_name);
    return true;
  }
  return false;
}

/**
 * @brief Returns the Solarus main loop object.
 * @return The main loop manager.
 */
MainLoop& Script::get_main_loop() {
  return main_loop;
}

/**
 * @brief If a game is currently running, returns that game.
 * @return The game currently running if any, NULL otherwise.
 */
Game* Script::get_current_game() {
  return current_game;
}

/**
 * @brief Sets the game currently running.
 * @param current_game The game currently running if any, NULL otherwise.
 */
void Script::set_current_game(Game* current_game) {
  this->current_game = current_game;
}

/**
 * @brief If no game is currently running, returns the current menu.
 * @return The current menu if any, NULL otherwise.
 */
CustomScreen* Script::get_current_screen() {
  return current_screen;
}

/**
 * @brief Sets the current menu.
 * @param current_screen The current menu if any, NULL otherwise.
 */
void Script::set_current_screen(CustomScreen* current_screen) {
  this->current_screen = current_screen;
}

/**
 * @brief Tells the Lua context what C++ functions it can call.
 */
void Script::register_modules() {

  // modules available to all scripts
  register_main_module();
  register_game_module();
  register_map_module();
  register_enemy_module();
  register_audio_module();
  register_timer_module();
  register_surface_module();
  register_text_surface_module();
  register_sprite_module();
  register_movement_module();
  register_item_module();
  register_input_module();
}

/**
 * @brief Defines some C++ functions into a Lua table.
 * @param module_name name of the table that will contain the functions
 * (e.g. "sol.main")
 * @param functions list of functions to define in the table
 * (must end with {NULLL, NULL})
 */
void Script::register_functions(const std::string& module_name,
    const luaL_Reg* functions) {

  // create a table and fill it with the methods
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
void Script::register_type(const std::string& module_name,
    const luaL_Reg* functions, const luaL_Reg* metamethods) {

  // create a table and fill it with the functions
  luaL_register(l, module_name.c_str(), functions);
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
 * @brief When Lua calls a C++ static method, this function retrieves the
 * corresponding Script object.
 *
 * @param l the Lua context
 * @return the Script object that initiated the call
 */
Script& Script::get_script(lua_State* l) {

  // retrieve the Script object
  lua_getfield(l, LUA_REGISTRYINDEX, "sol.cpp_object");
  Script* script = (Script*) lua_touserdata(l, -1);
  lua_pop(l, 1);
  return *script;
}

/**
 * @brief Prints on a line the content of the Lua stack for debugging purposes.
 */
void Script::print_stack() {

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
 * @brief For an index in the Lua stack, returns an equivalent positive index.
 * @param l A Lua state.
 * @param index An index in the stack (positive or negative).
 * @return The corresponding positive index.
 */
int Script::get_positive_index(lua_State* l, int index) {

  if (index < 0) {
    index = lua_gettop(l) + index + 1;
  }
  return index;
}

/**
 * @brief Creates a reference to the Lua value on top of the stack.
 */
int Script::create_ref() {
  return luaL_ref(l, LUA_REGISTRYINDEX);
}

/**
 * @brief Frees a Lua reference.
 * @param ref The Lua reference to unref.
 */
void Script::destroy_ref(int ref) {
  luaL_unref(l, LUA_REGISTRYINDEX, ref);
}

/**
 * @brief Pushes a Lua value from its reference.
 * @param l A Lua state.
 * @param ref A Lua reference.
 */
void Script::push_ref(lua_State* l, int ref) {
  lua_rawgeti(l, LUA_REGISTRYINDEX, ref);
}

/**
 * @brief Looks up the specified global Lua function and places it onto the stack if it exists.
 *
 * If the function is not found, the stack is left unchanged.
 *
 * @param function_name name of the function to find
 * @return true if the function was found
 */
bool Script::find_global_function(const std::string& function_name) {

  if (l == NULL) {
    return false;
  }

  lua_getglobal(l, function_name.c_str());

  bool exists = lua_isfunction(l, -1);
  if (!exists) { // restore the stack
    lua_pop(l, 1);
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
 * @param l A Lua state.
 * @param nb_arguments Number of arguments placed on the Lua stack above the
 * function to call.
 * @param nb_results Number of results expected (you get them on the stack if
 * there is no error).
 * @param function_name A name describing the Lua function (only used to print
 * the error message if any).
 * @return true in case of success.
 */
bool Script::call_function(lua_State* l, int nb_arguments, int nb_results,
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
bool Script::call_function(int nb_arguments, int nb_results,
    const std::string& function_name) {

  return call_function(l, nb_arguments, nb_results, function_name);
}

/**
 * @brief Updates the script.
 */
void Script::update() {

  update_displayables();
  update_timers();
}

/**
 * @brief This function is called when the game (if any) is being suspended
 * or resumed.
 * @param suspended true if the game is suspended, false if it is resumed.
 */
void Script::set_suspended(bool suspended) {

  // Notify the timers.
  if (l != NULL) {
    set_suspended_timers(suspended);
  }
}

/**
 * @brief Returns whether a value is a userdata of a given type.
 * @param l a Lua context
 * @param index an index in the stack
 * @param module_name name of a userdata metatable in the registry
 * @return true if the value is a userdata with this metatable
 */
bool Script::is_userdata(lua_State* l, int index,
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
 * @brief Pushes a userdata onto the stack.
 * @param l a Lua context
 * @param userdata a userdata
 */
void Script::push_userdata(lua_State* l, ExportableToLua& userdata) {

  Script& script = get_script(l);

  script.increment_refcount(&userdata);
                                  // ...
  ExportableToLua** block_adress =
    (ExportableToLua**) lua_newuserdata(l, sizeof(ExportableToLua*));
  *block_adress = &userdata;
                                  // ... userdata
  luaL_getmetatable(l, userdata.get_lua_type_name().c_str());
                                  // ... userdata mt
  lua_setmetatable(l, -2);
                                  // ... userdata
}

/**
 * @brief Checks that the value at the given index is userdata of the
 * specified type and returns it.
 * @param l a Lua state
 * @param index an index in the Lua stack
 * @param module_name name identifying the userdata type
 * @return the userdata at this index
 */
ExportableToLua& Script::check_userdata(lua_State* l, int index,
    const std::string& module_name) {

  index = get_positive_index(l, index);

  ExportableToLua** userdata =
    (ExportableToLua**) luaL_checkudata(l, index, module_name.c_str());
  return **userdata;
}

/**
 * @brief Compares two userdata of the same type.
 * @param l a Lua state
 * @return number of values to return to Lua
 */
int Script::userdata_meta_eq(lua_State* l) {

  ExportableToLua** userdata1 = (ExportableToLua**) lua_touserdata(l, 1);
  ExportableToLua** userdata2 = (ExportableToLua**) lua_touserdata(l, 2);

  lua_pushboolean(l, *userdata1 == *userdata2);

  return 1;
}

/**
 * @brief Finalizer of a userdata type.
 * @param l a Lua state
 * @return number of values to return to Lua
 */
int Script::userdata_meta_gc(lua_State* l) {

  Script& script = get_script(l);
  ExportableToLua** userdata = (ExportableToLua**) lua_touserdata(l, 1);

  script.decrement_refcount(*userdata);

  return 0;
}

/**
 * @brief Implementation of __newindex that allow userdata to be like tables.
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
int Script::userdata_meta_newindex_as_table(lua_State* l) {

  luaL_checktype(l, 1, LUA_TUSERDATA);
  luaL_checkany(l, 2);
  luaL_checkany(l, 3);

  ExportableToLua** userdata = (ExportableToLua**) lua_touserdata(l, 1);

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
 * @brief Implementation of __index that allow userdata to be like tables.
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
int Script::userdata_meta_index_as_table(lua_State* l) {

  /* The user wants to make udata[key] but udata is a userdata.
   * So what we retrieve instead is udata_tables[udata][key].
   * This redirection is totally transparent from the Lua side.
   * If udata_tables[udata][key] does not exist, we fall back
   * to the usual __index for userdata, i.e. we look for a method
   * in its type.
   */

  luaL_checktype(l, 1, LUA_TUSERDATA);
  luaL_checkany(l, 2);

  ExportableToLua** userdata = (ExportableToLua**) lua_touserdata(l, 1);

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
 * @brief Checks that the value at the given index is a color and returns it.
 * @param l a Lua state
 * @param index an index in the Lua stack
 * @return the color at this index
 */
Color Script::check_color(lua_State* l, int index) {

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
 * @brief Pushes a color onto the stack.
 * @param l A Lua context.
 * @param color A color.
 */
void Script::push_color(lua_State* l, const Color& color) {

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
 * @brief Calls a function stored in the registry with a reference and
 * releases this reference.
 * @param callback_ref reference of the function to call (if LUA_REFNIL,
 * nothing is done)
 */
void Script::do_callback(int callback_ref) {

  if (callback_ref != LUA_REFNIL) {
    lua_rawgeti(l, LUA_REGISTRYINDEX, callback_ref);
    if (!lua_isfunction(l, -1)) {
      Debug::die(StringConcat() << "No such Lua callback (function expected, got "
          << luaL_typename(l, -1) << ")");
    }

    call_function(0, 0, "callback");
    luaL_unref(l, LUA_REGISTRYINDEX, callback_ref);
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
void Script::cancel_callback(int callback_ref) {
  destroy_ref(callback_ref);
}

/**
 * @brief Returns whether a userdata was created by this script.
 * @param userdata a userdata
 * @return true if this script created the userdata
 */
bool Script::has_created(ExportableToLua* userdata) {
  return userdata->get_creator_script() == this;
}

/**
 * @brief Marks a userdata as created by this script.
 * @param userdata a userdata
 */
void Script::set_created(ExportableToLua* userdata) {
  userdata->set_creator_script(this);
}

/**
 * @brief Adds 1 to the reference counter of a userdata.
 *
 * If the object was not created by this script, nothing is done.
 *
 * @param userdata the userdata
 */
void Script::increment_refcount(ExportableToLua* userdata) {

  if (has_created(userdata)) {
    userdata->increment_refcount();
  }
}

/**
 * @brief Removes 1 to the reference counter of a userdata and possibly
 * destroys the object.
 *
 * If the counter gets to zero, the object is deleted immediately.
 * If the object was not created by this script, nothing is done.
 *
 * @param userdata the userdata
 */
void Script::decrement_refcount(ExportableToLua* userdata) {

  if (has_created(userdata)) {
    userdata->decrement_refcount();
    if (userdata->get_refcount() == 0) {
      delete userdata;
    }
  }
}

