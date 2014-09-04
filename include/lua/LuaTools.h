/*
 * Copyright (C) 2006-2014 Christopho, Solarus - http://www.solarus-games.org
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
#ifndef SOLARUS_LUA_TOOLS_H
#define SOLARUS_LUA_TOOLS_H

#include "Common.h"
#include "entities/Layer.h"
#include "lowlevel/Debug.h"
#include "lua/LuaException.h"
#include <string>
#include <lua.hpp>

/**
 * \brief Try block to be used at the Lua to C++ boundary.
 *
 * The whole code of any function that can directly be called from Lua must be
 * protected by a SOLARUS_LUA_API_TRY() block, including the declaration of
 * variables and the return instructions.
 */
#define SOLARUS_LUA_BOUNDARY_TRY() try

/**
 * \brief Catch block to be used at the Lua to C++ boundary.
 *
 * This macro is meant to be called at the end of in any C++ function that
 * can directly be called be Lua, after the SOLARUS_LUA_API_TRY() block.
 * Any exception is caught and generates a Lua error instead.
 *
 * \param l The Lua state.
 */
#define SOLARUS_LUA_BOUNDARY_CATCH(l)\
    catch (const LuaException& ex) {\
      luaL_error(l, ex.what());\
    }\
    catch (const std::exception& ex) {\
      luaL_error(l, (std::string("Unexpected exception: ") + ex.what()).c_str());\
    }\
    catch (...) {\
      luaL_error(l, "Unexpected exception");\
    }\
    return 0;

namespace solarus {

/**
 * \brief Provides general Lua helpers functions.
 *
 * All check_* and opt_* functions are similar to their equivalent of the
 * Lua API, but throw an LuaException in case of error instead of calling
 * lua_error(). This guarantees that stack-allocated objects are properly
 * destroyed even in case of error.
 */
class LuaTools {

  public:

    // Helpers.
    static int get_positive_index(lua_State* l, int index);
    static bool is_valid_lua_identifier(const std::string& name);

    // Error handling.
    static void error(
        lua_State* l,
        const std::string& message
    );
    static void arg_error(
        lua_State* l,
        int arg_index,
        const std::string& message
    );
    static void type_error(
        lua_State* l,
        int arg_index,
        const std::string& expected_type_name
    );
    static void check_type(
        lua_State* l,
        int arg_index,
        int expected_type
    );
    static void check_any(
        lua_State* l,
        int arg_index
    );

    // int
    static int check_int(
        lua_State* l,
        int index
    );
    static int check_int_field(
        lua_State* l,
        int table_index,
        const std::string& key
    );
    static int opt_int(
        lua_State* l,
        int index,
        int default_value
    );
    static int opt_int_field(
        lua_State* l,
        int table_index,
        const std::string& key,
        int default_value
    );

    // double
    static double check_number(
        lua_State* l,
        int index
    );
    static double check_number_field(
        lua_State* l,
        int table_index,
        const std::string& key
    );
    static double opt_number(
        lua_State* l,
        int index,
        double default_value
    );
    static double opt_number_field(
        lua_State* l, int table_index, const std::string& key,
        double default_value
    );

    // std::string
    static std::string check_string(
        lua_State* l,
        int index
    );
    static std::string check_string_field(
        lua_State* l,
        int table_index,
        const std::string& key
    );
    static std::string opt_string(
        lua_State* l,
        int index,
        const std::string& default_value
    );
    static std::string opt_string_field(
        lua_State* l,
        int table_index,
        const std::string& key,
        const std::string& default_value
    );

    // bool
    static bool check_boolean(
        lua_State* l,
        int index
    );
    static bool check_boolean_field(
        lua_State* l,
        int table_index,
        const std::string& key
    );
    static bool opt_boolean(
        lua_State* l,
        int index,
        bool default_value
    );
    static bool opt_boolean_field(
        lua_State* l,
        int table_index,
        const std::string& key,
        bool default_value
    );

    // function
    static int check_function(  // TODO shared_ptr return LuaScopedRef
        lua_State* l,
        int index
    );
    static int check_function_field(
        lua_State* l,
        int table_index,
        const std::string& key
    );
    static int opt_function(
        lua_State* l,
        int index
    );
    static int opt_function_field(
        lua_State* l,
        int table_index,
        const std::string& key
    );

    // Layer
    static bool is_layer(
        lua_State* l,
        int index
    );
    static Layer check_layer(
        lua_State* l,
        int index
    );
    static Layer check_layer_field(
        lua_State* l,
        int table_index,
        const std::string& key
    );
    static Layer opt_layer(
        lua_State* l,
        int index,
        Layer default_value
    );
    static Layer opt_layer_field(
        lua_State* l,
        int table_index,
        const std::string& key,
        Layer default_value
    );

    // Color
    static bool is_color(
        lua_State* l,
        int index
    );
    static Color check_color(
        lua_State* l,
        int index
    );
    static Color check_color_field(
        lua_State* l,
        int table_index,
        const std::string& key
    );
    static Color opt_color(
        lua_State* l,
        int index,
        const Color& default_value
    );
    static Color opt_color_field(
        lua_State* l,
        int table_index,
        const std::string& key,
        const Color& default_value
    );

    // enum
    template<typename E>
    static E check_enum(
        lua_State* l, int index, const std::string names[]
    );
    template<typename E>
    static E check_enum_field(
        lua_State* l,
        int table_index,
        const std::string& key,
        const std::string names[]
    );
    template<typename E>
    static E opt_enum(
        lua_State* l, int index, const std::string names[], E default_value
    );
    template<typename E>
    static E opt_enum_field(
        lua_State* l,
        int table_index,
        const std::string& key,
        const std::string names[],
        E default_value
    );

  private:

    LuaTools() = delete;  // Don't instantiate this class.

};

/**
 * \brief Checks whether a value is the name of an enumeration value and
 * returns this value.
 *
 * Throws a LuaException if the value is not a string or if the string cannot
 * be found in the array.
 * This is a useful function for mapping strings to C enums.
 *
 * This function is similar to luaL_checkoption() except that it accepts an
 * array of std::string instead of char*, and returns a value of enumerated
 * type E instead of int.
 *
 * \param l A Lua state.
 * \param index Index of a string in the Lua stack.
 * \param names An array of strings to search in. This array must be
 * terminated by an empty string. TODO std::array
 * \return The index (converted to the enumerated type E) where the string was
 * found in the array.
 */
template<typename E>
E LuaTools::check_enum(
    lua_State* l,
    int index,
    const std::string names[]
) {
  Debug::check_assertion(!names[0].empty(), "Invalid list of names");

  const std::string& name = luaL_checkstring(l, index);
  for (int i = 0; !names[i].empty(); ++i) {
    if (names[i] == name) {
      return E(i);
    }
  }

  // The value was not found. Build an error message with possible values.
  std::string allowed_names;
  for (int i = 0; !names[i].empty(); ++i) {
    allowed_names += "\"" + names[i] + "\", ";
  }
  allowed_names = allowed_names.substr(0, allowed_names.size() - 2);

  arg_error(l, index,
      std::string("Invalid name '") + name + "'. Allowed names are: "
      + allowed_names
  );
  throw;  // Make sure the compiler is happy.
}

/**
 * \brief Checks that a table field is the name of an enumeration value and
 * returns this value.
 *
 * This function acts like lua_getfield() followed by check_enum().
 *
 * \param l A Lua state.
 * \param table_index Index of a table in the stack.
 * \param key Key of the field to get in that table.
 * \param names An array of strings to search in. This array must be
 * terminated by an empty string.
 * \return The index (converted to the enumerated type E) where the string was
 * found in the array.
 */
template<typename E>
E LuaTools::check_enum_field(
    lua_State* l, int table_index, const std::string& key,
    const std::string names[]) {

  lua_getfield(l, table_index, key.c_str());
  if (!lua_isstring(l, -1)) {
    arg_error(l, table_index,
        std::string("Bad field '") + key + "' (string expected, got "
        + luaL_typename(l, -1)
    );
  }

  E value = check_enum<E>(l, -1, names);
  lua_pop(l, 1);
  return value;
}

/**
 * \brief Like LuaTools::check_enum() but with a default value.
 *
 * \param l A Lua state.
 * \param index Index of a string in the Lua stack.
 * \param names An array of strings to search in. This array must be
 * terminated by an empty string.
 * \param default_value The default value to return.
 * \return The index (converted to the enumerated type E) where the string was
 * found in the array.
 */
template<typename E>
E LuaTools::opt_enum(
    lua_State* l,
    int index,
    const std::string names[],
    E default_value
) {
  if (lua_isnoneornil(l, index)) {
    return default_value;
  }

  return check_enum<E>(l, index, names);
}

/**
 * \brief Like LuaTools::check_enum_field() but with a default value.
 * \param l A Lua state.
 * \param table_index Index of a table in the stack.
 * \param key Key of the field to get in that table.
 * \param names An array of strings to search in. This array must be
 * terminated by an empty string.
 * \param default_value The default value to return.
 * \return The index (converted to the enumerated type E) where the string was
 * found in the array.
 */
template<typename E>
E LuaTools::opt_enum_field(
    lua_State* l,
    int table_index,
    const std::string& key,
    const std::string names[],
    E default_value
) {
  lua_getfield(l, table_index, key.c_str());
  if (lua_isnil(l, -1)) {
    return default_value;
  }

  if (!lua_isstring(l, -1)) {
    arg_error(l, table_index,
        std::string("Bad field '") + key + "' (string expected, got "
        + luaL_typename(l, -1) + ")"
    );
  }
  return check_enum<E>(l, -1, names);
}

}

#endif

