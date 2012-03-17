/*
 * Copyright (C) 2006-2012 Christopho, Solarus - http://www.solarus-engine.org
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
#include "lowlevel/Geometry.h"
#include "Timer.h"
#include "CustomScreen.h"
#include <lua.hpp>
#include <sstream>
#include <cmath>

const char* Script::main_module_name = "sol.main";

/**
 * @brief Initializes the main features provided to Lua.
 */
void Script::initialize_main_module() {

  static const luaL_Reg methods[] = {
      { "include", main_api_include },
      { "exit", main_api_exit },
      { "start_screen", main_api_start_screen },
      { "start_game", main_api_start_game },
      { "get_distance", main_api_get_distance },
      { "get_angle", main_api_get_angle },
      { NULL, NULL }
  };

  luaL_register(l, main_module_name, methods);
}

/**
 * @brief Includes a script into the current Lua context.
 *
 * - Argument 1 (string): file name of the script without extension,
 *   relative to the data directory of the quest
 *
 * @param l the Lua context that is calling this function
 */
int Script::main_api_include(lua_State *l) {

  Script& script = get_script(l);
  const std::string& file_name = luaL_checkstring(l, 1);

  script.load(file_name);

  return 0;
}

/**
 * @brief Stops the program.
 * @param l the Lua context that is calling this function
 * @return the number of values to return to Lua
 */
int Script::main_api_exit(lua_State* l) {

  Script& script = get_script(l);

  script.get_screen().exit();

  return 0;
}

/**
 * @brief Ends the current screen and starts another screen.
 *
 * - Argument 1 (string): name of the screen to start
 *
 * @param l the Lua context that is calling this function
 * @return the number of values to return to Lua
 */
int Script::main_api_start_screen(lua_State* l) {

  Script& script = get_script(l);
  const std::string screen_name = luaL_checkstring(l, 1);

  script.get_screen().start_screen(screen_name);

  return 0;
}

/**
 * @brief Ends the current screen and starts a game.
 *
 * - Argument 1 (string): name of the savegame file to load
 *
 * @param l the Lua context that is calling this function
 * @return the number of values to return to Lua
 */
int Script::main_api_start_game(lua_State* l) {

  Script& script = get_script(l);
  const std::string savegame_file = luaL_checkstring(l, 1);

  script.get_screen().start_game(savegame_file);

  return 0;
}

/**
 * @brief Returns the distance in pixels between two points.
 *
 * - Argument 1 (integer): x coordinate of the first point
 * - Argument 2 (integer): y coordinate of the first point
 * - Argument 3 (integer): x coordinate of the second point
 * - Argument 4 (integer): y coordinate of the second point
 * - Return value (integer): the distance in pixels
 *
 * @param l the Lua context that is calling this function
 */
int Script::main_api_get_distance(lua_State *l) {

  int x1 = luaL_checkinteger(l, 1);
  int y1 = luaL_checkinteger(l, 2);
  int x2 = luaL_checkinteger(l, 3);
  int y2 = luaL_checkinteger(l, 4);

  int distance = (int) Geometry::get_distance(x1, y1, x2, y2);
  lua_pushinteger(l, distance);

  return 1;
}

/**
 * @brief Returns the angle between the specified vector and the x axis.
 *
 * - Argument 1 (integer): x coordinate of the first point of the vector
 * - Argument 2 (integer): y coordinate of the first point of the vector
 * - Argument 3 (integer): x coordinate of the second point of the vector
 * - Argument 4 (integer): y coordinate of the second point of the vector
 * - Return value (float): the angle between the vector and the x axis in radians
 *
 * @param l the Lua context that is calling this function
 */
int Script::main_api_get_angle(lua_State *l) {

  int x1 = luaL_checkinteger(l, 1);
  int y1 = luaL_checkinteger(l, 2);
  int x2 = luaL_checkinteger(l, 3);
  int y2 = luaL_checkinteger(l, 4);

  double angle = Geometry::get_angle(x1, y1, x2, y2);
  lua_pushnumber(l, angle);

  return 1;
}

