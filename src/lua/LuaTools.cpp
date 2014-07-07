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
#include "lua/LuaTools.h"
#include "lowlevel/Color.h"
#include <sstream>

namespace solarus {

/**
 * \brief Like luaL_error() but with an std::string message.
 * \param l A Lua state.
 * \param message Error message.
 * \return This function never returns.
 */
int LuaTools::error(lua_State* l, const std::string& message) {
  return luaL_error(l, message.c_str());
}

/**
 * \brief Like luaL_argerror() but with an std::string message.
 * \param l A Lua state.
 * \param arg_index Index of an argument in the stack.
 * \param message Error message.
 * \return This function never returns.
 */
int LuaTools::arg_error(lua_State* l, int arg_index, const std::string& message) {
  return luaL_argerror(l, arg_index, message.c_str());
}

/**
 * \brief Returns whether a value is a color.
 * \param l A Lua context.
 * \param index An index in the stack.
 * \return true if the value is a color, that is, an array with three integers.
 */
bool LuaTools::is_color(lua_State* l, int index) {

  bool result = false;
  if (lua_type(l, index) != LUA_TTABLE) {
    lua_rawgeti(l, index, 1);
    lua_rawgeti(l, index, 2);
    lua_rawgeti(l, index, 3);
    lua_rawgeti(l, index, 4);
    result = lua_isnumber(l, -4)
      && lua_isnumber(l, -3)
      && lua_isnumber(l, -2)
      && (lua_isnumber(l, -1) || lua_isnil(l, -1));
    lua_pop(l, 4);
  }
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
Color LuaTools::check_color(lua_State* l, int index) {

  index = get_positive_index(l, index);

  luaL_checktype(l, index, LUA_TTABLE);
  lua_rawgeti(l, index, 1);
  lua_rawgeti(l, index, 2);
  lua_rawgeti(l, index, 3);
  lua_rawgeti(l, index, 4);
  Color color(luaL_checkint(l, -4),
    luaL_checkint(l, -3),
    luaL_checkint(l, -2),
    luaL_optint(l, -1, 255));
  lua_pop(l, 4);

  return color;
}

/**
 * \brief Returns whether a value is a layer.
 * \param l A Lua context.
 * \param index An index in the stack.
 * \return \c true if the value is a layer, that is, a number between 0 and 2.
 */
bool LuaTools::is_layer(lua_State* l, int index) {

  int layer = luaL_checkint(l, index);
  return layer >= 0 && layer < LAYER_NB;
}

/**
 * \brief Checks that the value at the given index is a valid layer and returns it.
 * \param l A Lua state.
 * \param index An index in the Lua stack.
 * \return The layer at this index.
 */
Layer LuaTools::check_layer(lua_State* l, int index) {

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
 * \brief Checks that a table field is a number and returns it as an integer.
 *
 * This function acts like lua_getfield() followed by luaL_checkint().
 *
 * \param l A Lua state.
 * \param table_index Index of a table in the stack.
 * \param key Key of the field to get in that table.
 * \return The wanted field as an integer.
 */
int LuaTools::check_int_field(
    lua_State* l, int table_index, const std::string& key) {

  lua_getfield(l, table_index, key.c_str());
  if (!lua_isnumber(l, -1)) {
    arg_error(l, table_index,
        std::string("Bad field '") + key + "' (integer expected, got "
        + luaL_typename(l, -1) + ")"
    );
  }

  int value = int(lua_tointeger(l, -1));
  lua_pop(l, 1);
  return value;
}

/**
 * \brief Like check_int_field() but with a default value.
 *
 * This function acts like lua_getfield() followed by luaL_optint().
 *
 * \param l A Lua state.
 * \param table_index Index of a table in the stack.
 * \param key Key of the field to get in that table.
 * \param default_value The default value to return if the field is \c nil.
 * \return The wanted field as an integer.
 */
int LuaTools::opt_int_field(
    lua_State* l, int table_index, const std::string& key, int default_value) {

  lua_getfield(l, table_index, key.c_str());
  int value = default_value;
  if (!lua_isnil(l, -1)) {

    if (!lua_isnumber(l, -1)) {
      arg_error(l, table_index,
          std::string("Bad field '") + key + "' (integer expected, got "
          + luaL_typename(l, -1) + ")"
      );
    }
    value = int(lua_tointeger(l, -1));
  }

  lua_pop(l, 1);
  return value;
}

/**
 * \brief Checks that a table field is a number and returns it as a double.
 *
 * This function acts like lua_getfield() followed by luaL_checknumber().
 *
 * \param l A Lua state.
 * \param table_index Index of a table in the stack.
 * \param key Key of the field to get in that table.
 * \return The wanted field as a double.
 */
double LuaTools::check_number_field(
    lua_State* l, int table_index, const std::string& key) {

  lua_getfield(l, table_index, key.c_str());
  if (!lua_isnumber(l, -1)) {
    arg_error(l, table_index,
        std::string("Bad field '") + key + "' (number expected, got "
        + luaL_typename(l, -1) + ")"
    );
  }

  int value = lua_tonumber(l, -1);
  lua_pop(l, 1);
  return value;
}

/**
 * \brief Like check_number_field() but with a default value.
 *
 * This function acts like lua_getfield() followed by luaL_optnumber().
 *
 * \param l A Lua state.
 * \param table_index Index of a table in the stack.
 * \param key Key of the field to get in that table.
 * \param default_value The default value to return if the field is \c nil.
 * \return The wanted field as a double.
 */
double LuaTools::opt_number_field(
    lua_State* l, int table_index, const std::string& key, double default_value) {

  lua_getfield(l, table_index, key.c_str());
  double value = default_value;
  if (!lua_isnil(l, -1)) {

    if (!lua_isnumber(l, -1)) {
      arg_error(l, table_index,
          std::string("Bad field '") + key + "' (number expected, got "
          + luaL_typename(l, -1) + ")"
      );
    }
    value = lua_tonumber(l, -1);
  }

  lua_pop(l, 1);
  return value;
}

/**
 * \brief Checks that a table field is a string and returns it.
 *
 * This function acts like lua_getfield() followed by luaL_checkstring().
 *
 * \param l A Lua state.
 * \param table_index Index of a table in the stack.
 * \param key Key of the field to get in that table.
 * \return The wanted field as an string.
 */
const std::string LuaTools::check_string_field(
    lua_State* l, int table_index, const std::string& key) {

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
 * \brief Like check_string_field() but with a default value.
 *
 * This function acts like lua_getfield() followed by luaL_optstring().
 *
 * \param l A Lua state.
 * \param table_index Index of a table in the stack.
 * \param key Key of the field to get in that table.
 * \param default_value The default value to return if the field is \c nil.
 * \return The wanted field as a string.
 */
const std::string LuaTools::opt_string_field(
    lua_State* l, int table_index, const std::string& key, const std::string& default_value) {

  lua_getfield(l, table_index, key.c_str());
  std::string value;
  if (lua_isnil(l, -1)) {
    value = default_value;
  }
  else {
    if (!lua_isstring(l, -1)) {
      arg_error(l, table_index,
          std::string("Bad field '") + key + "' (string expected, got "
          + luaL_typename(l, -1) + ")"
      );
    }
    value = lua_tostring(l, -1);
  }

  lua_pop(l, 1);
  return value;
}

/**
 * \brief Checks that a table field is a boolean and returns it.
 *
 * This function acts like lua_getfield() followed by luaL_checktype()
 * and lua_toboolean().
 *
 * \param l A Lua state.
 * \param table_index Index of a table in the stack.
 * \param key Key of the field to get in that table.
 * \return The wanted field as a boolean.
 */
bool LuaTools::check_boolean_field(
    lua_State* l, int table_index, const std::string& key) {

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
bool LuaTools::opt_boolean_field(
    lua_State* l, int table_index, const std::string& key, bool default_value) {

  lua_getfield(l, table_index, key.c_str());
  bool value = default_value;
  if (!lua_isnil(l, -1)) {

    if (lua_type(l, -1) != LUA_TBOOLEAN) {
      arg_error(l, table_index,
          std::string("Bad field '") + key + "' (boolean expected, got "
          + luaL_typename(l, -1) + ")"
      );
    }
    value = lua_toboolean(l, -1);
  }

  lua_pop(l, 1);
  return value;
}

/**
 * \brief Checks that a table field is a function and returns a ref to it.
 *
 * This function acts like lua_getfield() followed by lua_isfunction() and
 * luaL_ref().
 *
 * \param l A Lua state.
 * \param table_index Index of a table in the stack.
 * \param key Key of the field to get in that table.
 * \return The wanted field as a Lua ref to the function.
 */
int LuaTools::check_function_field(
    lua_State* l, int table_index, const std::string& key) {

  lua_getfield(l, table_index, key.c_str());
  if (!lua_isfunction(l, -1)) {
    arg_error(l, table_index,
        std::string("Bad field '") + key + "' (function expected, got "
        + luaL_typename(l, -1) + ")"
    );
  }

  int ref = luaL_ref(l, LUA_REGISTRYINDEX);
  return ref;
}

/**
 * \brief Like check_function_field() but the field is optional.
 *
 * This function acts like lua_getfield() followed by lua_isfunction() and
 * luaL_ref().
 *
 * \param l A Lua state.
 * \param table_index Index of a table in the stack.
 * \param key Key of the field to get in that table.
 * \return The wanted field as a Lua ref to the function, or LUA_REFNIL.
 */
int LuaTools::opt_function_field(
    lua_State* l, int table_index, const std::string& key) {

  lua_getfield(l, table_index, key.c_str());
  int ref = LUA_REFNIL;
  if (lua_isnil(l, -1)) {
    lua_pop(l, 1);
  }
  else {
    if (!lua_isfunction(l, -1)) {
      arg_error(l, table_index,
          std::string("Bad field '") + key + "' (function expected, got "
          + luaL_typename(l, -1) + ")"
      );
    }
    ref = luaL_ref(l, LUA_REGISTRYINDEX);
  }

  return ref;
}

/**
 * \brief Checks that a table field is a valid layer and returns it.
 *
 * This function acts like lua_getfield() followed by check_layer().
 *
 * \param l A Lua state.
 * \param table_index Index of a table in the stack.
 * \param key Key of the field to get in that table.
 * \return The wanted field as a layer.
 */
Layer LuaTools::check_layer_field(
    lua_State* l, int table_index, const std::string& key) {

  lua_getfield(l, table_index, key.c_str());
  if (!is_layer(l, -1)) {
    arg_error(l, table_index,
        std::string("Bad field '") + key + "' (layer expected, got "
        + luaL_typename(l, -1) + ")"
    );
  }

  Layer value = Layer(lua_tointeger(l, -1));
  lua_pop(l, 1);
  return value;
}

/**
 * \brief Like check_layer_field() but with a default value.
 *
 * This function acts like lua_getfield() followed by is_layer().
 *
 * \param l A Lua state.
 * \param table_index Index of a table in the stack.
 * \param key Key of the field to get in that table.
 * \param default_value The default value to return if the field is \c nil.
 * \return The wanted field as a layer.
 */
Layer LuaTools::opt_layer_field(
    lua_State* l,
    int table_index,
    const std::string& key,
    Layer default_value) {

  lua_getfield(l, table_index, key.c_str());
  Layer value = default_value;
  if (!lua_isnil(l, -1)) {

    if (!is_layer(l, -1)) {
      arg_error(l, table_index,
          std::string("Bad field '") + key + "' (layer expected, got "
          + luaL_typename(l, -1) + ")"
      );
    }
    value = Layer(lua_tointeger(l, -1));
  }

  lua_pop(l, 1);
  return value;
}

/**
 * \brief For an index in the Lua stack, returns an equivalent positive index.
 *
 * Pseudo-indexes are unchanged.
 *
 * \param l A Lua state.
 * \param index An index in the stack.
 * \return The corresponding positive index.
 */
int LuaTools::get_positive_index(lua_State* l, int index) {

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
bool LuaTools::is_valid_lua_identifier(const std::string& name) {

  if (name.empty() || (name[0] >= '0' && name[0] <= '9')) {
    return false;
  }

  bool valid = true;
  for (std::string::const_iterator it = name.begin(); it != name.end() && valid; it++) {
    char character = *it;
    valid = (character == '_' ||
        (character >= 'a' && character <= 'z') ||
        (character >= 'A' && character <= 'Z') ||
        (character >= '0' && character <= '9'));
  }
  return valid;
}

}

