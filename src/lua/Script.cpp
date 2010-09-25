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
#include "lua/Scripts.h"
#include "Equipment.h"
#include "Savegame.h"
#include "Timer.h"
#include "ResourceManager.h"
#include "lowlevel/Sound.h"
#include "lowlevel/Music.h"
#include "lowlevel/FileTools.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"
#include <lua.hpp>

/**
 * @brief Creates a script.
 * @param scripts the list of all scripts
 */
Script::Script(Scripts &scripts):
  scripts(scripts), context(NULL) {

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
  std::list<Timer*>::iterator it;
  for (it = timers.begin(); it != timers.end(); it++) {
    delete *it;
  }

  // update the script list
  scripts.remove_script(*this);
}

/**
 * @brief Initializes the Lua context and loads the script from a file.
 * @param script_name name of a Lua script file (without extension)
 */
void Script::load(const std::string &script_name) {

  // create an execution context
  context = lua_open();
  luaL_openlibs(context);

  // put a pointer to this Script object in the global area of the Lua context
  lua_pushlightuserdata(context, this);
  lua_setglobal(context, "_cpp_object");

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

  // update the script list
  scripts.add_script(*this);
}

/**
 * @brief Tells the Lua context what C++ functions it can call.
 */
void Script::register_available_functions() {

  lua_register(context, "play_sound", l_play_sound);
  lua_register(context, "play_music", l_play_music);
  lua_register(context, "timer_start", l_timer_start);
  lua_register(context, "timer_stop", l_timer_stop);
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
    lua_getglobal(context, "_cpp_object");
    *script = (Script*) lua_touserdata(context, -1);
    lua_pop(context, 1);
  }
}

/**
 * @brief Calls a function without argument in the script.
 *
 * If the function does not exists in the script, nothing happens:
 * it just means that the function corresponds to an event that
 * the script does not want to handle.
 *
 * @param function_name name of the function to call
 * @return true if the function was called, false if it does not exist
 */
bool Script::notify_script(const std::string &function_name) {

  if (context == NULL) {
    return false;
  }

  lua_getglobal(context, function_name.c_str());
  bool exists = lua_isfunction(context, -1);

  if (exists) {
    lua_call(context, 0, 0);
  }
  else {
    lua_pop(context, -1);
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

  if (context == NULL) {
    return false;
  }

  lua_getglobal(context, function_name.c_str());
  bool exists = lua_isfunction(context, -1);

  if (exists) {
    lua_pushstring(context, arg1.c_str());
    lua_call(context, 1, 0);
  }
  else {
    lua_pop(context, -1);
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

  if (context == NULL) {
    return false;
  }

  lua_getglobal(context, function_name.c_str());
  bool exists = lua_isfunction(context, -1);

  if (exists) {
    lua_pushstring(context, arg1.c_str());
    lua_pushinteger(context, arg2);
    lua_call(context, 2, 0);
  }
  else {
    lua_pop(context, -1);
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

  if (context == NULL) {
    return false;
  }

  lua_getglobal(context, function_name.c_str());
  bool exists = lua_isfunction(context, -1);

  if (exists) {
    lua_pushstring(context, arg1.c_str());
    lua_pushinteger(context, arg2);
    lua_pushinteger(context, arg3);
    lua_call(context, 3, 0);
  }
  else {
    lua_pop(context, -1);
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

  if (context == NULL) {
    return false;
  }

  lua_getglobal(context, function_name.c_str());
  bool exists = lua_isfunction(context, -1);

  if (exists) {
    lua_pushstring(context, arg1.c_str());
    lua_pushstring(context, arg2.c_str());
    lua_pushinteger(context, arg3);
    lua_call(context, 3, 0);
  }
  else {
    lua_pop(context, -1);
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

  if (context == NULL) {
    return false;
  }

  lua_getglobal(context, function_name.c_str());
  bool exists = lua_isfunction(context, -1);

  if (exists) {
    lua_pushinteger(context, arg1);
    lua_pushstring(context, arg2.c_str());
    lua_pushinteger(context, arg3);
    lua_call(context, 3, 0);
  }
  else {
    lua_pop(context, -1);
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

  if (context == NULL) {
    return false;
  }

  lua_getglobal(context, function_name.c_str());
  bool exists = lua_isfunction(context, -1);

  if (exists) {
    lua_pushinteger(context, arg1);
    lua_call(context, 1, 0);
  }
  else {
    lua_pop(context, -1);
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

  if (context == NULL) {
    return false;
  }

  lua_getglobal(context, function_name.c_str());
  bool exists = lua_isfunction(context, -1);

  if (exists) {
    lua_pushinteger(context, arg1);
    lua_pushinteger(context, arg2);
    lua_call(context, 2, 0);
  }
  else {
    lua_pop(context, -1);
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

  if (context == NULL) {
    return false;
  }

  lua_getglobal(context, function_name.c_str());
  bool exists = lua_isfunction(context, -1);

  if (exists) {
    lua_pushboolean(context, arg1);
    lua_call(context, 1, 0);
  }
  else {
    lua_pop(context, -1);
  }

  return exists;
}

/**
 * @brief This function is called when the game is being suspended or resumed.
 * @param suspended true if the game is suspended, false if it is resumed
 */
void Script::set_suspended(bool suspended) {

  if (context != NULL) {

    // notify the timers
    std::list<Timer*>::iterator it;
    for (it = timers.begin(); it != timers.end(); it++) {
      (*it)->set_suspended(suspended);
    }

    // notify the script
    notify_script("event_set_suspended", suspended);
  }
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
      notify_script(timer->get_name());
      delete timer;
      timers.erase(it);
      it = timers.begin();
    }
  }

  // update the script
  notify_script("event_update");
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

// functions that can be called by the Lua script

/**
 * @brief Plays a sound.
 *
 * - Argument 1 (string): name of the sound
 */
int Script::l_play_sound(lua_State *l) {

  Script *script;
  called_by_script(l, 1, &script);
  const SoundId &sound_id = lua_tostring(l, 1);

  ResourceManager::get_sound(sound_id)->play();

  return 0;
}

/**
 * @brief Plays a music.
 * 
 * - Argument 1 (string): name of the music (possibly "none" or "same")
 */
int Script::l_play_music(lua_State *l) {

  Script *script;
  called_by_script(l, 1, &script);
  const MusicId &music_id = lua_tostring(l, 1);

  if (!Music::is_unchanged_id(music_id)) {

    // stop the current music
    Music *current_music = Music::get_current_music();
    if (current_music != NULL) {
      current_music->stop();
    }

    // play the new one
    if (!Music::is_none_id(music_id)) {
      ResourceManager::get_music(music_id)->play();
    }
  }

  return 0;
}

/**
 * @brief Starts a timer to run a Lua function after a delay.
 *
 * - Argument 1 (integer): the timer duration in milliseconds
 * - Argument 2 (string): name of the Lua function to call when the timer is finished
 * (no argument, no return value)
 * - Argument 3 (boolean): plays a sound until the timer expires
 */
int Script::l_timer_start(lua_State *l) {

  Script *script;
  called_by_script(l, 3, &script);
  uint32_t duration = lua_tointeger(l, 1);
  const std::string &callback_name = lua_tostring(l, 2);
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
 */
int Script::l_timer_stop(lua_State *l) {

  Script *script;
  called_by_script(l, 1, &script);
  const std::string &callback_name = lua_tostring(l, 1);

  script->remove_timer(callback_name);

  return 0;
}

