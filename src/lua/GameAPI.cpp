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
#include "Savegame.h"
#include "Equipment.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"
#include <lua.hpp>

/**
 * @brief Saves the game.
 */
int Script::game_api_save(lua_State *l) {

  Script& script = get_script(l, 0);

  script.get_game().get_savegame().save();

  return 0;
}

/**
 * @brief Resets the game (comes back to the title screen).
 */
int Script::game_api_reset(lua_State *l) {

  Script& script = get_script(l, 0);

  script.get_game().reset();

  return 0;
}

/**
 * @brief Restarts the game with the current savegame.
 *
 * The game is restarted with the current savegame state,
 * even if it is not saved.
 */
int Script::game_api_restart(lua_State *l) {

  Script& script = get_script(l, 0);

  script.get_game().restart();

  return 0;
}

/**
 * @brief Returns a string value saved.
 *
 * - Argument 1 (integer): index of the string value to get (0 to 63)
 * - Return value (string): the string saved at this index
 */
int Script::game_api_savegame_get_string(lua_State* l) {

  Script& script = get_script(l, 1);
  int index = luaL_checkinteger(l, 1);

  luaL_argcheck(l, index >= 0 && index < 64, 1,
      "The index of a savegame string should be between 0 and 63");

  const std::string &value = script.get_game().get_savegame().get_string(index);
  lua_pushstring(l, value.c_str());

  return 1;
}

/**
 * @brief Returns an integer value saved.
 *
 * - Argument 1 (integer): index of the integer value to get (0 to 2047)
 * - Return value (integer): the integer saved at this index
 */
int Script::game_api_savegame_get_integer(lua_State *l) {

  Script& script = get_script(l, 1);
  int index = luaL_checkinteger(l, 1);

  luaL_argcheck(l, index >= 0 && index < 2048, 1,
      "The index of a savegame integer should be between 0 and 2047");

  int value = script.get_game().get_savegame().get_integer(index);
  lua_pushinteger(l, value);

  return 1;
}

/**
 * @brief Returns a boolean value saved.
 *
 * - Argument 1 (integer): index of the boolean value to get
 * - Return value (boolean): the boolean saved at this index
 */
int Script::game_api_savegame_get_boolean(lua_State *l) {

  Script& script = get_script(l, 1);
  int index = luaL_checkinteger(l, 1);

  luaL_argcheck(l, index >= 0 && index < 32768, 1, (StringConcat()
      << "Invalid savegame boolean: " << index << " (should be between 0 and 32767").c_str());

  bool value = script.get_game().get_savegame().get_boolean(index);
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
int Script::game_api_savegame_set_string(lua_State *l) {

  Script& script = get_script(l, 2);
  int index = luaL_checkinteger(l, 1);
  const std::string &value = luaL_checkstring(l, 2);

  luaL_argcheck(l, index >= 0 && index < 63, 1, (StringConcat()
      << "Invalid savegame string: " << index << " (should be between 32 and 63").c_str());

  luaL_argcheck(l, index >= 32, 1, (StringConcat()
      << "Invalid savegame string: " << index << " (strings below 32 are read-only").c_str());

  script.get_game().get_savegame().set_string(index, value);

  return 0;
}

/**
 * @brief Sets an integer value saved.
 *
 * - Argument 1 (integer): index of the integer value to set, between 1024 and 2047
 * (lower indices are writable only by the game engine)
 * - Argument 2 (integer): the integer value to store at this index
 */
int Script::game_api_savegame_set_integer(lua_State *l) {

  Script& script = get_script(l, 2);
  int index = luaL_checkinteger(l, 1);
  int value = luaL_checkinteger(l, 2);

  luaL_argcheck(l, index >= 0 && index < 2048, 1, (StringConcat()
      << "Invalid savegame integer: " << index << " (should be between 32 and 63").c_str());

  luaL_argcheck(l, index >= 1024, 1, (StringConcat()
      << "Invalid savegame integer: " << index << " (integers below 32 are read-only").c_str());

  script.get_game().get_savegame().set_integer(index, value);

  return 0;
}

/**
 * @brief Sets a boolean value saved.
 *
 * - Argument 1 (integer): index of the boolean value to set, between 0 and 32767
 * - Argument 2 (boolean): the boolean value to store at this index
 */
int Script::game_api_savegame_set_boolean(lua_State *l) {

  Script& script = get_script(l, 2);
  int index = luaL_checkinteger(l, 1);
  bool value = lua_toboolean(l, 2) != 0;

  luaL_argcheck(l, index >= 0 && index < 32768, 1, (StringConcat()
      << "Invalid savegame boolean: " << index << " (should be between 0 and 32767").c_str());

  script.get_game().get_savegame().set_boolean(index, value);

  return 0;
}

/**
 * @brief Returns a string representing the name of the player.
 *
 * - Return value (string): the player's name
 */
int Script::game_api_savegame_get_name(lua_State *l) {

  Script& script = get_script(l, 0);

  const std::string &name = script.get_game().get_savegame().get_string(Savegame::PLAYER_NAME);
  lua_pushstring(l, name.c_str());

  return 1;
}

/**
 * @brief Returns the current level of life of the player.
 *
 * - Return value (integer): the level of life
 */
int Script::game_api_get_life(lua_State *l) {

  Script& script = get_script(l, 0);

  int life = script.get_game().get_equipment().get_life();
  lua_pushinteger(l, life);

  return 1;
}

/**
 * @brief Gives some life to the player.
 *
 * - Argument 1 (integer): amount of life to add
 */
int Script::game_api_add_life(lua_State *l) {

  Script& script = get_script(l, 1);

  int life = luaL_checkinteger(l, 1);

  script.get_game().get_equipment().add_life(life);

  return 0;
}

/**
 * @brief Removes some life from the player.
 *
 * - Argument 1 (integer): amount of life to remove
 */
int Script::game_api_remove_life(lua_State *l) {

  Script& script = get_script(l, 1);

  int life = luaL_checkinteger(l, 1);

  script.get_game().get_equipment().remove_life(life);

  return 0;
}

/**
 * @brief Returns the maximum level of life of the player.
 *
 * - Return value (integer): the maximum level of life
 */
int Script::game_api_get_max_life(lua_State *l) {

  Script& script = get_script(l, 0);

  int life = script.get_game().get_equipment().get_max_life();
  lua_pushinteger(l, life);

  return 1;
}

/**
 * @brief Sets the maximum level of life of the player.
 *
 * - Argument 1 (integer): the maximum level of life to set
 */
int Script::game_api_set_max_life(lua_State *l) {

  Script& script = get_script(l, 1);

  int life = luaL_checkinteger(l, 1);
  script.get_game().get_equipment().set_max_life(life);

  return 0;
}

/**
 * @brief Increases the maximum level of life of the player.
 *
 * - Argument 1 (integer): amount of life to add to the current maximum
 */
int Script::game_api_add_max_life(lua_State *l) {

  Script& script = get_script(l, 1);

  int life = luaL_checkinteger(l, 1);

  Equipment &equipment = script.get_game().get_equipment();
  equipment.set_max_life(equipment.get_max_life() + life);

  return 0;
}

/**
 * @brief Returns the current amount of money of the player.
 *
 * - Return value (integer): the amount of money
 */
int Script::game_api_get_money(lua_State *l) {

  Script& script = get_script(l, 0);

  int money = script.get_game().get_equipment().get_money();
  lua_pushinteger(l, money);

  return 1;
}

/**
 * @brief Gives some money to the player.
 *
 * - Argument 1 (integer): amount of money to add
 */
int Script::game_api_add_money(lua_State *l) {

  Script& script = get_script(l, 1);

  int money = luaL_checkinteger(l, 1);

  script.get_game().get_equipment().add_money(money);

  return 0;
}

/**
 * @brief Removes some money from the player.
 *
 * - Argument 1 (integer): amount of money to remove
 */
int Script::game_api_remove_money(lua_State *l) {

  Script& script = get_script(l, 1);

  int money = luaL_checkinteger(l, 1);

  script.get_game().get_equipment().remove_money(money);

  return 0;
}

/**
 * @brief Returns the current number of magic points.
 *
 * - Return value: the current number of magic points
 */
int Script::game_api_get_magic(lua_State *l) {

  Script& script = get_script(l, 0);

  int magic = script.get_game().get_equipment().get_magic();
  lua_pushinteger(l, magic);

  return 1;
}

/**
 * @brief Gives some magic points to the player.
 *
 * - Argument 1 (integer): number of magic points to add
 */
int Script::game_api_add_magic(lua_State *l) {

  Script& script = get_script(l, 1);

  int magic = luaL_checkinteger(l, 1);

  script.get_game().get_equipment().add_magic(magic);

  return 0;
}

/**
 * @brief Removes some magic points from the player.
 *
 * - Argument 1 (integer): number of magic points to remove
 */
int Script::game_api_remove_magic(lua_State *l) {

  Script& script = get_script(l, 1);

  int magic = luaL_checkinteger(l, 1);

  script.get_game().get_equipment().remove_magic(magic);

  return 0;
}

/**
 * @brief Starts a continuous decrease of the magic points.
 *
 * - Argument 1 (integer): delay in miliseconds between each decrease of 1 point
 */
int Script::game_api_start_decreasing_magic(lua_State *l) {

  Script& script = get_script(l, 1);

  uint32_t delay = luaL_checkinteger(l, 1);

  script.get_game().get_equipment().start_removing_magic(delay);

  return 0;
}

/**
 * @brief Stops a continuous decrease of the magic points.
 */
int Script::game_api_stop_decreasing_magic(lua_State *l) {

  Script& script = get_script(l, 0);

  script.get_game().get_equipment().stop_removing_magic();

  return 0;
}

/**
 * @brief Returns the maximum number of magic points of the player.
 *
 * - Return value (integer): the maximum number of magic points
 */
int Script::game_api_get_max_magic(lua_State *l) {

  Script& script = get_script(l, 0);

  int magic = script.get_game().get_equipment().get_max_magic();
  lua_pushinteger(l, magic);

  return 1;
}

/**
 * @brief Sets the maximum number of magic points of the player.
 *
 * - Argument 1 (integer): the maximum number of magic points to set
 */
int Script::game_api_set_max_magic(lua_State *l) {

  Script& script = get_script(l, 1);

  int magic = luaL_checkinteger(l, 1);
  script.get_game().get_equipment().set_max_magic(magic);

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
int Script::game_api_has_ability(lua_State *l) {

  Script& script = get_script(l, 1);

  const std::string &ability_name = luaL_checkstring(l, 1);

  bool has_ability = script.get_game().get_equipment().has_ability(ability_name);
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
int Script::game_api_set_ability(lua_State *l) {

  Script& script = get_script(l, 2);

  const std::string &ability_name = luaL_checkstring(l, 1);
  int level = luaL_checkinteger(l, 2);

  script.get_game().get_equipment().set_ability(ability_name, level);

  return 0;
}

/**
 * @brief Returns the level of an ability of the player.
 *
 * - Argument 1 (string): name of the ability to get
 * - Return value (integer): the level of this ability
 */
int Script::game_api_get_ability(lua_State *l) {

  Script& script = get_script(l, 1);

  const std::string &ability_name = luaL_checkstring(l, 1);

  int ability_level = script.get_game().get_equipment().get_ability(ability_name);
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
int Script::game_api_has_item(lua_State *l) {

  Script& script = get_script(l, 1);

  const std::string &item_name = luaL_checkstring(l, 1);

  bool has_item = script.get_game().get_equipment().has_item(item_name);
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
int Script::game_api_get_item(lua_State *l) {

  Script& script = get_script(l, 1);

  const std::string &item_name = luaL_checkstring(l, 1);

  int variant = script.get_game().get_equipment().get_item_variant(item_name);
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
int Script::game_api_set_item(lua_State *l) {

  Script& script = get_script(l, 2);

  const std::string &item_name = luaL_checkstring(l, 1);
  int variant = luaL_checkinteger(l, 2);

  script.get_game().get_equipment().set_item_variant(item_name, variant);

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
int Script::game_api_has_item_amount(lua_State *l) {

  Script& script = get_script(l, 2);

  const std::string &item_name = luaL_checkstring(l, 1);
  int amount = luaL_checkinteger(l, 2);

  bool has_amount = script.get_game().get_equipment().get_item_amount(item_name) >= amount;
  lua_pushboolean(l, has_amount);

  return 1;
}

/**
 * @brief Returns the amount the player has for an item.
 *
 * - Argument 1 (string): the name of an item having an amount
 * - Return value (integer): the amount possessed
 */
int Script::game_api_get_item_amount(lua_State *l) {

  Script& script = get_script(l, 1);

  const std::string &item_name = luaL_checkstring(l, 1);

  int amount = script.get_game().get_equipment().get_item_amount(item_name);
  lua_pushinteger(l, amount);

  return 1;
}

/**
 * @brief Adds an amount of the specified item.
 *
 * - Argument 1 (string): the name of an item having an amount
 * - Argument 2 (integer): the amount to add
 */
int Script::game_api_add_item_amount(lua_State *l) {

  Script& script = get_script(l, 2);

  const std::string &item_name = luaL_checkstring(l, 1);
  int amount = luaL_checkinteger(l, 2);

  script.get_game().get_equipment().add_item_amount(item_name, amount);

  return 0;
}

/**
 * @brief Removes an amount of the specified item.
 *
 * - Argument 1 (string): the name of an item having an amount
 * - Argument 2 (integer): the amount to remove
 */
int Script::game_api_remove_item_amount(lua_State *l) {

  Script& script = get_script(l, 2);

  const std::string &item_name = luaL_checkstring(l, 1);
  int amount = luaL_checkinteger(l, 2);

  script.get_game().get_equipment().remove_item_amount(item_name, amount);

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
int Script::game_api_is_dungeon_finished(lua_State *l) {

  Script& script = get_script(l, 1);

  int dungeon = luaL_checkinteger(l, 1);
  bool finished = script.get_game().get_equipment().is_dungeon_finished(dungeon);
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
int Script::game_api_set_dungeon_finished(lua_State *l) {

  Script& script = get_script(l, 1);

  int dungeon = luaL_checkinteger(l, 1);
  script.get_game().get_equipment().set_dungeon_finished(dungeon);

  return 0;
}

