/*
 * Copyright (C) 2006-2018 Christopho, Solarus - http://www.solarus-games.org
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
#include "solarus/core/CurrentQuest.h"
#include "solarus/core/QuestFiles.h"
#include "solarus/lua/LuaContext.h"
#include "solarus/lua/LuaTools.h"
#include <vector>

namespace Solarus {

/**
 * Name of the Lua table representing the language module.
 */
const std::string LuaContext::language_module_name = "sol.language";

/**
 * \brief Pushes a dialog onto the stack.
 * \param l A Lua context.
 * \param dialog A dialog.
 */
void LuaContext::push_dialog(lua_State* l, const Dialog& dialog) {

  lua_newtable(l);

  // Dialog id.
  push_string(l, dialog.get_id());
  lua_setfield(l, -2, "id");

  // Text.
  push_string(l, dialog.get_text());
  lua_setfield(l, -2, "text");

  // User properties.
  const std::map<std::string, std::string>& properties =
      dialog.get_properties();
  for (const auto& kvp: properties) {
    push_string(l, kvp.second);
    lua_setfield(l, -2, kvp.first.c_str());
  }
}

/**
 * \brief Initializes the language features provided to Lua.
 */
void LuaContext::register_language_module() {

  const std::vector<luaL_Reg> functions = {
      { "get_language", language_api_get_language },
      { "set_language", language_api_set_language },
      { "get_language_name", language_api_get_language_name },
      { "get_languages", language_api_get_languages },
      { "get_string", language_api_get_string },
      { "get_dialog", language_api_get_dialog }
  };

  register_functions(language_module_name, functions);
}

/**
 * \brief Implementation of sol.language.get_language().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::language_api_get_language(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const std::string& language = CurrentQuest::get_language();

    if (language.empty()) {  // Return nil if no language is set.
      lua_pushnil(l);
    }
    else  {  // Return the language code.
      push_string(l, language);
    }
    return 1;
  });
}

/**
 * \brief Implementation of sol.language.set_language().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::language_api_set_language(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const std::string& language_code = LuaTools::check_string(l, 1);

    if (!CurrentQuest::has_language(language_code)) {
      LuaTools::arg_error(l, 1, std::string("No such language: '") + language_code + "'");
    }
    CurrentQuest::set_language(language_code);

    return 0;
  });
}

/**
 * \brief Implementation of sol.language.get_language_name().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::language_api_get_language_name(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    std::string language_code;
    if (lua_gettop(l) >= 1) {
      language_code = LuaTools::check_string(l, 1);
      if (!CurrentQuest::has_language(language_code)) {
        LuaTools::arg_error(l, 1, std::string("No such language: '") + language_code + "'");
      }
    }
    else {
      language_code = CurrentQuest::get_language();
      if (language_code.empty()) {
        LuaTools::error(l, "No language is set");
      }
    }

    const std::string& name = CurrentQuest::get_language_name(language_code);
    push_string(l, name);

    return 1;
  });
}

/**
 * \brief Implementation of sol.language.get_languages().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::language_api_get_languages(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const std::map<std::string, std::string>& languages =
        CurrentQuest::get_resources(ResourceType::LANGUAGE);

    lua_newtable(l);
    int i = 1;
    for (const auto& kvp: languages) {
      const std::string& language_code = kvp.first;
      push_string(l, language_code);
      lua_rawseti(l, -2, i);
      ++i;
    }

    return 1;
  });
}

/**
 * \brief Implementation of sol.language.get_string().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::language_api_get_string(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const std::string& key = LuaTools::check_string(l, 1);

    if (!CurrentQuest::string_exists(key)) {
      lua_pushnil(l);
    }
    else {
      push_string(l, CurrentQuest::get_string(key));
    }
    return 1;
  });
}

/**
 * \brief Implementation of sol.language.get_dialog().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::language_api_get_dialog(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const std::string& dialog_id = LuaTools::check_string(l, 1);

    if (!CurrentQuest::dialog_exists(dialog_id)) {
      lua_pushnil(l);
    }
    else {
      push_dialog(l, CurrentQuest::get_dialog(dialog_id));
    }
    return 1;
  });
}

}

