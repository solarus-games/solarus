/*
 * Copyright (C) 2006-2013 Christopho, Solarus - http://www.solarus-games.org
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
#include "lowlevel/FileTools.h"

const std::string LuaContext::file_module_name = "sol.file";

/**
 * \brief Initializes the file features provided to Lua.
 */
void LuaContext::register_file_module() {

  static const luaL_Reg functions[] = {
      { "open", file_api_open },
      { "exists", file_api_exists },
      { "remove", file_api_remove },
      { "mkdir", file_api_mkdir },
      { NULL, NULL }
  };
  register_functions(file_module_name, functions);

  // Store the original io.open function in the registry.
  // We will need to access it from sol.file.open().
  lua_getglobal(l, "io");
  lua_getfield(l, -1, "open");
  Debug::check_assertion(lua_isfunction(l, -1), "Could not find io.open");
  lua_setfield(l, LUA_REGISTRYINDEX, "io.open");
}

/**
 * \brief Destroys any temporary files previously created.
 */
void LuaContext::destroy_files() {

}

/**
 * \brief Implementation of sol.file.open().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::file_api_open(lua_State* l) {

  // TODO
  return 0;
}

/**
 * \brief Implementation of sol.file.exists().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::file_api_exists(lua_State* l) {

  const std::string& file_name = luaL_checkstring(l, 1);

  lua_pushboolean(l, FileTools::data_file_exists(file_name, false));

  return 1;
}

/**
 * \brief Implementation of sol.file.remove().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::file_api_remove(lua_State* l) {

  const std::string& file_name = luaL_checkstring(l, 1);

  bool success = FileTools::data_file_delete(file_name);

  if (!success) {
    lua_pushnil(l);
    push_string(l, std::string("Failed to delete file '") + file_name + "'");
    return 2;
  }

  lua_pushboolean(l, true);
  return 1;
}

/**
 * \brief Implementation of sol.file.mkdir().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::file_api_mkdir(lua_State* l) {

  const std::string& dir_name = luaL_checkstring(l, 1);

  bool success = FileTools::data_file_mkdir(dir_name);

  if (!success) {
    lua_pushnil(l);
    push_string(l, std::string("Failed to create directory '") + dir_name + "'");
    return 2;
  }

  lua_pushboolean(l, true);
  return 1;
}

