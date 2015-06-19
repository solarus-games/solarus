/*
 * Copyright (C) 2006-2015 Christopho, Solarus - http://www.solarus-games.org
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
#include "solarus/lua/LuaTools.h"
#include "solarus/lowlevel/Color.h"
#include "solarus/lua/LuaException.h"
#include "solarus/lua/ScopedLuaRef.h"
#include <sstream>

namespace Solarus {
namespace LuaTools {

/**
 * \brief For an index in the Lua stack, returns an equivalent positive index.
 *
 * Pseudo-indexes are unchanged.
 *
 * \param l A Lua state.
 * \param index An index in the stack.
 * \return The corresponding positive index.
 */
int get_positive_index(lua_State* l, int index) {

  int positive_index = index;
  if (index < 0 && index >= -lua_gettop(l)) {
    positive_index = lua_gettop(l) + index + 1;
  }

  return positive_index;
}

/**
 * \brief Returns whether the specified name is a valid Lua identifier.
 * \param name The name to check.
 * \return true if the name only contains alphanumeric characters or '_' and
 * does not start with a digit.
 */
bool is_valid_lua_identifier(const std::string& name) {

  if (name.empty() || (name[0] >= '0' && name[0] <= '9')) {
    return false;
  }

  for (char character: name) {
    if (character != '_' &&
        !(character >= 'a' && character <= 'z') &&
        !(character >= 'A' && character <= 'Z') &&
        !(character >= '0' && character <= '9')) {
      return false;
    }
  }
  return true;
}

/**
 * \brief Creates a reference to the Lua value on top of the stack and pops
 * this value.
 * \param l A Lua state.
 * \return The reference created, wrapped in an object that manages its
 * lifetime.
 */
ScopedLuaRef create_ref(lua_State* l) {

  return ScopedLuaRef(l, luaL_ref(l, LUA_REGISTRYINDEX));
}

/**
 * \brief Creates a reference to the Lua value at the specified index.
 * \param l A Lua state.
 * \param index An index in the Lua stack.
 * \return The reference created, wrapped in an object that manages its
 * lifetime.
 */
ScopedLuaRef create_ref(lua_State* l, int index) {

  lua_pushvalue(l, index);
  return ScopedLuaRef(l, luaL_ref(l, LUA_REGISTRYINDEX));
}

/**
 * \brief Calls the Lua function with its arguments on top of the stack.
 *
 * This function is like lua_pcall, except that it additionally handles the
 * error message if an error occurs in the Lua code (the error is printed).
 * This function leaves the results on the stack if there is no error,
 * and leaves nothing on the stack in case of error.
 *
 * \param l A Lua state.
 * \param nb_arguments Number of arguments placed on the Lua stack above the
 * function to call.
 * \param nb_results Number of results expected (you get them on the stack if
 * there is no error).
 * \param function_name A name describing the Lua function (only used to print
 * the error message if any).
 * This is not a <tt>const std::string&</tt> but a <tt>const char*</tt> on
 * purpose to avoid costly conversions as this function is called very often.
 * \return true in case of success.
 */
bool call_function(
    lua_State* l,
    int nb_arguments,
    int nb_results,
    const char* function_name
) {
  if (lua_pcall(l, nb_arguments, nb_results, 0) != 0) {
    Debug::error(std::string("In ") + function_name + ": "
        + lua_tostring(l, -1)
    );
    lua_pop(l, 1);
    return false;
  }

  return true;
}

/**
 * \brief Similar to luaL_error() but throws a LuaException.
 *
 * This function never returns.
 *
 * \param l A Lua state.
 * \param message Error message.
 */
void error(lua_State* l, const std::string& message) {
  throw LuaException(l, message);
}

/**
 * \brief Similar to luaL_argerror() but throws a LuaException.
 *
 * This function never returns.
 *
 * \param l A Lua state.
 * \param arg_index Index of an argument in the stack.
 * \param message Error message.
 */
void arg_error(lua_State* l, int arg_index, const std::string& message) {

  // The code below is very similar to luaL_argerror(), but ends with
  // an exception instead of a luaL_error() call.

  std::ostringstream oss;
  lua_Debug info;
  if (!lua_getstack(l, 0, &info)) {
    // No stack frame.
     oss << "bad argument #" << arg_index << " (" << message << ")";
     error(l, oss.str());
  }

  lua_getinfo(l, "n", &info);
  if (std::string(info.namewhat) == "method") {
     arg_index--;  // Do not count self.
     if (arg_index == 0) {
       // Error is in the self argument itself.
       oss << "calling " << info.name << " on bad self (" << message << ")";
       error(l, oss.str());
     }
  }

  if (info.name == nullptr) {
    info.name = "?";
  }
  oss << "bad argument #" << arg_index << " to " << info.name << " (" << message << ")";
  error(l, oss.str());
}

/**
 * \brief Similar to luaL_typerror() but throws a LuaException.
 *
 * This function never returns.
 *
 * \param l A Lua state.
 * \param arg_index Index of an argument in the stack.
 * \param expected_type_name A name describing the type that was expected.
 */
void type_error(
    lua_State* l,
    int arg_index,
    const std::string& expected_type_name
) {
  arg_error(l, arg_index, std::string(expected_type_name) +
      " expected, got " + luaL_typename(l, arg_index));
}

/**
 * \brief Like luaL_checktype() but throws a LuaException in case of error.
 * \param l A Lua state.
 * \param arg_index Index of an argument in the stack.
 * \param expected_type A Lua type value.
 */
void check_type(
    lua_State* l,
    int arg_index,
    int expected_type
) {
  if (lua_type(l, arg_index) != expected_type) {
    arg_error(l, arg_index, std::string(lua_typename(l, expected_type)) +
        " expected, got " + luaL_typename(l, arg_index));
  }
}

/**
 * \brief Like luaL_checkany() but throws a LuaException in case of error.
 * \param l A Lua state.
 * \param arg_index Index of an argument in the stack.
 */
void check_any(
    lua_State* l,
    int arg_index
) {
  if (lua_type(l, arg_index) == LUA_TNONE) {
    arg_error(l, arg_index, "value expected");
  }
}

/**
 * \brief Checks that a value is a number and returns it as an integer.
 *
 * This function acts like luaL_checkint() except that it throws a LuaException
 * in case of error.
 *
 * \param l A Lua state.
 * \param index Index of a table in the stack.
 * \return The value as an integer.
 */
int check_int(
    lua_State* l,
    int index
) {
  if (!lua_isnumber(l, index)) {
    arg_error(l, index,
        std::string("integer expected, got ")
            + luaL_typename(l, index) + ")"
    );
  }

  return (int) lua_tointeger(l, index);
}

/**
 * \brief Checks that a table field is a number and returns it as an integer.
 *
 * This function acts like lua_getfield() followed by LuaTools::check_int().
 *
 * \param l A Lua state.
 * \param table_index Index of a table in the stack.
 * \param key Key of the field to get in that table.
 * \return The wanted field as an integer.
 */
int check_int_field(
    lua_State* l,
    int table_index,
    const std::string& key
) {
  lua_getfield(l, table_index, key.c_str());
  if (!lua_isnumber(l, -1)) {
    arg_error(l, table_index,
        std::string("Bad field '") + key + "' (integer expected, got "
        + luaL_typename(l, -1) + ")"
    );
  }

  int value = (int) lua_tointeger(l, -1);
  lua_pop(l, 1);
  return value;
}

/**
 * \brief Like LuaTools::check_int() but with a default value.
 *
 * This function acts like luaL_optint() except that it throws a LuaException
 * in case of error.
 *
 * \param l A Lua state.
 * \param index Index of a value in the stack.
 * \param default_value The default value to return if the value is \c nil.
 * \return The wanted value as an integer.
 */
int opt_int(
    lua_State* l,
    int index,
    int default_value
) {
  if (lua_isnoneornil(l, index)) {
    return default_value;
  }
  return check_int(l, index);
}

/**
 * \brief Like LuaTools::check_int_field() but with a default value.
 *
 * This function acts like lua_getfield() followed by luaL_optint().
 *
 * \param l A Lua state.
 * \param table_index Index of a table in the stack.
 * \param key Key of the field to get in that table.
 * \param default_value The default value to return if the field is \c nil.
 * \return The wanted field as an integer.
 */
int opt_int_field(
    lua_State* l,
    int table_index,
    const std::string& key,
    int default_value
) {
  lua_getfield(l, table_index, key.c_str());
  if (lua_isnil(l, -1)) {
    lua_pop(l, 1);
    return default_value;
  }

  if (!lua_isnumber(l, -1)) {
    arg_error(l, table_index,
        std::string("Bad field '") + key + "' (integer expected, got "
        + luaL_typename(l, -1) + ")"
    );
  }
  int value = (int) lua_tointeger(l, -1);
  lua_pop(l, 1);
  return value;
}

/**
 * \brief Checks that a value is a number and returns it as a double.
 *
 * This function acts like luaL_checknumber() except that it throws a
 * LuaException in case of error.
 *
 * \param l A Lua state.
 * \param index Index of a value in the stack.
 * \return The number value.
 */
double check_number(
    lua_State* l,
    int index
) {
  if (!lua_isnumber(l, index)) {
    arg_error(l, index,
        std::string("number expected, got ")
            + luaL_typename(l, index) + ")"
    );
  }

  return lua_tonumber(l, index);
}

/**
 * \brief Checks that a table field is a number and returns it as a double.
 *
 * This function acts like lua_getfield() followed by LuaTools::check_number().
 *
 * \param l A Lua state.
 * \param table_index Index of a table in the stack.
 * \param key Key of the field to get in that table.
 * \return The wanted field as a double.
 */
double check_number_field(
    lua_State* l,
    int table_index,
    const std::string& key
) {
  lua_getfield(l, table_index, key.c_str());
  if (!lua_isnumber(l, -1)) {
    arg_error(l, table_index,
        std::string("Bad field '") + key + "' (number expected, got "
        + luaL_typename(l, -1) + ")"
    );
  }

  double value = lua_tonumber(l, -1);
  lua_pop(l, 1);
  return value;
}

/**
 * \brief Like LuaTools::check_number() but with a default value.
 *
 * This function acts like luaL_optnumber() except that it throws a
 * LuaException in case of error.
 *
 * \param l A Lua state.
 * \param index Index of a value in the stack.
 * \param default_value The default value to return if the value is \c nil.
 * \return The wanted value as a double.
 */
double opt_number(
    lua_State* l,
    int index,
    double default_value
) {
  if (lua_isnoneornil(l, index)) {
    return default_value;
  }
  return check_number(l, index);
}

/**
 * \brief Like LuaTools::check_number_field() but with a default value.
 *
 * This function acts like lua_getfield() followed by LuaTools::opt_number().
 *
 * \param l A Lua state.
 * \param table_index Index of a table in the stack.
 * \param key Key of the field to get in that table.
 * \param default_value The default value to return if the field is \c nil.
 * \return The wanted field as a double.
 */
double opt_number_field(
    lua_State* l,
    int table_index,
    const std::string& key,
    double default_value
) {
  lua_getfield(l, table_index, key.c_str());
  if (lua_isnil(l, -1)) {
    lua_pop(l, 1);
    return default_value;
  }

  if (!lua_isnumber(l, -1)) {
    arg_error(l, table_index,
        std::string("Bad field '") + key + "' (number expected, got "
        + luaL_typename(l, -1) + ")"
    );
  }
  double value = lua_tonumber(l, -1);
  lua_pop(l, 1);
  return value;
}

/**
 * \brief Checks that a value is a string and returns it.
 *
 * This function acts like luaL_checkstring() except that it throws a
 * LuaException in case of error.
 *
 * \param l A Lua state.
 * \param index Index of a value in the stack.
 * \return The string value.
 */
std::string check_string(
    lua_State* l,
    int index
) {
  if (!lua_isstring(l, index)) {
    arg_error(l, index,
        std::string("string expected, got ")
            + luaL_typename(l, index) + ")"
    );
  }

  return lua_tostring(l, index);
}

/**
 * \brief Checks that a table field is a string and returns it.
 *
 * This function acts like lua_getfield() followed by LuaTools::check_string().
 *
 * \param l A Lua state.
 * \param table_index Index of a table in the stack.
 * \param key Key of the field to get in that table.
 * \return The wanted field as a string.
 */
std::string check_string_field(
    lua_State* l,
    int table_index,
    const std::string& key
) {
  lua_getfield(l, table_index, key.c_str());
  if (!lua_isstring(l, -1)) {
    arg_error(l, table_index,
        std::string("Bad field '") + key + "' (string expected, got "
        + luaL_typename(l, -1) + ")"
    );
  }

  const std::string& value = lua_tostring(l, -1);
  lua_pop(l, 1);
  return value;
}

/**
 * \brief Like LuaTools::check_string() but with a default value.
 *
 * This function acts like luaL_optstring() except that it throws a
 * LuaException in case of error.
 *
 * \param l A Lua state.
 * \param index Index of a value in the stack.
 * \param default_value The default value to return if the value is \c nil.
 * \return The wanted value as a string.
 */
std::string opt_string(
    lua_State* l,
    int index,
    const std::string& default_value
) {
  if (lua_isnoneornil(l, index)) {
    return default_value;
  }
  return check_string(l, index);
}

/**
 * \brief Like LuaTools::check_string_field() but with a default value.
 *
 * This function acts like lua_getfield() followed by LuaTools::opt_string().
 *
 * \param l A Lua state.
 * \param table_index Index of a table in the stack.
 * \param key Key of the field to get in that table.
 * \param default_value The default value to return if the field is \c nil.
 * \return The wanted field as a string.
 */
std::string opt_string_field(
    lua_State* l,
    int table_index,
    const std::string& key,
    const std::string& default_value
) {
  lua_getfield(l, table_index, key.c_str());
  if (lua_isnil(l, -1)) {
    lua_pop(l, 1);
    return default_value;
  }

  if (!lua_isstring(l, -1)) {
    arg_error(l, table_index,
        std::string("Bad field '") + key + "' (string expected, got "
        + luaL_typename(l, -1) + ")"
    );
  }
  const std::string& value = lua_tostring(l, -1);
  lua_pop(l, 1);
  return value;
}

/**
 * \brief Checks that a value is a boolean and returns it.
 *
 * This function throws a LuaException in case of error.
 *
 * \param l A Lua state.
 * \param index Index of a value in the stack.
 * \return The boolean value.
 */
bool check_boolean(
    lua_State* l,
    int index
) {
  if (!lua_isboolean(l, index)) {
    arg_error(l, index,
        std::string("boolean expected, got ")
            + luaL_typename(l, index) + ")"
    );
  }
  return lua_toboolean(l, index);
}

/**
 * \brief Checks that a table field is a boolean and returns it.
* \param l A Lua state.
 * \param table_index Index of a table in the stack.
 * \param key Key of the field to get in that table.
 * \return The wanted field as a boolean.
 */
bool check_boolean_field(
    lua_State* l,
    int table_index,
    const std::string& key
) {
  lua_getfield(l, table_index, key.c_str());
  if (lua_type(l, -1) != LUA_TBOOLEAN) {
    arg_error(l, table_index,
        std::string("Bad field '") + key + "' (boolean expected, got "
        + luaL_typename(l, -1) + ")"
    );
  }

  bool value = lua_toboolean(l, -1);
  lua_pop(l, 1);
  return value;
}

/**
 * \brief Like LuaTools::check_boolean() but with a default value.
 * \param l A Lua state.
 * \param index Index of a value in the stack.
 * \param default_value The default value to return if the value is \c nil.
 * \return The wanted value as a boolean.
 */
bool opt_boolean(
    lua_State* l,
    int index,
    bool default_value
) {
  if (lua_isnoneornil(l, index)) {
    return default_value;
  }
  return check_boolean(l, index);
}

/**
 * \brief Like check_boolean_field() but with a default value.
 *
 * This function acts like lua_getfield() followed by lua_toboolean().
 *
 * \param l A Lua state.
 * \param table_index Index of a table in the stack.
 * \param key Key of the field to get in that table.
 * \param default_value The default value to return if the field is \c nil.
 * \return The wanted field as a string.
 */
bool opt_boolean_field(
    lua_State* l,
    int table_index,
    const std::string& key,
    bool default_value
) {
  lua_getfield(l, table_index, key.c_str());
  if (lua_isnil(l, -1)) {
    lua_pop(l, 1);
    return default_value;
  }

  if (lua_type(l, -1) != LUA_TBOOLEAN) {
    arg_error(l, table_index,
        std::string("Bad field '") + key + "' (boolean expected, got "
        + luaL_typename(l, -1) + ")"
    );
  }
  return lua_toboolean(l, -1);
}

/**
 * \brief Checks that a value is a function and returns a ref to it.
 *
 * This function throws a LuaException in case of error.
 *
 * \param l A Lua state.
 * \param index Index of a value in the stack.
 * \return The wanted value as a Lua ref to the function.
 */
ScopedLuaRef check_function(
    lua_State* l,
    int index
) {
  check_type(l, index, LUA_TFUNCTION);
  return create_ref(l, index);  // Leave the function in the stack.
}

/**
 * \brief Checks that a table field is a function and returns a ref to it.
 * \param l A Lua state.
 * \param table_index Index of a table in the stack.
 * \param key Key of the field to get in that table.
 * \return The wanted field as a Lua ref to the function.
 */
ScopedLuaRef check_function_field(
    lua_State* l,
    int table_index,
    const std::string& key
) {
  lua_getfield(l, table_index, key.c_str());
  if (!lua_isfunction(l, -1)) {
    arg_error(l, table_index,
        std::string("Bad field '") + key + "' (function expected, got "
        + luaL_typename(l, -1) + ")"
    );
  }

  return create_ref(l);  // This also pops the function from the stack.
}

/**
 * \brief Like LuaTools::check_function() but the value is optional.
 * \param l A Lua state.
 * \param index Index of a value in the stack.
 * \return The wanted value as a Lua ref to the function, or an empty ref.
 */
ScopedLuaRef opt_function(
    lua_State* l,
    int index
) {
  if (lua_isnoneornil(l, index)) {
    return ScopedLuaRef();
  }
  return check_function(l, index);
}

/**
 * \brief Like LuaTools::check_function_field() but the field is optional.
 * \param l A Lua state.
 * \param table_index Index of a table in the stack.
 * \param key Key of the field to get in that table.
 * \return The wanted field as a Lua ref to the function, or an empty ref.
 */
ScopedLuaRef opt_function_field(
    lua_State* l,
    int table_index,
    const std::string& key
) {
  lua_getfield(l, table_index, key.c_str());
  if (lua_isnil(l, -1)) {
    lua_pop(l, 1);
    return ScopedLuaRef();
  }

  if (!lua_isfunction(l, -1)) {
    arg_error(l, table_index,
        std::string("Bad field '") + key + "' (function expected, got "
        + luaL_typename(l, -1) + ")"
    );
  }
  return create_ref(l);  // This also pops the function from the stack.
}

/**
 * \brief Returns whether a value is a layer.
 * \param l A Lua context.
 * \param index An index in the stack.
 * \return \c true if the value is a layer, that is, a number between 0 and 2.
 */
bool is_layer(
    lua_State* l,
    int index
) {
  if (!lua_isnumber(l, index)) {
    return false;
  }
  int layer = check_int(l, index);
  return layer >= 0 && layer < LAYER_NB;
}

/**
 * \brief Checks that the value at the given index is a valid layer and returns it.
 * \param l A Lua state.
 * \param index An index in the Lua stack.
 * \return The layer at this index.
 */
Layer check_layer(
    lua_State* l,
    int index
) {
  if (!is_layer(l, index)) {
    std::ostringstream oss;
    if (!lua_isnumber(l, index)) {
      oss << "Invalid layer";
    }
    else {
      oss << "Invalid layer: " << lua_tonumber(l, index);
    }
    arg_error(l, index, oss.str());
  }

  return Layer(lua_tointeger(l, index));
}

/**
 * \brief Checks that a table field is a valid layer and returns it.
 *
 * This function acts like lua_getfield() followed by LuaTools::check_layer().
 *
 * \param l A Lua state.
 * \param table_index Index of a table in the stack.
 * \param key Key of the field to get in that table.
 * \return The wanted field as a layer.
 */
Layer check_layer_field(
    lua_State* l,
    int table_index,
    const std::string& key
) {
  lua_getfield(l, table_index, key.c_str());
  if (!is_layer(l, -1)) {
    arg_error(l, table_index,
        std::string("Bad field '") + key + "' (layer expected, got "
        + luaL_typename(l, -1) + ")"
    );
  }

  Layer value = static_cast<Layer>(lua_tointeger(l, -1));
  lua_pop(l, 1);
  return value;
}

/**
 * \brief Like LuaTools::check_layer() but with a default value.
 * \param l A Lua state.
 * \param index Index of a value in the stack.
 * \param default_value The default value to return if the value is \c nil.
 * \return The wanted value as a layer.
 */
Layer opt_layer(
    lua_State* l,
    int index,
    Layer default_value
) {
  if (lua_isnoneornil(l, index)) {
    return default_value;
  }
  return check_layer(l, index);
}

/**
 * \brief Like LuaTools::check_layer_field() but with a default value.
 * \param l A Lua state.
 * \param table_index Index of a table in the stack.
 * \param key Key of the field to get in that table.
 * \param default_value The default value to return if the field is \c nil.
 * \return The wanted field as a layer.
 */
Layer opt_layer_field(
    lua_State* l,
    int table_index,
    const std::string& key,
    Layer default_value
) {
  lua_getfield(l, table_index, key.c_str());
  if (lua_isnil(l, -1)) {
    lua_pop(l, 1);
    return default_value;
  }

  if (!is_layer(l, -1)) {
    arg_error(l, table_index,
        std::string("Bad field '") + key + "' (layer expected, got "
        + luaL_typename(l, -1) + ")"
    );
  }
  Layer value = static_cast<Layer>(lua_tointeger(l, -1));
  lua_pop(l, 1);
  return value;
}

/**
 * \brief Returns whether a value is a color.
 * \param l A Lua context.
 * \param index An index in the stack.
 * \return true if the value is a color, that is, an array with three integers.
 */
bool is_color(lua_State* l, int index) {

  index = get_positive_index(l, index);

  if (lua_type(l, index) != LUA_TTABLE) {
    return false;
  }

  lua_rawgeti(l, index, 1);
  lua_rawgeti(l, index, 2);
  lua_rawgeti(l, index, 3);
  lua_rawgeti(l, index, 4);
  bool result = lua_isnumber(l, -4) &&
      lua_isnumber(l, -3) &&
      lua_isnumber(l, -2) &&
      (lua_isnumber(l, -1) || lua_isnil(l, -1));
  lua_pop(l, 4);
  return result;
}

/**
 * \brief Checks that the value at the given index is a color and returns it.
 *
 * Set opaque by default if alpha channel is not specified.
 *
 * \param l a Lua state
 * \param index an index in the Lua stack
 * \return the color at this index
 */
Color check_color(lua_State* l, int index) {

  index = get_positive_index(l, index);

  check_type(l, index, LUA_TTABLE);
  lua_rawgeti(l, index, 1);
  lua_rawgeti(l, index, 2);
  lua_rawgeti(l, index, 3);
  lua_rawgeti(l, index, 4);
  Color color(
      check_int(l, -4),
      check_int(l, -3),
      check_int(l, -2),
      opt_int(l, -1, 255)
  );
  lua_pop(l, 4);

  return color;
}

/**
 * \brief Checks that a table field is a color and returns it.
 *
 * This function acts like lua_getfield() followed by LuaTools::check_color().
 *
 * \param l A Lua state.
 * \param table_index Index of a table in the stack.
 * \param key Key of the field to get in that table.
 * \return The wanted field as a layer.
 */
Color check_color_field(
    lua_State* l,
    int table_index,
    const std::string& key
) {
  lua_getfield(l, table_index, key.c_str());
  if (!is_color(l, -1)) {
    arg_error(l, table_index,
        std::string("Bad field '") + key + "' (color table expected, got "
        + luaL_typename(l, -1) + ")"
    );
  }

  const Color& value = check_color(l, -1);
  lua_pop(l, 1);
  return value;
}

/**
 * \brief Like LuaTools::check_color() but with a default value.
 * \param l A Lua state.
 * \param index Index of a value in the stack.
 * \param default_value The default value to return if the value is \c nil.
 * \return The wanted value as a color.
 */
Color opt_color(
    lua_State* l,
    int index,
    const Color& default_value
) {
  if (lua_isnoneornil(l, index)) {
    return default_value;
  }
  return check_color(l, index);
}

/**
 * \brief Like LuaTools::check_color_field() but with a default value.
 * \param l A Lua state.
 * \param table_index Index of a table in the stack.
 * \param key Key of the field to get in that table.
 * \param default_value The default value to return if the field is \c nil.
 * \return The wanted field as a color.
 */
Color opt_color_field(
    lua_State* l,
    int table_index,
    const std::string& key,
    const Color& default_value
) {
  lua_getfield(l, table_index, key.c_str());
  if (lua_isnil(l, -1)) {
    lua_pop(l, 1);
    return default_value;
  }

  if (!is_color(l, -1)) {
    arg_error(l, table_index,
        std::string("Bad field '") + key + "' (color expected, got "
        + luaL_typename(l, -1) + ")"
    );
  }
  const Color& color = check_color(l, -1);
  lua_pop(l, 1);
  return color;
}

}
}
