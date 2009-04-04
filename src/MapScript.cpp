#include "MapScript.h"
#include "Map.h"
#include "FileTools.h"
#include "ResourceManager.h"
#include "Sound.h"
#include "ZSDX.h"
#include "Game.h"
#include "Equipment.h"
#include "DialogBox.h"
#include "Treasure.h"
#include "Savegame.h"
#include "Timer.h"
#include "Sprite.h"
#include "entities/Detector.h"
#include "entities/MapEntities.h"
#include "entities/InteractiveEntity.h"
#include "entities/Hero.h"
#include "entities/Chest.h"
#include "entities/DynamicTile.h"
#include "entities/Switch.h"
#include <iomanip>
#include <lua5.1/lua.hpp>
#include <stdarg.h>
using namespace std;

/**
 * Creates the script of a map.
 * @param map the map
 */
MapScript::MapScript(Map *map):
  map(map), context(NULL) {

}

/**
 * Desctructor.
 */
MapScript::~MapScript(void) {

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
 * Tells the Lua context what C functions it can call.
 */
void MapScript::register_c_functions(void) {

  lua_register(context, "freeze", l_freeze);
  lua_register(context, "unfreeze", l_unfreeze);
  lua_register(context, "play_sound", l_play_sound);
  lua_register(context, "start_message", l_start_message);
  lua_register(context, "set_message_variable", l_set_message_variable);
  lua_register(context, "give_treasure", l_give_treasure);
  lua_register(context, "give_treasure_with_amount", l_give_treasure_with_amount);
  lua_register(context, "savegame_get_string", l_savegame_get_string);
  lua_register(context, "savegame_get_integer", l_savegame_get_integer);
  lua_register(context, "savegame_get_boolean", l_savegame_get_boolean);
  lua_register(context, "savegame_set_string", l_savegame_set_string);
  lua_register(context, "savegame_set_integer", l_savegame_set_integer);
  lua_register(context, "savegame_set_boolean", l_savegame_set_boolean);
  lua_register(context, "start_timer", l_start_timer);
  lua_register(context, "stop_timer", l_stop_timer);
  lua_register(context, "move_camera", l_move_camera);
  lua_register(context, "restore_camera", l_restore_camera);
  lua_register(context, "npc_walk", l_npc_walk);
  lua_register(context, "npc_random_walk", l_npc_random_walk);
  lua_register(context, "npc_jump", l_npc_jump);
  lua_register(context, "npc_set_direction", l_npc_set_direction);
  lua_register(context, "npc_remove", l_npc_remove);
  lua_register(context, "set_chest_open", l_set_chest_open);
  lua_register(context, "get_rupees", l_get_rupees);
  lua_register(context, "remove_rupees", l_remove_rupees);
  lua_register(context, "disable_tile", l_disable_tile);
  lua_register(context, "enable_tile", l_enable_tile);
  lua_register(context, "is_tile_enabled", l_is_tile_enabled);
  lua_register(context, "reset_block", l_reset_block);
  lua_register(context, "reset_blocks", l_reset_blocks);
  lua_register(context, "interactive_entity_get_animation", l_interactive_entity_get_animation);
  lua_register(context, "interactive_entity_get_animation_delay", l_interactive_entity_get_animation_delay);
  lua_register(context, "interactive_entity_get_animation_frame", l_interactive_entity_get_animation_frame);
  lua_register(context, "interactive_entity_is_animation_paused", l_interactive_entity_is_animation_paused);
  lua_register(context, "interactive_entity_set_animation", l_interactive_entity_set_animation);
  lua_register(context, "interactive_entity_set_animation_delay", l_interactive_entity_set_animation_delay);
  lua_register(context, "interactive_entity_set_animation_frame", l_interactive_entity_set_animation_frame);
  lua_register(context, "interactive_entity_set_animation_paused", l_interactive_entity_set_animation_paused);
  lua_register(context, "equipment_get_tunic", l_equipment_get_tunic);
  lua_register(context, "equipment_get_sword", l_equipment_get_sword);
  lua_register(context, "equipment_get_shield", l_equipment_get_shield);
}

/**
 * Ensures the C function currently called by the script has the right number of arguments.
 * The program stops if the number of arguments is not correct
 * @param context the Lua context
 * @param nb_arguments the right number of arguments
 */
void MapScript::check_nb_arguments(lua_State *context, int nb_arguments) {

  if (lua_gettop(context) != nb_arguments) {
    DIE("Invalid number of arguments");
  }
}

/**
 * Loads the script and initializes it.
 */
void MapScript::initialize(void) {

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

  // initialize the script
  luaL_dostring(context, "math.randomseed(os.time())");

  // load the script
  if (luaL_dofile(context, file_name.c_str()) != 0) {
    DIE("Cannot load the script of map " << id << ": " << lua_tostring(context, -1));
  }

  event_map_started();
}

/**
 * Calls a function without arguments in the script.
 * If the function does not exists in the script, nothing happens:
 * it just means that the function corresponds to an event that
 * the script does not need to handle.
 * @param function_name name of the function to call
 * @return true if the function was called, false if it does not exist
 */
bool MapScript::call_lua_function(const char *function_name) {
  return call_lua_function(function_name, 0);
}

/**
 * Calls a function with arguments in the script.
 * The arguments must be of type char*.
 * If the function does not exists in the script, nothing happens:
 * it just means that the function corresponds to an event that
 * the script does not want to handle.
 * @param function_name name of the function to call
 * @return true if the function was called, false if it does not exist
 */
bool MapScript::call_lua_function(const char *function_name, int nb_arguments, ...) {

  lua_getglobal(context, function_name);

  bool exists = lua_isfunction(context, -1);

  if (exists) {

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

  return exists;
}

/**
 * This function is called when the game is being suspended
 * or resumed.
 * @param suspended true if the game is suspended, false if it is resumed
 */
void MapScript::set_suspended(bool suspended) {

  if (context != NULL) {
  
    // suspend or resume the timers
    std::list<Timer*>::iterator it;
    for (it = timers.begin(); it != timers.end(); it++) {
      (*it)->set_suspended(suspended);
    }

    // notify the script
    call_lua_function("event_set_suspended", 1, suspended ? "1" : "0");  
  }
}

/**
 * Updates the script.
 */
void MapScript::update(void) {

  // update the timers
  std::list<Timer*>::iterator it;

  for (it = timers.begin(); it != timers.end(); it++) {

    Timer *timer = *it;

    timer->update();
    if (timer->is_finished()) {
      call_lua_function(timer->get_callback_name());
      delete timer;
      timers.erase(it);
      it = timers.begin();
    }
  }

  // update the script
  call_lua_function("event_update");
}

/**
 * Adds a timer to the script.
 * @param timer the timer
 */
void MapScript::add_timer(Timer *timer) {
  timers.push_back(timer);
}

/**
 * Removes a timer if it exists.
 * @param callback_name name of the timer callback
 */
void MapScript::remove_timer(string callback_name) {

  bool found = false;
  std::list<Timer*>::iterator it;

  Timer *timer = NULL;
  for (it = timers.begin(); it != timers.end() && !found; it++) {

    timer = *it;
    if (timer->get_callback_name() == callback_name) {
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
 * Prevents the player from moving until unfreeze() is called.
 */
int MapScript::l_freeze(lua_State *l) {

  check_nb_arguments(l, 0);

  zsdx->game->get_hero()->freeze();

  return 0;
}

/**
 * Allows the player to move again after a freeze() call.
 */
int MapScript::l_unfreeze(lua_State *l) {

  check_nb_arguments(l, 0);

  zsdx->game->get_hero()->unfreeze();

  return 0;
}

/**
 * Creates a dialog box and starts displaying a message.
 * If the message is followed by other messages, they are also
 * displayed, posting a message_started() event each time.
 * If the message (or one of its next messages) contains a variable,
 * then call set_message_variable() to specify its value.
 * Argument 1 (string): id of the message to display
 */
int MapScript::l_start_message(lua_State *l) {

  check_nb_arguments(l, 1);
  MessageId message_id = lua_tostring(l, 1);

  zsdx->game->show_message(message_id);

  return 0;
}

/**
 * Set the value of the variable in a diabog box message.
 * The function has to be called after the dialog box is created,
 * i.e. after calling show_message.
 * Argument 1 (string): id of the message containing the variable
 * Argument 2 (string): value of the variable
 */
int MapScript::l_set_message_variable(lua_State *l) {

  check_nb_arguments(l, 2);
  MessageId message_id = lua_tostring(l, 1);
  MessageId value = lua_tostring(l, 2);

  zsdx->game->get_dialog_box()->set_variable(message_id, value);

  return 0;
}

/**
 * Plays a sound.
 * Argument 1 (string): name of the sound
 */
int MapScript::l_play_sound(lua_State *l) {

  check_nb_arguments(l, 1);
  SoundId sound_id = lua_tostring(l, 1);

  ResourceManager::get_sound(sound_id)->play();

  return 0;
}

/**
 * Gives a treasure to the player.
 * If the treasure comes from a chest, you don't have to call this function:
 * the treasure will be given to the player automatically when he opens the chest.
 * You can use this function to make a non-playing character
 * give a treasure to the player.
 * Argument 1 (integer): content of the treasure (see Treasure.h)
 * Argument 2 (integer): index of the savegame boolean variable that stores
 * the possession state of the treasure (or -1 if you don't want to save this treasure)
 */
int MapScript::l_give_treasure(lua_State *l) {

  check_nb_arguments(l, 2);
  Treasure::Content content = (Treasure::Content) lua_tointeger(l, 1);
  int savegame_variable = lua_tointeger(l, 2);

  zsdx->game->give_treasure(new Treasure(content, savegame_variable));

  return 0;
}

/**
 * Gives a treasure to the player, specifying the amount.
 * This function should be called only for for treasures with an amount, like arrows, apples, etc.,
 * otherwise the amount parameter will be ignored.
 * For example you can use this function to make a non-playing character
 * give a treasure to the player.
 * Argument 1 (integer): content of the treasure (see Treasure.h)
 * Argument 2 (integer): amount to give
 * Argument 3 (integer): index of the savegame boolean variable that stores
 * the possession state of the treasure (or -1 if you don't want to save this treasure)
 */
int MapScript::l_give_treasure_with_amount(lua_State *l) {

  check_nb_arguments(l, 3);
  Treasure::Content content = (Treasure::Content) lua_tointeger(l, 1);
  int amount = lua_tointeger(l, 2);
  int savegame_variable = lua_tointeger(l, 3);

  zsdx->game->give_treasure(new Treasure(content, amount, savegame_variable));

  return 0;
}

/**
 * Returns a string value saved.
 * Argument 1 (integer): index of the string value to get (0 to 63)
 * Return value (string): the string saved at this index
 */
int MapScript::l_savegame_get_string(lua_State *l) {

  check_nb_arguments(l, 1);
  int index = lua_tointeger(l, 1);

  string value = zsdx->game->get_savegame()->get_string(index);
  lua_pushstring(l, value.c_str());

  return 1;
}

/**
 * Returns an integer value saved.
 * Argument 1 (integer): index of the integer value to get (0 to 2047)
 * Return value (integer): the integer saved at this index
 */
int MapScript::l_savegame_get_integer(lua_State *l) {

  check_nb_arguments(l, 1);
  int index = lua_tointeger(l, 1);

  int value = zsdx->game->get_savegame()->get_integer(index);
  lua_pushinteger(l, value);

  return 1;
}

/**
 * Returns a boolean value saved.
 * Argument 1 (integer): index of the boolean value to get
 * Return value (boolean): the boolean saved at this index
 */
int MapScript::l_savegame_get_boolean(lua_State *l) {

  check_nb_arguments(l, 1);
  int index = lua_tointeger(l, 1);

  bool value = zsdx->game->get_savegame()->get_boolean(index);
  lua_pushboolean(l, value ? 1 : 0);

  return 1;
}

/**
 * Sets a string value saved.
 * Argument 1 (integer): index of the string value to set, between 32 and 63
 * (lower indices are writable only by the game engine)
 * Argument 2 (string): the string value to store at this index
 */
int MapScript::l_savegame_set_string(lua_State *l) {

  check_nb_arguments(l, 2);
  int index = lua_tointeger(l, 1);
  const char *value = lua_tostring(l, 2);

  if (index < 32) {
    DIE("Cannot change savegame string '" << index << "': string variables below 32 are read-only");
  }

  zsdx->game->get_savegame()->set_string(index, value);

  return 0;
}

/**
 * Sets an integer value saved.
 * Argument 1 (integer): index of the integer value to set, between 1024 and 2047
 * (lower indices are writable only by the game engine)
 * Argument 2 (integer): the integer value to store at this index
 */
int MapScript::l_savegame_set_integer(lua_State *l) {

  check_nb_arguments(l, 2);
  int index = lua_tointeger(l, 1);
  int value = lua_tointeger(l, 2);

  if (index < 1024) {
    DIE("Cannot change savegame integer '" << index << "': integer variables below 1024 are read-only");
  }

  zsdx->game->get_savegame()->set_integer(index, value);

  return 0;
}

/**
 * Sets a boolean value saved.
 * Argument 1 (integer): index of the boolean value to set, between 0 and 32767
 * Argument 2 (boolean): the boolean value to store at this index
 */
int MapScript::l_savegame_set_boolean(lua_State *l) {

  check_nb_arguments(l, 2);
  int index = lua_tointeger(l, 1);
  int value = lua_toboolean(l, 2);

  zsdx->game->get_savegame()->set_boolean(index, value != 0);

  return 0;
}

/**
 * Starts a timer to run a Lua function after a delay.
 * Argument 1 (integer): the timer duration in milliseconds
 * Argument 2 (string): name of the Lua function to call when the timer is finished
 * (no argument, no return value)
 * Argument 3 (boolean): plays a sound until the timer expires
 */
int MapScript::l_start_timer(lua_State *l) {

  check_nb_arguments(l, 3);
  Uint32 duration = lua_tointeger(l, 1);
  const char *callback_name = lua_tostring(l, 2);
  bool with_sound = lua_toboolean(l, 3) != 0;

  Timer *timer = new Timer(duration, callback_name, with_sound);
  MapScript *script = zsdx->game->get_current_script();
  script->add_timer(timer);

  return 0;
}

/**
 * Stops an existing timer.
 * Argument 1 (string): name of the Lua function that is supposed to be called
 * when the timer is finished
 */
int MapScript::l_stop_timer(lua_State *l) {

  check_nb_arguments(l, 1);
  const char *callback_name = lua_tostring(l, 1);

  MapScript *script = zsdx->game->get_current_script();
  script->remove_timer(callback_name);

  return 0;
}

/**
 * Moves the camera towards a target point.
 * Argument 1 (integer): x coordinate of the target point
 * Argument 2 (integer): y coordinate of the target point
 * Argument 3 (integer): speed of the camera movement (10 is normal)
 */
int MapScript::l_move_camera(lua_State *l) {

  check_nb_arguments(l, 3);
  int x = lua_tointeger(l, 1);
  int y = lua_tointeger(l, 2);
  int speed = lua_tointeger(l, 3);

  Map *map = zsdx->game->get_current_map();
  map->move_camera(x, y, speed);

  return 0;
}

/**
 * Moves the camera back to the hero.
 */
int MapScript::l_restore_camera(lua_State *l) {
  
  check_nb_arguments(l, 0);

  Map *map = zsdx->game->get_current_map();
  map->restore_camera();

  return 0;
}

/**
 * Makes an NPC walk with respect to a path.
 * Argument 1 (string): name of the NPC to make move
 * Argument 2 (string): the path (each character is a direction between '0' and '7'
 * Argument 3 (boolean): true to make the movement loop
 * Argument 4 (boolean): true to enable the collisions
 */
int MapScript::l_npc_walk(lua_State *l) {

  check_nb_arguments(l, 4);

  string name = lua_tostring(l, 1);
  string path = lua_tostring(l, 2);
  bool loop = lua_toboolean(l, 3) != 0;
  bool with_collisions = lua_toboolean(l, 4) != 0;

  Map *map = zsdx->game->get_current_map();
  InteractiveEntity *npc = (InteractiveEntity*) map->get_entities()->get_entity(MapEntity::INTERACTIVE_ENTITY, name);
  npc->walk(path, loop, with_collisions);

  return 0;
}

/**
 * Makes an NPC walk randomly.
 * Argument 1 (string): name of the NPC to make move
 */
int MapScript::l_npc_random_walk(lua_State *l) {

  check_nb_arguments(l, 1);

  string name = lua_tostring(l, 1);

  Map *map = zsdx->game->get_current_map();
  InteractiveEntity *npc = (InteractiveEntity*) map->get_entities()->get_entity(MapEntity::INTERACTIVE_ENTITY, name);
  npc->walk_random();

  return 0;
}

/**
 * Makes an NPC jump into a direction.
 * Argument 1 (string): name of the NPC to make move
 * Argument 2 (integer): the jump direction, between 0 and 7
 * Argument 3 (integer): the jump length in pixels
 * Argument 4 (boolean): true to enable the collisions
 */
int MapScript::l_npc_jump(lua_State *l) {

  check_nb_arguments(l, 4);

  string name = lua_tostring(l, 1);
  int direction = lua_tointeger(l, 2);
  int length = lua_tointeger(l, 3);
  bool with_collisions = lua_toboolean(l, 4) != 0;

  Map *map = zsdx->game->get_current_map();
  InteractiveEntity *npc = (InteractiveEntity*) map->get_entities()->get_entity(MapEntity::INTERACTIVE_ENTITY, name);
  npc->jump(direction, length, with_collisions);

  return 0;
}

/**
 * Sets the direction of an NPC's sprite.
 * Argument 1 (string): name of the NPC
 * Argument 2 (integer): the sprite's direction between 0 and 3
 */
int MapScript::l_npc_set_direction(lua_State *l) {

  check_nb_arguments(l, 2);

  string name = lua_tostring(l, 1);
  int direction = lua_tointeger(l, 2);

  Map *map = zsdx->game->get_current_map();
  InteractiveEntity *npc = (InteractiveEntity*) map->get_entities()->get_entity(MapEntity::INTERACTIVE_ENTITY, name);
  npc->set_sprite_direction(direction);

  return 0;
}

/**
 * Removes an NPC from the map.
 * Argument 1 (string): name of the NPC
 */
int MapScript::l_npc_remove(lua_State *l) {

  check_nb_arguments(l, 1);

  string name = lua_tostring(l, 1);

  Map *map = zsdx->game->get_current_map();
  map->get_entities()->remove_entity(MapEntity::INTERACTIVE_ENTITY, name);

  return 0;
}

/**
 * Opens or closes a chest.
 * Only the chest sprite is affected (use give_treasure to give a treasure to the player).
 * Argument 1 (string): name of the NPC
 * Argument 2 (integer): the sprite's direction between 0 and 3
 */
int MapScript::l_set_chest_open(lua_State *l) {

  check_nb_arguments(l, 2);

  string chest_name = lua_tostring(l, 1);
  bool open = lua_toboolean(l, 2) != 0;

  Map *map = zsdx->game->get_current_map();
  Chest *chest = (Chest*) map->get_entities()->get_entity(MapEntity::CHEST, chest_name);
  chest->set_open(open);

  return 0;
}

/**
 * Returns the current number of rupees of the player.
 * Return value (integer): the number of rupees
 */
int MapScript::l_get_rupees(lua_State *l) {

  check_nb_arguments(l, 0);
  int rupees = zsdx->game->get_equipment()->get_rupees();
  lua_pushinteger(l, rupees);
  return 1;
}

/**
 * Removes some rupees to the player.
 * Argument 1 (integer): number or rupees to remove
 */
int MapScript::l_remove_rupees(lua_State *l) {

  check_nb_arguments(l, 1);
  int rupees = lua_tointeger(l, 1);
  zsdx->game->get_equipment()->remove_rupees(rupees);
  return 0;
}

/**
 * Disables a dynamic tile.
 * Argument 1 (string): name of the dynamic tile to disable
 */
int MapScript::l_disable_tile(lua_State *l) {

  check_nb_arguments(l, 1);
  string dynamic_tile_name = lua_tostring(l, 1);
  
  Map *map = zsdx->game->get_current_map();
  DynamicTile *dynamic_tile = (DynamicTile*) map->get_entities()->get_entity(MapEntity::DYNAMIC_TILE, dynamic_tile_name);
  dynamic_tile->set_enabled(false);

  return 0;
}

/**
 * Enables a dynamic tile.
 * Argument 1 (string): name of the dynamic tile to enable
 */
int MapScript::l_enable_tile(lua_State *l) {

  check_nb_arguments(l, 1);
  string dynamic_tile_name = lua_tostring(l, 1);
  
  Map *map = zsdx->game->get_current_map();
  DynamicTile *dynamic_tile = (DynamicTile*) map->get_entities()->get_entity(MapEntity::DYNAMIC_TILE, dynamic_tile_name);
  dynamic_tile->set_enabled(true);

  return 0;
}

/**
 * Enables a dynamic tile.
 * Argument 1 (string): name of the dynamic tile to enable
 * Return value (boolean): true if this tile is enabled
 */
int MapScript::l_is_tile_enabled(lua_State *l) {

  check_nb_arguments(l, 1);
  string dynamic_tile_name = lua_tostring(l, 1);
  
  Map *map = zsdx->game->get_current_map();
  DynamicTile *dynamic_tile = (DynamicTile*) map->get_entities()->get_entity(MapEntity::DYNAMIC_TILE, dynamic_tile_name);
  lua_pushboolean(l, dynamic_tile->is_enabled() ? 1 : 0);

  return 1;
}

/**
 * Replaces a block at its initial position.
 * Argument 1 (string): name of the block to reset
 */
int MapScript::l_reset_block(lua_State *l) {

  check_nb_arguments(l, 1);
  string block_name = lua_tostring(l, 1);
  
  Map *map = zsdx->game->get_current_map();
  Block *block = (Block*) map->get_entities()->get_entity(MapEntity::BLOCK, block_name);
  block->reset();

  return 0;
}

/**
 * Replaces all blocks of the map at their initial position.
 */
int MapScript::l_reset_blocks(lua_State *l) {

  check_nb_arguments(l, 0);
  
  Map *map = zsdx->game->get_current_map();
  std::list<MapEntity*> *blocks = map->get_entities()->get_entities(MapEntity::BLOCK);

  std::list<MapEntity*>::iterator i;
  for (i = blocks->begin(); i != blocks->end(); i++) {
    ((Block*) (*i))->reset();
  }

  delete blocks;

  return 0;
}

/**
 * Returns the current animation of an interactive entity's sprite.
 * Argument 1 (string): name of the interactive entity
 * Return value (string): name of the current animation
 */
int MapScript::l_interactive_entity_get_animation(lua_State *l) {

  check_nb_arguments(l, 1);

  string name = lua_tostring(l, 1);

  Map *map = zsdx->game->get_current_map();
  InteractiveEntity *entity = (InteractiveEntity*) map->get_entities()->get_entity(MapEntity::INTERACTIVE_ENTITY, name);
  string animation = entity->get_sprite()->get_current_animation();

  lua_pushstring(l, animation.c_str());

  return 1;
}

/**
 * Returns the animation speed of an interactive entity's sprite.
 * Argument 1 (string): name of the interactive entity
 * Return value (integer): delay between two frames in milliseconds
 */
int MapScript::l_interactive_entity_get_animation_delay(lua_State *l) {

  check_nb_arguments(l, 1);

  string name = lua_tostring(l, 1);

  Map *map = zsdx->game->get_current_map();
  InteractiveEntity *entity = (InteractiveEntity*) map->get_entities()->get_entity(MapEntity::INTERACTIVE_ENTITY, name);
  Uint32 delay = entity->get_sprite()->get_frame_delay();

  lua_pushinteger(l, delay);

  return 1;
}

/**
 * Returns the current animation frame of an interactive entity's sprite.
 * Argument 1 (string): name of the interactive entity
 * Return value (integer): frame number
 */
int MapScript::l_interactive_entity_get_animation_frame(lua_State *l) {

  check_nb_arguments(l, 1);

  string name = lua_tostring(l, 1);

  Map *map = zsdx->game->get_current_map();
  InteractiveEntity *entity = (InteractiveEntity*) map->get_entities()->get_entity(MapEntity::INTERACTIVE_ENTITY, name);
  int frame = entity->get_sprite()->get_current_frame();

  lua_pushinteger(l, frame);

  return 1;
}

/**
 * Returns whether the animation of an interactive entity's sprite is paused.
 * Argument 1 (string): name of the interactive entity
 * Return value (boolean): true if the animation is paused
 */
int MapScript::l_interactive_entity_is_animation_paused(lua_State *l) {

  check_nb_arguments(l, 1);

  string name = lua_tostring(l, 1);

  Map *map = zsdx->game->get_current_map();
  InteractiveEntity *entity = (InteractiveEntity*) map->get_entities()->get_entity(MapEntity::INTERACTIVE_ENTITY, name);
  bool paused = entity->get_sprite()->is_paused();

  lua_pushboolean(l, paused ? 1 : 0);

  return 1;
}

/**
 * Sets the animation of an interactive entity's sprite.
 * Argument 1 (string): name of the interactive entity
 * Argument 2 (string): name of the animation to set
 */
int MapScript::l_interactive_entity_set_animation(lua_State *l) {

  check_nb_arguments(l, 2);

  string name = lua_tostring(l, 1);
  string animation = lua_tostring(l, 2);

  Map *map = zsdx->game->get_current_map();
  InteractiveEntity *entity = (InteractiveEntity*) map->get_entities()->get_entity(MapEntity::INTERACTIVE_ENTITY, name);
  entity->get_sprite()->set_current_animation(animation);

  return 0;
}

/**
 * Sets the animation speed of an interactive entity's sprite.
 * Argument 1 (string): name of the interactive entity
 * Argument 2 (integer): delay between two frames in milliseconds
 */
int MapScript::l_interactive_entity_set_animation_delay(lua_State *l) {

  check_nb_arguments(l, 2);

  string name = lua_tostring(l, 1);
  Uint32 delay = lua_tointeger(l, 2);

  Map *map = zsdx->game->get_current_map();
  InteractiveEntity *entity = (InteractiveEntity*) map->get_entities()->get_entity(MapEntity::INTERACTIVE_ENTITY, name);
  entity->get_sprite()->set_frame_delay(delay);

  return 0;
}

/**
 * Sets the current animation frame of an interactive entity's sprite.
 * Argument 1 (string): name of the interactive entity
 * Argument 2 (integer): frame number
 */
int MapScript::l_interactive_entity_set_animation_frame(lua_State *l) {

  check_nb_arguments(l, 2);

  string name = lua_tostring(l, 1);
  int frame = lua_tointeger(l, 2);

  Map *map = zsdx->game->get_current_map();
  InteractiveEntity *entity = (InteractiveEntity*) map->get_entities()->get_entity(MapEntity::INTERACTIVE_ENTITY, name);
  entity->get_sprite()->set_current_frame(frame);

  return 0;
}

/**
 * Pauses or resumes the animation of an interactive entity's sprite.
 * Argument 1 (string): name of the interactive entity
 * Argument 2 (boolean): true to pause, false to resume
 */
int MapScript::l_interactive_entity_set_animation_paused(lua_State *l) {

  check_nb_arguments(l, 2);

  string name = lua_tostring(l, 1);
  bool paused = lua_toboolean(l, 2) != 0;

  Map *map = zsdx->game->get_current_map();
  InteractiveEntity *entity = (InteractiveEntity*) map->get_entities()->get_entity(MapEntity::INTERACTIVE_ENTITY, name);
  entity->get_sprite()->set_paused(paused);

  return 0;
}

/**
 * Returns the tunic of the hero.
 * Return value (integer): the tunic number (0 to 2)
 */
int MapScript::l_equipment_get_tunic(lua_State *l) {

  check_nb_arguments(l, 0);

  int tunic = zsdx->game->get_equipment()->get_tunic();

  lua_pushinteger(l, tunic);

  return 1;
}

/**
 * Returns the sword of the hero.
 * Return value (integer): the sword number (0 to 4)
 */
int MapScript::l_equipment_get_sword(lua_State *l) {

  check_nb_arguments(l, 0);

  int sword = zsdx->game->get_equipment()->get_sword();

  lua_pushinteger(l, sword);

  return 1;
}

/**
 * Returns the shield of the hero.
 * Return value (integer): the shield number (0 to 3)
 */
int MapScript::l_equipment_get_shield(lua_State *l) {

  check_nb_arguments(l, 0);

  int shield = zsdx->game->get_equipment()->get_shield();

  lua_pushinteger(l, shield);

  return 1;
}

// event functions, i.e. functions called by the C++ engine to notify the map script that something happened

/**
 * Notifies the script that the map has just been started.
 */
void MapScript::event_map_started(void) {
  call_lua_function("event_map_started");
}

/**
 * Notifies the script that a message has just started to be displayed
 * in the dialog box.
 * @param message_id id of the message
 */
void MapScript::event_message_started(MessageId message_id) {
  call_lua_function("event_message_started", 1, message_id.c_str());
}

/**
 * Notifies the script that the dialog box has just finished.
 * This function is called when the last message of a dialog is finished.
 * @param first_message_id id of the first message in the message sequence
 * that has just finished
 * @param answer the answer selected by the player: 0 for the first one, 
 * 1 for the second one, -1 if there was no question
 */
void MapScript::event_message_sequence_finished(MessageId first_message_id, int answer) {

  char s[16];
  sprintf(s, "%d", answer);
  call_lua_function("event_message_sequence_finished", 2, first_message_id.c_str(), s);
}

/**
 * Notifies the script that a switch has just been enabled.
 * @param sw the switch
 */
void MapScript::event_switch_enabled(Switch *sw) {
  call_lua_function("event_switch_enabled", 1, sw->get_name().c_str());
}

/**
 * Notifies the script that a switch has just been disabled.
 * @param sw the switch
 */
void MapScript::event_switch_disabled(Switch *sw) {
  call_lua_function("event_switch_disabled", 1, sw->get_name().c_str());
}

/**
 * Notifies the script that the camera moved by a call to move_camera() has reached its target.
 */
void MapScript::event_camera_reached_target(void) {
  call_lua_function("event_camera_reached_target");
}


/**
 * Notifies the script that the player has just pressed the action
 * key in front an interactive entity.
 * @param entity_name name of the interactive entity
 */
void MapScript::event_interaction(string entity_name) {
  call_lua_function("event_interaction", 1, entity_name.c_str());
}

/**
 * Notifies the script that the player has just pressed the action
 * key in front an NPC.
 * @param npc_name name of the NPC
 */
void MapScript::event_npc_dialog(string npc_name) {
  call_lua_function("event_npc_dialog", 1, npc_name.c_str());
}

/**
 * Notifies the script that an NPC has just finished its movement.
 * @param npc_name name of the NPC
 */
void MapScript::event_npc_movement_finished(string npc_name) {
  call_lua_function("event_npc_movement_finished", 1, npc_name.c_str());
}

/**
 * Notifies the script that the player has just open an empty chest.
 * What happend next is defined by your script. The hero is in state FREEZE
 * so if you do something else than giving the player a treasure,
 * don't forget to call unfreeze() when you have finished.
 * @param chest_name name of the chest
 * @return true if the script has handled the event
 */
bool MapScript::event_open_empty_chest(string chest_name) {
  return call_lua_function("event_open_empty_chest", 1, chest_name.c_str());
}

/**
 * Notifies the script that the player has just obtained a treasure.
 * The treasure source does not matter: it can come from a chest, 
 * a pickable item or the script.
 * @param content the content obtained
 * @param savegame_variable the boolean variable where this treasure is saved
 * (or -1 if the treasure is not saved)
 */
void MapScript::event_got_treasure(Treasure::Content content, int savegame_variable) {

  char s_content[16];
  sprintf(s_content, "%d", content);

  char s_savegame_variable[16];
  sprintf(s_savegame_variable, "%d", savegame_variable);
  call_lua_function("event_got_treasure", 2, s_content, s_savegame_variable);
}
