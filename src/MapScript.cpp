#include "MapScript.h"
#include "Map.h"
#include "FileTools.h"
#include "ResourceManager.h"
#include "Sound.h"
#include "ZSDX.h"
#include "Game.h"
#include "DialogBox.h"
#include <iomanip>
#include <lua5.1/lua.hpp>
#include <stdarg.h>

static void check_nb_arguments(lua_State *context, int nb_arguments);

/**
 * Declaration of all functions that can be called by the LUA code.
 */

typedef int (FunctionAvailableToScript) (lua_State *l);

static FunctionAvailableToScript l_start_message;
static FunctionAvailableToScript l_set_message_variable;
static FunctionAvailableToScript l_play_sound;

/**
 * Creates the script of a map.
 * @param map the map
 */
MapScript::MapScript(Map *map) {

  // get the id of the map
  int id = (int) map->get_id();

  // compute the file name, depending on the id
  std::ostringstream oss;
  oss << "maps/map" << std::setfill('0') << std::setw(4) << id << ".lua";
  string file_name = FileTools::data_file_add_prefix(oss.str());

  // create an execution context
  context = lua_open();
  luaL_openlibs(context);

  // register the C functions accessible to the script
  register_c_functions();

  // load the script
  if (luaL_dofile(context, file_name.c_str()) != 0) {
    DIE("Cannot load the script of map " << id << ": " << lua_tostring(context, -1));
  }
}

/**
 * Desctructor.
 */
MapScript::~MapScript(void) {
  lua_close(context);
}

/**
 * Tells the Lua context what C functions it can call.
 */
void MapScript::register_c_functions(void) {

  lua_register(context, "play_sound", l_play_sound);
  lua_register(context, "start_message", l_start_message);
  lua_register(context, "set_message_variable", l_set_message_variable);
}

/**
 * Ensures the C function currently called by the script has the right number of arguments.
 * The program stops if the number of arguments is not correct
 * @param context the Lua context
 * @param nb_arguments the right number of arguments
 */
static void check_nb_arguments(lua_State *context, int nb_arguments) {

  if (lua_gettop(context) != nb_arguments) {
    DIE("Invalid number of arguments");
  }
}

// functions that can be called by the Lua script

/**
 * Creates a dialog box and starts displaying a message.
 * If the message is followed by other messages, they are also
 * displayed, posting a message_started() event each time.
 * If the message (or one of its next messages) contains a variable,
 * then call set_message_variable() to specify its value.
 * Argument 1 (string): id of the message to display
 */
int l_start_message(lua_State *l) {

  check_nb_arguments(l, 1);
  MessageId message_id = lua_tostring(l, 1);

  zsdx->game->show_message(message_id);

  return 1;
}

/**
 * Set the value of the variable in a diabog box message.
 * The function has to be called after the dialog box is created,
 * i.e. after calling show_message.
 * Argument 1 (string): id of the message containing the variable
 * Argument 2 (string): value of the variable
 */
int l_set_message_variable(lua_State *l) {

  check_nb_arguments(l, 2);
  MessageId message_id = lua_tostring(l, 1);
  MessageId value = lua_tostring(l, 2);

  zsdx->game->get_dialog_box()->set_variable(message_id, value);

  return 2;
}

/**
 * Plays a sound.
 * Argument 1 (string): name of the sound
 */
int l_play_sound(lua_State *l) {

  check_nb_arguments(l, 1);
  SoundId sound_id = lua_tostring(l, 1);

  ResourceManager::get_sound(sound_id)->play();

  return 1;
}

/**
 * Calls a function without arguments in the script.
 * If the function does not exists in the script, nothing happens:
 * it just means that the function corresponds to an event that
 * the script does not need to handle.
 * @param function_name name of the function to call
 */
void MapScript::call_lua_function(const char *function_name) {
  call_lua_function(function_name, 0);
}

/**
 * Calls a function with arguments in the script.
 * The arguments must be of type char*.
 * If the function does not exists in the script, nothing happens:
 * it just means that the function corresponds to an event that
 * the script does not want to handle.
 * @param function_name name of the function to call
 */
void MapScript::call_lua_function(const char *function_name, int nb_arguments, ...) {

  lua_getglobal(context, function_name);
  if (lua_isfunction(context, -1)) {

    // the function exists: push the arguments
    va_list arg;
    va_start(arg, nb_arguments);
    for (int i = 0; i < nb_arguments; i++) {
      const char* value = va_arg(arg, char*);
      lua_pushstring(context, value);
    }
    va_end(arg);

    // call the function
    lua_call(context, nb_arguments, 0);
  }
  else {
    lua_pop(context, -1);
  }
}

// event functions, i.e. functions called by the C++ engine to notify the map script of something

void MapScript::event_map_started(void) {
  call_lua_function("map_started");
}

void MapScript::event_message_started(MessageId messageId) {
  call_lua_function("message_started", 1, messageId.c_str());
}
