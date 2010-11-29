/*
 * Copyright (C) 2009 Christopho, Solarus - http://www.solarus-engine.org
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
#include "Game.h"
#include "Timer.h"
#include "movements/Movement.h"
#include "lowlevel/FileTools.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"
#include <lua.hpp>
#include <sstream>
#include <cstdarg>

/**
 * @brief Creates a script.
 * @param apis_enabled an OR-combination of APIs to enable
 */
Script::Script(uint32_t apis_enabled):
  context(NULL),
  apis_enabled(apis_enabled) {

}

/**
 * @brief Desctructor.
 */
Script::~Script() {

  // close the Lua execution context
  if (context != NULL) {
    lua_close(context);
  }

  // delete the timers
  {
    std::list<Timer*>::iterator it;
    for (it = timers.begin(); it != timers.end(); it++) {
      delete *it;
    }
  }

  // delete the movements still stored by this script, if any
  {
    std::map<int, Movement*>::iterator it;
    for (it = unassigned_movements.begin(); it != unassigned_movements.end(); it++) {
      delete it->second;
    }
  }
}

void Script::initialize_lua_context() {

  // initialize fields
  next_sprite_handle = 1;

  // create an execution context
  context = lua_open();
  luaL_openlibs(context);

  // put a pointer to this Script object in the Lua context
  lua_pushstring(context, "sol.cpp_object");
  lua_pushlightuserdata(context, this);
  lua_settable(context, LUA_REGISTRYINDEX); // registry["sol.cpp_object"] = this

  // create the Solarus table that will be available to the script
  lua_newtable(context);
  lua_setglobal(context, "sol");

  // register the C++ functions accessible to the script
  register_apis();

  // initialize the script
  luaL_dostring(context, "math.randomseed(os.time())");
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
  Debug::assert(is_loaded(),
      StringConcat() << "Cannot load script '" << script_name << "'");
}

/**
 * @brief Initializes the Lua context and loads the script from a file.
 *
 * If the script file does not exist, the field context is NULL.
 *
 * @param script_name name of a Lua script file (without extension)
 */
void Script::load_if_exists(const std::string &script_name) {

  if (context == NULL) {
    initialize_lua_context();
  }

  // determine the file name (.lua or .luac)
  std::ostringstream oss;
  oss << script_name << ".lua";
  std::string clear_script_name = oss.str();
  oss << "c";
  std::string compiled_script_name = oss.str();

  std::string file_name;
#if SOLARUS_DEBUG_LEVEL == 0
  file_name = compiled_script_name; // in release mode, only use the compiled version of the script (.luac)
#else
  if (FileTools::data_file_exists(clear_script_name)) {
    file_name = clear_script_name; // in debug mode, we prefer using the clear text script, when available
  }
  else {
    file_name = compiled_script_name;
  }
#endif

  if (FileTools::data_file_exists(file_name)) {
    // load the file
    size_t size;
    char *buffer;
    FileTools::data_file_open_buffer(file_name, &buffer, &size);
    luaL_loadbuffer(context, buffer, size, file_name.c_str());
    FileTools::data_file_close_buffer(buffer);
    lua_call(context, 0, 0);
  }
  else {
    // uninitialize Lua
    lua_close(context);
    context = NULL;
  }
}

/**
 * @brief Returns whether this script is loaded.
 * @return true if the script is loaded
 */
bool Script::is_loaded() {
  return context != NULL;
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
 * @brief Tells the Lua context what C++ functions it can call.
 */
void Script::register_apis() {

  if (apis_enabled && MAIN_API) {
    register_main_api();
  }
  if (apis_enabled && GAME_API) {
    register_game_api();
  }
  if (apis_enabled && MAP_API) {
    register_map_api();
  }
  if (apis_enabled && ITEM_API) {
    register_item_api();
  }
}

/**
 * @brief Registers to the script the functions of the main API.
 */
void Script::register_main_api() {

  static luaL_Reg main_api[] = {
      { "include", main_api_include },
      { "play_sound", main_api_play_sound },
      { "play_music", main_api_play_music },
      { "timer_start", main_api_timer_start },
      { "timer_stop", main_api_timer_stop },
      { "sprite_get_animation", main_api_sprite_get_animation },
      { "sprite_set_animation", main_api_sprite_set_animation },
      { "sprite_get_direction", main_api_sprite_get_direction },
      { "sprite_set_direction", main_api_sprite_set_direction },
      { "sprite_get_frame", main_api_sprite_get_frame },
      { "sprite_set_frame", main_api_sprite_set_frame },
      { "sprite_get_frame_delay", main_api_sprite_get_frame_delay },
      { "sprite_set_frame_delay", main_api_sprite_set_frame_delay },
      { "sprite_is_paused", main_api_sprite_is_paused },
      { "sprite_set_paused", main_api_sprite_set_paused },
      { "sprite_set_animation_ignore_suspend", main_api_sprite_set_animation_ignore_suspend },
      { "sprite_fade", main_api_sprite_fade },
      { "pixel_movement_create", main_api_pixel_movement_create },
      { "random_movement_create", main_api_random_movement_create },
      { "path_movement_create", main_api_path_movement_create },
      { "random_path_movement_create", main_api_random_path_movement_create },
      { "jump_movement_create", main_api_jump_movement_create },
      { "movement_get_property", main_api_movement_get_property },
      { "movement_set_property", main_api_movement_set_property },
      { NULL, NULL }
  };
  luaL_register(context, "sol.main", main_api);
}

/**
 * @brief Registers to the script the functions of the game API.
 */
void Script::register_game_api() {

  static luaL_Reg game_api[] = {
      { "savegame_get_string", game_api_savegame_get_string },
      { "savegame_get_integer", game_api_savegame_get_integer },
      { "savegame_get_boolean", game_api_savegame_get_boolean },
      { "savegame_set_string", game_api_savegame_set_string },
      { "savegame_set_integer", game_api_savegame_set_integer },
      { "savegame_set_boolean", game_api_savegame_set_boolean },
      { "savegame_get_name", game_api_savegame_get_name },
      { "get_life", game_api_get_life },
      { "add_life", game_api_add_life },
      { "remove_life", game_api_remove_life },
      { "get_max_life", game_api_get_max_life },
      { "set_max_life", game_api_set_max_life },
      { "add_max_life", game_api_add_max_life },
      { "get_money", game_api_get_money },
      { "add_money", game_api_add_money },
      { "remove_money", game_api_remove_money },
      { "has_ability", game_api_has_ability },
      { "get_ability", game_api_get_ability },
      { "set_ability", game_api_set_ability },
      { "has_item", game_api_has_item },
      { "get_item", game_api_get_item },
      { "set_item", game_api_set_item },
      { "has_item_amount", game_api_has_item_amount },
      { "get_item_amount", game_api_get_item_amount },
      { "add_item_amount", game_api_add_item_amount },
      { "remove_item_amount", game_api_remove_item_amount },
      { "is_dungeon_finished", game_api_is_dungeon_finished },
      { "set_dungeon_finished", game_api_set_dungeon_finished },
      { NULL, NULL }
  };
  luaL_register(context, "sol.game", game_api);
}

/**
 * @brief Registers to the script the functions of the map API.
 */
void Script::register_map_api() {

  static luaL_Reg map_api[] = {
      { "dialog_start", map_api_dialog_start },
      { "dialog_set_variable", map_api_dialog_set_variable },
      { "dialog_set_style", map_api_dialog_set_style },
      { "hud_set_enabled", map_api_hud_set_enabled },
      { "hud_set_pause_enabled", map_api_hud_set_pause_enabled },
      { "treasure_give", map_api_treasure_give },
      { "camera_move", map_api_camera_move },
      { "camera_restore", map_api_camera_restore },
      { "hero_freeze", map_api_hero_freeze },
      { "hero_unfreeze", map_api_hero_unfreeze },
      { "hero_set_map", map_api_hero_set_map },
      { "hero_set_visible", map_api_hero_set_visible },
      { "hero_get_direction", map_api_hero_get_direction },
      { "hero_set_direction", map_api_hero_set_direction },
      { "npc_get_position", map_api_npc_get_position },
      { "npc_set_position", map_api_npc_set_position },
      { "hero_align_on_sensor", map_api_hero_align_on_sensor },
      { "hero_walk", map_api_hero_walk },
      { "hero_start_jumping", map_api_hero_start_jumping },
      { "hero_start_victory_sequence", map_api_hero_start_victory_sequence },
      { "hero_start_boomerang", map_api_hero_start_boomerang },
      { "hero_start_bow", map_api_hero_start_bow },
      { "hero_start_running", map_api_hero_start_running },
      { "npc_start_movement", map_api_npc_start_movement },
      { "npc_get_sprite", map_api_npc_get_sprite },
      { "npc_remove", map_api_npc_remove },
      { "interactive_entity_get_sprite", map_api_interactive_entity_get_sprite },
      { "interactive_entity_remove", map_api_interactive_entity_remove },
      { "chest_set_open", map_api_chest_set_open },
      { "chest_set_hidden", map_api_chest_set_hidden },
      { "chest_is_hidden", map_api_chest_is_hidden },
      { "tile_set_enabled", map_api_tile_set_enabled },
      { "tile_set_group_enabled", map_api_tile_set_group_enabled },
      { "tile_is_enabled", map_api_tile_is_enabled },
      { "block_reset", map_api_block_reset },
      { "block_reset_all", map_api_block_reset_all },
      { "shop_item_remove", map_api_shop_item_remove },
      { "switch_is_enabled", map_api_switch_is_enabled },
      { "switch_set_enabled", map_api_switch_set_enabled },
      { "switch_set_locked", map_api_switch_set_locked },
      { "enemy_is_dead", map_api_enemy_is_dead },
      { "enemy_is_group_dead", map_api_enemy_is_group_dead },
      { "enemy_set_enabled", map_api_enemy_set_enabled },
      { "enemy_start_boss", map_api_enemy_start_boss },
      { "enemy_end_boss", map_api_enemy_end_boss },
      { "enemy_start_miniboss", map_api_enemy_start_miniboss },
      { "enemy_end_miniboss", map_api_enemy_end_miniboss },
      { "sensor_remove", map_api_sensor_remove },
      { "door_open", map_api_door_open },
      { "door_close", map_api_door_close },
      { "door_is_open", map_api_door_is_open },
      { "door_set_open", map_api_door_set_open },
      { NULL, NULL }
  };
  luaL_register(context, "sol.map", map_api);
}

/**
 * @brief Registers to the script the functions of the item API.
 */
void Script::register_item_api() {

  static luaL_Reg item_api[] = {
      { "get_variant", item_api_get_variant },
      { "set_variant", item_api_set_variant },
      { "get_amount", item_api_get_amount },
      { "set_amount", item_api_set_amount },
      { "add_amount", item_api_add_amount },
      { "remove_amount", item_api_remove_amount },
      { "get_sprite", item_api_get_sprite },
      { "get_movement", item_api_get_movement },
      { "start_movement", item_api_start_movement },
      { "set_finished", item_api_set_finished },
      { NULL, NULL }
  };
  luaL_register(context, "sol.item", item_api);
}

/**
 * @brief Checks the number of arguments provided to a C++ function
 * called by the Lua script.
 *
 * In any C++ function called by the Lua script (i.e. a function prefixed by "l_"),
 * the first instruction calls this function.
 * It checks the number of arguments provided to the C++ function called by the Lua script
 * and retrieves the current Script object.
 *
 * @param context the Lua context
 * @param nb_arguments the number of arguments to check (if it is incorrect, the program stops)
 * @param script if not NULL, a pointer to the Script object will be copied there so that the static C++ function
 * called by the Lua script can access it
 */
void Script::called_by_script(lua_State *context, int nb_arguments, Script **script) {

  // check the number of arguments
  Debug::assert(lua_gettop(context) == nb_arguments, "Invalid number of arguments when calling C++ from Lua");

  // retrieve the Script object
  if (script != NULL) {
    lua_pushstring(context, "sol.cpp_object");
    lua_gettable(context, LUA_REGISTRYINDEX);
    *script = (Script*) lua_touserdata(context, -1);
    lua_pop(context, 1);
  }
}

/**
 * @brief Prints on a line the content of the Lua stack for debugging purposes.
 */
void Script::print_stack() {

  int i;
  int top = lua_gettop(context);

  for (i = 1; i <= top; i++) {

    int type = lua_type(context, i);
    switch (type) {

      case LUA_TSTRING:
	std::cout << lua_tostring(context, i);
	break;

      case LUA_TBOOLEAN:
	std::cout << (lua_toboolean(context, i) ? "true" : "false");
	break;

      case LUA_TNUMBER:
	std::cout << lua_tonumber(context, i);
	break;

      default:
	std::cout << lua_typename(context, type);
	break;

    }
    std::cout << " ";
  }
  std::cout << std::endl;
}

/**
 * @brief Looks up the specified Lua function and places it onto the stack if it exists.
 *
 * If the function is not found, the stack is left unchanged.
 *
 * @param function_name of the function to find
 * (may be prefixed by the name of several Lua tables, typically sol.main.some_function)
 * @return true if the function was found
 */
bool Script::find_lua_function(const std::string &function_name) {

  if (context == NULL) {
    return false;
  }

  // we want to allow to set sol.main.some_function as a timer callback, even if it is not a global function

  size_t index = function_name.find(".");
  if (index == std::string::npos) {

    // usual Lua function
    lua_getglobal(context, function_name.c_str());
  }
  else {

    // function in a table (e.g. sol.main.some_function)
    std::string table_name = function_name.substr(0, index);
    std::string tail = function_name.substr(index + 1);

    lua_getglobal(context, table_name.c_str());

    index = tail.find(".");
    while (index != std::string::npos) { // there may even be several intermediary tables

      table_name = tail.substr(0, index);
      tail = tail.substr(index + 1);

      lua_pushstring(context, table_name.c_str());
      lua_gettable(context, -2);
      lua_remove(context, -2);

      index = tail.find(".");
    }

    // now tail is the function name without prefix, and the table that contains it is on the top of the stack
    lua_pushstring(context, tail.c_str());
    lua_gettable(context, -2);
    lua_remove(context, -2);
  }

  bool exists = lua_isfunction(context, -1);
  if (!exists) { // restore the stack
    lua_pop(context, 1);
  }

  return exists;
}

/**
 * @brief Calls a Lua function of the current script, possibly with arguments and return values of various types.
 *
 * This is just a convenient method to push the parameters and pop the results for you
 * in addition to calling the Lua function.
 * However, this function uses the variable number of parameters mechanism of cstdarg, which
 * is inherently C and not C++.
 * This means you have to use C-style strings instead of std::string.
 *
 * The arguments and results of the Lua function are passed thanks to the variable number of
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
 * the copy would be a const char* that you would have to free yourselft.
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
bool Script::notify_script(const std::string &function_name, const std::string &format, ...) {

  // find the function and push it onto the stack
  bool exists = find_lua_function(function_name);

  if (exists) {

    va_list args;
    va_start(args, format);

    // push the arguments
    unsigned int i;
    unsigned int nb_arguments = 0;
    bool end_arguments = false;
    for (i = 0; i < format.size() && !end_arguments; i++) {
      switch (format[i]) {
	case 'i':	lua_pushinteger(context, va_arg(args, int));	break;
	case 'b':	lua_pushboolean(context, va_arg(args, int));	break; 		// cstdarg refuses bool
	case 's':	lua_pushstring(context, va_arg(args, const char*));	break;	// and std::string
	case ' ':	end_arguments = true;
	default:	Debug::die(StringConcat() << "Invalid character '" << format[i] << "' in format string '" << format);
      }

      if (format[i] != ' ') {
	nb_arguments++;
      }
    }

    // call the function
    int nb_results = format.size() - i;
    lua_call(context, nb_arguments, nb_results);

    // get the results
    for (int i = 0; i < nb_results; i++) {
      char type = format[nb_arguments + i + 1];
      int stack_index = -nb_results + i;
      switch (type) {
	case 'i':	*va_arg(args, int*) = lua_tointeger(context, stack_index);	break;
	case 'b':	*va_arg(args, int*) = lua_toboolean(context, stack_index);	break;
	case 's':	Debug::die("String results are not supported by Script::notify_script(), please make the call yourself");
	default:	Debug::die(StringConcat() << "Invalid character '" << type << "' in format string '" << format);
      }
    }
    lua_pop(context, nb_results);
    va_end(args);
  }

  return exists;
}

/**
 * @brief Updates the script.
 */
void Script::update() {

  // update the timers
  std::list<Timer*>::iterator it;

  for (it = timers.begin(); it != timers.end(); it++) {

    Timer *timer = *it;

    timer->update();
    if (timer->is_finished()) {
      
      bool invoked = notify_script(timer->get_name()); 
      if (!invoked) {
	Debug::die(StringConcat() << "No such timer callback function: '" << timer->get_name() << "'");
      }

      delete timer;
      timers.erase(it);
      it = timers.begin();
    }
  }
}

/**
 * @brief This function is called when the game (if any) is being suspended or resumed.
 * @param suspended true if the game is suspended, false if it is resumed
 */
void Script::set_suspended(bool suspended) {

  if (context != NULL) {

    // notify the timers
    std::list<Timer*>::iterator it;
    for (it = timers.begin(); it != timers.end(); it++) {
      (*it)->set_suspended(suspended);
    }
  }
}

/**
 * @brief Adds a timer to the script.
 * @param timer the timer
 */
void Script::add_timer(Timer *timer) {
  timers.push_back(timer);
}

/**
 * @brief Removes a timer if it exists.
 * @param callback_name name of the timer callback
 */
void Script::remove_timer(const std::string &callback_name) {

  bool found = false;
  std::list<Timer*>::iterator it;

  Timer *timer = NULL;
  for (it = timers.begin(); it != timers.end() && !found; it++) {

    timer = *it;
    if (timer->get_name() == callback_name) {
      delete timer;
      found = true;
    }
  }

  if (found) {
    timers.remove(timer);
  }
}

/**
 * @brief Returns whether a timer just created should be initially suspended.
 * @return true to initially suspend a new timer
 */
bool Script::is_new_timer_suspended(void) {

  if (apis_enabled && GAME_API) {
    // start the timer even if the game is suspended (e.g. a timer started during a camera movement)
    // except when it is suspended because of a dialog box
    return get_game().is_showing_message();
  }

  return false;
}

/**
 * @brief Makes a sprite accessible from the script.
 * @param sprite the sprite to make accessible
 * @return a handle that can be used by scripts to refer to this sprite
 */
int Script::create_sprite_handle(Sprite &sprite) {

  int handle = next_sprite_handle++;
  sprites[handle] = &sprite;
  return handle;
}

/**
 * @brief Returns a sprite handled by this script.
 * @param sprite_handle handle of the sprite to get
 * @return the corresponding sprite
 */
Sprite& Script::get_sprite(int sprite_handle) {

  Debug::assert(sprites.count(sprite_handle) > 0,
    StringConcat() << "No sprite with handle '" << sprite_handle << "'");

  return *sprites[sprite_handle];
}

/**
 * @brief Makes a movement accessible from the script.
 *
 * If the movement is already accessible from this script,
 * this returns the already known handle.
 *
 * @param movement the movement to make accessible
 * @return a handle that can be used by scripts to refer to this movement
 */
int Script::create_movement_handle(Movement &movement) {

  int handle = movement.get_unique_id();
  if (movements.find(handle) == movements.end()) {
    movements[handle] = &movement;
    unassigned_movements[handle] = &movement;
    movement.set_suspended(true); // suspended until it is assigned to an object
  }

  return handle;
}

/**
 * @brief Returns a movement handled by this script.
 * @param movement_handle handle of the movement to get
 * @return the corresponding movement
 */
Movement& Script::get_movement(int movement_handle) {

  Debug::assert(movements.count(movement_handle) > 0,
    StringConcat() << "No movement with handle '" << movement_handle << "'");

  return *movements[movement_handle];
}

/**
 * @brief Starts a movement handled by this script and removes it from the list of unassigned movements.
 *
 * This function is called when the movement is assigned to an object.
 *
 * @param movement_handle handle of the movement
 * @return the corresponding movement
 */
Movement& Script::start_movement(int movement_handle) {

  Movement &movement = get_movement(movement_handle);

  movement.set_suspended(false);
  unassigned_movements.erase(movement_handle);

  return movement;
}
