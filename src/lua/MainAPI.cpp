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
#include "lowlevel/Geometry.h"
#include "Timer.h"
#include "CustomScreen.h"
#include <lua.hpp>
#include <sstream>
#include <cmath>

const std::string Script::main_module_name = "sol.main";

/**
 * @brief Initializes the main features provided to Lua.
 */
void Script::register_main_module() {

  static const luaL_Reg functions[] = {
      { "include", main_api_include },
      { "exit", main_api_exit },
      { "start_screen", main_api_start_screen },
      { "get_distance", main_api_get_distance },
      { "get_angle", main_api_get_angle },
      { NULL, NULL }
  };
  register_functions(main_module_name, functions);
}

/**
 * @brief Implementation of \ref lua_api_main_include.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int Script::main_api_include(lua_State *l) {

  Script& script = get_script(l);
  const std::string& file_name = luaL_checkstring(l, 1);

  script.load(file_name);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_main_exit.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int Script::main_api_exit(lua_State* l) {

  Script& script = get_script(l);

  script.get_screen().exit();

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_main_start_screen.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int Script::main_api_start_screen(lua_State* l) {

  Script& script = get_script(l);
  const std::string screen_name = luaL_checkstring(l, 1);

  script.get_screen().start_screen(screen_name);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_main_get_distance.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
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
 * @brief Implementation of \ref lua_api_main_get_angle.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
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

