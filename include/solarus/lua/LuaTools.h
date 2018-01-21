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
#ifndef SOLARUS_LUA_TOOLS_H
#define SOLARUS_LUA_TOOLS_H

#include "solarus/core/Common.h"
#include "solarus/core/Debug.h"
#include "solarus/core/EnumInfo.h"
#include "solarus/core/SolarusFatal.h"
#include "solarus/lua/LuaException.h"
#include <map>
#include <string>
#include <lua.hpp>

namespace Solarus {

class Color;
class Map;
class ScopedLuaRef;

/**
 * \brief Provides general Lua helpers functions.
 *
 * All check_* and opt_* functions are similar to their equivalent of the
 * Lua API, but throw an LuaException in case of error instead of calling
 * lua_error(). This guarantees that stack-allocated objects are properly
 * destroyed even in case of error.
 */
namespace LuaTools {

// Helpers.
int get_positive_index(lua_State* l, int index);
bool is_valid_lua_identifier(const std::string& name);
ScopedLuaRef create_ref(lua_State* l);
ScopedLuaRef create_ref(lua_State* l, int index);
bool call_function(
    lua_State* l,
    int nb_arguments,
    int nb_results,
    const char* function_name
);
bool do_string(
    lua_State* l,
    const std::string& code,
    const std::string& chunk_name
);

// Error handling.
template<typename Callable>
int exception_boundary_handle(
    lua_State* l,
    Callable&& func
);

void error(
    lua_State* l,
    const std::string& message
);
void arg_error(
    lua_State* l,
    int arg_index,
    const std::string& message
);
void type_error(
    lua_State* l,
    int arg_index,
    const std::string& expected_type_name
);
void check_type(
    lua_State* l,
    int arg_index,
    int expected_type
);
void check_any(
    lua_State* l,
    int arg_index
);

// int
int check_int(
    lua_State* l,
    int index
);
int check_int_field(
    lua_State* l,
    int table_index,
    const std::string& key
);
int opt_int(
    lua_State* l,
    int index,
    int default_value
);
int opt_int_field(
    lua_State* l,
    int table_index,
    const std::string& key,
    int default_value
);

// double
double check_number(
    lua_State* l,
    int index
);
double check_number_field(
    lua_State* l,
    int table_index,
    const std::string& key
);
double opt_number(
    lua_State* l,
    int index,
    double default_value
);
double opt_number_field(
    lua_State* l,
    int table_index,
    const std::string& key,
    double default_value
);

// std::string
std::string check_string(
    lua_State* l,
    int index
);
std::string check_string_field(
    lua_State* l,
    int table_index,
    const std::string& key
);
std::string opt_string(
    lua_State* l,
    int index,
    const std::string& default_value
);
std::string opt_string_field(
    lua_State* l,
    int table_index,
    const std::string& key,
    const std::string& default_value
);

// bool
bool check_boolean(
    lua_State* l,
    int index
);
bool check_boolean_field(
    lua_State* l,
    int table_index,
    const std::string& key
);
bool opt_boolean(
    lua_State* l,
    int index,
    bool default_value
);
bool opt_boolean_field(
    lua_State* l,
    int table_index,
    const std::string& key,
    bool default_value
);

// function
ScopedLuaRef check_function(
    lua_State* l,
    int index
);
ScopedLuaRef check_function_field(
    lua_State* l,
    int table_index,
    const std::string& key
);
ScopedLuaRef opt_function(
    lua_State* l,
    int index
);
ScopedLuaRef opt_function_field(
    lua_State* l,
    int table_index,
    const std::string& key
);

// Layer
bool is_layer(
    lua_State* l,
    int index,
    const Map& map
);
int check_layer(
    lua_State* l,
    int index,
    const Map& map
);
int check_layer_field(
    lua_State* l,
    int table_index,
    const std::string& key,
    const Map& map
);
int opt_layer(
    lua_State* l,
    int index,
    const Map& map,
    int default_value
);
int opt_layer_field(
    lua_State* l,
    int table_index,
    const std::string& key,
    const Map& map,
    int default_value
);

// Color
bool is_color(
    lua_State* l,
    int index
);
Color check_color(
    lua_State* l,
    int index
);
Color check_color_field(
    lua_State* l,
    int table_index,
    const std::string& key
);
Color opt_color(
    lua_State* l,
    int index,
    const Color& default_value
);
Color opt_color_field(
    lua_State* l,
    int table_index,
    const std::string& key,
    const Color& default_value
);

// enum
template<typename E>
E check_enum(
    lua_State* l,
    int index
);
template<typename E>
E check_enum_field(
    lua_State* l,
    int table_index,
    const std::string& key
);
template<typename E>
E opt_enum(
    lua_State* l,
    int index,
    E default_value
);
template<typename E>
E opt_enum_field(
    lua_State* l,
    int table_index,
    const std::string& key,
    E default_value
);

template<typename E>
E check_enum(
    lua_State* l,
    int index,
    const std::map<E, std::string>& names
);
template<typename E>
E check_enum_field(
    lua_State* l,
    int table_index,
    const std::string& key,
    const std::map<E, std::string>& names
);
template<typename E>
E opt_enum(
    lua_State* l,
    int index,
    const std::map<E, std::string>& names,
    E default_value
);
template<typename E>
E opt_enum_field(
    lua_State* l,
    int table_index,
    const std::string& key,
    const std::map<E, std::string>& names,
    E default_value
);

}

}

#include "solarus/lua/LuaTools.inl"

#endif
