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
#include "lowlevel/FileTools.h"
#include "StringResource.h"
#include <lua.hpp>

const std::string LuaContext::language_module_name = "sol.language";

/**
 * @brief Initializes the language features provided to Lua.
 */
void LuaContext::register_language_module() {

  static const luaL_Reg functions[] = {
      { "get_language", language_api_get_language },
      { "set_language", language_api_set_language },
      { "get_language_name", language_api_get_language_name },
      { "get_languages", language_api_get_languages },
      { "get_default_language", language_api_get_default_language },
      { "get_string", language_api_get_string },
      { "get_dialog", language_api_get_dialog },
      { NULL, NULL }
  };
  register_functions(language_module_name, functions);
}

/**
 * @brief Implementation of \ref lua_api_language_get_language.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::language_api_get_language(lua_State* l) {

  const std::string& language = FileTools::get_language();

  if (language.empty()) {  // Return nil if no language is set.
    lua_pushnil(l);
  }
  else  {  // Return the language code.
    push_string(l, language);
  }
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_language_set_language.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::language_api_set_language(lua_State* l) {

  const std::string& language_code = luaL_checkstring(l, 1);

  FileTools::set_language(language_code);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_language_get_language_name.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::language_api_get_language_name(lua_State* l) {

  std::string language_code;
  if (lua_gettop(l) >= 1) {
    language_code = luaL_checkstring(l, 1);
  }
  else {
    language_code = FileTools::get_language();
  }

  const std::map<std::string, std::string>& languages =
    FileTools::get_languages();
  if (languages.count(language_code) == 0) {
    lua_pushnil(l);
  }
  else {
    const std::string& name = languages.find(language_code)->second;
    push_string(l, name);
  }

  return 1;
}

/**
 * @brief Implementation of \ref lua_api_language_get_languages.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::language_api_get_languages(lua_State* l) {

  const std::map<std::string, std::string>& languages =
    FileTools::get_languages();

  lua_newtable(l);

  std::map<std::string, std::string>::const_iterator it;
  int i = 1;
  for (it = languages.begin(); it != languages.end(); it++) {
    const std::string& language_code = it->first;
    push_string(l, language_code);
    lua_rawseti(l, -2, i);
    ++i;
  }

  return 1;
}

/**
 * @brief Implementation of \ref lua_api_language_get_default_language.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::language_api_get_default_language(lua_State* l) {

  const std::string& language = FileTools::get_default_language();

  push_string(l, language);
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_language_get_string.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::language_api_get_string(lua_State* l) {

  const std::string& key = luaL_checkstring(l, 1);

  push_string(l, StringResource::get_string(key));

  return 1;
}

/**
 * @brief Implementation of \ref lua_api_language_get_dialog.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::language_api_get_dialog(lua_State* l) {

  // TODO
  lua_pushnil(l);

  return 1;
}

