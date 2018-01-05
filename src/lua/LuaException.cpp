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
#include "solarus/lua/LuaException.h"

namespace Solarus {

/**
 * \brief Creates a Lua exception with the specified error message.
 * \param l The Lua state where the error occurred.
 * \param error_message The error message.
 */
LuaException::LuaException(
    lua_State* l,
    const std::string& error_message
):
    l(l),
    error_message(error_message) {
}

/**
 * \brief Returns The Lua context where the error occurred.
 * \return The Lua context.
 */
lua_State* LuaException::get_lua_state() const {
  return l;
}

/**
 * \brief Returns the error message.
 * \return The error message of the exception.
 */
const char* LuaException::what() const throw() {
  return error_message.c_str();
}

}

