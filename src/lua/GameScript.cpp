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
#include "lua/GameScript.h"
#include "Game.h"
#include "Savegame.h"
#include "Equipment.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"
#include <lua.hpp>

/**
 * @brief Creates a game script.
 * @param scripts the list of scripts
 * @param game the game
 */
GameScript::GameScript(Game &game):
  Script(), game(game) {

}

/**
 * @brief Desctructor.
 */
GameScript::~GameScript() {

}

/**
 * @brief Tells the Lua context what C++ functions it can call.
 */
void GameScript::register_available_functions() {

  // functions global to all types of scripts
  Script::register_available_functions();

  // functions specific to game scripts
  static luaL_Reg functions[] = {
    { "savegame_get_string", l_savegame_get_string },
    { "savegame_get_integer", l_savegame_get_integer },
    { "savegame_get_boolean", l_savegame_get_boolean },
    { "savegame_set_string", l_savegame_set_string },
    { "savegame_set_integer", l_savegame_set_integer },
    { "savegame_set_boolean", l_savegame_set_boolean },
    { "savegame_get_name", l_savegame_get_name },
    { "equipment_get_life", l_equipment_get_life },
    { "equipment_add_life", l_equipment_add_life },
    { "equipment_remove_life", l_equipment_remove_life },
    { "equipment_get_money", l_equipment_get_money },
    { "equipment_add_money", l_equipment_add_money },
    { "equipment_remove_money", l_equipment_remove_money },
    { "equipment_has_ability", l_equipment_has_ability },
    { "equipment_get_ability", l_equipment_get_ability },
    { "equipment_set_ability", l_equipment_set_ability },
    { "equipment_has_item", l_equipment_has_item },
    { "equipment_get_item", l_equipment_get_item },
    { "equipment_set_item", l_equipment_set_item },
    { "equipment_has_item_amount", l_equipment_has_item_amount },
    { "equipment_get_item_amount", l_equipment_get_item_amount },
    { "equipment_add_item_amount", l_equipment_add_item_amount },
    { "equipment_remove_item_amount", l_equipment_remove_item_amount },
    { "equipment_is_dungeon_finished", l_equipment_is_dungeon_finished },
    { "equipment_set_dungeon_finished", l_equipment_set_dungeon_finished },
    { NULL, NULL }
  };

  luaL_register(context, "sol.game", functions);
}

/**
 * @brief Returns whether a timer just created should be initially suspended.
 * @return true to initially suspend a new timer
 */
bool GameScript::is_new_timer_suspended(void) {

  // start the timer even if the game is suspended (e.g. a timer started during a camera movement)
  // except when it is suspended because of a dialog box
  return game.is_showing_message();
}

/**
 * @brief Checks the number of arguments provided to a C++ function called by the Lua script.
 *
 * In any C++ function called by the Lua script (i.e. a function prefixed by "l_"),
 * the first instruction calls this function.
 * It checks the number of arguments provided to the C++ function called by the Lua script
 * and retrieves the current MapScript object.
 *
 * @param context the Lua context
 * @param nb_arguments the number of arguments to check (if it is incorrect, the program stops)
 * @param game_script if not NULL, a pointer to the MapScript object will be copied there so that the static C++ function
 * called by the Lua script can access it
 */

void GameScript::called_by_script(lua_State *context, int nb_arguments, GameScript **game_script) {

  Script *script;
  Script::called_by_script(context, nb_arguments, &script);
  *game_script = (GameScript*) script;
}

// functions that can be called by the Lua script

/**
 * @brief Returns a string value saved.
 *
 * - Argument 1 (integer): index of the string value to get (0 to 63)
 * - Return value (string): the string saved at this index
 */
int GameScript::l_savegame_get_string(lua_State *l) {

  GameScript *script;
  called_by_script(l, 1, &script);
  int index = lua_tointeger(l, 1);

  const std::string &value = script->game.get_savegame().get_string(index);
  lua_pushstring(l, value.c_str());

  return 1;
}

/**
 * @brief Returns an integer value saved.
 *
 * - Argument 1 (integer): index of the integer value to get (0 to 2047)
 * - Return value (integer): the integer saved at this index
 */
int GameScript::l_savegame_get_integer(lua_State *l) {

  GameScript *script;
  called_by_script(l, 1, &script);
  int index = lua_tointeger(l, 1);

  int value = script->game.get_savegame().get_integer(index);
  lua_pushinteger(l, value);

  return 1;
}

/**
 * @brief Returns a boolean value saved.
 *
 * - Argument 1 (integer): index of the boolean value to get
 * - Return value (boolean): the boolean saved at this index
 */
int GameScript::l_savegame_get_boolean(lua_State *l) {

  GameScript *script;
  called_by_script(l, 1, &script);
  int index = lua_tointeger(l, 1);

  bool value = script->game.get_savegame().get_boolean(index);
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
int GameScript::l_savegame_set_string(lua_State *l) {

  GameScript *script;
  called_by_script(l, 2, &script);
  int index = lua_tointeger(l, 1);
  const std::string &value = lua_tostring(l, 2);

  Debug::assert(index >= 32, StringConcat() << "Cannot change savegame string #" << index << ": string variables below 32 are read-only");

  script->game.get_savegame().set_string(index, value);

  return 0;
}

/**
 * @brief Sets an integer value saved.
 *
 * - Argument 1 (integer): index of the integer value to set, between 1024 and 2047
 * (lower indices are writable only by the game engine)
 * - Argument 2 (integer): the integer value to store at this index
 */
int GameScript::l_savegame_set_integer(lua_State *l) {

  GameScript *script;
  called_by_script(l, 2, &script);
  int index = lua_tointeger(l, 1);
  int value = lua_tointeger(l, 2);

  Debug::assert(index >= 1024, StringConcat() << "Cannot change savegame integer #" << index << ": integer variables below 1024 are read-only");

  script->game.get_savegame().set_integer(index, value);

  return 0;
}

/**
 * @brief Sets a boolean value saved.
 *
 * - Argument 1 (integer): index of the boolean value to set, between 0 and 32767
 * - Argument 2 (boolean): the boolean value to store at this index
 */
int GameScript::l_savegame_set_boolean(lua_State *l) {

  GameScript *script;
  called_by_script(l, 2, &script);
  int index = lua_tointeger(l, 1);
  int value = lua_toboolean(l, 2);

  script->game.get_savegame().set_boolean(index, value != 0);

  return 0;
}

/**
 * @brief Returns a string representing the name of the player.
 * 
 * - Return value (string): the player's name
 */
int GameScript::l_savegame_get_name(lua_State *l) {

  GameScript *script;
  called_by_script(l, 0, &script);

  const std::string &name = script->game.get_savegame().get_string(Savegame::PLAYER_NAME);
  lua_pushstring(l, name.c_str());

  return 1;
}

/**
 * @brief Returns the current level of life of the player.
 *
 * - Return value (integer): the level of life
 */
int GameScript::l_equipment_get_life(lua_State *l) {

  GameScript *script;
  called_by_script(l, 0, &script);

  int life = script->game.get_equipment().get_life();
  lua_pushinteger(l, life);

  return 1;
}

/**
 * @brief Gives some life to the player.
 *
 * - Argument 1 (integer): amount of life to add
 */
int GameScript::l_equipment_add_life(lua_State *l) {

  GameScript *script;
  called_by_script(l, 1, &script);

  int life = lua_tointeger(l, 1);

  script->game.get_equipment().add_life(life);

  return 0;
}

/**
 * @brief Removes some life from the player.
 *
 * - Argument 1 (integer): amount of life to remove
 */
int GameScript::l_equipment_remove_life(lua_State *l) {

  GameScript *script;
  called_by_script(l, 1, &script);

  int life = lua_tointeger(l, 1);

  script->game.get_equipment().remove_life(life);

  return 0;
}

/**
 * @brief Returns the current amount of money of the player.
 *
 * - Return value (integer): the amount of money
 */
int GameScript::l_equipment_get_money(lua_State *l) {

  GameScript *script;
  called_by_script(l, 0, &script);

  int money = script->game.get_equipment().get_money();
  lua_pushinteger(l, money);

  return 1;
}

/**
 * @brief Gives some money to the player.
 *
 * - Argument 1 (integer): amount of money to add
 */
int GameScript::l_equipment_add_money(lua_State *l) {

  GameScript *script;
  called_by_script(l, 1, &script);

  int money = lua_tointeger(l, 1);

  script->game.get_equipment().add_money(money);

  return 0;
}

/**
 * @brief Removes some money from the player.
 *
 * - Argument 1 (integer): amount of money to remove
 */
int GameScript::l_equipment_remove_money(lua_State *l) {

  GameScript *script;
  called_by_script(l, 1, &script);

  int money = lua_tointeger(l, 1);

  script->game.get_equipment().remove_money(money);

  return 0;
}

/**
 * @brief Returns whether the player has the specified ability.
 *
 * This is equivalent to equipment_get_ability(ability_name) > 0.
 *
 * - Argument 1 (string): name of the ability to get
 * - Return value (boolean): true if the level of this ability is greater than 0
 */
int GameScript::l_equipment_has_ability(lua_State *l) {

  GameScript *script;
  called_by_script(l, 1, &script);

  const std::string &ability_name = lua_tostring(l, 1);

  bool has_ability = script->game.get_equipment().has_ability(ability_name);
  lua_pushboolean(l, has_ability);

  return 1;
}

/**
 * @brief Sets the level of an ability of the player.
 *
 * This function is typically called when the player obtains
 * an item that gives an ability
 *
 * - Argument 1 (string): name of the ability to set
 * - Argument 2 (integer): the level of this ability
 */
int GameScript::l_equipment_set_ability(lua_State *l) {

  GameScript *script;
  called_by_script(l, 2, &script);

  const std::string &ability_name = lua_tostring(l, 1);
  int level = lua_tointeger(l, 2);

  script->game.get_equipment().set_ability(ability_name, level);

  return 0;
}

/**
 * @brief Returns the level of an ability of the player.
 *
 * - Argument 1 (string): name of the ability to get
 * - Return value (integer): the level of this ability
 */
int GameScript::l_equipment_get_ability(lua_State *l) {

  GameScript *script;
  called_by_script(l, 1, &script);

  const std::string &ability_name = lua_tostring(l, 1);

  int ability_level = script->game.get_equipment().get_ability(ability_name);
  lua_pushinteger(l, ability_level);

  return 1;
}

/**
 * @brief Returns whether the player has the specified item.
 *
 * This is equivalent to equipment_get_item(item_name) > 0.
 *
 * - Argument 1 (string): an item name
 * - Return value (boolean): true if the player has this item
 */
int GameScript::l_equipment_has_item(lua_State *l) {

  GameScript *script;
  called_by_script(l, 1, &script);

  const std::string &item_name = lua_tostring(l, 1);

  bool has_item = script->game.get_equipment().has_item(item_name);
  lua_pushboolean(l, has_item);

  return 1;
}

/**
 * @brief Returns the possession state (also called the variant) of an item.
 *
 * - Argument 1 (string): an item name
 * - Return value (integer): the possession state of this item
 *   (0 if the player does not have the item)
 */
int GameScript::l_equipment_get_item(lua_State *l) {

  GameScript *script;
  called_by_script(l, 1, &script);

  const std::string &item_name = lua_tostring(l, 1);

  int variant = script->game.get_equipment().get_item_variant(item_name);
  lua_pushinteger(l, variant);

  return 1;
}

/**
 * @brief Sets the possession state of an item.
 *
 * - Argument 1 (string): an item name
 * - Argument 2 (integer): the possession state of this inventory item
 * (a value of 0 removes the item)
 */
int GameScript::l_equipment_set_item(lua_State *l) {

  GameScript *script;
  called_by_script(l, 2, &script);

  const std::string &item_name = lua_tostring(l, 1);
  int variant = lua_tointeger(l, 2);

  script->game.get_equipment().set_item_variant(item_name, variant);

  return 1;
}

/**
 * @brief Returns whether the player has at least the specified amount of an item.
 *
 * This is equivalent to equipment_get_item_amount(item_name, amount) > 0.
 *
 * - Argument 1 (string): the name of an item having an amount
 * - Argument 2 (integer): the amount to check
 * - Return value (integer): true if the player has at least this amount
 */
int GameScript::l_equipment_has_item_amount(lua_State *l) {

  GameScript *script;
  called_by_script(l, 1, &script);

  const std::string &item_name = lua_tostring(l, 1);
  int amount = lua_tointeger(l, 2);

  bool has_amount = script->game.get_equipment().get_item_amount(item_name) > amount;
  lua_pushboolean(l, has_amount);

  return 1;
}

/**
 * @brief Returns the amount the player has for an item.
 *
 * - Argument 1 (string): the name of an item having an amount
 * - Return value (integer): the amount possessed
 */
int GameScript::l_equipment_get_item_amount(lua_State *l) {

  GameScript *script;
  called_by_script(l, 1, &script);

  const std::string &item_name = lua_tostring(l, 1);

  int amount = script->game.get_equipment().get_item_amount(item_name);
  lua_pushinteger(l, amount);

  return 1;
}

/**
 * @brief Adds an amount of the specified item.
 *
 * - Argument 1 (string): the name of an item having an amount
 * - Argument 2 (integer): the amount to add
 */
int GameScript::l_equipment_add_item_amount(lua_State *l) {

  GameScript *script;
  called_by_script(l, 2, &script);

  const std::string &item_name = lua_tostring(l, 1);
  int amount = lua_tointeger(l, 2);

  script->game.get_equipment().add_item_amount(item_name, amount);

  return 0;
}

/**
 * @brief Removes an amount of the specified item.
 *
 * - Argument 1 (string): the name of an item having an amount
 * - Argument 2 (integer): the amount to remove
 */
int GameScript::l_equipment_remove_item_amount(lua_State *l) {

  GameScript *script;
  called_by_script(l, 2, &script);

  const std::string &item_name = lua_tostring(l, 1);
  int amount = lua_tointeger(l, 2);

  script->game.get_equipment().remove_item_amount(item_name, amount);

  return 0;
}

/**
 * @brief Returns whether a dungeon is finished.
 *
 * A dungeon is considered as finished if the function dungeon_set_finished() was
 * called from the script of a map in that dungeon.
 * This information is saved by the engine (see include/Savegame.h).
 * - Argument 1 (integer): number of the dungeon to test
 * - Return value (boolean): true if that dungeon is finished
 *
 * @param l the Lua context that is calling this function
 */
int GameScript::l_equipment_is_dungeon_finished(lua_State *l) {

  GameScript *script;
  called_by_script(l, 1, &script);

  int dungeon = lua_tointeger(l, 1);
  bool finished = script->game.get_equipment().is_dungeon_finished(dungeon);
  lua_pushboolean(l, finished);

  return 1;
}

/**
 * @brief Sets a dungeon as finished.
 *
 * You should call this function when the final dialog of the dungeon ending
 * sequence is finished.
 * - Argument 1 (integer): number of the dungeon to set
 *
 * @param l the Lua context that is calling this function
 */
int GameScript::l_equipment_set_dungeon_finished(lua_State *l) {

  GameScript *script;
  called_by_script(l, 1, &script);

  int dungeon = lua_tointeger(l, 1);
  script->game.get_equipment().set_dungeon_finished(dungeon);

  return 0;
}

