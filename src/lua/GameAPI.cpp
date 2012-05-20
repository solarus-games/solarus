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
#include "Game.h"
#include "Savegame.h"
#include "Equipment.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"
#include <lua.hpp>

const std::string Script::game_module_name = "sol.game";

/**
 * @brief Initializes the game features provided to Lua.
 */
void Script::register_game_module() {

  static const luaL_Reg methods[] = {
      { "exists", game_api_exists },
      { "delete", game_api_delete },
      { "load", game_api_load },
      { "save", game_api_save },
      { "start", game_api_start },
      { "get_string", game_api_get_string },
      { "get_integer", game_api_get_integer },
      { "get_boolean", game_api_get_boolean },
      { "set_string", game_api_set_string },
      { "set_integer", game_api_set_integer },
      { "set_boolean", game_api_set_boolean },
      { "get_player_name", game_api_get_player_name },
      { "set_player_name", game_api_set_player_name },
      { "get_life", game_api_get_life },
      { "set_life", game_api_set_life },
      { "add_life", game_api_add_life },
      { "remove_life", game_api_remove_life },
      { "get_max_life", game_api_get_max_life },
      { "set_max_life", game_api_set_max_life },
      { "add_max_life", game_api_add_max_life },
      { "get_money", game_api_get_money },
      { "set_money", game_api_set_money },
      { "add_money", game_api_add_money },
      { "remove_money", game_api_remove_money },
      { "get_magic", game_api_get_magic},
      { "set_magic", game_api_set_magic},
      { "add_magic", game_api_add_magic},
      { "remove_magic", game_api_remove_magic},
      { "start_decreasing_magic", game_api_start_decreasing_magic},
      { "stop_decreasing_magic", game_api_stop_decreasing_magic},
      { "get_max_magic", game_api_get_max_magic},
      { "set_max_magic", game_api_set_max_magic},
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
  static const luaL_Reg metamethods[] = {
      { "__eq", userdata_meta_eq },
      { NULL, NULL }
  };
  register_type(game_module_name, methods, metamethods);
}

/**
 * @brief Checks that the userdata at the specified index of the stack is a
 * game and returns it.
 * @param l a Lua context
 * @param index an index in the stack
 * @return the game
 */
Equipment& Script::check_game(lua_State* l, int index) {
  return static_cast<Equipment&>(check_userdata(l, index, game_module_name));
}

/**
 * @brief Pushes a game userdata onto the stack.
 * @param l a Lua context
 * @param sprite a sprite
 */
void Script::push_game(lua_State* l, Equipment& game) {
  push_userdata(l, game);
}

/**
 * @brief Implementation of \ref lua_api_game_exists.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int Script::game_api_exists(lua_State *l) {

  // TODO

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_game_delete.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int Script::game_api_delete(lua_State *l) {

  // TODO

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_game_load.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int Script::game_api_load(lua_State *l) {

  // TODO

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_game_save.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int Script::game_api_save(lua_State *l) {

  Script& script = get_script(l);

  script.get_game().get_savegame().save();

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_game_start.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int Script::game_api_start(lua_State *l) {

  Script& script = get_script(l);

  script.get_game().restart();

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_game_get_string.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int Script::game_api_get_string(lua_State* l) {

  Script& script = get_script(l);
  int index = luaL_checkinteger(l, 1);

  luaL_argcheck(l, index >= 0 && index < 64, 1,
      "The index of a savegame string should be between 0 and 63");

  const std::string &value = script.get_game().get_savegame().get_string(index);
  lua_pushstring(l, value.c_str());

  return 1;
}

/**
 * @brief Implementation of \ref lua_api_game_get_integer.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int Script::game_api_get_integer(lua_State *l) {

  Script& script = get_script(l);
  int index = luaL_checkinteger(l, 1);

  luaL_argcheck(l, index >= 0 && index < 2048, 1,
      "The index of a savegame integer should be between 0 and 2047");

  int value = script.get_game().get_savegame().get_integer(index);
  lua_pushinteger(l, value);

  return 1;
}

/**
 * @brief Implementation of \ref lua_api_game_get_boolean.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int Script::game_api_get_boolean(lua_State *l) {

  Script& script = get_script(l);
  int index = luaL_checkinteger(l, 1);

  luaL_argcheck(l, index >= 0 && index < 32768, 1, (StringConcat()
      << "Invalid savegame boolean: " << index << " (should be between 0 and 32767").c_str());

  bool value = script.get_game().get_savegame().get_boolean(index);
  lua_pushboolean(l, value ? 1 : 0);

  return 1;
}

/**
 * @brief Implementation of \ref lua_api_game_set_string.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int Script::game_api_set_string(lua_State *l) {

  Script& script = get_script(l);
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
 * @brief Implementation of \ref lua_api_game_set_integer.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int Script::game_api_set_integer(lua_State *l) {

  Script& script = get_script(l);
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
 * @brief Implementation of \ref lua_api_game_set_boolean.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int Script::game_api_set_boolean(lua_State *l) {

  Script& script = get_script(l);
  int index = luaL_checkinteger(l, 1);
  bool value = lua_toboolean(l, 2) != 0;

  luaL_argcheck(l, index >= 0 && index < 32768, 1, (StringConcat()
      << "Invalid savegame boolean: " << index << " (should be between 0 and 32767").c_str());

  script.get_game().get_savegame().set_boolean(index, value);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_game_get_player_name.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int Script::game_api_get_player_name(lua_State *l) {

  Script& script = get_script(l);

  const std::string &name = script.get_game().get_savegame().get_string(Savegame::PLAYER_NAME);
  lua_pushstring(l, name.c_str());

  return 1;
}

/**
 * @brief Implementation of \ref lua_api_game_set_player_name.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int Script::game_api_set_player_name(lua_State *l) {

  // TODO

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_game_get_life.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int Script::game_api_get_life(lua_State *l) {

  Script& script = get_script(l);

  int life = script.get_game().get_equipment().get_life();
  lua_pushinteger(l, life);

  return 1;
}

/**
 * @brief Implementation of \ref lua_api_game_set_life.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int Script::game_api_set_life(lua_State *l) {

  // TODO

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_game_add_life.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int Script::game_api_add_life(lua_State *l) {

  Script& script = get_script(l);

  int life = luaL_checkinteger(l, 1);

  script.get_game().get_equipment().add_life(life);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_game_remove_life.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int Script::game_api_remove_life(lua_State *l) {

  Script& script = get_script(l);

  int life = luaL_checkinteger(l, 1);

  script.get_game().get_equipment().remove_life(life);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_game_get_max_life.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int Script::game_api_get_max_life(lua_State *l) {

  Script& script = get_script(l);

  int life = script.get_game().get_equipment().get_max_life();
  lua_pushinteger(l, life);

  return 1;
}

/**
 * @brief Implementation of \ref lua_api_game_set_max_life.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int Script::game_api_set_max_life(lua_State *l) {

  Script& script = get_script(l);

  int life = luaL_checkinteger(l, 1);
  script.get_game().get_equipment().set_max_life(life);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_game_add_max_life.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int Script::game_api_add_max_life(lua_State *l) {

  Script& script = get_script(l);

  int life = luaL_checkinteger(l, 1);

  Equipment &equipment = script.get_game().get_equipment();
  equipment.set_max_life(equipment.get_max_life() + life);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_game_get_money.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int Script::game_api_get_money(lua_State *l) {

  Script& script = get_script(l);

  int money = script.get_game().get_equipment().get_money();
  lua_pushinteger(l, money);

  return 1;
}

/**
 * @brief Implementation of \ref lua_api_game_set_money.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int Script::game_api_set_money(lua_State *l) {

  // TODO

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_game_add_money.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int Script::game_api_add_money(lua_State *l) {

  Script& script = get_script(l);

  int money = luaL_checkinteger(l, 1);

  script.get_game().get_equipment().add_money(money);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_game_remove_money.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int Script::game_api_remove_money(lua_State *l) {

  Script& script = get_script(l);

  int money = luaL_checkinteger(l, 1);

  script.get_game().get_equipment().remove_money(money);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_game_get_magic.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int Script::game_api_get_magic(lua_State *l) {

  Script& script = get_script(l);

  int magic = script.get_game().get_equipment().get_magic();
  lua_pushinteger(l, magic);

  return 1;
}

/**
 * @brief Implementation of \ref lua_api_game_set_magic.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int Script::game_api_set_magic(lua_State *l) {

  // TODO

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_game_add_magic.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int Script::game_api_add_magic(lua_State *l) {

  Script& script = get_script(l);

  int magic = luaL_checkinteger(l, 1);

  script.get_game().get_equipment().add_magic(magic);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_game_remove_magic.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int Script::game_api_remove_magic(lua_State *l) {

  Script& script = get_script(l);

  int magic = luaL_checkinteger(l, 1);

  script.get_game().get_equipment().remove_magic(magic);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_game_start_decreasing_magic.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int Script::game_api_start_decreasing_magic(lua_State *l) {

  Script& script = get_script(l);

  uint32_t delay = luaL_checkinteger(l, 1);

  script.get_game().get_equipment().start_removing_magic(delay);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_game_stop_decreasing_magic.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int Script::game_api_stop_decreasing_magic(lua_State *l) {

  Script& script = get_script(l);

  script.get_game().get_equipment().stop_removing_magic();

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_game_get_max_magic.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int Script::game_api_get_max_magic(lua_State *l) {

  Script& script = get_script(l);

  int magic = script.get_game().get_equipment().get_max_magic();
  lua_pushinteger(l, magic);

  return 1;
}

/**
 * @brief Implementation of \ref lua_api_game_set_max_magic.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int Script::game_api_set_max_magic(lua_State *l) {

  Script& script = get_script(l);

  int magic = luaL_checkinteger(l, 1);
  script.get_game().get_equipment().set_max_magic(magic);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_game_has_ability.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int Script::game_api_has_ability(lua_State *l) {

  Script& script = get_script(l);

  const std::string &ability_name = luaL_checkstring(l, 1);

  bool has_ability = script.get_game().get_equipment().has_ability(ability_name);
  lua_pushboolean(l, has_ability);

  return 1;
}

/**
 * @brief Implementation of \ref lua_api_game_get_ability.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int Script::game_api_get_ability(lua_State *l) {

  Script& script = get_script(l);

  const std::string &ability_name = luaL_checkstring(l, 1);

  int ability_level = script.get_game().get_equipment().get_ability(ability_name);
  lua_pushinteger(l, ability_level);

  return 1;
}

/**
 * @brief Implementation of \ref lua_api_game_set_ability.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int Script::game_api_set_ability(lua_State *l) {

  Script& script = get_script(l);

  const std::string &ability_name = luaL_checkstring(l, 1);
  int level = luaL_checkinteger(l, 2);

  script.get_game().get_equipment().set_ability(ability_name, level);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_game_has_item.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int Script::game_api_has_item(lua_State *l) {

  Script& script = get_script(l);

  const std::string &item_name = luaL_checkstring(l, 1);

  bool has_item = script.get_game().get_equipment().has_item(item_name);
  lua_pushboolean(l, has_item);

  return 1;
}

/**
 * @brief Implementation of \ref lua_api_game_get_item.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int Script::game_api_get_item(lua_State *l) {

  Script& script = get_script(l);

  const std::string &item_name = luaL_checkstring(l, 1);

  int variant = script.get_game().get_equipment().get_item_variant(item_name);
  lua_pushinteger(l, variant);

  return 1;
}

/**
 * @brief Implementation of \ref lua_api_game_set_item.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int Script::game_api_set_item(lua_State *l) {

  Script& script = get_script(l);

  const std::string &item_name = luaL_checkstring(l, 1);
  int variant = luaL_checkinteger(l, 2);

  script.get_game().get_equipment().set_item_variant(item_name, variant);

  return 1;
}

/**
 * @brief Implementation of \ref lua_api_game_has_item_amount.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int Script::game_api_has_item_amount(lua_State *l) {

  Script& script = get_script(l);

  const std::string &item_name = luaL_checkstring(l, 1);
  int amount = luaL_checkinteger(l, 2);

  bool has_amount = script.get_game().get_equipment().get_item_amount(item_name) >= amount;
  lua_pushboolean(l, has_amount);

  return 1;
}

/**
 * @brief Implementation of \ref lua_api_game_get_item_amount.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int Script::game_api_get_item_amount(lua_State *l) {

  Script& script = get_script(l);

  const std::string &item_name = luaL_checkstring(l, 1);

  int amount = script.get_game().get_equipment().get_item_amount(item_name);
  lua_pushinteger(l, amount);

  return 1;
}

/**
 * @brief Implementation of \ref lua_api_game_add_item_amount.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int Script::game_api_add_item_amount(lua_State *l) {

  Script& script = get_script(l);

  const std::string &item_name = luaL_checkstring(l, 1);
  int amount = luaL_checkinteger(l, 2);

  script.get_game().get_equipment().add_item_amount(item_name, amount);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_game_remove_item_amount.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int Script::game_api_remove_item_amount(lua_State *l) {

  Script& script = get_script(l);

  const std::string &item_name = luaL_checkstring(l, 1);
  int amount = luaL_checkinteger(l, 2);

  script.get_game().get_equipment().remove_item_amount(item_name, amount);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_game_is_dungeon_finished.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int Script::game_api_is_dungeon_finished(lua_State *l) {

  Script& script = get_script(l);

  int dungeon = luaL_checkinteger(l, 1);
  bool finished = script.get_game().get_equipment().is_dungeon_finished(dungeon);
  lua_pushboolean(l, finished);

  return 1;
}

/**
 * @brief Implementation of \ref lua_api_game_set_dungeon_finished.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int Script::game_api_set_dungeon_finished(lua_State *l) {

  Script& script = get_script(l);

  int dungeon = luaL_checkinteger(l, 1);
  script.get_game().get_equipment().set_dungeon_finished(dungeon);

  return 0;
}

