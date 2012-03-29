/*
 * Copyright (C) 2006-2012 Christopho, Solarus - http://www.solarus-engine.org
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
#include "Map.h"
#include "Timer.h"
#include "Sprite.h"
#include <lua.hpp>
#include <sstream>
#include <cstdarg>

/**
 * @brief Creates a script.
 * @param apis_enabled an OR-combination of optional APIs to enable
 * (0 for none)
 */
Script::Script(uint32_t apis_enabled):
  l(NULL),
  music_played(false),
  apis_enabled(apis_enabled) {

}

/**
 * @brief Desctructor.
 */
Script::~Script() {

  // close the Lua execution context
  if (l != NULL) {
    lua_close(l);
  }

  // delete unfinished timers
  std::map<Timer*, int>::iterator it;
  for (it = timers.begin(); it != timers.end(); it++) {
    delete it->first;
  }
}

/**
 * @brief Initializes the Lua context for this script.
 */
void Script::initialize_lua_context() {

  // create an execution context
  l = lua_open();
  luaL_openlibs(l);

  // put a pointer to this Script object in the Lua context
  lua_pushstring(l, "sol.cpp_object");
  lua_pushlightuserdata(l, this);
  lua_settable(l, LUA_REGISTRYINDEX); // registry["sol.cpp_object"] = this

  // create the Solarus table that will be available to the script
  lua_newtable(l);
  lua_setglobal(l, "sol");

  // register the C++ functions and types accessible by the script
  register_modules();
}

/**
 * @brief Initializes the Lua context and loads the script from a file.
 *
 * The script file must exist.
 *
 * @param script_name name of a Lua script file (without extension)
 */
void Script::load(const std::string &script_name) {

  load_if_exists(script_name);
  Debug::check_assertion(is_loaded(),
      StringConcat() << "Cannot load script '" << script_name << "'");
}

/**
 * @brief Initializes the Lua context and loads the script from a file.
 *
 * If the script file does not exist, the field context is NULL.
 *
 * @param script_name name of a Lua script file (without extension)
 */
void Script::load_if_exists(const std::string& script_name) {

  if (l == NULL) {
    initialize_lua_context();
  }

  // determine the file name (.lua)
  std::ostringstream oss;
  oss << script_name << ".lua";
  std::string file_name = oss.str();

  if (FileTools::data_file_exists(file_name)) {
    // load the file
    size_t size;
    char* buffer;
    FileTools::data_file_open_buffer(file_name, &buffer, &size);
    luaL_loadbuffer(l, buffer, size, file_name.c_str());
    FileTools::data_file_close_buffer(buffer);
    if (lua_pcall(l, 0, 0, 0) != 0) {
      Debug::die(StringConcat() << "Error: failed to load script '" << script_name
          << "': " << lua_tostring(l, -1));
      lua_pop(l, 1);
    }
  }
  else {
    // uninitialize Lua
    lua_close(l);
    l = NULL;
  }
}

/**
 * @brief Returns whether this script is loaded.
 * @return true if the script is loaded
 */
bool Script::is_loaded() {
  return l != NULL;
}

/**
 * @brief Returns the game associated to this script (if any).
 *
 * Scripts that enable the game API must redefine this function.
 * This function is called by the implementation of the game API.
 *
 * @return the game
 */
Game& Script::get_game() {

  Debug::die("This script does not provide the game API");
  throw; // to make the compiler happy
}

/**
 * @brief Returns the map associated to this script (if any).
 *
 * Scripts that enable the map API must redefine this function.
 * This function is called by the implementation of the map API.
 *
 * @return the map
 */
Map& Script::get_map() {

  Debug::die("This script does not provide the map API");
  throw;
}

/**
 * @brief Returns the properties of the item associated to this script (if any).
 *
 * Scripts that enable the item API must redefine this function.
 * This function is called by the implementation of the item API.
 *
 * @return the item properties
 */
ItemProperties& Script::get_item_properties() {

  Debug::die("This script does not provide the item API");
  throw;
}

/**
 * @brief Returns the enemy associated to this script (if any).
 *
 * Scripts that enable the enemy API must redefine this function.
 * This function is called by the implementation of the enemy API.
 *
 * @return the enemy
 */
Enemy& Script::get_enemy() {

  Debug::die("This script does not provide the enemy API");
  throw;
}

/**
 * @brief Returns the screen controlled by this script (if any).
 *
 * Scripts that want to control the screen must redefine this function.
 *
 * @return the screen controlled by this script
 */
CustomScreen& Script::get_screen() {

  Debug::die("This script does not control the screen");
  throw;
}

/**
 * @brief Tells the Lua context what C++ functions it can call.
 */
void Script::register_modules() {

  // modules available to all scripts
  register_main_module();
  register_audio_module();
  register_timer_module();
  register_surface_module();
  register_text_surface_module();
  register_sprite_module();
  register_movement_module();

  if (apis_enabled && GAME_API) {
    register_game_module();
  }
  if (apis_enabled && MAP_API) {
    register_map_module();
  }
  if (apis_enabled && ITEM_API) {
    register_item_module();
  }
  if (apis_enabled && ENEMY_API) {
    register_enemy_module();
  }
  if (apis_enabled && INPUT_API) {
    register_input_module();
  }
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

  // create a table and fill it with the methods
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
  lua_pushvalue(l, -2);
                                  // module mt module
  // metatable.__index = module
  lua_setfield(l, -2, "__index");
                                  // module mt
  lua_pop(l, 2);
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
 * @brief Looks up the specified global Lua function and places it onto the stack if it exists.
 *
 * If the function is not found, the stack is left unchanged.
 *
 * @param function_name of the function to find
 * @return true if the function was found
 */
bool Script::find_lua_function(const std::string& function_name) {

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
 * @brief Calls a Lua function of the current script, possibly with arguments and return values of various types.
 *
 * TODO kill this function and push parameters manually instead. This function is too limited
 * because it is restricted to basic C types (varargs don't work with std::string).
 * It is also error-prone: bool return values segfault on some systems and there are no
 * compilation warnings.
 *
 * This is just a convenient method to push the parameters and pop the results for you
 * in addition to calling the Lua function.
 * However, this function uses the variable number of parameters mechanism of cstdarg, which
 * is inherently C and not C++.
 * This means you have to use C-style strings instead of std::string and int instead of bool.
 *
 * The arguments and results of the Lua function are passed thanks to the variadic
 * parameters (possibly of different types) of this C++ method.
 * The format parameter of this C++ method specifies the type of each
 * argument and each result of the Lua function to call.
 * The types of the arguments should be described in the format string as a sequence of characters
 * where each character represents a type ('i': int, 'b': bool, 's': const char*).
 * If you expect some results to get returned by the Lua function,
 * the format string should then take a space character,
 * and the types of the results are then specified in the same way,
 * except that results of type string are not accepted.
 * The space character is optional if no result is returned.
 * This means an empty format string can be used when the Lua function has no argument
 * and no return value.
 *
 * The remaining parameters of this C++ method (of variable number)
 * must match the specified format,
 * passing values for the arguments and pointers for the results.
 *
 * Let's take an example:
 * assuming that the Lua function you want to call takes as arguments
 * a string plus two integers and returns a boolean,
 * the format string you should specify is: "sii b".
 * You should then give four parameters of types const char*, int, int and bool*.
 *
 * If the Lua function does not exists in the script,
 * nothing happens and this C++ function returns false.
 * It just means that the function corresponds to an event that
 * the script does not want to handle.
 * If the Lua function exists, the arguments are pushed onto the stack, the function is executed,
 * the results (if any) are popped from the stack and stored into your pointed areas,
 * and this C++ method returns true.
 * In both cases, the Lua stack is left unchanged.
 *
 * This function does not support results of type string because it would cause some
 * complications and we want to keep its usage simple.
 * If you need to call a function with a result of type string, you can still do it,
 * but not with this C++ method.
 * I explain now what the problem is with string results.
 * If a Lua function returns a string, the memory used by the const char* pointer is discarded
 * when the C++ code pops it from the stack.
 * And this C++ method is supposed to to the job for you, so it pops the results
 * from the stack before returning them to you.
 * As a consequence, allowing string results
 * would require that you pop the results yourself, after having read them.
 * Another solution would be to return a copy of the string,
 * but because the variable number of parameters mechanism cannot use std::string,
 * the copy would be a const char* that you would have to free yourself.
 * As this function wants to simplify your code by doing the job for you,
 * both solutions are bad ideas.
 * However, in Solarus, calling Lua from C++ is used only to notify a script that something
 * happened (recall the name of this C++ method), not to ask strings to them.
 *
 * @param function_name name of the function to call
 * (may be prefixed by the name of several Lua tables, typically sol.main.some_function)
 * @param format a string describing the types of arguments to pass to the Lua function
 * and the types of return values to get (see above)
 * @return true if the function was called successfully, false if it does not exist
 */
bool Script::notify_script(const std::string& function_name, const char* format, ...) {

  // find the function and push it onto the stack
  bool exists = find_lua_function(function_name);

  if (exists) {

    va_list args;
    va_start(args, format);

    // push the arguments
    unsigned int i;
    unsigned int nb_arguments = 0;
    bool end_arguments = false;
    for (i = 0; i < strlen(format) && !end_arguments; i++) {
      switch (format[i]) {
        case 'i':	lua_pushinteger(l, va_arg(args, int));	break;
        case 'b':	lua_pushboolean(l, va_arg(args, int));	break; 		// cstdarg refuses bool
        case 's':	lua_pushstring(l, va_arg(args, const char*));	break;	// and std::string
        case ' ':	end_arguments = true;	break;
        default:	Debug::die(StringConcat() << "Invalid character '" << format[i] << "' in format string '" << format);
      }

      if (format[i] != ' ') {
        nb_arguments++;
      }
    }

    // call the function
    int nb_results = strlen(format) - i;
    if (!call_script(nb_arguments, nb_results, function_name)) {
      nb_results = 0;
    }

    // get the results
    for (int i = 0; i < nb_results; i++) {
      char type = format[nb_arguments + i + 1];
      int stack_index = -nb_results + i;
      switch (type) {
        case 'i':	*va_arg(args, int*) = lua_tointeger(l, stack_index);	break;
        case 'b':	*va_arg(args, int*) = lua_toboolean(l, stack_index);	break;
        case 's':	Debug::die("String results are not supported by Script::notify_script(), please make the call yourself");
        default:	Debug::die(StringConcat() << "Invalid character '" << type << "' in format string '" << format);
      }
    }
    lua_pop(l, nb_results);
    va_end(args);
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
bool Script::call_script(int nb_arguments, int nb_results,
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
 * @brief Updates the script.
 */
void Script::update() {
  update_displayables();
  update_timers();
}

/**
 * @brief This function is called when the game (if any) is being suspended or resumed.
 * @param suspended true if the game is suspended, false if it is resumed
 */
void Script::set_suspended(bool suspended) {

  if (l != NULL) {

    // notify the timers
    std::map<Timer*, int>::iterator it;
    for (it = timers.begin(); it != timers.end(); it++) {
      it->first->set_suspended(suspended);
    }
  }
}

/**
 * @brief Returns whether this script has played a music,
 * i.e. called the sol.main.play_music(music_id).
 * @return true if this script has played a music
 */
bool Script::has_played_music() {
  return music_played;
}

/**
 * @brief Returns whether a value is a userdata of a given type.
 * @param l a Lua context
 * @param index an index in the stack
 * @param module_name name of a userdata metatable in the registry
 * @return true if the value is a userdata with this metatable
 */
bool Script::is_userdata(lua_State* l, int index, const std::string& module_name) {

  if (index < 0) {
    // ensure a positive index
    index = lua_gettop(l) + index + 1;
  }

                                  /* ... udata ... */
  void *udata = lua_touserdata(l, index);
  if (udata == NULL) {
    // it's not a userdata
    return false;
  }
  if (!lua_getmetatable(l, index)) {
    // the userdata has no metatable
    return false;
  }
                                  /* ... udata ... mt_found */
  lua_getfield(l, LUA_REGISTRYINDEX, module_name.c_str());
                                  /* ... udata ... mt_found mt_expected */
  bool result = lua_rawequal(l, -1, -2);
  lua_pop(l, 2);
                                  /* ... udata ... */
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
 *
 * @param l a Lua state
 * @param index an index in the Lua stack
 * @param module_name name identifying the userdata type
 * @return the userdata at this index
 */
ExportableToLua& Script::check_userdata(lua_State* l, int index,
    const std::string& module_name) {

  if (index < 0) {
    // ensure a positive index
    index = lua_gettop(l) + index + 1;
  }

  ExportableToLua** userdata =
    (ExportableToLua**) luaL_checkudata(l, index, module_name.c_str());
  return **userdata;
}

/**
 * @brief Finalizes a userdata.
 *
 * - Argument 1: a userdata
 *
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
 * @brief Checks that the value at the given index is a color and returns it.
 * @param l a Lua state
 * @param index an index in the Lua stack
 * @return the color at this index
 */
Color Script::check_color(lua_State* l, int index) {

  if (index < 0) {
    // ensure a positive index
    index = lua_gettop(l) + index + 1;
  }

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
 * @brief Notifies the script that another map has just been started.
 * @param map the new current map
 */
void Script::event_map_changed(Map &map) {

  notify_script("event_map_changed", "i", map.get_id());
}

/**
 * @brief Notifies the script that a dialog has just started to be displayed
 * in the dialog box.
 * @param dialog_id id of the dialog
 */
void Script::event_dialog_started(const std::string& dialog_id) {

  notify_script("event_dialog_started", "s", dialog_id.c_str());
}

/**
 * @brief Notifies the script that the dialog box has just finished.
 *
 * This function is called when the last group of 3 lines of a dialog is
 * finished.
 * The dialog box has just been closed but the game is still suspended.
 * Note that this event is not called if the dialog was skipped.
 *
 * @param dialog_id id of the dialog that has just finished
 * @param answer the answer selected by the player: 0 for the first one,
 * 1 for the second one, -1 if there was no question
 */
void Script::event_dialog_finished(const std::string& dialog_id, int answer) {

  notify_script("event_dialog_finished", "si", dialog_id.c_str(), answer);
}

/**
 * @brief Notifies the script that the player has just pressed the action
 * key in front of an NPC.
 * @param npc_name name of the NPC
 */
void Script::event_npc_interaction(const std::string& npc_name) {

  notify_script("event_npc_interaction", "s", npc_name.c_str());
}

/**
 * @brief Notifies the script that the hero is using an inventory item
 * in front of an NPC.
 *
 * @param npc_name name of the npc the hero is facing
 * @param item_name name of the inventory item that is being used
 * @param variant variant of this inventory item
 * @return true if the script has handled the event,
 * i.e. if the function event_npc_interaction_item exists in the script and returned true
 */
bool Script::event_npc_interaction_item(const std::string& npc_name,
    const std::string& item_name, int variant) {

  int interaction = 0;
  notify_script("event_npc_interaction_item", "ssi b", npc_name.c_str(),
      item_name.c_str(), variant, &interaction);

  return interaction != 0;
}

/**
 * @brief Notifies the script that there was just a collision between an NPC and fire.
 * @param npc_name name of the NPC
 */
void Script::event_npc_collision_fire(const std::string &npc_name) {

  notify_script("event_npc_collision_fire", "s", npc_name.c_str());
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

    if (lua_pcall(l, 0, 0, 0) != 0) {
      Debug::print(StringConcat() << "Error in callback function:"
          << lua_tostring(l, -1));
      lua_pop(l, 1);
    }
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

  luaL_unref(l, LUA_REGISTRYINDEX, callback_ref);
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

