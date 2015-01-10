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

namespace Solarus {
namespace LuaTools {

/**
 * \brief Function to be used at the Lua to C++ boundary.
 *
 * The whole code of any function that can directly be called from Lua must be
 * wrapped in a lambda passed to exception_boundary_handle, including the
 * declaration of variables and the return instructions.
 * Any exception is caught and generates a Lua error instead.
 *
 * \param l The Lua state.
 * \param func The lambda wrapping the code.
 * \return The return value of the wrapped code.
 */
template<typename Callable>
int exception_boundary_handle(
    lua_State* l,
    Callable&& func
) {
  try
  {
    return func();
  }
  catch (const LuaException& ex) {
    luaL_error(l, ex.what());
  }
  catch (const SolarusFatal& ex) {
    luaL_error(l, (std::string("Internal error: ") + ex.what()).c_str());
  }
  catch (const std::exception& ex) {
    luaL_error(l, (std::string("Unexpected exception: ") + ex.what()).c_str());
  }
  catch (...) {
    luaL_error(l, "Unexpected exception");
  }
  return 0;
}

/**
 * \brief Checks whether a value is the name of an enumeration value and
 * returns this value.
 *
 * Throws a LuaException if the value is not a string or if the string is not
 * a valid name for the enum.
 * This is a useful function for mapping strings to C or C++ enums.
 *
 * This function is similar to luaL_checkoption() except that it accepts an
 * std::map<E, std::string> instead of an array of char*,
 * and returns a value of enumerated type E instead of int.
 *
 * \param l A Lua state.
 * \param index Index of a string in the Lua stack.
 * \param names A mapping of enum values to strings to search in.
 * \return The enumerated value corresponding to this string.
 */
template<typename E>
E check_enum(
    lua_State* l,
    int index,
    const std::map<E, std::string>& names
) {
  const std::string& name = LuaTools::check_string(l, index);
  for (const auto& kvp: names) {
    if (kvp.second == name) {
      return kvp.first;
    }
  }

  // The value was not found. Build an error message with possible values.
  std::string allowed_names;
  for (const auto& kvp: names) {
    allowed_names += "\"" + kvp.second + "\", ";
  }
  allowed_names = allowed_names.substr(0, allowed_names.size() - 2);

  arg_error(l, index,
      std::string("Invalid name '") + name + "'. Allowed names are: "
      + allowed_names
  );
  return E();  // Make sure the compiler is happy.
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
 * \param names A mapping of enum values to strings to search in.
 * \return The enumerated value corresponding to this string.
 */
template<typename E>
E check_enum_field(
    lua_State* l,
    int table_index,
    const std::string& key,
    const std::map<E, std::string>& names
) {
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
 * \param names A mapping of enum values to strings to search in.
 * \param default_value The default value to return.
 * \return The enumerated value corresponding to this string.
 */
template<typename E>
E opt_enum(
    lua_State* l,
    int index,
    const std::map<E, std::string>& names,
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
 * \param names A mapping of enum values to strings to search in.
 * \param default_value The default value to return.
 * \return The enumerated value corresponding to this string.
 */
template<typename E>
E opt_enum_field(
    lua_State* l,
    int table_index,
    const std::string& key,
    const std::map<E, std::string>& names,
    E default_value
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
  E value = check_enum<E>(l, -1, names);
  lua_pop(l, 1);
  return value;
}

}  // namespace LuaTools
}  // namespace Solarus
