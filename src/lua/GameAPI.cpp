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
#include "lua/LuaContext.h"
#include "MainLoop.h"
#include "Game.h"
#include "Savegame.h"
#include "Equipment.h"
#include "lowlevel/FileTools.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"
#include <lua.hpp>

const std::string LuaContext::game_module_name = "sol.game";

/**
 * @brief Initializes the game features provided to Lua.
 */
void LuaContext::register_game_module() {

  static const luaL_Reg methods[] = {
      { "exists", game_api_exists },
      { "delete", game_api_delete },
      { "load", game_api_load },
      { "save", game_api_save },
      { "start", game_api_start },
      { "is_started", game_api_is_started },
      { "is_suspended", game_api_is_suspended },
      { "is_paused", game_api_is_paused },
      { "set_paused", game_api_set_paused },
      { "is_showing_dialog", game_api_is_showing_dialog },
      { "get_map", game_api_get_map },
      { "get_value", game_api_get_value },
      { "set_value", game_api_set_value },
      { "get_starting_location", game_api_get_starting_location },
      { "set_starting_location", game_api_set_starting_location },
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
      { "get_max_money", game_api_get_max_money },
      { "set_max_money", game_api_set_max_money },
      { "get_magic", game_api_get_magic},
      { "set_magic", game_api_set_magic},
      { "add_magic", game_api_add_magic},
      { "remove_magic", game_api_remove_magic},
      { "get_max_magic", game_api_get_max_magic},
      { "set_max_magic", game_api_set_max_magic},
      { "has_ability", game_api_has_ability },
      { "get_ability", game_api_get_ability },
      { "set_ability", game_api_set_ability },
      { "get_item", game_api_get_item },
      { "get_item_assigned", game_api_get_item_assigned },
      { "set_item_assigned", game_api_set_item_assigned },
      { "is_command_pressed", game_api_is_command_pressed },
      { "get_commands_direction", game_api_get_commands_direction },
      { "get_command_effect", game_api_get_command_effect },
      { "get_command_keyboard_binding", game_api_get_command_keyboard_binding },
      { "set_command_keyboard_binding", game_api_set_command_keyboard_binding },
      { "get_command_joypad_binding", game_api_get_command_joypad_binding },
      { "set_command_joypad_binding", game_api_set_command_joypad_binding },
      { NULL, NULL }
  };
  static const luaL_Reg metamethods[] = {
      { "__gc", userdata_meta_gc },
      { "__newindex", userdata_meta_newindex_as_table },
      { "__index", userdata_meta_index_as_table },
      { NULL, NULL }
  };
  register_type(game_module_name, methods, metamethods);
}

/**
 * @brief Returns whether a value is a userdata of type game.
 * @param l A Lua context.
 * @param index An index in the stack.
 * @return true if the value at this index is a game.
 */
bool LuaContext::is_game(lua_State* l, int index) {
  return is_userdata(l, index, game_module_name);
}

/**
 * @brief Checks that the userdata at the specified index of the stack is a
 * game and returns it.
 * @param l a Lua context
 * @param index an index in the stack
 * @return the game
 */
Savegame& LuaContext::check_game(lua_State* l, int index) {
  return static_cast<Savegame&>(check_userdata(l, index, game_module_name));
}

/**
 * @brief Pushes a game userdata onto the stack.
 * @param l A Lua context.
 * @param game A game.
 */
void LuaContext::push_game(lua_State* l, Savegame& game) {
  push_userdata(l, game);
}

/**
 * @brief Implementation of \ref lua_api_game_exists.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::game_api_exists(lua_State* l) {

  const std::string& file_name = luaL_checkstring(l, 1);

  bool exists = FileTools::data_file_exists(
      FileTools::get_quest_write_dir() + "/" + file_name);

  lua_pushboolean(l, exists);
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_game_delete.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::game_api_delete(lua_State* l) {

  const std::string& file_name = luaL_checkstring(l, 1);

  FileTools::data_file_delete(
      FileTools::get_quest_write_dir() + "/" + file_name);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_game_load.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::game_api_load(lua_State* l) {

  const std::string& file_name = luaL_checkstring(l, 1);

  Savegame* savegame = new Savegame(get_lua_context(l).get_main_loop(), file_name);

  savegame->increment_refcount();
  savegame->get_equipment().load_items();
  savegame->decrement_refcount();

  push_game(l, *savegame);
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_game_save.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::game_api_save(lua_State* l) {

  Savegame& savegame = check_game(l, 1);
  savegame.save();

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_game_start.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::game_api_start(lua_State* l) {

  Savegame& savegame = check_game(l, 1);

  Game* game = savegame.get_game();
  if (game != NULL) {
    // A game is already running with this savegame: restart it.
    game->restart();
  }
  else {
    // Create a new game to run.
    MainLoop& main_loop = savegame.get_lua_context().get_main_loop();
    Game* game = new Game(main_loop, &savegame);
    main_loop.set_game(game);
  }

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_game_is_started.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::game_api_is_started(lua_State* l) {

  Savegame& savegame = check_game(l, 1);

  Game* game = savegame.get_game();
  bool is_started = game != NULL;

  lua_pushboolean(l, is_started);
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_game_is_suspended.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::game_api_is_suspended(lua_State* l) {

  Savegame& savegame = check_game(l, 1);

  Game* game = savegame.get_game();
  bool is_suspended = game != NULL && game->is_suspended();

  lua_pushboolean(l, is_suspended);
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_game_is_paused.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::game_api_is_paused(lua_State* l) {

  Savegame& savegame = check_game(l, 1);

  Game* game = savegame.get_game();
  bool is_paused = game != NULL && game->is_paused();

  lua_pushboolean(l, is_paused);
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_game_set_paused.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::game_api_set_paused(lua_State* l) {

  Savegame& savegame = check_game(l, 1);
  bool paused = true;
  if (lua_gettop(l) >= 2) {
    paused = lua_toboolean(l, 2);
  }

  Game* game = savegame.get_game();
  if (game != NULL) {
    game->set_paused(paused);
  }

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_game_is_showing_dialog.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::game_api_is_showing_dialog(lua_State* l) {

  Savegame& savegame = check_game(l, 1);

  Game* game = savegame.get_game();
  bool is_showing_dialog = game != NULL && game->is_showing_dialog();

  lua_pushboolean(l, is_showing_dialog);
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_game_get_map.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::game_api_get_map(lua_State* l) {

  Savegame& savegame = check_game(l, 1);

  Game* game = savegame.get_game();
  if (game == NULL || !game->has_current_map()) {
    lua_pushnil(l);
  }
  else {
    push_map(l, game->get_current_map());
  }
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_game_get_value.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::game_api_get_value(lua_State* l) {

  Savegame& savegame = check_game(l, 1);
  const std::string& key = luaL_checkstring(l, 2);

  if (!is_valid_lua_identifier(key)) {
    luaL_argerror(l, 3, (StringConcat() <<
        "Invalid savegame variable '" << key <<
        "': the name should only contain alphanumeric characters or '_'" <<
        " and cannot start with a digit").c_str());
  }

  if (savegame.is_boolean(key)) {
    lua_pushboolean(l, savegame.get_boolean(key));
  }
  else if (savegame.is_integer(key)) {
    lua_pushinteger(l, savegame.get_integer(key));
  }
  else if (savegame.is_string(key)) {
    lua_pushstring(l, savegame.get_string(key).c_str());
  }
  else {
    lua_pushnil(l);
  }

  return 1;
}

/**
 * @brief Implementation of \ref lua_api_game_set_value.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::game_api_set_value(lua_State* l) {

  Savegame& savegame = check_game(l, 1);
  const std::string& key = luaL_checkstring(l, 2);

  if (key[0] == '_') {
    luaL_argerror(l, 3, (StringConcat() <<
        "Invalid savegame variable '" << key <<
        "': names prefixed by '_' are reserved for built-in variables").c_str());
  }

  if (!is_valid_lua_identifier(key)) {
    luaL_argerror(l, 3, (StringConcat() <<
        "Invalid savegame variable '" << key <<
        "': the name should only contain alphanumeric characters or '_'" <<
        " and cannot start with a digit").c_str());
  }

  switch (lua_type(l, 3)) {

    case LUA_TBOOLEAN:
      savegame.set_boolean(key, lua_toboolean(l, 3));
      break;

    case LUA_TNUMBER:
      savegame.set_integer(key, lua_tointeger(l, 3));
      break;

    case LUA_TSTRING:
      savegame.set_string(key, lua_tostring(l, 3));
      break;

    default:
      luaL_argerror(l, 3, (StringConcat() <<
          "Expected string, number or boolean, got " << luaL_typename(l, 3)).c_str());
  }

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_game_get_starting_location.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::game_api_get_starting_location(lua_State* l) {

  Savegame& savegame = check_game(l, 1);

  push_string(l, savegame.get_string(Savegame::KEY_STARTING_MAP));
  push_string(l, savegame.get_string(Savegame::KEY_STARTING_POINT));
  return 2;
}

/**
 * @brief Implementation of \ref lua_api_game_set_starting_location.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::game_api_set_starting_location(lua_State* l) {

  Savegame& savegame = check_game(l, 1);
  const std::string& map_id = luaL_checkstring(l, 2);
  const std::string& destination_name = luaL_checkstring(l, 3);

  savegame.set_string(Savegame::KEY_STARTING_MAP, map_id);
  savegame.set_string(Savegame::KEY_STARTING_POINT, destination_name);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_game_get_life.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::game_api_get_life(lua_State* l) {

  Savegame& savegame = check_game(l, 1);

  int life = savegame.get_equipment().get_life();
  lua_pushinteger(l, life);
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_game_set_life.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::game_api_set_life(lua_State* l) {

  Savegame& savegame = check_game(l, 1);
  int life = luaL_checkinteger(l, 2);

  savegame.get_equipment().set_life(life);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_game_add_life.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::game_api_add_life(lua_State* l) {

  Savegame& savegame = check_game(l, 1);
  int life = luaL_checkinteger(l, 2);

  savegame.get_equipment().add_life(life);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_game_remove_life.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::game_api_remove_life(lua_State* l) {

  Savegame& savegame = check_game(l, 1);
  int life = luaL_checkinteger(l, 2);

  savegame.get_equipment().remove_life(life);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_game_get_max_life.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::game_api_get_max_life(lua_State* l) {

  Savegame& savegame = check_game(l, 1);

  int life = savegame.get_equipment().get_max_life();

  lua_pushinteger(l, life);
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_game_set_max_life.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::game_api_set_max_life(lua_State* l) {

  Savegame& savegame = check_game(l, 1);
  int life = luaL_checkinteger(l, 2);

  savegame.get_equipment().set_max_life(life);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_game_add_max_life.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::game_api_add_max_life(lua_State* l) {

  Savegame& savegame = check_game(l, 1);
  int life = luaL_checkinteger(l, 2);

  Equipment &equipment = savegame.get_equipment();
  equipment.set_max_life(equipment.get_max_life() + life);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_game_get_money.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::game_api_get_money(lua_State* l) {

  Savegame& savegame = check_game(l, 1);

  int money = savegame.get_equipment().get_money();

  lua_pushinteger(l, money);
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_game_set_money.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::game_api_set_money(lua_State* l) {

  Savegame& savegame = check_game(l, 1);
  int money = luaL_checkinteger(l, 2);

  savegame.get_equipment().set_money(money);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_game_add_money.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::game_api_add_money(lua_State* l) {

  Savegame& savegame = check_game(l, 1);
  int money = luaL_checkinteger(l, 2);

  savegame.get_equipment().add_money(money);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_game_remove_money.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::game_api_remove_money(lua_State* l) {

  Savegame& savegame = check_game(l, 1);
  int money = luaL_checkinteger(l, 2);

  savegame.get_equipment().remove_money(money);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_game_get_max_money.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::game_api_get_max_money(lua_State* l) {

  Savegame& savegame = check_game(l, 1);

  int money = savegame.get_equipment().get_max_money();

  lua_pushinteger(l, money);
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_game_set_max_money.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::game_api_set_max_money(lua_State* l) {

  Savegame& savegame = check_game(l, 1);
  int money = luaL_checkinteger(l, 2);

  savegame.get_equipment().set_max_money(money);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_game_get_magic.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::game_api_get_magic(lua_State* l) {

  Savegame& savegame = check_game(l, 1);

  int magic = savegame.get_equipment().get_magic();

  lua_pushinteger(l, magic);
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_game_set_magic.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::game_api_set_magic(lua_State* l) {

  Savegame& savegame = check_game(l, 1);
  int magic = luaL_checkinteger(l, 2);

  savegame.get_equipment().set_magic(magic);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_game_add_magic.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::game_api_add_magic(lua_State* l) {

  Savegame& savegame = check_game(l, 1);
  int magic = luaL_checkinteger(l, 2);

  savegame.get_equipment().add_magic(magic);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_game_remove_magic.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::game_api_remove_magic(lua_State* l) {

  Savegame& savegame = check_game(l, 1);
  int magic = luaL_checkinteger(l, 2);

  savegame.get_equipment().remove_magic(magic);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_game_get_max_magic.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::game_api_get_max_magic(lua_State* l) {

  Savegame& savegame = check_game(l, 1);

  int magic = savegame.get_equipment().get_max_magic();

  lua_pushinteger(l, magic);
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_game_set_max_magic.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::game_api_set_max_magic(lua_State* l) {

  Savegame& savegame = check_game(l, 1);
  int magic = luaL_checkinteger(l, 2);

  savegame.get_equipment().set_max_magic(magic);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_game_has_ability.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::game_api_has_ability(lua_State* l) {

  Savegame& savegame = check_game(l, 1);
  const std::string& ability_name = luaL_checkstring(l, 2);

  bool has_ability = savegame.get_equipment().has_ability(ability_name);

  lua_pushboolean(l, has_ability);
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_game_get_ability.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::game_api_get_ability(lua_State* l) {

  Savegame& savegame = check_game(l, 1);
  const std::string& ability_name = luaL_checkstring(l, 2);

  int ability_level = savegame.get_equipment().get_ability(ability_name);

  lua_pushinteger(l, ability_level);
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_game_set_ability.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::game_api_set_ability(lua_State* l) {

  Savegame& savegame = check_game(l, 1);
  const std::string& ability_name = luaL_checkstring(l, 2);
  int level = luaL_checkinteger(l, 3);

  savegame.get_equipment().set_ability(ability_name, level);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_game_get_item.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::game_api_get_item(lua_State* l) {

  Savegame& savegame = check_game(l, 1);
  const std::string& item_name = luaL_checkstring(l, 2);

  push_item(l, savegame.get_equipment().get_item(item_name));
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_game_get_item_assigned.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::game_api_get_item_assigned(lua_State* l) {

  Savegame& savegame = check_game(l, 1);
  int slot = luaL_checkinteger(l, 2);

  if (slot < 1 || slot > 2) {
    luaL_argerror(l, 2, "The item slot should be 1 or 2");
  }

  EquipmentItem* item = savegame.get_equipment().get_item_assigned(slot);

  if (item == NULL) {
    lua_pushnil(l);
  }
  else {
    push_item(l, *item);
  }
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_game_set_item_assigned.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::game_api_set_item_assigned(lua_State* l) {

  Savegame& savegame = check_game(l, 1);
  int slot = luaL_checkinteger(l, 2);
  EquipmentItem* item = NULL;
  if (!lua_isnil(l, 3)) {
    item = &check_item(l, 3);
  }

  if (slot < 1 || slot > 2) {
    luaL_argerror(l, 2, "The item slot should be 1 or 2");
  }

  savegame.get_equipment().set_item_assigned(slot, item);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_game_is_command_pressed.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::game_api_is_command_pressed(lua_State* l) {

  Savegame& savegame = check_game(l, 1);
  GameCommands::Command command = check_enum<GameCommands::Command>(
      l, 2, GameCommands::command_names);

  GameCommands& commands = savegame.get_game()->get_commands();
  lua_pushboolean(l, commands.is_command_pressed(command));

  return 1;
}

/**
 * @brief Implementation of \ref lua_api_game_get_commands_direction.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::game_api_get_commands_direction(lua_State* l) {

  Savegame& savegame = check_game(l, 1);

  GameCommands& commands = savegame.get_game()->get_commands();
  lua_pushinteger(l, commands.get_wanted_direction8());

  return 1;
}

/**
 * @brief Implementation of \ref lua_api_game_get_command_effect.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::game_api_get_command_effect(lua_State* l) {

  Savegame& savegame = check_game(l, 1);
  GameCommands::Command command = check_enum<GameCommands::Command>(
      l, 2, GameCommands::command_names);

  Game* game = savegame.get_game();
  if (game == NULL) {
    lua_pushnil(l);
  }
  else {

    std::string effect_name;
    switch (command) {

      case GameCommands::ACTION:
      {
        KeysEffect::ActionKeyEffect effect = game->get_keys_effect().get_action_key_effect();
        effect_name = KeysEffect::get_action_key_effect_name(effect);
        break;
      }

      case GameCommands::ATTACK:
      {
        KeysEffect::SwordKeyEffect effect = game->get_keys_effect().get_sword_key_effect();
        effect_name = KeysEffect::get_sword_key_effect_name(effect);
        break;
      }

      case GameCommands::ITEM_1:
      {
        effect_name = game->is_suspended() ? "" : "use_item_1";
        break;
      }

      case GameCommands::ITEM_2:
      {
        effect_name = game->is_suspended() ? "" : "use_item_2";
        break;
      }

      case GameCommands::PAUSE:
      {
        KeysEffect::PauseKeyEffect effect = game->get_keys_effect().get_pause_key_effect();
        effect_name = KeysEffect::get_pause_key_effect_name(effect);
        break;
      }

      case GameCommands::RIGHT:
      {
        effect_name = game->is_suspended() ? "" : "move_right";
        break;
      }

      case GameCommands::UP:
      {
        effect_name = game->is_suspended() ? "" : "move_up";
        break;
      }

      case GameCommands::LEFT:
      {
        effect_name = game->is_suspended() ? "" : "move_left";
        break;
      }

      case GameCommands::DOWN:
      {
        effect_name = game->is_suspended() ? "" : "move_down";
        break;
      }

      default:
        Debug::die(StringConcat() << "Invalid game command: " << command);
    }

    if (effect_name.empty()) {
      lua_pushnil(l);
    }
    else {
      push_string(l, effect_name);
    }
  }

  return 1;
}

/**
 * @brief Implementation of \ref lua_api_game_get_command_keyboard_binding.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::game_api_get_command_keyboard_binding(lua_State* l) {

  Savegame& savegame = check_game(l, 1);
  GameCommands::Command command = check_enum<GameCommands::Command>(
      l, 2, GameCommands::command_names);

  GameCommands& commands = savegame.get_game()->get_commands();
  InputEvent::KeyboardKey key = commands.get_keyboard_binding(command);
  const std::string& key_name = InputEvent::get_keyboard_key_name(key);

  if (key_name.empty()) {
    lua_pushnil(l);
  }
  else {
    push_string(l, key_name);
  }
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_game_set_command_keyboard_binding.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::game_api_set_command_keyboard_binding(lua_State* l) {

  Savegame& savegame = check_game(l, 1);
  GameCommands::Command command = check_enum<GameCommands::Command>(
      l, 2, GameCommands::command_names);
  if (lua_gettop(l) <= 3) {
    luaL_typerror(l, 3, "string or nil");
  }
  const std::string& key_name = luaL_optstring(l, 3, "");

  GameCommands& commands = savegame.get_game()->get_commands();
  InputEvent::KeyboardKey key = InputEvent::get_keyboard_key_by_name(key_name);
  if (!key_name.empty() && key == InputEvent::KEY_NONE) {
    luaL_argerror(l, 3, (StringConcat() <<
          "Invalid keyboard key name: '" << key_name << "'").c_str());
  }
  commands.set_keyboard_binding(command, key);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_game_get_command_joypad_binding.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::game_api_get_command_joypad_binding(lua_State* l) {

  Savegame& savegame = check_game(l, 1);
  GameCommands::Command command = check_enum<GameCommands::Command>(
      l, 2, GameCommands::command_names);

  GameCommands& commands = savegame.get_game()->get_commands();
  const std::string& joypad_string = commands.get_joypad_binding(command);

  if (joypad_string.empty()) {
    lua_pushnil(l);
  }
  else {
    push_string(l, joypad_string);
  }
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_game_set_command_joypad_binding.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::game_api_set_command_joypad_binding(lua_State* l) {

  Savegame& savegame = check_game(l, 1);
  GameCommands::Command command = check_enum<GameCommands::Command>(
      l, 2, GameCommands::command_names);
  if (lua_gettop(l) <= 3) {
    luaL_typerror(l, 3, "string or nil");
  }
  const std::string& joypad_string = luaL_optstring(l, 3, "");

  if (!joypad_string.empty() && !GameCommands::is_joypad_string_valid(joypad_string)) {
    luaL_argerror(l, 3, (StringConcat() <<
          "Invalid joypad string: '" << joypad_string << "'").c_str());
  }
  GameCommands& commands = savegame.get_game()->get_commands();
  commands.set_joypad_binding(command, joypad_string);

  return 0;
}

/**
 * @brief Calls the on_update() method of a Lua game.
 * @param game A game.
 */
void LuaContext::game_on_update(Game& game) {

  push_game(l, game.get_savegame());
  on_update();
  menus_on_update(-1);
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_draw() method of a Lua game.
 * @param game A game.
 * @param dst_surface The destination surface.
 */
void LuaContext::game_on_draw(Game& game, Surface& dst_surface) {

  push_game(l, game.get_savegame());
  menus_on_draw(-1, dst_surface);
  on_draw(dst_surface);
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_started() method of a Lua game.
 * @param game A game.
 */
void LuaContext::game_on_started(Game& game) {

  push_game(l, game.get_savegame());
  on_started();
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_finished() method of a Lua game.
 * @param game A game.
 */
void LuaContext::game_on_finished(Game& game) {

  push_game(l, game.get_savegame());
  on_finished();
  remove_timers(-1);  // Stop timers and menus associated to this game.
  remove_menus(-1);
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_changed() method of a Lua game.
 * @param game A game.
 * @param map The new active map.
 */
void LuaContext::game_on_map_changed(Game& game, Map& map) {

  push_game(l, game.get_savegame());
  on_map_changed(map);
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_paused() method of a Lua game.
 * @param game A game.
 */
void LuaContext::game_on_paused(Game& game) {

  push_game(l, game.get_savegame());
  on_paused();
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_unpaused() method of a Lua game.
 * @param game A game.
 */
void LuaContext::game_on_unpaused(Game& game) {

  push_game(l, game.get_savegame());
  on_unpaused();
  lua_pop(l, 1);
}

/**
 * @brief Notifies a Lua game that an input event has just occurred.
 *
 * The appropriate callback in the game is triggered if it exists.
 *
 * @param game A game.
 * @param event The input event to handle.
 * @return \c true if the event was handled and should stop being propagated.
 */
bool LuaContext::game_on_input(Game& game, InputEvent& event) {

  bool handled = false;
  push_game(l, game.get_savegame());
  handled = on_input(event);
  if (!handled) {
    handled = menus_on_input(-1, event);
  }
  lua_pop(l, 1);
  return handled;
}

/**
 * @brief Calls the on_command_pressed() method of a Lua game.
 * @param game A game.
 * @return \c true if the event was handled and should stop being propagated.
 */
bool LuaContext::game_on_command_pressed(Game& game, GameCommands::Command command) {

  bool handled = false;
  push_game(l, game.get_savegame());
  handled = on_command_pressed(command);
  if (!handled) {
    handled = menus_on_command_pressed(-1, command);
  }
  lua_pop(l, 1);
  return handled;
}

/**
 * @brief Calls the on_command_released() method of a Lua game.
 * @param game A game.
 * @return \c true if the event was handled and should stop being propagated.
 */
bool LuaContext::game_on_command_released(Game& game, GameCommands::Command command) {

  bool handled = false;
  push_game(l, game.get_savegame());
  handled = on_command_released(command);
  if (!handled) {
    handled = menus_on_command_released(-1, command);
  }
  lua_pop(l, 1);
  return handled;
}

