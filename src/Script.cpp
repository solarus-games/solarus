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
#include "Script.h"
#include "Game.h"
#include "Map.h"
#include "Equipment.h"
#include "DialogBox.h"
#include "Treasure.h"
#include "Savegame.h"
#include "Timer.h"
#include "InventoryItem.h"
#include "entities/Hero.h"
#include "lowlevel/Sound.h"
#include "lowlevel/Music.h"
#include "lowlevel/FileTools.h"
#include <lua.hpp>

/**
 * @brief Creates a script.
 * @param game the game
 */
Script::Script(Game *game):
  game(game), context(NULL) {

}

/**
 * @brief Desctructor.
 */
Script::~Script(void) {

  // close the Lua execution context
  if (context != NULL) {
    lua_close(context);
  }

  // delete the timers
  std::list<Timer*>::iterator it;
  for (it = timers.begin(); it != timers.end(); it++) {
    delete *it;
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
}

/**
 * @brief Tells the Lua context what C++ functions it can call.
 */
void Script::register_available_functions(void) {

  lua_register(context, "hero_freeze", l_hero_freeze);
  lua_register(context, "hero_unfreeze", l_hero_unfreeze);
  lua_register(context, "hero_set_pause_enabled", l_hero_set_pause_enabled);
  lua_register(context, "dialog_start", l_dialog_start);
  lua_register(context, "dialog_set_variable", l_dialog_set_variable);
  lua_register(context, "dialog_set_style", l_dialog_set_style);
  lua_register(context, "hud_set_enabled", l_hud_set_enabled);
  lua_register(context, "play_sound", l_play_sound);
  lua_register(context, "play_music", l_play_music);
  lua_register(context, "timer_start", l_timer_start);
  lua_register(context, "timer_stop", l_timer_stop);
  lua_register(context, "camera_move", l_camera_move);
  lua_register(context, "camera_restore", l_camera_restore);
  lua_register(context, "savegame_get_string", l_savegame_get_string);
  lua_register(context, "savegame_get_integer", l_savegame_get_integer);
  lua_register(context, "savegame_get_boolean", l_savegame_get_boolean);
  lua_register(context, "savegame_set_string", l_savegame_set_string);
  lua_register(context, "savegame_set_integer", l_savegame_set_integer);
  lua_register(context, "savegame_set_boolean", l_savegame_set_boolean);
  lua_register(context, "savegame_get_name", l_savegame_get_name);
  lua_register(context, "equipment_get_rupees", l_equipment_get_rupees);
  lua_register(context, "equipment_remove_rupees", l_equipment_remove_rupees);
  lua_register(context, "equipment_get_tunic", l_equipment_get_tunic);
  lua_register(context, "equipment_get_sword", l_equipment_get_sword);
  lua_register(context, "equipment_get_shield", l_equipment_get_shield);
  lua_register(context, "inventory_item_get", l_inventory_item_get);
  lua_register(context, "inventory_item_set", l_inventory_item_set);
  lua_register(context, "inventory_item_get_amount", l_inventory_item_get_amount);
  lua_register(context, "inventory_item_remove_amount", l_inventory_item_remove_amount);
  lua_register(context, "inventory_item_is_bottle", l_inventory_item_is_bottle);
  lua_register(context, "treasure_give", l_treasure_give);
  lua_register(context, "treasure_give_with_amount", l_treasure_give_with_amount);
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
  if (lua_gettop(context) != nb_arguments) {
    DIE("Invalid number of arguments");
  }

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
bool Script::call_script_function(const std::string &function_name) {

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
bool Script::call_script_function(const std::string &function_name, const std::string &arg1) {

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
bool Script::call_script_function(const std::string &function_name,
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
bool Script::call_script_function(const std::string &function_name,
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
 * @param arg1 argument of the function
 * @return true if the function was called, false if it does not exist
 */
bool Script::call_script_function(const std::string &function_name, int arg1) {

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
bool Script::call_script_function(const std::string &function_name, int arg1, int arg2) {

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
bool Script::call_script_function(const std::string &function_name, bool arg1) {

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
    event_set_suspended(suspended);
  }
}

/**
 * @brief Updates the script.
 */
void Script::update(void) {

  // update the timers
  std::list<Timer*>::iterator it;

  for (it = timers.begin(); it != timers.end(); it++) {

    Timer *timer = *it;

    timer->update();
    if (timer->is_finished()) {
      call_script_function(timer->get_name());
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

// functions that can be called by the Lua script

/**
 * @brief Prevents the player from moving until hero_unfreeze() is called.
 */
int Script::l_hero_freeze(lua_State *l) {

  Script *script;
  called_by_script(l, 0, &script);

  script->hero->set_freezed(true);

  return 0;
}

/**
 * @brief Allows the player to move again after a call to hero_freeze().
 */
int Script::l_hero_unfreeze(lua_State *l) {

  Script *script;
  called_by_script(l, 0, &script);

  script->hero->set_freezed(false);

  return 0;
}

/**
 * @brief Sets whether the player can pause the game.
 * - Argument 1 (boolean): true to enable the pause key
 */
int Script::l_hero_set_pause_enabled(lua_State *l) {

  Script *script;
  called_by_script(l, 1, &script);
  bool pause_key_available = lua_toboolean(l, 1) != 0;

  script->game->set_pause_key_available(pause_key_available);

  return 0;
}

/**
 * @brief Creates a dialog box and starts displaying a message.
 *
 * If the message is followed by other messages, they are also
 * displayed.
 * If the message (or one of its next messages) contains a variable,
 * then you have to call dialog_set_variable() to specify its value.
 * 
 * - Argument 1 (string): id of the message to display
 */
int Script::l_dialog_start(lua_State *l) {

  Script *script;
  called_by_script(l, 1, &script);
  const std::string &message_id = lua_tostring(l, 1);

  script->game->get_dialog_box()->start_dialog(message_id);

  return 0;
}

/**
 * @brief Sets the value of the variable in a diabog.
 *
 * The function has to be called after the dialog box is created,
 * i.e. after calling dialog_start().
 * 
 * - Argument 1 (string): id of the message containing the variable
 * - Argument 2 (string): value of the variable
 */
int Script::l_dialog_set_variable(lua_State *l) {

  Script *script;
  called_by_script(l, 2, &script);
  const MessageId &message_id = lua_tostring(l, 1);
  const std::string &value = lua_tostring(l, 2);

  script->game->get_dialog_box()->set_variable(message_id, value);

  return 0;
}

/**
 * @brief Changes the style of the future dialog boxes.
 * 
 * - Argument 1 (integer): the style to set (see the DialogBox::Style enum)
 */
int Script::l_dialog_set_style(lua_State *l) {

  Script *script;
  called_by_script(l, 1, &script);
  int style = lua_tointeger(l, 1);

  script->game->get_dialog_box()->set_style(DialogBox::Style(style));

  return 0;
}

/**
 * @brief Enables or disables the head up display.
 *
 * - Argument 1 (boolean): true to enable it, false to disable it
 */
int Script::l_hud_set_enabled(lua_State *l) {

  Script *script;
  called_by_script(l, 1, &script);
  bool enabled = lua_toboolean(l, 1) != 0;

  script->game->set_hud_enabled(enabled);
  return 0;
}

/**
 * @brief Plays a sound.
 *
 * - Argument 1 (string): name of the sound
 */
int Script::l_play_sound(lua_State *l) {

  Script *script;
  called_by_script(l, 1, &script);
  const SoundId &sound_id = lua_tostring(l, 1);

  script->game->play_sound(sound_id);

  return 0;
}

/**
 * @brief Plays a music.
 * 
 * - Argument 1 (string): name of the music
 */
int Script::l_play_music(lua_State *l) {

  Script *script;
  called_by_script(l, 1, &script);
  const MusicId &music_id = lua_tostring(l, 1);

  script->game->play_music(music_id);

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

  Timer *timer = new Timer(script->game, duration, callback_name, with_sound);
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

/**
 * @brief Moves the camera towards a target point.
 *
 * - Argument 1 (integer): x coordinate of the target point
 * - Argument 2 (integer): y coordinate of the target point
 * - Argument 3 (integer): speed of the camera movement (10 is normal)
 */
int Script::l_camera_move(lua_State *l) {

  Script *script;
  called_by_script(l, 3, &script);
  int x = lua_tointeger(l, 1);
  int y = lua_tointeger(l, 2);
  int speed = lua_tointeger(l, 3);

  script->game->get_current_map()->move_camera(x, y, speed);

  return 0;
}

/**
 * @brief Moves the camera back to the hero.
 */
int Script::l_camera_restore(lua_State *l) {

  Script *script;
  called_by_script(l, 0, &script);

  script->game->get_current_map()->restore_camera();

  return 0;
}

/**
 * @brief Returns a string value saved.
 *
 * - Argument 1 (integer): index of the string value to get (0 to 63)
 * - Return value (string): the string saved at this index
 */
int Script::l_savegame_get_string(lua_State *l) {

  Script *script;
  called_by_script(l, 1, &script);
  int index = lua_tointeger(l, 1);

  const std::string &value = script->game->get_savegame()->get_string(index);
  lua_pushstring(l, value.c_str());

  return 1;
}

/**
 * @brief Returns an integer value saved.
 *
 * - Argument 1 (integer): index of the integer value to get (0 to 2047)
 * - Return value (integer): the integer saved at this index
 */
int Script::l_savegame_get_integer(lua_State *l) {

  Script *script;
  called_by_script(l, 1, &script);
  int index = lua_tointeger(l, 1);

  int value = script->game->get_savegame()->get_integer(index);
  lua_pushinteger(l, value);

  return 1;
}

/**
 * @brief Returns a boolean value saved.
 *
 * - Argument 1 (integer): index of the boolean value to get
 * - Return value (boolean): the boolean saved at this index
 */
int Script::l_savegame_get_boolean(lua_State *l) {

  Script *script;
  called_by_script(l, 1, &script);
  int index = lua_tointeger(l, 1);

  bool value = script->game->get_savegame()->get_boolean(index);
  lua_pushboolean(l, value ? 1 : 0);

  return 1;
}

/**
 * @brief Sets a string value saved.
 *
 * - Argument 1 (integer): index of the string value to set, between 32 and 63
 * (lower indices are writable only by the game engine)
 * - Argument 2 (string): the string value to store at this index
 */
int Script::l_savegame_set_string(lua_State *l) {

  Script *script;
  called_by_script(l, 2, &script);
  int index = lua_tointeger(l, 1);
  const std::string &value = lua_tostring(l, 2);

  if (index < 32) {
    DIE("Cannot change savegame string '" << index << "': string variables below 32 are read-only");
  }

  script->game->get_savegame()->set_string(index, value);

  return 0;
}

/**
 * @brief Sets an integer value saved.
 *
 * - Argument 1 (integer): index of the integer value to set, between 1024 and 2047
 * (lower indices are writable only by the game engine)
 * - Argument 2 (integer): the integer value to store at this index
 */
int Script::l_savegame_set_integer(lua_State *l) {

  Script *script;
  called_by_script(l, 2, &script);
  int index = lua_tointeger(l, 1);
  int value = lua_tointeger(l, 2);

  if (index < 1024) {
    DIE("Cannot change savegame integer '" << index << "': integer variables below 1024 are read-only");
  }

  script->game->get_savegame()->set_integer(index, value);

  return 0;
}

/**
 * @brief Sets a boolean value saved.
 *
 * - Argument 1 (integer): index of the boolean value to set, between 0 and 32767
 * - Argument 2 (boolean): the boolean value to store at this index
 */
int Script::l_savegame_set_boolean(lua_State *l) {

  Script *script;
  called_by_script(l, 2, &script);
  int index = lua_tointeger(l, 1);
  int value = lua_toboolean(l, 2);

  script->game->get_savegame()->set_boolean(index, value != 0);

  return 0;
}

/**
 * @brief Returns a string representing the name of the player.
 * 
 * - Return value (string): the player's name
 */
int Script::l_savegame_get_name(lua_State *l) {

  Script *script;
  called_by_script(l, 0, &script);

  const std::string &name = script->game->get_savegame()->get_string(Savegame::PLAYER_NAME);
  lua_pushstring(l, name.c_str());

  return 1;
}

/**
 * @brief Returns the current number of rupees of the player.
 *
 * - Return value (integer): the number of rupees
 */
int Script::l_equipment_get_rupees(lua_State *l) {

  Script *script;
  called_by_script(l, 0, &script);

  int rupees = script->game->get_equipment()->get_rupees();
  lua_pushinteger(l, rupees);

  return 1;
}

/**
 * @brief Removes some rupees to the player.
 *
 * - Argument 1 (integer): number or rupees to remove
 */
int Script::l_equipment_remove_rupees(lua_State *l) {

  Script *script;
  called_by_script(l, 1, &script);

  int rupees = lua_tointeger(l, 1);

  script->game->get_equipment()->remove_rupees(rupees);

  return 0;
}

/**
 * @brief Returns the tunic of the hero.
 *
 * - Return value (integer): the tunic number (0 to 2)
 */
int Script::l_equipment_get_tunic(lua_State *l) {

  Script *script;
  called_by_script(l, 0, &script);

  int tunic = script->game->get_equipment()->get_tunic();

  lua_pushinteger(l, tunic);

  return 1;
}

/**
 * @brief Returns the sword of the hero.
 *
 * - Return value (integer): the sword number (0 to 4)
 */
int Script::l_equipment_get_sword(lua_State *l) {

  Script *script;
  called_by_script(l, 0, &script);

  int sword = script->game->get_equipment()->get_sword();

  lua_pushinteger(l, sword);

  return 1;
}

/**
 * @brief Returns the shield of the hero.
 *
 * - Return value (integer): the shield number (0 to 3)
 */
int Script::l_equipment_get_shield(lua_State *l) {

  Script *script;
  called_by_script(l, 0, &script);

  int shield = script->game->get_equipment()->get_shield();

  lua_pushinteger(l, shield);

  return 1;
}

/**
 * @brief Returns the possession state of an item from the inventory.
 *
 * - Argument 1 (integer): an inventory item id
 * - Return value (integer): the possession state of this inventory item
 */
int Script::l_inventory_item_get(lua_State *l) {

  Script *script;
  called_by_script(l, 1, &script);

  InventoryItemId item_id = InventoryItemId(lua_tointeger(l, 1));

  int variant = script->game->get_equipment()->has_inventory_item(item_id);
  lua_pushinteger(l, variant);

  return 1;
}

/**
 * @brief Sets the possession state of an item from the inventory
 *
 * - Argument 1 (integer): an inventory item id
 * - Argument 2 (integer): the possession state of this inventory item
 * (a value of 0 removes the inventory item)
 */
int Script::l_inventory_item_set(lua_State *l) {

  Script *script;
  called_by_script(l, 2, &script);

  InventoryItemId item_id = InventoryItemId(lua_tointeger(l, 1));

  int variant = lua_tointeger(l, 2);
  script->game->get_equipment()->give_inventory_item(item_id, variant);
  lua_pushinteger(l, variant);

  return 1;
}


/**
 * @brief Returns the amount the player has for an item from the inventory.
 *
 * - Argument 1 (integer): an inventory item id having an amount (e.g. the bombs)
 * - Return value (integer): the amount possessed
 */
int Script::l_inventory_item_get_amount(lua_State *l) {

  Script *script;
  called_by_script(l, 1, &script);

  InventoryItemId item_id = InventoryItemId(lua_tointeger(l, 1));

  int amount = script->game->get_equipment()->get_inventory_item_amount(item_id);
  lua_pushinteger(l, amount);

  return 1;
}

/**
 * @brief Removes from the inventory an amount of the specified item.
 *
 * - Argument 1 (integer): an inventory item id having an amount (e.g. the bombs)
 * - Argument 2 (integer): the amount possessed
 */
int Script::l_inventory_item_remove_amount(lua_State *l) {

  Script *script;
  called_by_script(l, 2, &script);

  InventoryItemId item_id = InventoryItemId(lua_tointeger(l, 1));

  int amount = lua_tointeger(l, 2);
  script->game->get_equipment()->remove_inventory_item_amount(item_id, amount);

  return 0;
}

/**
 * @brief Returns whether the specified inventory item is corresponds to a bottle.
 *
 * - Argument 1 (integer): an inventory item id
 * - Return value (integer): true if it is a bottle
 */
int Script::l_inventory_item_is_bottle(lua_State *l) {

  called_by_script(l, 1, NULL);

  InventoryItemId item_id = InventoryItemId(lua_tointeger(l, 1));

  bool bottle = InventoryItem::is_bottle(item_id);
  lua_pushboolean(l, bottle ? 1 : 0);

  return 1;
}

/**
 * @brief Gives a treasure to the player.
 *
 * If the treasure comes from a chest, you don't have to call this function:
 * the treasure will be given to the player automatically when he opens the chest.
 * You can use this function to make a non-playing character
 * give a treasure to the player.
 *
 * - Argument 1 (integer): content of the treasure (see Treasure.h)
 * - Argument 2 (integer): index of the savegame boolean variable that stores
 * the possession state of the treasure (or -1 if you don't want to save this treasure)
 */
int Script::l_treasure_give(lua_State *l) {

  Script *script;
  called_by_script(l, 2, &script);
  Treasure::Content content = (Treasure::Content) lua_tointeger(l, 1);
  int savegame_variable = lua_tointeger(l, 2);

  Game *game = script->game;
  game->give_treasure(new Treasure(game, content, savegame_variable));

  return 0;
}

/**
 * @brief Gives a treasure to the player, specifying the amount.
 *
 * This function should be called only for for treasures with an amount, like arrows, apples, etc.,
 * otherwise the amount parameter will be ignored.
 * For example you can use this function to make a non-playing character
 * give a treasure to the player.
 *
 * - Argument 1 (integer): content of the treasure (see Treasure.h)
 * - Argument 2 (integer): amount to give
 * - Argument 3 (integer): index of the savegame boolean variable that stores
 * the possession state of the treasure (or -1 if you don't want to save this treasure)
 */
int Script::l_treasure_give_with_amount(lua_State *l) {

  Script *script;
  called_by_script(l, 3, &script);
  Treasure::Content content = (Treasure::Content) lua_tointeger(l, 1);
  int amount = lua_tointeger(l, 2);
  int savegame_variable = lua_tointeger(l, 3);

  Game *game = script->game;
  game->give_treasure(new Treasure(game, content, amount, savegame_variable));

  return 0;
}

// event functions, i.e. functions called by the C++ engine to notify the map script that something happened

/**
 * @brief The update event is called at each cycle.
 *
 * Implementing this event should be done with care as it may
 * reduce the performances dramatically.
 */
void Script::event_update(void) {
  call_script_function("event_update");
}

/**
 * @brief Notifies the script that the game is being suspended or resumed.
 * @param suspended true if the game is suspended, false if it is resumed
 */
void Script::event_set_suspended(bool suspended) {
  call_script_function("event_set_suspended", suspended);
}

/**
 * @brief Notifies the script that a dialog has just started to be displayed
 * in the dialog box.
 * @param message_id id of the first message in this dialog
 */
void Script::event_dialog_started(const MessageId &message_id) {
  call_script_function("event_dialog_started", message_id);
}

/**
 * @brief Notifies the script that the dialog box has just finished.
 *
 * This function is called when the last message of a dialog is finished.
 * The dialog box has just been closed but the game is still suspended.
 * Note that this event is not called if the dialog was skipped.
 *
 * @param first_message_id id of the first message in the dialog
 * that has just finished
 * @param answer the answer selected by the player: 0 for the first one,
 * 1 for the second one, -1 if there was no question
 */
void Script::event_dialog_finished(const MessageId &first_message_id, int answer) {
  call_script_function("event_dialog_finished", first_message_id, answer);
}

/**
 * @brief Notifies the script that the camera moved by a call to move_camera() has reached its target.
 */
void Script::event_camera_reached_target(void) {
  call_script_function("event_camera_reached_target");
}

/**
 * @brief Notifies the script that the camera moved by a call to restore_camera() has reached the hero.
 */
void Script::event_camera_back(void) {
  call_script_function("event_camera_back");
}

/**
 * @brief Notifies the script that the player is obtaining a treasure.
 *
 * The treasure source does not matter: it can come from a chest,
 * a pickable item or the script.
 *
 * @param content the content obtained
 * @param savegame_variable the boolean variable where this treasure is saved
 * (or -1 if the treasure is not saved)
 */
void Script::event_treasure_obtaining(Treasure::Content content, int savegame_variable) {
  call_script_function("event_treasure_obtaining", content, savegame_variable);
}

/**
 * @brief Notifies the script that the player has just finished obtaining a treasure.
 *
 * The treasure source does not matter: it can come from a chest,
 * a pickable item or the script.
 *
 * @param content the content obtained
 * @param savegame_variable the boolean variable where this treasure is saved
 * (or -1 if the treasure is not saved)
 */
void Script::event_treasure_obtained(Treasure::Content content, int savegame_variable) {
  call_script_function("event_treasure_obtained", content, savegame_variable);
}

