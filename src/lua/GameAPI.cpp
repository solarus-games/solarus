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
      { "get_map", game_api_get_map },
      { "get_value", game_api_get_value },
      { "set_value", game_api_set_value },
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
      { NULL, NULL }
  };
  static const luaL_Reg metamethods[] = {
      { "__eq", userdata_meta_eq },
      { "__gc", userdata_meta_gc },
      { "__newindex", userdata_meta_newindex_as_table },
      { "__index", userdata_meta_index_as_table },
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

  bool exists = FileTools::data_file_exists(file_name);

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

  FileTools::data_file_delete(file_name);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_game_load.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::game_api_load(lua_State* l) {

  const std::string& file_name = luaL_checkstring(l, 1);

  Savegame* savegame = new Savegame(file_name);

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
    MainLoop& main_loop = get_lua_context(l).get_main_loop();
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
 * @brief Implementation of \ref lua_api_game_get_map.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int LuaContext::game_api_get_map(lua_State* l) {

  Savegame& savegame = check_game(l, 1);

  Game* game = savegame.get_game();
  if (game == NULL) {
    lua_pushnil(l);
  }
  else {
    push_map(l, savegame.get_game()->get_current_map());
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
  const std::string& savegame_variable = luaL_checkstring(l, 2);

  if (savegame_variable[0] == '_') {
    luaL_argerror(l, 3, (StringConcat() <<
        "Invalid savegame variable '" << savegame_variable <<
        "': names prefixed by '_' are reserved for built-in variables").c_str());
  }

  switch (lua_type(l, 3)) {

    case LUA_TSTRING:
      savegame.set_string(savegame_variable, lua_tostring(l, 3));
      break;

    case LUA_TNUMBER:
      savegame.set_integer(savegame_variable, lua_tointeger(l, 3));
      break;

    case LUA_TBOOLEAN:
      savegame.set_integer(savegame_variable, lua_toboolean(l, 3));
      break;

    default:
      luaL_argerror(l, 3, (StringConcat() <<
          "Expected string, number or boolean, got " << luaL_typename(l, 3)).c_str());
  }

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
 * @brief Notifies a Lua game that an input event has just occurred.
 *
 * The appropriate callback in the game is triggered if it exists.
 *
 * @param event The input event to handle.
 * @param game A game.
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

