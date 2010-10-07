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
#include "Equipment.h"
#include "Savegame.h"
#include "Timer.h"
#include "Sprite.h"
#include "lowlevel/Sound.h"
#include "lowlevel/Music.h"
#include "lowlevel/FileTools.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"
#include <lua.hpp>

/**
 * @brief Creates a script.
 */
Script::Script():
  context(NULL) {

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

  // delete the sprites created by this script
  {
    std::list<Sprite*>::iterator it;
    for (it = created_sprites.begin(); it != created_sprites.end(); it++) {
      delete *it;
    }
  }
}

/**
 * @brief Initializes the Lua context and loads the script from a file.
 * @param script_name name of a Lua script file (without extension)
 */
void Script::load(const std::string &script_name) {

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
  register_available_functions();

  // initialize the script
  luaL_dostring(context, "math.randomseed(os.time())");

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

  // load the file
  size_t size;
  char *buffer;
  FileTools::data_file_open_buffer(file_name, &buffer, &size);
  luaL_loadbuffer(context, buffer, size, file_name.c_str());
  FileTools::data_file_close_buffer(buffer);
  lua_call(context, 0, 0);
}

/**
 * @brief Tells the Lua context what C++ functions it can call.
 */
void Script::register_available_functions() {

  static luaL_Reg functions[] = {

    { "play_sound", l_play_sound },
    { "play_music", l_play_music },

    { "timer_start", l_timer_start },
    { "timer_stop", l_timer_stop },

    { "sprite_create", l_sprite_create },
    { "sprite_remove", l_sprite_remove },
    { "sprite_get_animation", l_sprite_get_animation },
    { "sprite_set_animation", l_sprite_set_animation },
    { "sprite_get_direction", l_sprite_get_direction },
    { "sprite_set_direction", l_sprite_set_direction },
    { "sprite_get_frame", l_sprite_get_frame },
    { "sprite_set_frame", l_sprite_set_frame },
    { "sprite_get_frame_delay", l_sprite_get_frame_delay },
    { "sprite_set_frame_delay", l_sprite_set_frame_delay },
    { "sprite_is_paused", l_sprite_is_paused },
    { "sprite_set_paused", l_sprite_set_paused },
    { "sprite_set_animation_ignore_suspend", l_sprite_set_animation_ignore_suspend },
    { "sprite_fade", l_sprite_fade },

    { NULL, NULL }
  };

  luaL_register(context, "sol.main", functions);
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
  Debug::assert(lua_gettop(context) == nb_arguments, "Invalid number of arguments");

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
 * @param name of the function to find
 * (may be prefixed by the name of several Lua tables, typically sol.main.some_function)
 * @return true if the function was found
 */
bool Script::find_lua_function(const std::string &function_name) {

  if (context == NULL) {
    return false;
  }

  // TODO implement a simpler solution to the timer problem
  // (which is: allowing to set sol.main.some_function as a timer callback, even if it is not a global function):
  // store the callback as a lua function object instead of a string, if possible

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
 * @brief Calls a function without argument in the script.
 *
 * If the function does not exists in the script, nothing happens:
 * it just means that the function corresponds to an event that
 * the script does not want to handle.
 *
 * @param function_name name of the function to call
 * (may be prefixed by the name of several Lua tables, typically sol.main.some_function)
 * @return true if the function was called, false if it does not exist
 */
bool Script::notify_script(const std::string &function_name) {

  // find the function and push it onto the stack
  bool exists = find_lua_function(function_name);

  // call the function
  if (exists) {
    lua_call(context, 0, 0);
  }

  return exists;
}

/**
 * @brief Calls a function in the script.
 * @param function_name name of the function to call
 * @param arg1 argument of the function
 * @return true if the function was called, false if it does not exist
 */
bool Script::notify_script(const std::string &function_name, const std::string &arg1) {

  bool exists = find_lua_function(function_name);

  if (exists) {
    lua_pushstring(context, arg1.c_str());
    lua_call(context, 1, 0);
  }

  return exists;
}

/**
 * @brief Calls a function in the script.
 * @param function_name name of the function to call
 * @param arg1 first argument of the function
 * @param arg2 second argument of the function
 * @return true if the function was called, false if it does not exist
 */
bool Script::notify_script(const std::string &function_name,
				  const std::string &arg1, int arg2) {

  bool exists = find_lua_function(function_name);

  if (exists) {
    lua_pushstring(context, arg1.c_str());
    lua_pushinteger(context, arg2);
    lua_call(context, 2, 0);
  }

  return exists;
}

/**
 * @brief Calls a function in the script.
 * @param function_name name of the function to call
 * @param arg1 first argument of the function
 * @param arg2 second argument of the function
 * @param arg3 third argument of the function
 * @return true if the function was called, false if it does not exist
 */
bool Script::notify_script(const std::string &function_name,
				  const std::string &arg1, int arg2, int arg3) {

  bool exists = find_lua_function(function_name);

  if (exists) {
    lua_pushstring(context, arg1.c_str());
    lua_pushinteger(context, arg2);
    lua_pushinteger(context, arg3);
    lua_call(context, 3, 0);
  }

  return exists;
}

/**
 * @brief Calls a function in the script.
 * @param function_name name of the function to call
 * @param arg1 first argument of the function
 * @param arg2 second argument of the function
 * @param arg3 third argument of the function
 * @return true if the function was called, false if it does not exist
 */
bool Script::notify_script(const std::string &function_name,
				  const std::string &arg1, const std::string &arg2, int arg3) {

  bool exists = find_lua_function(function_name);

  if (exists) {
    lua_pushstring(context, arg1.c_str());
    lua_pushstring(context, arg2.c_str());
    lua_pushinteger(context, arg3);
    lua_call(context, 3, 0);
  }

  return exists;
}

/**
 * @brief Calls a function in the script.
 * @param function_name name of the function to call
 * @param arg1 first argument of the function
 * @param arg2 second argument of the function
 * @param arg3 third argument of the function
 * @return true if the function was called, false if it does not exist
 */
bool Script::notify_script(const std::string &function_name,
				  int arg1, const std::string &arg2, int arg3) {

  bool exists = find_lua_function(function_name);

  if (exists) {
    lua_pushinteger(context, arg1);
    lua_pushstring(context, arg2.c_str());
    lua_pushinteger(context, arg3);
    lua_call(context, 3, 0);
  }

  return exists;
}

/**
 * @brief Calls a function in the script.
 * @param function_name name of the function to call
 * @param arg1 argument of the function
 * @return true if the function was called, false if it does not exist
 */
bool Script::notify_script(const std::string &function_name, int arg1) {

  bool exists = find_lua_function(function_name);

  if (exists) {
    lua_pushinteger(context, arg1);
    lua_call(context, 1, 0);
  }

  return exists;
}

/**
 * @brief Calls a function in the script.
 * @param function_name name of the function to call
 * @param arg1 first argument of the function
 * @param arg2 second argument of the function
 * @return true if the function was called, false if it does not exist
 */
bool Script::notify_script(const std::string &function_name, int arg1, int arg2) {

  bool exists = find_lua_function(function_name);

  if (exists) {
    lua_pushinteger(context, arg1);
    lua_pushinteger(context, arg2);
    lua_call(context, 2, 0);
  }

  return exists;
}

/**
 * @brief Calls a function in the script.
 * @param function_name name of the function to call
 * @param arg1 argument of the function
 * @return true if the function was called, false if it does not exist
 */
bool Script::notify_script(const std::string &function_name, bool arg1) {

  bool exists = find_lua_function(function_name);

  if (exists) {
    lua_pushboolean(context, arg1);
    lua_call(context, 1, 0);
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

  // update the script
  event_update();
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
  return false;
}

/**
 * @brief Creates a new sprite that will be accessible from the script.
 * @param sprite_id a name that will identify the sprite from the script
 * @param animation_set_id animation set of the sprite to create
 */
void Script::create_sprite(const std::string &sprite_id, const SpriteAnimationSetId &animation_set_id) {

  Sprite *sprite = new Sprite(animation_set_id);
  add_existing_sprite(sprite_id, *sprite);
  created_sprites.push_back(sprite);
}

/**
 * @brief Makes an existing sprite accessible from the script.
 * @param sprite_id a name that will identify the sprite from the script
 * @param sprite the existing sprite to associate to sprite_id
 */
void Script::add_existing_sprite(const std::string &sprite_id, Sprite &sprite) {

  Debug::assert(sprites.count(sprite_id) == 0,
    StringConcat() << "This script already has a sprite with id '" << sprite_id << "'");

  sprites[sprite_id] = &sprite;
}

/**
 * @brief Returns a sprite handled by this script.
 * @param sprite_id id of the sprite to get
 * @return the corresponding sprite
 */
Sprite& Script::get_sprite(const std::string &sprite_id) {

  Debug::assert(sprites.count(sprite_id) > 0,
    StringConcat() << "No sprite with id '" << sprite_id << "'");

  return *sprites[sprite_id];
}

// functions that can be called by the Lua script

/**
 * @brief Plays a sound.
 *
 * - Argument 1 (string): name of the sound
 *
 * @param l the Lua context that is calling this function
 */
int Script::l_play_sound(lua_State *l) {

  Script *script;
  called_by_script(l, 1, &script);
  const SoundId &sound_id = luaL_checkstring(l, 1);

  Sound::play(sound_id);

  return 0;
}

/**
 * @brief Plays a music.
 * 
 * - Argument 1 (string): name of the music (possibly "none" or "same")
 *
 * @param l the Lua context that is calling this function
 */
int Script::l_play_music(lua_State *l) {

  Script *script;
  called_by_script(l, 1, &script);
  const MusicId &music_id = luaL_checkstring(l, 1);
  Music::play(music_id);

  return 0;
}

/**
 * @brief Starts a timer to run a Lua function after a delay.
 *
 * - Argument 1 (integer): the timer duration in milliseconds
 * - Argument 2 (string): name of the Lua function to call when the timer is finished
 * (no argument, no return value)
 * - Argument 3 (boolean): plays a sound until the timer expires
 *
 * @param l the Lua context that is calling this function
 */
int Script::l_timer_start(lua_State *l) {

  Script *script;
  called_by_script(l, 3, &script);
  uint32_t duration = luaL_checkinteger(l, 1);
  const std::string &callback_name = luaL_checkstring(l, 2);
  bool with_sound = lua_toboolean(l, 3) != 0;

  Timer *timer = new Timer(duration, callback_name, with_sound);
  if (script->is_new_timer_suspended()) {
    timer->set_suspended(true);
  }
  script->add_timer(timer);

  return 0;
}

/**
 * @brief Stops an existing timer.
 *
 * - Argument 1 (string): name of the Lua function that is supposed to be called
 * when the timer is finished
 *
 * @param l the Lua context that is calling this function
 */
int Script::l_timer_stop(lua_State *l) {

  Script *script;
  called_by_script(l, 1, &script);
  const std::string &callback_name = luaL_checkstring(l, 1);

  script->remove_timer(callback_name);

  return 0;
}

/**
 * @brief Creates a sprite that will be stored by the script.
 *
 * The sprite created will be accessible only from this script.
 * No other sprite will the same id should exist in this script.
 * You can destroy your sprite later with sol.main.sprite_remove().
 * If you don't, it will be destroyed automatically when the script is destroyed.
 * - Argument 1 (string): a name to identify the created sprite later
 * - Argument 2 (string): animation set to use
 *
 * @param l the Lua context that is calling this function
 */
int Script::l_sprite_create(lua_State *l) {

  Script *script;
  called_by_script(l, 2, &script);
  const std::string &sprite_id = luaL_checkstring(l, 1);
  const std::string &animation_set_id = luaL_checkstring(l, 2);

  script->create_sprite(sprite_id, animation_set_id);

  return 0;
}

/**
 * @brief Destroys a sprite previously created inside this script with sol.main.sprite_create().
 *
 * Sprites handled by this script but created elsewhere (e.g. an NPC's sprite)
 * must never be destroyed here. Call this function only for sprites that were created
 * by calling sol.main.sprite_create().
 * - Argument 1 (string): id of the sprite to destroy
 *
 * @param l the Lua context that is calling this function
 */
int Script::l_sprite_remove(lua_State *l) {

  Script *script;
  called_by_script(l, 1, &script);
  const std::string &sprite_id = luaL_checkstring(l, 1);

  Sprite *sprite = &script->get_sprite(sprite_id);

  script->created_sprites.remove(sprite);
  script->sprites.erase(sprite_id);
  delete sprite;

  return 0;
}

/**
 * @brief Returns the current animation of a sprite.
 * 
 * - Argument 1 (string): id of the sprite
 * - Return value (string): name of the current animation
 *
 * @param l the Lua context that is calling this function
 */
int Script::l_sprite_get_animation(lua_State *l) {

  Script *script;
  called_by_script(l, 1, &script);
  const std::string &sprite_id = luaL_checkstring(l, 1);

  const Sprite &sprite = script->get_sprite(sprite_id);
  const std::string animation_name = sprite.get_current_animation();
  lua_pushstring(l, animation_name.c_str());

  return 1;
}

/**
 * @brief Sets the current animation of a sprite.
 * 
 * - Argument 1 (string): id of the sprite
 * - Argument 2 (string): name of the animation to set
 *
 * @param l the Lua context that is calling this function
 */
int Script::l_sprite_set_animation(lua_State *l) {

  Script *script;
  called_by_script(l, 2, &script);
  const std::string &sprite_id = luaL_checkstring(l, 1);
  const std::string &animation_name = luaL_checkstring(l, 2);

  Sprite &sprite = script->get_sprite(sprite_id);
  sprite.set_current_animation(animation_name);
  sprite.restart_animation();

  return 0;
}

/**
 * @brief Returns the current direction of a sprite.
 * 
 * - Argument 1 (string): id of the sprite
 * - Return value (integer): direction of the sprite
 *
 * @param l the Lua context that is calling this function
 */
int Script::l_sprite_get_direction(lua_State *l) {

  Script *script;
  called_by_script(l, 1, &script);
  const std::string &sprite_id = luaL_checkstring(l, 1);

  const Sprite &sprite = script->get_sprite(sprite_id);
  lua_pushinteger(l, sprite.get_current_direction());

  return 1;
}

/**
 * @brief Sets the current direction of a sprite.
 * 
 * - Argument 1 (string): id of the sprite
 * - Argument 2 (integer): direction to set
 *
 * @param l the Lua context that is calling this function
 */
int Script::l_sprite_set_direction(lua_State *l) {

  Script *script;
  called_by_script(l, 2, &script);
  const std::string &sprite_id = luaL_checkstring(l, 1);
  int direction = luaL_checkinteger(l, 2);

  Sprite &sprite = script->get_sprite(sprite_id);
  sprite.set_current_direction(direction);

  return 0;
}

/**
 * @brief Returns the current frame of a sprite.
 * 
 * - Argument 1 (string): id of the sprite
 * - Return value (integer): index of the current frame of the animation
 *
 * @param l the Lua context that is calling this function
 */
int Script::l_sprite_get_frame(lua_State *l) {

  Script *script;
  called_by_script(l, 1, &script);
  const std::string &sprite_id = luaL_checkstring(l, 1);

  const Sprite &sprite = script->get_sprite(sprite_id);
  lua_pushinteger(l, sprite.get_current_frame());

  return 1;
}

/**
 * @brief Sets the current frame of a sprite.
 * 
 * - Argument 1 (string): id of the sprite
 * - Argument 2 (integer): index of the frame to set in the animation
 *
 * @param l the Lua context that is calling this function
 */
int Script::l_sprite_set_frame(lua_State *l) {

  Script *script;
  called_by_script(l, 2, &script);
  const std::string &sprite_id = luaL_checkstring(l, 1);
  int frame = luaL_checkinteger(l, 2);

  Sprite &sprite = script->get_sprite(sprite_id);
  sprite.set_current_frame(frame);

  return 0;
}

/**
 * @brief Returns the delay between two frames of the animation of a sprite.
 * 
 * - Argument 1 (string): id of the sprite
 * - Return value: the delay between two frames in milliseconds
 *
 * @param l the Lua context that is calling this function
 */
int Script::l_sprite_get_frame_delay(lua_State *l) {

  Script *script;
  called_by_script(l, 1, &script);
  const std::string &sprite_id = luaL_checkstring(l, 1);

  const Sprite &sprite = script->get_sprite(sprite_id);
  lua_pushinteger(l, sprite.get_frame_delay());

  return 1;
}

/**
 * @brief Sets the delay between two frames of the animation of a sprite.
 * 
 * - Argument 1 (string): id of the sprite
 * - Argument 2 (integer): the new delay in milliseconds
 *
 * @param l the Lua context that is calling this function
 */
int Script::l_sprite_set_frame_delay(lua_State *l) {

  Script *script;
  called_by_script(l, 2, &script);
  const std::string &sprite_id = luaL_checkstring(l, 1);
  uint32_t delay = luaL_checkinteger(l, 2);

  Sprite &sprite = script->get_sprite(sprite_id);
  sprite.set_frame_delay(delay);

  return 0;
}

/**
 * @brief Returns whether the animation of a sprite is paused.
 * 
 * - Argument 1 (string): id of the sprite
 * - Return value (boolean): true if the animation is currently paused
 *
 * @param l the Lua context that is calling this function
 */
int Script::l_sprite_is_paused(lua_State *l) {

  Script *script;
  called_by_script(l, 1, &script);
  const std::string &sprite_id = luaL_checkstring(l, 1);

  Sprite &sprite = script->get_sprite(sprite_id);
  lua_pushboolean(l, sprite.is_paused() ? 1 : 0);

  return 1;
}

/**
 * @brief Pauses or resumes the animation of a sprite.
 * 
 * - Argument 1 (string): id of the sprite
 * - Argument 2 (boolean): true to pause the animation, false to resume it
 *
 * @param l the Lua context that is calling this function
 */
int Script::l_sprite_set_paused(lua_State *l) {

  Script *script;
  called_by_script(l, 2, &script);
  const std::string &sprite_id = luaL_checkstring(l, 1);
  bool paused = lua_toboolean(l, 2) != 0;

  Sprite &sprite = script->get_sprite(sprite_id);
  sprite.set_paused(paused);

  return 0;
}

/**
 * @brief Sets whether the animation of a sprite should continue even when
 * the sprite receives a set_suspended(true) call.
 *
 * - Argument 1 (string): id of the sprite
 * - Argument 2 (boolean): true to continue the animation when the sprite is suspended
 *
 * @param l the Lua context that is calling this function
 */
int Script::l_sprite_set_animation_ignore_suspend(lua_State *l) {

  Script *script;
  called_by_script(l, 1, &script);
  const std::string &sprite_id = luaL_checkstring(l, 1);
  bool ignore_suspend = lua_toboolean(l, 2) != 0;

  Sprite &sprite = script->get_sprite(sprite_id);
  sprite.set_ignore_suspend(ignore_suspend);

  return 0;
}

/**
 * @brief Starts a fade-in or a fade-out effect on a sprite.
 * 
 * - Argument 1 (string): id of the sprite
 * - Argument 2 (integer): direction of the effect: 0 for fade-in, 1 for fade-out
 *
 * @param l the Lua context that is calling this function
 */
int Script::l_sprite_fade(lua_State *l) {

  Script *script;
  called_by_script(l, 2, &script);
  const std::string &sprite_id = luaL_checkstring(l, 1);
  int direction = luaL_checkinteger(l, 2);

  Sprite &sprite = script->get_sprite(sprite_id);
  sprite.start_fading(direction);

  return 0;
}

// event functions, i.e. calling Lua from C++

/**
 * @brief Notifies the script that it can update itself.
 *
 * This function is called at each cycle of the main loop,
 * so if you define it in your script, take care of the performances.
 */
void Script::event_update() {

  notify_script("event_update");
}


